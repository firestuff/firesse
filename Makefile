FIRE_CXX ?= clang++
FIRE_CXXFLAGS ?= -O3 -std=gnu++2a -Wall -Werror
FIRE_LDLIBS ?= -lgflags -lglog -lpthread

all: firesse.a firesse.o example_clock

objects = server.o stream.o

firecgi/firecgi.o:
	$(MAKE) --directory=firecgi

firesse.a: $(objects)
	ar rcs $@ $^

firesse.o: $(objects) firecgi/firecgi.o
	ld --relocatable --output=$@ $+

example_clock: example_clock.o firesse.o
	$(FIRE_CXX) $(FIRE_CXXFLAGS) -o $@ $+ $(FIRE_LDLIBS)

%.o: %.cc *.h Makefile
	$(FIRE_CXX) $(FIRE_CXXFLAGS) -c -o $@ $<

clean:
	$(MAKE) --directory=firecgi clean
	rm --force example_clock *.o *.a

asan:
	$(MAKE) clean
	FIRE_CXXFLAGS="-O1 -g -fsanitize=address -fno-omit-frame-pointer -std=gnu++2a -Wall -Werror" $(MAKE) all
