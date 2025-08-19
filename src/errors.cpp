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
    std::cout << "ERROR: Line " << lineNo << "," << std::endl;
    std::cout << getClassName() << ": " << message << std::endl;

    exit(1);
}

SyntaxError::SyntaxError(std::string errorMessage, int line) : Error(errorMessage, line) {}

MissingTerminatorError::MissingTerminatorError(std::string errorMessage, int line) : Error(errorMessage, line) {}

IdentifierError::IdentifierError(std::string errorMessage, int line) : Error(errorMessage, line) {}