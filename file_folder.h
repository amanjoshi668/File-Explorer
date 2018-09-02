#include <iostream>
#include <sys/stat.h>
#include "common.h"
#include "error.h"
#include "grp.h"
#include "pwd.h"

#ifndef FILE_FOLDER_H
#define FILE_FOLDER_H
struct file_folder{
    string name_of_file_or_folder;
    error *Error;
    string name_of_file_or_folder_for_stat;
    int d_type;
    struct stat sb;
    string user_name,group_name;
    string permissions, last_modified;
    double size;
    string unit;
    file_folder (string name, int type, string root);
    bool is_file();
    bool is_folder();
    int get_stat();
    bool operator<( const file_folder &val ) const{
        return this->name_of_file_or_folder < val.name_of_file_or_folder;
    }
};

#endif