/**
 * @file SpamDetector.hpp
 * @author  Jason Elter <jason.elter@mail.huji.ac.il>
 * @version 1.0
 * @date 24 January 2020
 *
 * @brief Simple use of the HashMap class for detecting spam words from given database.
 *
 * @section DESCRIPTION
 * This file implements the SpamDetector.
 */

#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <vector>
#include "HashMap.hpp"

// Constants.
#define USAGE_MSG "Usage: SpamDetector <database path> <message path> <threshold>"
#define INPUT_ERROR "Invalid input"
#define SEPARATOR ","
#define SPAM "SPAM"
#define NOT_SPAM "NOT_SPAM"

#define ARGS_COUNT 4
#define DATABASE_INDEX 1
#define MESSAGE_INDEX 2
#define THRESHOLD_INDEX 3

const char CAPS_MIN = 'A', CAPS_MAX = 'Z';

// Prints that there has been an input error and exits the program.
static void badInput()
{
    std::cerr << INPUT_ERROR << std::endl;
    exit(EXIT_FAILURE);
}

// Returns true if the given string represents a non-negative number. Otherwise, returns false.
static bool _isNonNegativeNumber(const std::string &st)
{
    for (char ch : st)
    {
        if (!std::isdigit(ch))
        {
            return false;
        }
    }
    return !st.empty();
}

// Converts a given non-const string to lowercase and returns reference to it.
static std::string &_toLowercase(std::string &st)
{
    int length = st.length();
    for (int i = 0; i < length; i++)
    {
        char &ch = st[i];
        if (ch >= CAPS_MIN && ch <= CAPS_MAX)
        {
            ch = (char) tolower(ch);
        }
    }
    return st;
}

// Returns copy of const string in lowercase.
static std::string _toLowercase(const std::string &st)
{
    std::string copy(st);
    return _toLowercase(copy);
}

/*
 * Reads a CSV file and loads the phrases and scores into two vectors in the correct order.
 *
 * @param pathString C style string that contains path to CSV file.
 * @param phrases An empty vector to fill with the database phrases.
 * @param scores An empty vector to fill with the database scores.
 */
static void
loadDatabase(const char *pathString, std::vector<std::string> &phrases, std::vector<int> &scores)
{
    typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
    boost::char_separator<char> sep{SEPARATOR};
    boost::filesystem::path p(pathString);
    if (!boost::filesystem::exists(p))
    {
        badInput();
    }

    // Read lines from file.
    boost::filesystem::ifstream file(p);
    for (std::string line; std::getline(file, line);)
    {
        if (line.empty() && !file.eof())
        {
            badInput();
        }

        // Tokenize and get values.
        tokenizer tok{line, sep};
        tokenizer::iterator it = tok.begin();
        if (it == tok.end())
        {
            badInput();
        }

        phrases.push_back(_toLowercase(*it));
        if (++it == tok.end() || !_isNonNegativeNumber(*it))
        {
            badInput();
        }
        scores.push_back(std::stoi(*it));

        // Check that only 2 columns have been given.
        if (++it != tok.end())
        {
            badInput();
        }
    }
}

/*
 * Scores an email according to given database and returns the score.
 *
 * @param pathString The path to the email file.
 * @param database A HashMap that maps from bad phrases to scores.
 * @return The score the email got.
 */
static int scoreEmail(const char *pathString, HashMap<std::string, int> &database)
{
    boost::filesystem::path p(pathString);
    if (!boost::filesystem::exists(p))
    {
        badInput();
    }

    // Read lines from file.
    int score = 0;
    boost::filesystem::ifstream file(p);
    for (std::string line; std::getline(file, line);)
    {
        if (!line.empty())
        {
            _toLowercase(line);
            for (const std::pair<std::string, int> &pair : database)
            {
                int phraseIndex = line.find(pair.first);
                while (phraseIndex != std::string::npos)
                {
                    line.replace(phraseIndex, pair.first.size(), SEPARATOR);
                    score += pair.second;
                    phraseIndex = line.find(pair.first, phraseIndex);
                }
            }
        }
    }
    return score;
}

/**
 * Program's main that receives <database path> <message path> <threshold>
 * and prints whether or not the given email is spam or not,
 * according to the given database and threshold.
 *
 * @param argc Count of args.
 * @param argv Args values.
 * @return Program exit status code.
 */
int main(int argc, char **argv)
{
    if (argc != ARGS_COUNT)
    {
        std::cerr << USAGE_MSG << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string thresholdString(argv[THRESHOLD_INDEX]);
    int threshold;
    if (!_isNonNegativeNumber(thresholdString) || (threshold = std::stoi(thresholdString)) == 0)
    {
        badInput();
    }

    std::vector<std::string> phrases;
    std::vector<int> scores;

    loadDatabase(argv[DATABASE_INDEX], phrases, scores);
    HashMap<std::string, int> database(phrases, scores);

    int score = scoreEmail(argv[MESSAGE_INDEX], database);
    std::cout << ((score >= threshold) ? SPAM : NOT_SPAM) << std::endl;

    return EXIT_SUCCESS;
}