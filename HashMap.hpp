/**
 * @file HashMap.hpp
 * @author  Jason Elter <jason.elter@mail.huji.ac.il>
 * @version 1.0
 * @date 24 January 2020
 *
 * @brief Implementation of a HashMap in c++.
 *
 * @section DESCRIPTION
 * This is a header and implementation file for the HashMap class.
 */

#ifndef SPAMDETECTOR_HASHMAP_HPP
#define SPAMDETECTOR_HASHMAP_HPP

#include <vector>
#include <list>

#define DEFAULT_SIZE 0
#define DEFAULT_CAPACITY 16
#define MIN_CAPACITY 1
#define CHANGE_FACTOR 2
#define ERROR_VECTOR_INPUT "ERROR: HashMap should receive 2 valid vectors of equal size."
#define ERROR_KEY_NOT_FOUND "ERROR: HashMap key not found."
#define ERROR_OUT_OF_RANGE "ERROR: Attempting to use HashMap iterator outside of range."

const double MIN_LOAD_FACTOR = 0.25, MAX_LOAD_FACTOR = 0.75;
const bool END_FLAG = false;


/**
 * Generic abstract exception for HashMap exceptions.
 */
class HashMapException : public std::exception
{
public:
    const char *what() const noexcept override = 0;
};

/**
 * Exception for problems with vector input in HashMap ctor.
 */
class VectorInputException : public HashMapException
{
public:
    const char *what() const noexcept override
    {
        return ERROR_VECTOR_INPUT;
    }
};

/**
 * Exception for not finding a key in a HashMap.
 */
class KeyNotFoundException : public HashMapException
{
public:
    const char *what() const noexcept override
    {
        return ERROR_KEY_NOT_FOUND;
    }
};

/**
 * Exception for going out of range in HashMap iterator.
 */
class OutOfRangeException : public HashMapException
{
    const char *what() const noexcept override
    {
        return ERROR_OUT_OF_RANGE;
    }
};

/**
 * Generic map class that uses open-hashing.
 *
 * @tparam KeyT The key type.
 * @tparam ValueT The value type.
 */
template<typename KeyT, typename ValueT>
class HashMap
{
    typedef std::vector<std::pair<KeyT, ValueT> *> HashRow;

    // The hashing function.
    int _hash(const KeyT &key) const noexcept
    {
        return std::hash<KeyT>{}(key) & (_capacity - 1);
    }

    // Copies another HashMap into this one.
    void _copy(const HashMap &other) noexcept;

    // Rehashes this map.
    void _rehashArray(int newCapacity) noexcept;

    int _size, _capacity;
    ValueT _defaultValue;
    HashRow *_arr;

public:
    // Constructors and destructors.
    /**
    * Creates an empty HashMap.
    */
    HashMap() noexcept;

    /**
     * Creates a new HashMap from two vectors: one with keys and one with values.
     * The mapping is done by the order of the vectors.
     * The vectors have to be of the same size.
     *
     * @param keys A vector of keys.
     * @param values A vector of values.
     * @throws VectorInputException if vectors aren't of same size.
     */
    HashMap(const std::vector<KeyT> &keys, const std::vector<ValueT> &values);

    /**
     * Copy constructor for HashMap.
     *
     * @param other The other HashMap.
     */
    HashMap(const HashMap &other) noexcept;

    /**
     * Destructor for HashMap.
     */
    ~HashMap() noexcept;

    // Inner classes.
    /**
     * const forward iterator class for HashMap.
     */
    class const_iterator
    {
        int _i, _j, _capacity;
        HashRow *_arr;


    public:
        // iterator traits.
        typedef std::forward_iterator_tag iterator_category;
        typedef std::pair<KeyT, ValueT> value_type;
        typedef std::ptrdiff_t difference_type;
        typedef value_type* pointer;
        typedef value_type& reference;

        /**
         * Creates new iterator from within instance of HashMap.
         *
         * @param arr The HashMaps array.
         * @param capacity The HashMaps capacity.
         * @param begin if true then starts at start. Otherwise at end of iterator.
         */
        explicit const_iterator(HashRow *arr, int capacity, bool begin = true) noexcept;

