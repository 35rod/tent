BUILD := build
OBJ := obj

CFLAGS := -Wall -Wextra -pedantic -Wshadow
OBJLIST := $(OBJ)/main.o $(OBJ)/lexer.o $(OBJ)/token.o $(OBJ)/parser.o $(OBJ)/ast.o $(OBJ)/evaluator.o

all: $(BUILD)/main

clean:
	rm obj/* build/*

$(BUILD):
	mkdir $(BUILD)
	
$(OBJ):
	mkdir $(OBJ)

$(BUILD)/main: $(OBJLIST) $(BUILD)
	g++ -o $(BUILD)/main $(OBJLIST)

$(OBJ)/lexer.o: lexer.cpp lexer.hpp $(OBJ)
	g++ -o $(OBJ)/lexer.o -c lexer.cpp $(CFLAGS)

$(OBJ)/token.o: token.cpp token.hpp $(OBJ)
	g++ -o $(OBJ)/token.o -c token.cpp $(CFLAGS)

$(OBJ)/parser.o: parser.cpp parser.hpp token.hpp ast.hpp $(OBJ)
	g++ -o $(OBJ)/parser.o -c parser.cpp $(CFLAGS)

$(OBJ)/ast.o: ast.cpp ast.hpp $(OBJ)
	g++ -o $(OBJ)/ast.o -c ast.cpp $(CFLAGS)

$(OBJ)/evaluator.o: evaluator.cpp evaluator.hpp ast.hpp $(OBJ)
	g++ -o $(OBJ)/evaluator.o -c evaluator.cpp $(CFLAGS)

$(OBJ)/main.o: main.cpp lexer.hpp parser.hpp $(OBJ)
	g++ -o $(OBJ)/main.o -c main.cpp $(CFLAGS)
