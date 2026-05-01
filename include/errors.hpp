#pragma once

#include <string>
#include <vector>

#include "span.hpp"

struct TracebackFrame {
  std::string label;
  Span span;
  std::string filename;

  TracebackFrame(std::string frameLabel, Span frameSpan,
                 std::string frameFilename = "<stdin>");
};

class Error {
protected:
  std::string message;
  Span span;
  std::string hint;
  std::string filename;
  std::vector<TracebackFrame> traceback;

public:
  Error(std::string msg, Span s, std::string hintMsg = "",
        std::string fname = "<stdin>", std::vector<TracebackFrame> tb = {});

  virtual std::string getClassName() const;
  virtual ~Error() = default;

  friend class Diagnostics;
};

class SyntaxError : public Error {
public:
  SyntaxError(std::string msg, Span s, std::string hintMsg = "",
              std::string fname = "<stdin>");

  std::string getClassName() const override;
};

class MissingTerminatorError : public Error {
public:
  MissingTerminatorError(std::string msg, Span s, std::string hintMsg = "",
                         std::string fname = "<stdin>");

  std::string getClassName() const override;
};

class IdentifierError : public Error {
public:
  IdentifierError(std::string msg, Span s, std::string hintMsg = "",
                  std::string fname = "<stdin>");

  std::string getClassName() const override;
};

class TypeError : public Error {
public:
  TypeError(std::string msg, Span s, std::string hintMsg = "",
            std::string fname = "<stdin>");

  std::string getClassName() const override;
};

class RuntimeError : public Error {
public:
  RuntimeError(std::string msg, Span s, std::string hintMsg = "",
               std::string fname = "<stdin>",
               std::vector<TracebackFrame> tb = {});

  std::string getClassName() const override;
};
