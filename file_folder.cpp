#include "file_folder.h"

file_folder :: file_folder(string name,  int type , string root_path){
    this->Error = error :: getInstance();
    this->name_of_file_or_folder = name;
    this->d_type = type;
    if(type!=9)this->name_of_file_or_folder_for_stat = root_path + "/" + name;
    else this->name_of_file_or_folder_for_stat = name;
}

bool file_folder :: is_file(){
    if(d_type == 8)return true;
    return false; 
}

bool file_folder :: is_folder(){
    if(d_type == 4)return true;
    return false; 
}

int file_folder :: get_stat(){
    if(this->name_of_file_or_folder_for_stat.c_str() == NULL){
        cout<<"Let me know"<<endl;
    }
    //cerr << this->name_of_file_or_folder_for_stat << endl;
    if(stat(this->name_of_file_or_folder_for_stat.c_str(), &(this->sb)) == -1){
        this->Error->is_error = true;
        this->Error->error_code += "Error in opening "+this->name_of_file_or_folder;
        cerr<<"Error in opening "<<this->name_of_file_or_folder_for_stat<<endl;
        //cerr<<this->name_of_file_or_folder_for_stat.c_str()<<endl;
        return 1;
    }
    //cout << "Debug " << endl;
    if(S_ISDIR(this->sb.st_mode)){
        this->d_type = 4;
    }
    else{
        this->d_type = 8;
    }
    group *grp;
    passwd *pwd;
    grp = getgrgid(this->sb.st_gid);
    this->group_name = string(grp->gr_name);
    //cout << grou
    pwd = getpwuid(this->sb.st_uid);
    this->user_name = string(pwd->pw_name);
    //cout << user_name << " Debuf" << endl;
    int x = (int)this->sb.st_mode;
    vector<int> temp;
    while(x>0){
        temp.push_back(x%8);
        x/=8;
    }
    map<int,string> convert_octal_permission;
    convert_octal_permission[0] = "---";
    convert_octal_permission[1] = "--x";
    convert_octal_permission[2] = "-w-";
    convert_octal_permission[3] = "-wx";
    convert_octal_permission[4] = "r--";
    convert_octal_permission[5] = "r-x";
    convert_octal_permission[6] = "rw-";
    convert_octal_permission[7] = "rwx";
    if( this->is_file() ) this->permissions = ".";
    else this->permissions = "d";
    this->permissions += convert_octal_permission[temp[2]]+convert_octal_permission[temp[1]]+convert_octal_permission[temp[0]];
    long long size = this->sb.st_size;
    if(size<1024){
        this->size = size;
        this->unit = "";
    }
    else if(size<1024*1024){
        double f = (this->sb.st_size)*1.0/1024.0;
        if(f<10)this->size = ROUNDF(f, 10);
        else this->size = int(f);
        this->unit = "K";
    }
    else if(size<1024*1024*1024){
        double f = (this->sb.st_size)*1.0/(1024.0*1024);
        if(f<10)this->size = ROUNDF(f, 10);
        else this->size = int(f);
        this->unit = "M";
    }
    else if(size<1024LL*1024LL*1024*1024){
        double f = (this->sb.st_size)*1.0/(1024.0*1024*1024);
        if(f<10)this->size = ROUNDF(f, 10);
        else this->size = int(f);
        this->unit = "G";
    }
    else{
        double f = (this->sb.st_size)*1.0/(1024.0*1024*1024*1024);
        if(f<10)this->size = ROUNDF(f, 10);
        else this->size = int(f);
        this->unit = "T";
    }
    this->last_modified = string(ctime(&this->sb.st_mtime));
    return 0;
}