        /**
         * Copy constructor for iterator.
         *
         * @param other The iterator to copy.
         */
        const_iterator(const const_iterator &other) noexcept : _i(other._i), _j(other._j), _capacity(other._capacity),
                                                               _arr(other._arr) {}

        /**
         * -> operator for iterator.
         *
         * @throws OutOfRangeException if iterator has gone out of valid range.
         * @return address f of pair to be used in -> operation.
         */
        const std::pair<KeyT, ValueT> *operator->() const;

        /**
         * Dereference operator for iterator.
         *
         * @throws OutOfRangeException if iterator has gone out of valid range.
         * @return The current pair.
         */
        const std::pair<KeyT, ValueT> &operator*() const
        {
            return *((*this).operator->());
        }

        /**
         * Advances to operator by 1 and returns instance of this iterator after advancement.
         *
         * @return Instance of this iterator after advancement.
         */
        const_iterator &operator++() noexcept;

        /**
         * Advances to operator by 1 and returns copy of this iterator before advancement.
         *
         * @return Copy of this iterator before advancement.
         */
        const const_iterator operator++(int) noexcept;

        /**
         * Assignment operator for iterator.
         *
         * @param other The other iterator.
         * @return Instance of this iterator after assignment.
         */
        const_iterator &operator=(const const_iterator &other) noexcept;

        /**
         * Returns true if both iterators point at same HashMap and at the same location.
         * Otherwise, returns false.
         *
         * @param other The other iterator.
         * @return true if both iterators point at same HashMap at same location. Otherwise, false.
         */
        bool operator==(const const_iterator &other) const noexcept
        {
            return (_arr == other._arr) && (_i == other._i) && (_j == other._j);
        }

        /**
         * Returns true if both iterators don't point at same HashMap at the same location.
         * Otherwise, returns false.
         *
         * @param other The other iterator.
         * @return true if both don't point at same HashMap at same location. Otherwise, false.
         */
        bool operator!=(const const_iterator &other) const noexcept
        {
            return !(*this == other);
        }

    };

    // Methods.
    /**
     * Returns how many elements are currently in this map.
     *
     * @return How many elements are currently in this map.
     */
    int size() const noexcept
    {
        return _size;
    }

    /**
     * Returns the actual current capacity of this map.
     *
     * @return The actual current capacity of this map.
     */
    int capacity() const noexcept
    {
        return _capacity;
    }

    /**
     * Returns true if there no elements in this map. Otherwise, returns false.
     *
     * @return True if there no elements in this map. Otherwise, returns false.
     */
    bool empty() const noexcept
    {
        return _size == 0;
    }

    /**
     * Returns true if insertion to this map is successful. Otherwise returns false.
     * Failure to insert happens when key already exists in this map.
     *
     * @param key The key to insert.
     * @param value The value to insert.
     * @return True if insertion to this map is successful. Otherwise returns false.
     */
    bool insert(const KeyT &key, const ValueT &value) noexcept;

    /**
     * Returns true if this map contains the given key. Otherwise, returns false.
     *
     * @param key The key to find.
     * @return True if this map contains the given key. Otherwise, returns false.
     */
    bool containsKey(const KeyT &key) const noexcept;

    /**
     * Returns the value paired with the given key, if it is in this map.
     * Otherwise, throws exception. (Const version)
     *
     * @param key The key to find.
     * @throws KeyNotFoundException if key isn't in this map.
     * @return The value paired with the given key.
     */
    const ValueT &at(const KeyT &key) const;

    /**
     * Returns the value paired with the given key, if it is in this map.
     * Otherwise, throws exception.
     *
     * @param key The key to find.
     * @throws KeyNotFoundException if key isn't in this map.
     * @return The value paired with the given key.
     */
    ValueT &at(const KeyT &key);

    /**
     * Returns true if given key was found in this map and erases it. Otherwise, returns false.
     *
     * @param key The key to erase.
     * @return True if given key was found in this map and erases it. Otherwise, returns false.
     */
    bool erase(const KeyT &key) noexcept;

    /**
     * Returns this map's load factor.
     *
     * @return This map's load factor.
     */
    double getLoadFactor() const noexcept
    {
        return (double) _size / _capacity;
    }

