#include "errors.hpp"

#include <utility>

TracebackFrame::TracebackFrame(
    std::string frameLabel,
    Span frameSpan,
    std::string frameFilename
) : label(frameLabel), span(frameSpan), filename(frameFilename) {}

Error::Error(
    std::string msg,
    Span s,
    std::string hintMsg,
    std::string fname,
    std::vector<TracebackFrame> tb
) : message(msg), span(s), hint(hintMsg), filename(fname), traceback(std::move(tb)) {}

std::string Error::getClassName() const {
    return "Error";
}

SyntaxError::SyntaxError(
    std::string msg,
    Span s,
    std::string hintMsg,
    std::string fname
) : Error(msg, s, hintMsg, fname) {}

std::string SyntaxError::getClassName() const {
    return "SyntaxError";
}

MissingTerminatorError::MissingTerminatorError(
    std::string msg,
    Span s,
    std::string hintMsg,
    std::string fname
) : Error(msg, s, hintMsg, fname) {}

std::string MissingTerminatorError::getClassName() const {
    return "MissingTerminatorError";
}

IdentifierError::IdentifierError(
    std::string msg,
    Span s,
    std::string hintMsg,
    std::string fname
) : Error(msg, s, hintMsg, fname) {}

std::string IdentifierError::getClassName() const {
    return "IdentifierError";
}

TypeError::TypeError(
    std::string msg,
    Span s,
    std::string hintMsg,
    std::string fname
) : Error(msg, s, hintMsg, fname) {}

std::string TypeError::getClassName() const {
    return "TypeError";
}

RuntimeError::RuntimeError(
    std::string msg,
    Span s,
    std::string hintMsg,
    std::string fname,
    std::vector<TracebackFrame> tb
) : Error(msg, s, hintMsg, fname, std::move(tb)) {}

std::string RuntimeError::getClassName() const {
    return "RuntimeError";
}
