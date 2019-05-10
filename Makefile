FIRE_CXX ?= clang++
FIRE_CXXFLAGS ?= -O3 -std=gnu++2a -Wall -Werror -Wextra -fPIE -fPIC -fstack-protector-strong -fsanitize=safe-stack -fsanitize=safe-stack
FIRE_LDFLAGS ?= -fuse-ld=gold -flto -Wl,-z,relro -Wl,-z,now
FIRE_LDLIBS ?= -lgflags -lglog -lpthread

all: firesse.a firesse.o firesse.so example_clock

objects = server.o stream.o

firecgi/firecgi.o:
	$(MAKE) --directory=firecgi firecgi.o

firesse.a: $(objects)
	ar rcs $@ $^

firesse.o: $(objects) firecgi/firecgi.o
	gold -z relro -z now -r --output=$@ $+

firesse.so: $(objects) firecgi/firecgi.o
	$(FIRE_CXX) $(FIRE_CXXFLAGS) $(FIRE_LDFLAGS) -shared -o $@ $+ $(FIRE_LDFLIBS)

example_clock: example_clock.o firesse.o
	$(FIRE_CXX) $(FIRE_CXXFLAGS) $(FIRE_LDFLAGS) -pie -o $@ $+ $(FIRE_LDLIBS)

%.o: %.cc *.h Makefile
	$(FIRE_CXX) $(FIRE_CXXFLAGS) -c -o $@ $<

clean:
	$(MAKE) --directory=firecgi clean
	rm --force example_clock *.so *.o *.a

asan:
	$(MAKE) clean
	FIRE_CXXFLAGS="-O1 -g -fsanitize=address -fno-omit-frame-pointer -std=gnu++2a -fPIE -fPIC" $(MAKE) all

tsan:
	$(MAKE) clean
	FIRE_CXXFLAGS="-O1 -g -fsanitize=thread -std=gnu++2a -fPIE -fPIC" $(MAKE) all

ubsan:
	$(MAKE) clean
	FIRE_CXXFLAGS="-O1 -g -fsanitize=undefined -std=gnu++2a -fPIE -fPIC" $(MAKE) all
