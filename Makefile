CXX ?= clang

all: pridecat

pridecat: main.cpp
	$(CXX) main.cpp -o pridecat -std=c++11 -lstdc++ -Wall -Wextra -O3 -s

install: pridecat
	cp pridecat /usr/local/bin/pridecat

uninstall:
	rm -f /usr/local/bin/pridecat

clean:
	rm -f pridecat
