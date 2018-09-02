#include "common.h"
#include <termios.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <limits.h>
#include <bits/stdc++.h>
#include <fcntl.h>
#include "error.h"
#include "terminal.h"
#include "screen.h"
using namespace std;
/*
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
bool error :: is_error;
bool error :: instanceFlag;
bool error :: error_printed;
error* error :: Single; 
string error :: error_code;
void error :: print_error(bool normal, int number_of_rows){
    cout<<"\e["<<number_of_rows<<";1H";
    if(normal){
        cout<<"Normal Mode";
        cout<<"\e[1m"<<": ERROR"<<this->error_code<<"\e[0m";
    }
    else { 
        cout<<"Command Mode";
        cout<<"\e[1m"<<": ERROR"<<this->error_code<<"\e[0m";
    }
    this->error_code = "";
    this->is_error = false;
    this->error_printed = true;
}
void error :: remove_error(bool normal, int number_of_rows){
    cout<<"\e["<<number_of_rows<<";1H";
    cout<<"\e[2K";
    if(normal){
        cout<<"Normal Mode";
    }
    else { 
        cout<<"Command Mode :";
        //cout<<"\e[1m"<<": ERROR"<<this->error_code<<"\e[0m";
    }
    this->error_printed = false;
}
error* error :: getInstance(){
    if(!instanceFlag){
        Single = new error();
        Single -> is_error = false;
        Single -> error_printed = false;
        Single -> error_code = "";
        instanceFlag = true;
        return Single;
    }
    else {
        return Single;
    }
}*/

/*
struct terminal{
    termios initial_settings, new_settings;
    FILE *input,*output;
    error *Error;
    void switch_to_non_canonical_mode();
    int switch_to_canonical_mode();
    terminal (FILE *in, FILE *out);
};

terminal::terminal(FILE *in, FILE *out){
    this->Error = error :: getInstance();
	if(tcgetattr(fileno(in), &this->initial_settings)!=0){
        this->Error->is_error = true;
        this->Error->error_code += "Could not get the attributes of Terminal";
    }
    this->new_settings = this->initial_settings;
    this->input = in;
    this->output = out;
}

int terminal :: switch_to_canonical_mode(){
    new_settings.c_lflag &= ~ICANON;
    //new_settings.c_lflag &= ~ISIG;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;
    lo x = tcsetattr(fileno(input), TCSANOW, &new_settings);
    if(x!=0){
        fprintf(stderr,"Could not set attributes\n");
        this->Error->is_error = true;
        this->Error->error_code += "Could not set the attributes of Terminal";
        return 1;
    }
    return 0;
}

void terminal :: switch_to_non_canonical_mode(){
    lo x = tcsetattr(fileno(input), TCSANOW, &initial_settings);
    if(x!=0){
        fprintf(stderr,"Could not set attributes Back to original\n");
        cerr<<"Please use \"tput reset \" command to restore the original settings of terminal"<<endl;
        this->Error->is_error = true;
        this->Error->error_code += "Could not set the attributes of Terminal back to Normal Please use \"tput reset \" command to restore the original settings of terminal";
        return ;
    }
    return ;
}*/


/*
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

*/
/*
struct directory{
    DIR *direct;
    dirent *pDirent;
    string current_directory;
    vector<file_folder> all_files_folder;
    directory ();
    int open_directory(string name);
    error *Error;
};

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
*/
/*
struct command{
    string full_command;
    string command_type;
    error *Error;
    vector<string> arguments;
    int take_command_input(FILE *input, FILE *output, int command_pos, int number_of_rows);
    void execute_command();
    command();
};
command :: command(){
    this->Error = error :: getInstance();
}

void command :: execute_command(){
    //cerr<<this->full_command<<endl;
    istringstream iss(this->full_command);
    iss>>this->command;
    string temp;
    while(iss >> temp){
        this->arguments.push_back(temp);
    }
}
*/
/*
void command :: execute_command(){
    istringstream iss(this->full_command);
    iss>>this->command_type;
    string temp;
    string full_command_space="";
    while(iss >> temp){
        //iss>>temp;derr(temp);
        full_command_space += " "+temp;
    }
    bool slash = false;
    bool quote = false;
    string current="";
    for(int i=1;i<full_command_space.length();i++){
        if(full_command_space[i]=='\\'){
            if(slash){
                current += "\\";
                slash = false;
            }
            else {
                slash = true;
            }
        }
        else if(full_command_space[i] == '\''){
            if(slash){
                current += "'";
                slash = false;
            }
            else if(!quote){
                quote = true;
            }
            else {
                this->arguments.push_back(current);
                current = "";
                quote = false;
                i++;
            }
        }
        else if(full_command_space[i]==' ' and !quote){
            this->arguments.push_back(current);
            current = "";
        }
        else {
            current += full_command_space[i];
        }            
    }
    if(!current.empty()){
        this->arguments.push_back(current);
    }
}

int command :: take_command_input(FILE *input, FILE *output, int command_pos, int number_of_rows){
    this->full_command = "";
    char choice;
    do{
        choice = fgetc(input);
        if(this->Error->error_printed){
            Error->remove_error(false, number_of_rows);
        }
        if(choice == '\n' or choice == '\r'){
            this->execute_command(); 
            return 0;
        }
        else if(choice == '\e'){
            return 1;
        }
        else if((int) choice == 127 ){
            if(command_pos>15){
                cout<<"\e["<<number_of_rows<<";"<<command_pos-1<<"H"<<" "<<"\e["<<number_of_rows<<";"<<command_pos-1<<"H";
                this->full_command.pop_back();
                command_pos--;
            }
        }
        else {
            this->full_command+=choice;
            cout<<choice;
            command_pos++;
        }
    }while(1);
    return 0;
}*/
/*

*/

int main(int argc, char* argv[]){
    freopen("/home/aman/Documents/OS/file_explorer/error.txt", "w", stderr);
    cout<<"\e[?1049h";
    FILE *input,*output;
    input = fopen("/dev/tty","r");
	output = fopen("/dev/tty","w");
    error *Error;
    Error = error :: getInstance();
    Error->is_error=false;
    terminal Terminal(input,output);
    if(Terminal.switch_to_canonical_mode()==1){
        cout<<"\e[?1049l";
        return 1;
    }
    string home="";
    if(argc>=2 and argv[1]!=NULL)home = string(argv[1]);
    screen Screen(input,output,home);
    char choice;
    do{
        choice = fgetc(input);
        if(choice == '\033'){
            choice = fgetc(input);
            if(choice == '['){
                choice = fgetc(input);
                if(choice == 'A'){
                    Screen.move_up();
                }
                else if(choice =='B'){
                    Screen.move_down();
                }
                else if(choice == 'C'){
                    Screen.move_right();
                }
                else if(choice == 'D'){
                    Screen.move_left();
                }
            }
        }
        else if(choice == 'h' or choice =='H'){
            Screen.move_home();
        }
        else if(choice == '\n' or choice == '\r'){
            Screen.move_into();
        }
        else if((int)choice == 127){
            Screen.move_back();
        }
        else if(choice == ':'){
            Screen.normal = false;
            Screen.command_mode();
        }
    }while(choice != 'q' and choice != 'Q');
    Terminal.switch_to_non_canonical_mode();
    cout<<"\e[?1049l";
    return 0;
}