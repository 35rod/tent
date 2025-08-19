#include "errors.hpp"

Error::Error(std::string errorMessage, int line) : message(errorMessage), lineNo(line) {
    printException();
}

std::string Error::getClassName() const {
    std::string res = typeid(*this).name();
    res.erase(0, 1);

    return res;
};

void Error::printException() {
    if (lineNo == -1)
        std::cerr << "\x1b[38;5;9m[ERROR] " + getClassName() + " on unknown line:\n"
                  << "        " + message + "\x1b[0m" << std::endl;
    else
        std::cerr << "\x1b[38;5;9m[ERROR] " + getClassName() + " on line " << lineNo << ":\n"
                  << "        " + message + "\x1b[0m" << std::endl;
    exit(1);
}

SyntaxError::SyntaxError(std::string errorMessage, int line) : Error(errorMessage, line) {}

MissingTerminatorError::MissingTerminatorError(std::string errorMessage, int line) : Error(errorMessage, line) {}

IdentifierError::IdentifierError(std::string errorMessage, int line) : Error(errorMessage, line) {}

TypeError::TypeError(std::string errorMessage, int line) : Error(errorMessage, line) {}
