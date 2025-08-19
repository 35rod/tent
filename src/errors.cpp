#include "errors.hpp"
#include "util-log.hpp"

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
        errlogf(1, "%s on unknown line:\n"
                   "%8s%s\n",
            getClassName().c_str(),
            "", message.c_str());
    
    errlogf(1, "%s on line %d:\n"
               "%8s%s\n",
        getClassName().c_str(), lineNo,
		"", message.c_str());
}

SyntaxError::SyntaxError(std::string errorMessage, int line) : Error(errorMessage, line) {}

MissingTerminatorError::MissingTerminatorError(std::string errorMessage, int line) : Error(errorMessage, line) {}

IdentifierError::IdentifierError(std::string errorMessage, int line) : Error(errorMessage, line) {}

TypeError::TypeError(std::string errorMessage, int line) : Error(errorMessage, line) {}
