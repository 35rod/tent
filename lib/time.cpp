// time_native.cpp
#include "native.hpp"
#include <chrono>
#include <thread>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <memory>
#include <unordered_map>

using namespace std::chrono;

Value time__sleep(const std::vector<Value>& args) {
    if (args.size() != 1) {
        std::cerr << "sleep(seconds) expects 1 argument\n";
        return Value();
    }
	
    if (std::holds_alternative<tn_int_t>(args[0].v)) {
        tn_int_t s = std::get<tn_int_t>(args[0].v);
        std::this_thread::sleep_for(std::chrono::duration<tn_int_t>(s));
    } else if (std::holds_alternative<tn_dec_t>(args[0].v)) {
        tn_dec_t d = std::get<tn_dec_t>(args[0].v);
        auto micros = static_cast<long long>(d * 1'000'000.0);
        if (micros > 0)
            std::this_thread::sleep_for(std::chrono::microseconds(micros));
    } else {
        std::cerr << "sleep(seconds) expects a numeric argument\n";
    }

    return Value((tn_int_t)1);
}

Value time__sleep_ms(const std::vector<Value>& args) {
    if (args.size() != 1) {
        std::cerr << "sleep_ms(ms) expects 1 argument\n";
        return Value();
    }
    if (!std::holds_alternative<tn_int_t>(args[0].v)) {
        std::cerr << "sleep_ms(ms) expects integer milliseconds\n";
        return Value();
    }

    tn_int_t ms = std::get<tn_int_t>(args[0].v);
    if (ms > 0) std::this_thread::sleep_for(std::chrono::milliseconds(ms));

    return Value((tn_int_t)1);
}

Value time__time(const std::vector<Value>&) {
    auto now = system_clock::now();
    auto epoch = now.time_since_epoch();
    long long micros = duration_cast<microseconds>(epoch).count();
    tn_dec_t seconds = static_cast<tn_dec_t>(micros) / 1'000'000.0;
    return Value(seconds);
}

static std::string strftime_from_tm(const std::string& fmt, std::tm tm_struct) {
    std::ostringstream oss;
#if defined(__GNUC__) || defined(__clang__) || defined(_MSC_VER)
    try {
        oss << std::put_time(&tm_struct, fmt.c_str());
        return oss.str();
    } catch (...) {
        // fallback to strftime
    }
#endif
    // fallback
    const size_t BUF_SIZE = 256;
    char buf[BUF_SIZE];
    if (std::strftime(buf, BUF_SIZE, fmt.c_str(), &tm_struct)) {
        return std::string(buf);
    } else {
        return std::string("");
    }
}

Value time__strftime(const std::vector<Value>& args) {
    if (args.size() < 1 || !std::holds_alternative<std::string>(args[0].v)) {
        std::cerr << "strftime(fmt) expects a format string\n";
        return Value(std::string(""));
    }
    std::string fmt = std::get<std::string>(args[0].v);

    std::time_t seconds_part;
    std::tm tm_struct;
    if (args.size() >= 2) {
        if (std::holds_alternative<tn_int_t>(args[1].v)) {
            seconds_part = static_cast<std::time_t>(std::get<tn_int_t>(args[1].v));
        } else if (std::holds_alternative<tn_dec_t>(args[1].v)) {
            tn_dec_t d = std::get<tn_dec_t>(args[1].v);
            seconds_part = static_cast<std::time_t>(std::floor(d));
        } else {
            std::cerr << "strftime: second arg must be epoch seconds (int or dec)\n";
            return Value(std::string(""));
        }

#if defined(_WIN32)
        localtime_s(&tm_struct, &seconds_part);
#else
        localtime_r(&seconds_part, &tm_struct);
#endif
    } else {
        // use now
        auto now = system_clock::now();
        auto epoch = now.time_since_epoch();
        long long secs = duration_cast<seconds>(epoch).count();
        seconds_part = static_cast<time_t>(secs);
#if defined(_WIN32)
        localtime_s(&tm_struct, &seconds_part);
#else
        localtime_r(&seconds_part, &tm_struct);
#endif
    }

    std::string out = strftime_from_tm(fmt, tm_struct);
    return Value(out);
}

extern "C" void registerFunctions(std::unordered_map<std::string, NativeFn>& table) {
    table["sleep"] = time__sleep;
    table["sleep_ms"] = time__sleep_ms;
    table["time"] = time__time;
    table["strftime"] = time__strftime;
}
