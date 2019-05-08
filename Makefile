FIRE_CXX ?= clang++
FIRE_CXXFLAGS ?= -O3 -std=gnu++2a -Wall -Werror
FIRE_LDLIBS ?= -lgflags -lglog -lpthread

all: firesse.a example_clock

objects = firesse.o stream.o

_firecgi:
	$(MAKE) --directory=firecgi

firesse.a: $(objects) _firecgi
	ar x firecgi/firecgi.a
	ar rcs $@ $(objects) $(shell ar t firecgi/firecgi.a)

example_clock: example_clock.o firesse.a
	$(FIRE_CXX) $(FIRE_CXXFLAGS) -o $@ $+ $(FIRE_LDLIBS)

%.o: %.cc *.h Makefile
	$(FIRE_CXX) $(FIRE_CXXFLAGS) -c -o $@ $<

clean:
	$(MAKE) --directory=firecgi clean
	rm --force example_clock *.o *.a

asan:
	$(MAKE) clean
	FIRE_CXXFLAGS="-O1 -g -fsanitize=address -fno-omit-frame-pointer -std=gnu++2a -Wall -Werror" $(MAKE) all
