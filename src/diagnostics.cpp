#include "diagnostics.hpp"

#include <iostream>
#include <sstream>

bool Diagnostics::has_errors() const { return !errors.empty(); }

void Diagnostics::print_errors() const {
  for (const auto &err : errors) {
    std::cerr << format(*err) << "\n";
  }
  std::cerr.flush();
}

std::string Diagnostics::format(const Error &err) const {
  std::ostringstream out;
  const Span &span = err.span;
  const std::string_view &lineText = span.getLineText();
  const std::string &filename = err.filename;

  out << BOLD << RED << "[" << err.getClassName() << "]" << RESET << " " << RED
      << err.message << RESET << "\n";

  if (!err.traceback.empty()) {
    out << CYAN << "Traceback (most recent call last):" << RESET << "\n";
    for (const TracebackFrame &frame : err.traceback) {
      out << GRAY << "  --> ";
      if (!frame.filename.empty()) {
        out << frame.filename;
      }
      if (frame.span.getLineNum() != 0) {
        out << ":" << frame.span.getLineNum();
        if (frame.span.getStartCol() != 0) {
          out << ":" << frame.span.getStartCol();
        }
      }
      out << RESET;

      if (!frame.label.empty()) {
        out << " in " << BOLD << frame.label << RESET;
      }

      out << "\n";
    }
  }

  if (span.getLineNum() != 0) {
    out << GRAY << "  --> ";
    if (!filename.empty())
      out << filename << ":";
    out << span.getLineNum();
    if (span.getStartCol() != 0)
      out << ":" << span.getStartCol();
    out << RESET << "\n";
  }

  if (!lineText.empty()) {
    const std::string lineNoStr = std::to_string(span.getLineNum());
    int paddingWidth = lineNoStr.length();

    out << GRAY << std::string(paddingWidth, ' ') << " |" << RESET << "\n";
    out << BOLD << lineNoStr << RESET << GRAY << " | " << RESET << lineText
        << "\n";

    if (span.getStartCol() != 0 && span.getEndCol() >= span.getStartCol()) {
      out << GRAY << std::string(paddingWidth, ' ') << " | " << RESET;

      if (span.getStartCol() > 0) {
        for (size_t i = 0; i < span.getStartCol() - 1; ++i) {
          if (i < lineText.size() && lineText[i] == '\t') {
            out << '\t';
          } else {
            out << ' ';
          }
        }
      }

      out << BOLD << RED
          << std::string(span.getEndCol() - span.getStartCol() + 1, '^');
      out << RESET << "\n";
    }

    out << GRAY << std::string(paddingWidth, ' ') << " |" << RESET << "\n";
  }

  if (!err.hint.empty()) {
    out << YELLOW << "Hint: " << RESET << err.hint << "\n";
  }

  return out.str();
}
