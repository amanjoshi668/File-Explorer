#include <iostream>
#include <termios.h>
#include "common.h"
#include "error.h"
#ifndef TERMINAL_H
#define TERMINAL_H

struct terminal{
    termios initial_settings, new_settings;
    FILE *input,*output;
    error *Error;
    void switch_to_non_canonical_mode();
    int switch_to_canonical_mode();
    terminal (FILE *in, FILE *out);
};

#endif