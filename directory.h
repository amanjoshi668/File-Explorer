#include <iostream>
#include <sys/stat.h>
#include "common.h"
#include "error.h"
#include <dirent.h>
#include "file_folder.h"
#ifndef DIRECTORY_H
#define DIRECTORY_H

struct directory{
    DIR *direct;
    dirent *pDirent;
    string current_directory;
    vector<file_folder> all_files_folder;
    directory ();
    int open_directory(string name);
    error *Error;
};

#endif