#pragma once

#include "span.hpp"
#include <iostream>

struct Value;
class ASTVisitor;

inline void printIndent(int indent) { printf("%*s", indent, " "); }

class ASTNode {
protected:
  Span span;

public:
  ASTNode(Span s) : span(s) {}

  virtual void print(int indent) {
    printIndent(indent);
    std::cout << "ASTNode()" << std::endl;
  }

  virtual Value accept(ASTVisitor &visitor) = 0;

  friend class Evaluator;

  virtual ~ASTNode() = default;
};

class NullLiteral {
public:
  NullLiteral() {}

  void print(int indent) {
    printIndent(indent);
    std::cout << "NullLiteral()\n";
  }
};

class NoOp : public ASTNode {
public:
  NoOp() : ASTNode(Span()) {}

  void print(int indent) override {
    printIndent(indent);
    std::cout << "NoOp()\n";
  }

  Value accept(ASTVisitor &visitor) override;
};
