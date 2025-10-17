#pragma once
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

class Error : public std::exception {
    protected:
        std::string message;
        std::string filename;
        std::string hint;
        std::string lineText;
        int lineNo;
        int colNo;

        static constexpr const char* RED = "\033[31m";
        static constexpr const char* YELLOW = "\033[33m";
        static constexpr const char* CYAN = "\033[36m";
        static constexpr const char* GRAY = "\033[90m";
        static constexpr const char* RESET = "\033[0m";
        static constexpr const char* BOLD = "\033[1m";
    public:
        Error(
            std::string msg, 
            int line = -1,
            int col = -1,
            std::string file = "",
            std::string hintMsg = "",
            std::string lineSrc = ""
        );

        virtual std::string getClassName() const;
        virtual std::string format() const;
        virtual void print() const;
        const char* what() const noexcept override;
        void setHint(const std::string& h);
        virtual ~Error() = default;
};

class SyntaxError : public Error {
    public:
        SyntaxError(
            std::string msg,
            int line = -1,
            int col = -1,
            std::string file = "",
            std::string hintMsg = "",
            std::string lineSrc = ""
        );

        std::string getClassName() const override;
};

class MissingTerminatorError : public SyntaxError {
    public:
        MissingTerminatorError(
            std::string msg,
            int line = -1,
            int col = -1,
            std::string file = "",
            std::string hintMsg = "",
            std::string lineSrc = ""
        );

        std::string getClassName() const override;
};

class IdentifierError : public Error {
    public:
        IdentifierError(
            std::string msg,
            int line = -1,
            int col = -1,
            std::string file = "",
            std::string hintMsg = "",
            std::string lineSrc = ""
        );

    std::string getClassName() const override;
};

class TypeError : public Error {
    public:
        TypeError(
            std::string msg,
            int line = -1,
            int col = -1,
            std::string file = "",
            std::string hintMsg = "",
            std::string lineSrc = ""
        );

        std::string getClassName() const override;
};