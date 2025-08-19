#pragma once

#include <iostream>

class Error {
    protected:
        std::string message;
        int lineNo;
    
    public:
        virtual std::string getClassName() const;
        void printException();

        Error(std::string errorMessage, int line);
};

class SyntaxError : public Error {
    public:
        SyntaxError(std::string errorMessage, int line);
};

class MissingTerminatorError : public Error {
    public:
        MissingTerminatorError(std::string errorMessage, int line);
};

class IdentifierError : public Error {
    public:
        IdentifierError(std::string erorrMessage, int line);
};