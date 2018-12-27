COMPILER=g++
CFLAGS=-Wall
EXECUTABLE=sets_test

SOURCES=main.cpp lazySync.h set.h fineGrainedSync.h

all: $(SOURCES)
	$(COMPILER) $(CFLAGS) -o $(EXECUTABLE) $(SOURCES) -pthread
clean:
	rm -f *.o $(EXECUTABLE)
