#include "errors.hpp"

#include <iostream>
#include <sstream>

Error::Error(
    std::string msg,
    int line,
    int col,
    std::string file,
    std::string hintMsg,
    std::string lineSrc
) : message(msg), filename(file), hint(hintMsg), lineText(lineSrc), lineNo(line), colNo(col) {}

std::string Error::getClassName() const {
    return "Error";
}

std::string Error::format() const {
    std::ostringstream out;

    out << BOLD << RED << "[" << getClassName() << "]" << RESET  << " "
        << RED << message << RESET << "\n";
    
    if (lineNo >= 0) {
        out << GRAY << "  --> ";
        if (!filename.empty()) out << filename << ":";
        out << lineNo;
        if (colNo >= 0) out << ":" << colNo;
        out << RESET << "\n";
    }

    if (!lineText.empty()) {
        std::string lineNoStr = std::to_string(lineNo);
        int paddingWidth = lineNoStr.length();

        out << GRAY << std::string(paddingWidth, ' ') << " |" << RESET << "\n";
        out << BOLD << lineNoStr << RESET << GRAY << " | " << RESET << lineText << "\n";

        int arrowCol = std::min(colNo, (int)lineText.size() + 1);

        if (arrowCol > 0) {
            out << GRAY << std::string(paddingWidth, ' ') << " | " << RESET;
            
            for (int i = 0; i < arrowCol - 1; i++) {
                out << (lineText[i] == '\t' ? '\t' : ' ');
            }

            out << BOLD << RED << "^" << RESET << "\n";
        }

        out << GRAY << std::string(paddingWidth, ' ') << " |" << RESET << "\n";
    }

    if (!hint.empty()) {
        out << YELLOW << "Hint: " << RESET << hint << "\n";
    }

    return out.str();
}

void Error::print() const {
    std::cerr << format() << std::endl;
}

const char* Error::what() const noexcept {
    return message.c_str();
}

void Error::setHint(const std::string& h) {
    hint = h;
}

SyntaxError::SyntaxError(
    std::string msg,
    int line,
    int col,
    std::string file,
    std::string hintMsg,
    std::string lineSrc
) : Error(msg, line, col, file, hintMsg, lineSrc) {}

std::string SyntaxError::getClassName() const {
    return "SyntaxError";
}

MissingTerminatorError::MissingTerminatorError(
    std::string msg,
    int line,
    int col,
    std::string file,
    std::string hintMsg,
    std::string lineSrc
) : SyntaxError(msg, line, col, file, hintMsg, lineSrc) {}

std::string MissingTerminatorError::getClassName() const {
    return "MissingTerminatorError";
}

IdentifierError::IdentifierError(
    std::string msg,
    int line,
    int col,
    std::string file,
    std::string hintMsg,
    std::string lineSrc
) : Error(msg, line, col, file, hintMsg, lineSrc) {}

std::string IdentifierError::getClassName() const {
    return "IdentifierError";
}

TypeError::TypeError(
    std::string msg,
    int line,
    int col,
    std::string file,
    std::string hintMsg,
    std::string lineSrc
) : Error(msg, line, col, file, hintMsg, lineSrc) {}

std::string TypeError::getClassName() const {
    return "TypeError";
}
