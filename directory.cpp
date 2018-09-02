#include "directory.h"
 
directory :: directory(){
    this->Error = error :: getInstance();
    this->current_directory = "";
    all_files_folder.clear();
}

int directory :: open_directory(string name){
    this->current_directory = name;
    this->direct = opendir(name.c_str());
    if(this->direct == NULL){
        this->Error->is_error = true;
        this->Error->error_code += "Cant open Directory" + name;
        cerr << "Can't open Directory " << name << endl;
        return 1;
    }
    cerr<<"name"<<name<<endl;
    this->all_files_folder.clear();
    while((this->pDirent = readdir(this->direct)) != NULL)if(this->pDirent->d_name!=NULL){
        string file_name = string(this->pDirent->d_name);
        file_folder current(file_name, (int)this->pDirent->d_type, this->current_directory);
        current.get_stat();
        this->all_files_folder.push_back(current);
    }
    sort(all(this->all_files_folder));
    return 0;
}