    /**
     * Returns the size of the bucket which contains the given key, if it is in this map.
     * Otherwise, throws exception.
     *
     * @param key The key with which to find the bucket.
     * @throws KeyNotFoundException if key isn't in this map.
     * @return The size of the bucket which contains the given key.
     */
    int bucketSize(const KeyT &key) const;

    /**
     * Returns the index of the bucket which contains the given key, if it is in this map.
     * Otherwise, throws exception.
     *
     * @param key The key with which to find the bucket.
     * @throws KeyNotFoundException if key isn't in this map.
     * @return The index of the bucket which contains the given key.
     */
    int bucketIndex(const KeyT &key) const;

    /**
     * Clears this map from all elements, while not changing the capacity.
     */
    void clear() noexcept;

    /**
     * Returns starting iterator for this map.
     *
     * @return Starting iterator for this map.
     */
    const_iterator begin() const
    {
        return const_iterator(_arr, _capacity);
    }

    /**
     * Returns end iterator for this map.
     *
     * @return End iterator for this map.
     */
    const_iterator end() const
    {
        return const_iterator(_arr, _capacity, END_FLAG);
    }

    /**
     * Returns starting iterator for this map.
     *
     * @return Starting iterator for this map.
     */
    const_iterator cbegin() const noexcept
    {
        return const_iterator(_arr, _capacity);
    }

    /**
     * Returns end iterator for this map.
     *
     * @return End iterator for this map.
     */
    const_iterator cend() const noexcept
    {
        return const_iterator(_arr, _capacity, END_FLAG);
    }

    // Operators.
    /**
     * Assignment operator for this map.
     *
     * @param other The map to copy.
     * @return Instance of this map after copying the given one.
     */
    HashMap<KeyT, ValueT> &operator=(const HashMap<KeyT, ValueT> &other) noexcept;

    /**
     * Returns the value paired with the given key, if it is in this map.
     * Otherwise, undefined behaviour. (Const version)
     *
     * @param key The key to find.
     * @return The value paired with the given key.
     */
    const ValueT &operator[](const KeyT &key) const noexcept;

    /**
     * Returns the value paired with the given key, if it is in this map.
     * Otherwise, undefined behaviour. (Const version)
     *
     * @param key The key to find.
     * @return The value paired with the given key.
     */
    ValueT &operator[](const KeyT &key) noexcept;

    /**
     * Returns true if both maps contain equal elements. Otherwise, returns false.
     *
     * @param other The map to compare too.
     * @return True if both maps contain equal elements. Otherwise, returns false.
     */
    bool operator==(const HashMap &other) const noexcept;

    /**
     * Returns true if both maps don't contain equal elements. Otherwise, returns false.
     *
     * @param other The map to compare too.
     * @return True if both maps don't contain equal elements. Otherwise, returns false.
     */
    bool operator!=(const HashMap &other) const noexcept
    {
        return !(*this == other);
    }
};

/*
 * Private helper function that returns a pointer to the value if the given key is in this row.
 * Otherwise, return nullptr. (This way this function can be used to save code in multiple places)
 */
template<typename KeyT, typename ValueT>
static ValueT *_getValue(const KeyT &key, const std::vector<std::pair<KeyT, ValueT> *> &row) noexcept
{
    for (auto *pair : row)
    {
        if (pair->first == key)
        {
            return &(pair->second);
        }
    }
    return nullptr;
}

/*
 * Private helper function that returns true if a key is in this row and deletes it.
 * Otherwise, returns false.
 */
template<typename KeyT, typename ValueT>
static bool _deleteValue(const KeyT &key, std::vector<std::pair<KeyT, ValueT> *> &row) noexcept
{
    for (auto it = row.begin(); it != row.end(); ++it)
    {
        if ((*it)->first == key)
        {
            delete (*it);
            if (row.back() != *it) // swap with back then pop for O(1) erase.
            {
                std::swap(row.back(), *it);
            }
            row.pop_back();
            return true;
        }
    }
    return false;
}

