#pragma once

#include <cstdint>
#include <string>
#include <vector>

#define BIT(n) (1 << n)
enum {
  DEBUG = BIT(0),
  DRY_RUN = BIT(1),
  COMPILE = BIT(2),
  REPL = BIT(3),
  SAVE_TEMPS = BIT(4),
};

struct Config {
  uint64_t runtime_flags = 0;
  std::string src_filename;
  std::string out_filename;
  std::string prog_name;
  std::vector<std::string> prog_args;
  std::string system_compiler;
  std::vector<std::string> search_dirs;

  bool is_flag_set(uint64_t flag) const {
    return (runtime_flags & flag) != 0;
  }

  void set_flag(uint64_t flag) {
    runtime_flags |= flag;
  }
};
