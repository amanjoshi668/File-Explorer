#include <iostream>
#include "common.h"
#include <termios.h>
#ifndef ERROR_H
#define ERROR_H
struct error{
    static bool is_error;
    static bool instanceFlag;
    static string error_code;
    static bool error_printed;
    static error *Single;
    void print_error(bool normal, int number_of_rows);
    void remove_error(bool normal, int number_of_rows);
    static error* getInstance();
};

#endif