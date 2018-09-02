#include "error.h"
#include <dirent.h>

#ifndef COMMAND_H
#define COMMAND_H

struct command{
    string full_command;
    string command_type;
    error *Error;
    vector<string> arguments;
    int take_command_input(FILE *input, FILE *output, int command_pos, int number_of_rows);
    void execute_command();
    command();
};

#endif