#pragma once

#include <string>
#include <algorithm>

class Span {
	int lineNum;
	int startCol;
	int endCol;
	std::string lineText;

	public:
		Span(int line, int start, int end, const std::string& text)
		: lineNum(line), startCol(start), endCol(end), lineText(text) {}

		Span() : lineNum(0), startCol(0), endCol(0) {}

		static Span combine(const Span& a, const Span& b) {
			int start = std::min(a.startCol, b.startCol);
			int end = std::max(a.endCol, b.endCol);

			return Span(a.lineNum, start, end, a.lineText);
		}

		int getLineNum() const { return lineNum; }
		int getStartCol() const { return startCol; }
		int getEndCol() const { return endCol; }
		const std::string& getLineText() const { return lineText; }

		Span& setEndCol(int col) {
			this->endCol = col;
			return *this;
		}
};