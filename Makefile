CC = g++
CCFLAGS = -c -Wall -std=c++14
LDFLAGS = -lm -L/usr/lib/ -l boost_system -l boost_filesystem

CLASSES = SpamDetector

OBJS = $(patsubst %, %.o,  $(CLASSES))

SpamDetector: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o SpamDetector

%.o: %.cpp
	$(CC) $(CCFLAGS) $*.cpp

depend:
	makedepend -- $(CCFLAGS) -- $(SRCS)

clean:
	rm -rf *.o