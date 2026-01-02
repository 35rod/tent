#pragma once

#include <vector>
#include <memory>

#include "errors.hpp"

static constexpr const char* RED = "\033[31m";
static constexpr const char* YELLOW = "\033[33m";
static constexpr const char* CYAN = "\033[36m";
static constexpr const char* GRAY = "\033[90m";
static constexpr const char* RESET = "\033[0m";
static constexpr const char* BOLD = "\033[1m";

class Diagnostics {
	public:
		bool has_errors() const;
		void print_errors() const;
		std::string format(const Error& err) const;

		std::vector<std::unique_ptr<Error>> errors;

		template <typename T, typename... Args>
		void report(Args&&... args) {
			errors.push_back(std::make_unique<T>(std::forward<Args>(args)...));
		}
};
