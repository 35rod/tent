#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  include <fcntl.h>
#  include <spawn.h>
#  include <sys/wait.h>
#  include <unistd.h>
   extern char** environ;
#endif

using Clock = std::chrono::steady_clock;
using Ms    = std::chrono::duration<double, std::milli>;

static const int COL_NAME  = 22;
static const int COL_STAT  =  9;

// Returns elapsed milliseconds, or -1.0 on failure.
static double run_once(const std::string& tent_bin, const std::string& tent_file,
                       const std::string& work_dir) {
    auto t0 = Clock::now();

#if defined(_WIN32)
    // Build a properly-quoted command line: "binary" "file"
    std::wstring wbin(tent_bin.begin(), tent_bin.end());
    std::wstring wfile(tent_file.begin(), tent_file.end());
    std::wstring wwork(work_dir.begin(), work_dir.end());
    std::wstring cmd = L"\"" + wbin + L"\" \"" + wfile + L"\"";

    STARTUPINFOW si{};
    si.cb          = sizeof(si);
    si.dwFlags     = STARTF_USESTDHANDLES;
    si.hStdInput   = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput  = INVALID_HANDLE_VALUE; // discard stdout
    si.hStdError   = GetStdHandle(STD_ERROR_HANDLE);

    // Open NUL for stdout
    SECURITY_ATTRIBUTES sa{sizeof(sa), nullptr, TRUE};
    HANDLE nul = CreateFileW(L"NUL", GENERIC_WRITE, FILE_SHARE_WRITE, &sa,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    si.hStdOutput = nul;

    PROCESS_INFORMATION pi{};
    BOOL ok = CreateProcessW(
        wbin.c_str(),
        const_cast<wchar_t*>(cmd.c_str()),
        nullptr, nullptr,
        TRUE, 0,
        nullptr,
        wwork.empty() ? nullptr : wwork.c_str(),
        &si, &pi
    );

    CloseHandle(nul);
    if (!ok) return -1.0;

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exit_code = 0;
    GetExitCodeProcess(pi.hProcess, &exit_code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    auto t1 = Clock::now();
    if (exit_code != 0) return -1.0;

#else
    // Redirect child stdout to /dev/null so I/O doesn't skew timing.
    int null_fd = open("/dev/null", O_WRONLY);
    if (null_fd < 0) return -1.0;

    posix_spawn_file_actions_t fa;
    posix_spawn_file_actions_init(&fa);
    posix_spawn_file_actions_adddup2(&fa, null_fd, STDOUT_FILENO);

    char* argv[] = {
        const_cast<char*>(tent_bin.c_str()),
        const_cast<char*>(tent_file.c_str()),
        nullptr
    };

    pid_t pid = 0;
    int rc = posix_spawn(&pid, tent_bin.c_str(), &fa, nullptr, argv, environ);
    posix_spawn_file_actions_destroy(&fa);
    close(null_fd);

    if (rc != 0) return -1.0;

    int status = 0;
    if (waitpid(pid, &status, 0) < 0) return -1.0;

    auto t1 = Clock::now();
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) return -1.0;
#endif

    return Ms(t1 - t0).count();
}

static double median(std::vector<double>& v) {
    std::sort(v.begin(), v.end());
    size_t n = v.size();
    return (n % 2 == 1) ? v[n / 2] : (v[n / 2 - 1] + v[n / 2]) / 2.0;
}

static void print_stat(const std::string& label, double val) {
    std::cout << std::setw(COL_STAT) << std::right
              << std::fixed << std::setprecision(3) << val << " ms";
    (void)label;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Usage: bench_runner <tent_bin> <iterations> <file.tent>...\n";
        return 1;
    }

    std::string tent_bin   = std::filesystem::absolute(argv[1]).string();
    int         iterations = std::atoi(argv[2]);

    if (iterations < 1) {
        std::cerr << "iterations must be >= 1\n";
        return 1;
    }

    // Work dir = parent of tent binary (so lib/ is reachable via default search dirs)
    std::string work_dir = std::filesystem::absolute(argv[1]).parent_path().string();

    bool any_failed = false;

    // Header
    std::cout << "\n";
    std::cout << std::left << std::setw(COL_NAME) << "benchmark"
              << std::right
              << std::setw(COL_STAT + 3) << "min"
              << std::setw(COL_STAT + 3) << "median"
              << std::setw(COL_STAT + 3) << "mean"
              << "  (" << iterations << " runs)\n";
    std::cout << std::string(COL_NAME + (COL_STAT + 3) * 3, '-') << "\n";

    for (int i = 3; i < argc; i++) {
        std::string file     = std::filesystem::absolute(argv[i]).string();
        std::string name     = std::filesystem::path(argv[i]).stem().string();

        std::vector<double> times;
        times.reserve(iterations);
        bool failed = false;

        for (int r = 0; r < iterations; r++) {
            double ms = run_once(tent_bin, file, work_dir);
            if (ms < 0.0) {
                std::cerr << "  [FAILED] " << name << " (run " << (r + 1) << ")\n";
                failed = true;
                break;
            }
            times.push_back(ms);
        }

        std::cout << std::left << std::setw(COL_NAME) << name;
        if (failed || times.empty()) {
            std::cout << "  FAILED\n";
            any_failed = true;
        } else {
            double mn  = *std::min_element(times.begin(), times.end());
            double med = median(times);
            double avg = std::accumulate(times.begin(), times.end(), 0.0) / times.size();

            print_stat("min", mn);
            std::cout << "  ";
            print_stat("median", med);
            std::cout << "  ";
            print_stat("mean", avg);
            std::cout << "\n";
        }
    }

    std::cout << "\n";
    return any_failed ? 1 : 0;
}
