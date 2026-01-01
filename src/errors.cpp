#include "errors.hpp"

#include <iostream>
#include <sstream>

Error::Error(
    std::string msg, 
    Span s,
    std::string hintMsg,
    std::string fname
) : message(msg), span(s), hint(hintMsg), filename(fname) {}

std::string Error::getClassName() const {
    return "Error";
}

SyntaxError::SyntaxError(
    std::string msg, 
    Span s,
    std::string hintMsg,
    std::string fname
) : Error(msg, s, hintMsg, fname) {}

std::string SyntaxError::getClassName() const {
    return "SyntaxError";
}

MissingTerminatorError::MissingTerminatorError(
    std::string msg, 
    Span s,
    std::string hintMsg,
    std::string fname
) : Error(msg, s, hintMsg, fname) {}

std::string MissingTerminatorError::getClassName() const {
    return "MissingTerminatorError";
}

IdentifierError::IdentifierError(
    std::string msg, 
    Span s,
    std::string hintMsg,
    std::string fname
) : Error(msg, s, hintMsg, fname) {}

std::string IdentifierError::getClassName() const {
    return "IdentifierError";
}

TypeError::TypeError(
    std::string msg, 
    Span s,
    std::string hintMsg,
    std::string fname
) : Error(msg, s, hintMsg, fname) {}

std::string TypeError::getClassName() const {
    return "TypeError";
}