// Private helper function that copies another map into this one.
template<typename KeyT, typename ValueT>
void HashMap<KeyT, ValueT>::_copy(const HashMap &other) noexcept
{
    _size = other._size;
    _capacity = other._capacity;
    _arr = new std::vector<std::pair<KeyT, ValueT> *>[_capacity];
    for (int i = 0; i < _capacity; i++)
    {
        auto &thisRow = _arr[i], &otherRow = other._arr[i];
        for (auto *pair : otherRow)
        {
            thisRow.push_back(new std::pair<KeyT, ValueT>(*pair));
        }
    }
}

// Private method that rehashes this map.
template<typename KeyT, typename ValueT>
void HashMap<KeyT, ValueT>::_rehashArray(int newCapacity) noexcept
{
    auto *temp = new std::vector<std::pair<KeyT, ValueT> *>[newCapacity];
    for (int i = 0; i < _capacity; i++)
    {
        auto &row = _arr[i];
        for (auto *pair : row)
        {
            temp[(std::hash<KeyT>{}(pair->first) & (newCapacity - 1))].push_back(pair);
        }
        row.clear();
    }
    delete[] _arr;
    _arr = temp;
    _capacity = newCapacity;
}

/**
 * Creates an empty HashMap.
 */
template<typename KeyT, typename ValueT>
HashMap<KeyT, ValueT>::HashMap() noexcept : _size(DEFAULT_SIZE), _capacity(DEFAULT_CAPACITY)
{
    _arr = new std::vector<std::pair<KeyT, ValueT> *>[DEFAULT_CAPACITY];
}

/**
 * Creates a new HashMap from two vectors: one with keys and one with values.
 * The mapping is done by the order of the vectors.
 * The vectors have to be of the same size.
 *
 * @param keys A vector of keys.
 * @param values A vector of values.
 * @throws VectorInputException if vectors aren't of same size.
 */
template<typename KeyT, typename ValueT>
HashMap<KeyT, ValueT>::HashMap(const std::vector<KeyT> &keys, const std::vector<ValueT> &values) :
        _size(keys.size()), _capacity(DEFAULT_CAPACITY)
{
    if (_size != (int)values.size())
    {
        throw VectorInputException();
    }

    while (_capacity <= _size || getLoadFactor() > MAX_LOAD_FACTOR)
    { _capacity *= 2; }

    _arr = new std::vector<std::pair<KeyT, ValueT> *>[_capacity];
    for (int i = 0; i < _size; i++)
    {
        auto *pair = new std::pair<KeyT, ValueT>(keys[i], values[i]);
        auto &row = _arr[_hash(pair->first)];
        if (_deleteValue(pair->first, row)) // If duplicate keys then remove pair and adjust size.
        {
            _size--;
        }
        row.push_back(pair);
    }

}

/**
 * Copy constructor for HashMap.
 *
 * @param other The other HashMap.
 */
template<typename KeyT, typename ValueT>
HashMap<KeyT, ValueT>::HashMap(const HashMap &other) noexcept
{
    _copy(other);
}

/**
 * Destructor for HashMap.
 */
template<typename KeyT, typename ValueT>
HashMap<KeyT, ValueT>::~HashMap() noexcept
{
    clear();
    delete[] _arr;
}

/**
 * Returns true if insertion to this map is successful. Otherwise returns false.
 * Failure to insert happens when key already exists in this map.
 *
 * @param key The key to insert.
 * @param value The value to insert.
 * @return True if insertion to this map is successful. Otherwise returns false.
 */
template<typename KeyT, typename ValueT>
bool HashMap<KeyT, ValueT>::insert(const KeyT &key, const ValueT &value) noexcept
{
    auto &row = _arr[_hash(key)];
    if (_getValue(key, row) != nullptr)
    {
        return false;
    }

    row.push_back(new std::pair<KeyT, ValueT>(key, value));
    _size++;
    if (getLoadFactor() > MAX_LOAD_FACTOR)
    {
        _rehashArray(_capacity * CHANGE_FACTOR);
    }
    return true;
}

/**
 * Returns true if this map contains the given key. Otherwise, returns false.
 *
 * @param key The key to find.
 * @return True if this map contains the given key. Otherwise, returns false.
 */
template<typename KeyT, typename ValueT>
bool HashMap<KeyT, ValueT>::containsKey(const KeyT &key) const noexcept
{
    return _getValue(key, _arr[_hash(key)]) != nullptr;
}

