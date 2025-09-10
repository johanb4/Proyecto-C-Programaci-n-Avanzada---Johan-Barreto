CXX=g++
CXXFLAGS=-std=c++11 -Wall -O2 -I.

all: app

app: src/main.cpp src/io.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f app
