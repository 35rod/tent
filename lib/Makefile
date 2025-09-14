CXX := g++
CXXFLAGS := -std=c++17 -fPIC -Wall -Wextra -Wpedantic
BREW_PREFIX := $(shell brew --prefix)
INCLUDES := -I../include -I$(BREW_PREFIX)/include
LIBS := -L$(BREW_PREFIX)/lib -lfltk
RPATH := -Wl,-rpath,$(BREW_PREFIX)/lib

SRC := $(wildcard *.cpp)
SO := $(SRC:.cpp=.so)

all: $(SO)

%.so: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -shared $(RPATH) -o $@ $< $(LIBS)

clean:
	rm -f $(SO)