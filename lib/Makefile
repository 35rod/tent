CXX := g++
CXXFLAGS := -std=c++17 -fPIC -Wall -Wextra -Wpedantic
INCLUDES := -I../include

SRC := $(wildcard *.cpp)
SO := $(SRC:.cpp=.so)

all: $(SO)

%.so: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -shared -o $@ $<

clean:
	rm -f $(SO)