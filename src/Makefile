BINARIES = aad-pam test
SOURCES = main.cpp jwt.cpp utils.cpp
TARGETS = $(BINARIES)

LINK.o = g++

CFLAGS += -Werror -pedantic

CXXFLAGS += $(CFLAGS)

.PHONY : clean

all: $(TARGETS)

test: clean
	g++ aad-test.cpp jwt.cpp utils.cpp connection.cpp helpers.cpp restclient.cpp -lcurl -lcryptopp -lpthread -lpam -lpam_misc $(CXXFLAGS) -I ../include/ -std=c++11 -o test-aad.out

aad-pam: clean
	g++ -fPIC -fno-stack-protector -c aad-pam.cpp jwt.cpp utils.cpp connection.cpp helpers.cpp restclient.cpp -lcurl -lcryptopp -lpthread $(CXXFLAGS) -I ../include/ -std=c++11

clean: 
	rm -f *.exe* *.o $(TARGETS) *.bak *~

install:
