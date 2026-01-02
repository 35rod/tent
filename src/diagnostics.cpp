#include "diagnostics.hpp"

#include <iostream>
#include <sstream>

bool Diagnostics::has_errors() const {
    return !errors.empty();
}

void Diagnostics::print_errors() const {
    for (const auto& err : errors) {
        std::cerr << format(*err) << "\n";
    }
    std::cerr.flush();
}

std::string Diagnostics::format(const Error& err) const {
    std::ostringstream out;
    const Span& span = err.span;
    const std::string_view& lineText = span.getLineText();
    const std::string& filename = err.filename;

    out << BOLD << RED << "[" << err.getClassName() << "]" << RESET << " "
        << RED << err.message << RESET << "\n";

    if (span.getLineNum() != 0) {
        out << GRAY << "  --> ";
        if (!filename.empty()) out << filename << ":";
        out << span.getLineNum();
        if (span.getStartCol() != 0) out << ":" << span.getStartCol();
        out << RESET << "\n";
    }

    if (!lineText.empty()) {
        const std::string lineNoStr = std::to_string(span.getLineNum());
        int paddingWidth = lineNoStr.length();

        out << GRAY << std::string(paddingWidth, ' ') << " |" << RESET << "\n";
        out << BOLD << lineNoStr << RESET << GRAY << " | " << RESET << lineText << "\n";

        if (span.getStartCol() != 0 && span.getEndCol() >= span.getStartCol()) {
            out << GRAY << std::string(paddingWidth, ' ') << " | " << RESET;

            for (size_t i = 0; i < span.getStartCol()-2; ++i) {
                out << (i < lineText.size() && lineText[i] == '\t' ? '\t' : ' ');
            }

            out << BOLD << RED << std::string(span.getEndCol() - span.getStartCol() + 1, '^');
            out << RESET << "\n";
        }

        out << GRAY << std::string(paddingWidth, ' ') << " |" << RESET << "\n";
    }

    if (!err.hint.empty()) {
        out << YELLOW << "Hint: " << RESET << err.hint << "\n";
    }

    return out.str();
}
