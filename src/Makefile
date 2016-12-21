BINARIES = aad-pam test
SOURCES = main.cpp jwt.cpp utils.cpp
TARGETS = $(BINARIES)

LINK.o = g++

CFLAGS += -Werror

CXXFLAGS += $(CFLAGS)

.PHONY : clean

all: $(TARGETS)

test: clean
	g++ aad-test.cpp jwt.cpp utils.cpp connection.cpp helpers.cpp restclient.cpp -lcryptopp -lpthread -lpam -lcurl -lpam_misc $(CXXFLAGS) -I ../include/ -std=c++11 -o test-aad.out

aad-pam: clean
	g++ -fPIC -fno-stack-protector -c aad-pam.cpp jwt.cpp utils.cpp connection.cpp helpers.cpp /usr/lib/librestclient-cpp.a /usr/lib/libcryptopp.a /usr/lib/x86_64-linux-gnu/libpthread.a /usr/lib/x86_64-linux-gnu/libpam.a /usr/lib/x86_64-linux-gnu/libpam_misc.a /usr/lib/x86_64-linux-gnu/libcurl.a $(CXXFLAGS) -I ../include/ -std=c++11

clean: 
	rm -f *.exe* *.o $(TARGETS) *.bak *~

install:
