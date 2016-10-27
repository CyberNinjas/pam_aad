BINARIES = aad-pam
SOURCES = main.cpp jwt.cpp utils.cpp
TARGETS = $(BINARIES)

LINK.o = g++

CFLAGS += -Werror -pedantic

CXXFLAGS += $(CFLAGS)

.PHONY : clean

all: $(TARGETS)

aad-pam: clean
	g++ main.cpp jwt.cpp utils.cpp -lcurl -lrestclient-cpp -lcryptopp -lpthread $(CXXFLAGS) -I ../include/ -std=c++11 -o aad.o

tests: clean
	g++ -o tests main.cpp jwt.cpp utils.cpp tests.cpp -lcryptopp -lpthread -lunittest++ $(CXXFLAGS) -I ../include/ -std=c++11

clean: 
	rm -f *.exe* *.o $(TARGETS) *.bak *~
