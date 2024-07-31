CXX ?= clang

all: pridecat

pridecat: main.cpp
	$(CXX) main.cpp -o pridecat -std=c++17 -lstdc++ -Wall -Wextra -O3

install: pridecat
	cp pridecat /usr/local/bin/pridecat

uninstall:
	rm -f /usr/local/bin/pridecat

clean:
	rm -f pridecat
