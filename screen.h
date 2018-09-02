#include <iostream>
#include <sys/stat.h>
#include "error.h"
#include <dirent.h>
#include "command.h"
#include "directory.h"
#include "unistd.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#ifndef SCREEN_H
#define SCREEN_H

struct screen{
    int x_pos, y_pos;
    int command_pos;
    int number_of_rows, number_of_columns;
    int current_top, current_bottom;
    int current_position_in_history;
    pair<string,string> search_elems;
    string search_title;
    bool normal;
    command Command;
    string HOME;
    deque<string> history;
    bool isSearch;
    directory current_directory;
    FILE *input,*output;
    screen(FILE *in, FILE *out, string home);
    error *Error;
    void flush();
    void get_screen_size();
    void move_down();
    void move_up();
    void move_left();
    void move_right();
    void move_home();
    void move_back();
    void fill_screen();
    void move_into();
    void change_directory(string,int);
    void command_mode();
    void execute_command();
    void create_dir(string, __mode_t);
    void create_file();
    void copy();
    void copy_file(string, string);
    void recursive_copy(string, string);
    void recursive_delete(string);
    void __delete();
    void move();
    void rename();
    void goto_location();
    void snapshot();
    void recursive_snapshot(string, string, string);
    void search(string, string);
    void recursive_search(string, string, vector<string> &);
};

#endif