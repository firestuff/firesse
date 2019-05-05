FIRE_CXX ?= clang++
FIRE_CXXFLAGS ?= -O3 -std=gnu++2a -Wall -Werror
FIRE_LDLIBS ?= -lgflags -lglog -lpthread

all: firesse.a example_clock

objects = firesse.o stream.o

firesse.a: $(objects)
	$(MAKE) --directory=firecgi
	ar rcs $@ $^

example_clock: example_clock.o $(objects)
	$(MAKE) --directory=firecgi
	$(FIRE_CXX) $(FIRE_CXXFLAGS) -o $@ $+ firecgi/firecgi.a firecgi/firebuf/firebuf.a $(FIRE_LDLIBS)

%.o: %.cc *.h Makefile
	$(FIRE_CXX) $(FIRE_CXXFLAGS) -c -o $@ $<

clean:
	$(MAKE) --directory=firecgi clean
	rm --force example_clock *.o *.a