/**
 * Returns the value paired with the given key, if it is in this map.
 * Otherwise, throws exception. (Const version)
 *
 * @param key The key to find.
 * @throws KeyNotFoundException if key isn't in this map.
 * @return The value paired with the given key.
 */
template<typename KeyT, typename ValueT>
const ValueT &HashMap<KeyT, ValueT>::at(const KeyT &key) const
{
    ValueT *value = _getValue(key, _arr[_hash(key)]);
    if (value != nullptr)
    {
        return *value;
    }
    throw KeyNotFoundException();
}

/**
 * Returns the value paired with the given key, if it is in this map.
 * Otherwise, throws exception.
 *
 * @param key The key to find.
 * @throws KeyNotFoundException if key isn't in this map.
 * @return The value paired with the given key.
 */
template<typename KeyT, typename ValueT>
ValueT &HashMap<KeyT, ValueT>::at(const KeyT &key)
{
    ValueT *value = _getValue(key, _arr[_hash(key)]);
    if (value != nullptr)
    {
        return *value;
    }
    throw KeyNotFoundException();
}

/**
 * Returns the value paired with the given key, if it is in this map.
 * Otherwise, undefined behaviour. (Const version)
 *
 * @param key The key to find.
 * @return The value paired with the given key.
 */
template<typename KeyT, typename ValueT>
const ValueT &HashMap<KeyT, ValueT>::operator[](const KeyT &key) const noexcept
{
    ValueT *value = _getValue(key, _arr[_hash(key)]);
    if (value != nullptr)
    {
        return *value;
    }
    return _defaultValue;
}

/**
 * Returns the value paired with the given key, if it is in this map.
 * Otherwise, undefined behaviour.
 *
 * @param key The key to find.
 * @return The value paired with the given key.
 */
template<typename KeyT, typename ValueT>
ValueT &HashMap<KeyT, ValueT>::operator[](const KeyT &key) noexcept
{
    ValueT *value = _getValue(key, _arr[_hash(key)]);
    if (value != nullptr)
    {
        return *value;
    }

    auto *pair = new std::pair<KeyT, ValueT>(key, ValueT());
    _arr[_hash(key)].push_back(pair);
    _size++;
    if (getLoadFactor() > MAX_LOAD_FACTOR)
    {
        _rehashArray(_capacity * CHANGE_FACTOR);
    }
    return pair->second;
}

/**
 * Returns true if given key was found in this map and erases it. Otherwise, returns false.
 *
 * @param key The key to erase.
 * @return True if given key was found in this map and erases it. Otherwise, returns false.
 */
template<typename KeyT, typename ValueT>
bool HashMap<KeyT, ValueT>::erase(const KeyT &key) noexcept
{
    if (_deleteValue(key, _arr[_hash(key)]))
    {
        _size--;
        if ((getLoadFactor() < MIN_LOAD_FACTOR) && (_capacity > MIN_CAPACITY))
        {
            _rehashArray(_capacity / CHANGE_FACTOR);
        }
        return true;
    }
    return false;
}

/**
 * Returns the size of the bucket which contains the given key, if it is in this map.
 * Otherwise, throws exception.
 *
 * @param key The key with which to find the bucket.
 * @throws KeyNotFoundException if key isn't in this map.
 * @return The size of the bucket which contains the given key.
 */
template<typename KeyT, typename ValueT>
int HashMap<KeyT, ValueT>::bucketSize(const KeyT &key) const
{
    auto &row = _arr[_hash(key)];
    if (_getValue(key, row) != nullptr)
    {
        return row.size();
    }
    throw KeyNotFoundException();
}

/**
 * Returns the index of the bucket which contains the given key, if it is in this map.
 * Otherwise, throws exception.
 *
 * @param key The key with which to find the bucket.
 * @throws KeyNotFoundException if key isn't in this map.
 * @return The index of the bucket which contains the given key.
 */
template<typename KeyT, typename ValueT>
int HashMap<KeyT, ValueT>::bucketIndex(const KeyT &key) const
{
    int index = _hash(key);
    if (_getValue(key, _arr[index]) != nullptr)
    {
        return index;
    }
    throw KeyNotFoundException();
}

