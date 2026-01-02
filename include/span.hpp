#pragma once

#include <algorithm>
#include <string_view>

class Span {
    // numbers are one-indexed, and non-zero unless unknown
    size_t lineNum;
    size_t startCol;
    size_t endCol; // inclusive
    std::string_view lineText; // view of the entire text of the line, not just the relevant bit

    public:
        Span(size_t line, size_t start, size_t end, std::string_view text)
        : lineNum(line), startCol(start), endCol(end), lineText(text) {}

        Span() : lineNum(0), startCol(0), endCol(0) {}

        static Span combine(const Span& a, const Span& b) {
            size_t start = std::min(a.startCol, b.startCol);
            size_t end = std::max(a.endCol, b.endCol);

            return Span(a.lineNum, start, end, a.lineText);
        }

        size_t getLineNum() const { return lineNum; }
        size_t getStartCol() const { return startCol; }
        size_t getEndCol() const { return endCol; }
        const std::string_view& getLineText() const { return lineText; }

        Span& setEndCol(size_t col) {
            this->endCol = col;
            return *this;
        }
};
