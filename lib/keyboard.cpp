#include "native.hpp"

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#endif

int khbit() {
#ifdef _WIN32
    return _khbit();
#else
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
#endif
}

int getch() {
#ifdef _WIN32
    return _getch();
#else
    int r;
    unsigned char c;
    if ((r = read(STDIN_FILENO, &c, sizeof(c))) < 0)
        return r;
    return c;
#endif
}

#ifndef _WIN32
void set_terminal_raw_mode() {
    struct termios old_tio, new_tio;
    tcgetattr(STDIN_FILENO, &old_tio);
    new_tio = old_tio;
    new_tio.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_terminal_mode() {
    struct termios old_tio;
    tcgetattr(STDIN_FILENO, &old_tio);
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
}
#endif

Value keyboardtn__init(const std::vector<Value>&) {
    #ifndef _WIN32
        set_terminal_raw_mode();
    #endif

    return Value();
}

Value keyboardtn__isPressed(const std::vector<Value>&) {
    return tn_bool_t(khbit());
}

Value keyboardtn__getKey(const std::vector<Value>&) {
    return tn_int_t(getch());
}

Value keyboardtn__end(const std::vector<Value>&) {
    #ifndef _WIN32
        restore_terminal_mode();
    #endif

    return Value();
}

extern "C" void registerFunctions(std::unordered_map<std::string, NativeFn>& table) {
    table["init"] = keyboardtn__init;
    table["isPressed"] = keyboardtn__isPressed;
    table["getKey"] = keyboardtn__getKey;
    table["end"] = keyboardtn__end;
}