/**
 * Clears this map from all elements, while not changing the capacity.
 */
template<typename KeyT, typename ValueT>
void HashMap<KeyT, ValueT>::clear() noexcept
{
    for (int i = 0; i < _capacity; i++)
    {
        auto &row = _arr[i];
        for (auto *pair : row)
        {
            delete pair;
        }
        row.clear();
    }
    _size = 0;
}

/**
 * Assignment operator for this map.
 *
 * @param other The map to copy.
 * @return Instance of this map after copying the given one.
 */
template<typename KeyT, typename ValueT>
HashMap<KeyT, ValueT> &HashMap<KeyT, ValueT>::operator=(const HashMap<KeyT, ValueT> &other) noexcept
{
    if (_arr != other._arr) // if not same object.
    {
        clear();
        delete[] _arr;
        _copy(other);
    }
    return *this;
}

/**
 * Returns true if both maps contain equal elements. Otherwise, returns false.
 *
 * @param other The map to compare too.
 * @return True if both maps contain equal elements. Otherwise, returns false.
 */
template<typename KeyT, typename ValueT>
bool HashMap<KeyT, ValueT>::operator==(const HashMap &other) const noexcept
{
    if (_size != other._size)
    {
        return false;
    }

    for (int i = 0; i < _capacity; i++)
    {
        auto &row = _arr[i];
        for (auto *pair : row)
        {
            if (!other.containsKey(pair->first) || other[pair->first] != pair->second)
            {
                return false;
            }
        }
    }
    return true;
}

/**
 * Creates new iterator from within instance of HashMap.
 *
 * @param arr The HashMaps array.
 * @param capacity The HashMaps capacity.
 * @param begin if true then starts at start. Otherwise at end of iterator.
 */
template<typename KeyT, typename ValueT>
HashMap<KeyT, ValueT>::const_iterator::const_iterator(HashRow *arr, int capacity, bool begin) noexcept :
        _j(0), _capacity(capacity), _arr(arr)
{
    if (begin)
    {
        _i = 0;
        while (_arr[_i].empty() && (++_i < _capacity)); // Skip to first valid.
    }
    else
    {
        _i = capacity;
    }
}

/**
 * -> operator for iterator.
 *
 * @throws OutOfRangeException if iterator has gone out of valid range.
 * @return address f of pair to be used in -> operation.
 */
template<typename KeyT, typename ValueT>
const std::pair<KeyT, ValueT> *HashMap<KeyT, ValueT>::const_iterator::operator->() const
{
    if (_i < _capacity)
    {
        return _arr[_i][_j];
    }
    throw OutOfRangeException();
}

/**
 * Advances to operator by 1 and returns instance of this iterator after advancement.
 *
 * @return Instance of this iterator after advancement.
 */
template<typename KeyT, typename ValueT>
typename HashMap<KeyT, ValueT>::const_iterator &
HashMap<KeyT, ValueT>::const_iterator::operator++() noexcept
{
    if (_i < _capacity)
    {
        if (++_j >= (int)_arr[_i].size())
        {
            _j = 0;
            while ((++_i < _capacity) && _arr[_i].empty());
        }
    }
    return *this;
}

/**
 * Advances to operator by 1 and returns copy of this iterator before advancement.
 *
 * @return Copy of this iterator before advancement.
 */
template<typename KeyT, typename ValueT>
const typename HashMap<KeyT, ValueT>::const_iterator
HashMap<KeyT, ValueT>::const_iterator::operator++(int) noexcept
{
    const const_iterator temp(*this);
    ++*this;
    return temp;
}

/**
 * Assignment operator for iterator.
 *
 * @param other The other iterator.
 * @return Instance of this iterator after assignment.
 */
template<typename KeyT, typename ValueT>
typename HashMap<KeyT, ValueT>::const_iterator &
HashMap<KeyT, ValueT>::const_iterator::operator=(
        const HashMap<KeyT, ValueT>::const_iterator &other) noexcept
{
    if (*this != other)
    {
        _arr = other._arr;
        _i = other._i;
        _j = other._j;
        _capacity = other._capacity;
    }
    return *this;
}

#endif //SPAMDETECTOR_HASHMAP_HPP
