// tent_build_native.cpp
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include "native.hpp"

struct BuildTarget {
    std::string name;
    std::vector<std::string> sources;
    std::vector<std::string> include_paths;
    std::vector<std::string> flags;
    std::vector<std::string> linked_libs;
    bool is_executable = false;
};

static std::vector<BuildTarget> targets;

Value create_target(const std::vector<Value>& args) {
    if (args.size() < 2) return Value((tn_int_t)-1);
    std::string name = std::get<std::string>(args[0].v);
    std::string type = std::get<std::string>(args[1].v);

    BuildTarget t;
    t.name = name;
    t.is_executable = (type == "exe");
    targets.push_back(t);

    return Value((tn_int_t)(targets.size() - 1));
}

Value add_source(const std::vector<Value>& args) {
    if (args.size() != 2) return Value((tn_int_t)-1);
    tn_int_t idx = std::get<tn_int_t>(args[0].v);
    std::string src = std::get<std::string>(args[1].v);
    targets[idx].sources.push_back(src);
    return Value((tn_int_t)0);
}

Value add_include(const std::vector<Value>& args) {
    if (args.size() != 2) return Value((tn_int_t)-1);
    tn_int_t idx = std::get<tn_int_t>(args[0].v);
    std::string path = std::get<std::string>(args[1].v);
    targets[idx].include_paths.push_back(path);
    return Value((tn_int_t)0);
}

Value add_flag(const std::vector<Value>& args) {
    if (args.size() != 2) return Value((tn_int_t)-1);
    tn_int_t idx = std::get<tn_int_t>(args[0].v);
    std::string flag = std::get<std::string>(args[1].v);
    targets[idx].flags.push_back(flag);
    return Value((tn_int_t)0);
}

Value link_lib(const std::vector<Value>& args) {
    if (args.size() != 2) return Value((tn_int_t)-1);
    tn_int_t idx = std::get<tn_int_t>(args[0].v);
    std::string lib = std::get<std::string>(args[1].v);
    targets[idx].linked_libs.push_back(lib);
    return Value((tn_int_t)0);
}

Value build_all(const std::vector<Value>&) {
    for (auto& t : targets) {
        std::string cmd;
        std::string compiler = "g++";
        if (!t.is_executable) {
            // Static lib
            std::string obj_files;
            for (auto& s : t.sources) {
                obj_files += s + ".o ";
                std::string compile_cmd = compiler + " -c " + s + " -o " + s + ".o";
                for (auto& f : t.flags) compile_cmd += " " + f;
                for (auto& inc : t.include_paths) compile_cmd += " -I" + inc;
                std::system(compile_cmd.c_str());
            }
            cmd = "ar rcs lib" + t.name + ".a " + obj_files;
        } else {
            cmd = compiler + " -o " + t.name + " ";
            for (auto& s : t.sources) cmd += s + " ";
            for (auto& f : t.flags) cmd += f + " ";
            for (auto& inc : t.include_paths) cmd += "-I" + inc + " ";
            for (auto& lib : t.linked_libs) cmd += "-l" + lib + " ";
        }
        std::cout << "[BUILD] Running: " << cmd << std::endl;
        std::system(cmd.c_str());
    }
    return Value((tn_int_t)0);
}

extern "C" void registerFunctions(std::unordered_map<std::string, NativeFn>& table) {
    table["create_target"] = create_target;
    table["add_source"] = add_source;
    table["add_include"] = add_include;
    table["add_flag"] = add_flag;
    table["link_lib"] = link_lib;
    table["build_all"] = build_all;
}
