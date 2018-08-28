#include "common.h"
#include <termios.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <pwd.h>
#include <limits.h>
#include <grp.h>
#include <bits/stdc++.h>
#include <fcntl.h>
using namespace std;

struct terminal{
    termios initial_settings, new_settings;
    FILE *input,*output;
    void switch_to_non_canonical_mode();
    int switch_to_canonical_mode();
    terminal (FILE *in, FILE *out);
};

terminal :: terminal(FILE *in, FILE *out){
	tcgetattr(fileno(in), &this->initial_settings);
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
        return 1;
    }
    return 0;
}

void terminal :: switch_to_non_canonical_mode(){
    lo x = tcsetattr(fileno(input), TCSANOW, &initial_settings);
    if(x!=0){
        fprintf(stderr,"Could not set attributes Back to original\n");
        cerr<<"Please use \"tput reset \" command to restore the original settings of terminal"<<endl;
        return ;
    }
    return ;
}



struct file_folder{
    string name_of_file_or_folder;
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

file_folder :: file_folder(string name, int type, string root_path){
    this->name_of_file_or_folder = name;
    this->d_type = type;
    this->name_of_file_or_folder_for_stat = root_path + "/" + name;
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
        cerr<<"Cant't execute stat"<<this->name_of_file_or_folder_for_stat<<endl;
        //cerr<<this->name_of_file_or_folder_for_stat.c_str()<<endl;
        return 1;
    }
    //cout << "Debug " << endl;
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



struct directory{
    DIR *direct;
    dirent *pDirent;
    string current_directory;
    vector<file_folder> all_files_folder;
    directory ();
    int open_directory(string name);
};

directory :: directory(){
    this->current_directory = "";
    all_files_folder.clear();
}

int directory :: open_directory(string name){
    this->current_directory = name;
    this->direct = opendir(name.c_str());
    if(this->direct == NULL){
        cerr<<"Can't open Directory "<<name<<endl;
        return 1;
    }
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

struct command{
    string full_command;
    string command;
    vector<string> arguments;
    int take_command_input(FILE *input, FILE *output, int command_pos, int number_of_rows);
    void execute_command();
};

void command :: execute_command(){
    istringstream iss(this->full_command);
    iss>>this->command;
    string temp;
    while(iss >> temp){
        this->arguments.push_back(temp);
    }
}

int command :: take_command_input(FILE *input, FILE *output, int command_pos, int number_of_rows){
    this->full_command = "";
    char choice;
    do{
        choice = fgetc(input);
        if(choice == '\n' or choice == '\r'){
            this->execute_command(); 
            return 0;
        }
        else if(choice == '\e'){
            return 1;
        }
        else if((int) choice == 128 ){
            cout<<"\e["<<number_of_rows<<";"<<command_pos-1<<"H"<<" "<<"\e["<<number_of_rows<<";"<<command_pos-1<<"H"<<" ";
            this->full_command.pop_back();
            command_pos--;
        }
        else {
            this->full_command+=choice;
            cout<<choice;
            command_pos++;
        }
    }while(1);
    return 0;
}

struct screen{
    int x_pos, y_pos;
    int command_pos;
    int number_of_rows, number_of_columns;
    int current_top, current_bottom;
    int current_position_in_history;
    bool normal;
    command Command;
    string HOME;
    deque<string> history;
    directory current_directory;
    FILE *input,*output;
    screen(FILE *in, FILE *out, string home);
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
};

void screen :: recursive_copy(string source, string destination){
    struct stat tmp ={0};
    if(stat(source.c_str(), &tmp) == -1){
        cerr<<"Cant't execute stat"<<source<<endl;
        return ;
    }
    if (!S_ISDIR(tmp.st_mode)){
        this->copy_file(source, destination);
        return;
    }
    else {
        cerr<<"Can't execute the stat "<<source;
    }
    DIR *direct = opendir(source.c_str());
    if(direct == NULL){
        cerr<<"Can't open Directory "<<source<<endl;
        return ;
    }
    dirent *pDirent;
    this->create_dir(destination, tmp.st_mode);
    while((pDirent = readdir(direct)) != NULL)if(string(pDirent->d_name)!="." and string(pDirent->d_name)!=".."){
        string file_name = "/" + string(pDirent->d_name);
        string source_file_name = source + file_name;
        string destination_file_name = destination + file_name;
        recursive_copy(source_file_name, destination_file_name);
    }
    return ;
}

void screen :: copy(){
    string destination = this->Command.arguments.back();
    destination = destination.substr(1,destination.size()-1);
    destination = this->HOME + destination;
    for(int i=0;i<this->Command.arguments.size()-1 ;i++){
        string source = this->current_directory.current_directory + "/" + this->Command.arguments[i];
        string destination_file = destination + "/" + this->Command.arguments[i];
        //derr2(source,destination_file);
        recursive_copy(source, destination_file);
    }   
}

void screen :: __delete(){
    string source = this->Command.arguments[0];
    source = source.substr(1,source.size()-1);
    source = this->HOME + source;
    recursive_delete(source);
    this->normal = true;
    this->change_directory(this->current_directory.current_directory, this->current_position_in_history);
}
void screen :: recursive_delete(string source){
    //derr(source);
    struct stat tmp ={0};
    if(stat(source.c_str(), &tmp) == -1){
        cerr<<"Cant't execute stat"<<source<<endl;
        return ;
    }
    int x = stat(source.c_str(),&tmp);
    if (!S_ISDIR(tmp.st_mode)){
        int x = unlink(source.c_str());
        if(x!=0){
            cerr<<"Can't delete file "<<source<<endl;
        }
        return;
    }
    DIR *direct = opendir(source.c_str());
    if(direct == NULL){
        cerr<<"Can't open Directory "<<source<<endl;
        return ;
    }
    dirent *pDirent;
    while((pDirent = readdir(direct)) != NULL)if(string(pDirent->d_name)!="." and string(pDirent->d_name)!=".."){
        string file_name = "/" + string(pDirent->d_name);
        string source_file_name = source + file_name;
        recursive_delete(source_file_name);
    }
    x = rmdir(source.c_str());
    if(x!=0){
        cerr<<"Can't delete file "<<source<<endl;
    }
    return;
}

void screen :: copy_file(string source, string destination){
    struct stat tmp ={0};
    stat(source.c_str(),&tmp);
    int in_fd = open(source.c_str(), O_RDONLY);
    int out_fd = creat(destination.c_str(), tmp.st_mode);
    //chmod(destination.c_str(), tmp.st_mode);
    if(in_fd ==-1 or out_fd == -1){
        cerr<<"Can't open files"<<endl;
        return;
    }
    char buff[BUFFERSIZE];
    int n_chars;
    while( (n_chars = read(in_fd, buff, BUFFERSIZE)) > 0 ){
        if( write(out_fd, buff, n_chars) != n_chars ){
            cerr<<"Write error to "<<destination<<endl;
        }    
        if( n_chars == -1 ){
            cerr<<"Read error from "<<source<<endl;
        }
    }
}

void screen :: create_dir(string directory="", __mode_t t = 0700){
    struct stat st = {0};
    string pathname = "";
    if(this->Command.arguments[1]=="."){
        pathname = this->current_directory.current_directory;
    }
    else {
        pathname = this->HOME + this->Command.arguments[1].substr(1,this->Command.arguments[1].size()-1);
    }
    pathname = pathname + "/" + this->Command.arguments[0];
    if(!directory.empty()){
        pathname = directory;
    }
    if(stat(pathname.c_str(), &st) != -1){
        cerr<<"The Folder name already exists"<<endl;
    }
    mkdir(pathname.c_str(),t);
    this->normal = true;
    this->change_directory(this->current_directory.current_directory, this->current_position_in_history);
}

void screen :: create_file(){
    struct stat st = {0};
    string pathname = "";
    if(this->Command.arguments[1]=="."){
        pathname = this->current_directory.current_directory;
    }
    else {
        pathname = this->HOME + this->Command.arguments[1].substr(1,this->Command.arguments[1].size()-1);
    }
    pathname = pathname + "/" + this->Command.arguments[0];
    auto fp = fopen(pathname.c_str(),"a");
    this->normal = true;
    this->change_directory(this->current_directory.current_directory, this->current_position_in_history);
}
void screen :: move(){
    string destination = this->Command.arguments.back();
    destination = destination.substr(1,destination.size()-1);
    destination = this->HOME + destination;
    for(int i=0;i<this->Command.arguments.size()-1 ;i++){
        string source = this->current_directory.current_directory + "/" + this->Command.arguments[i];
        string destination_file = destination + "/" + this->Command.arguments[i];
        //derr2(source,destination_file);
        recursive_copy(source, destination_file);
        recursive_delete(source);
    }   
    this->normal = true;
    this->change_directory(this->current_directory.current_directory, this->current_position_in_history);
}

void screen :: rename(){
    string destination = this->current_directory.current_directory + "/";
    string source = destination + this->Command.arguments[0];
    destination += this->Command.arguments[1];
    recursive_copy(source,destination);
    recursive_delete(source);
    this->normal = true;
    this->change_directory(this->current_directory.current_directory, this->current_position_in_history);
}

void screen :: execute_command(){
    if(this->Command.command == "copy"){
        this->copy();
    }
    else if(this->Command.command == "move"){
        this->move();
    }
    else if(this->Command.command == "rename"){
        this->rename();
    }
    else if(this->Command.command == "create_file"){
        this->create_file();
        //cout<<"I am ‘:create_file";
    }
    else if(this->Command.command == "create_dir"){
        this->create_dir();
        //cout<<"I am ‘:create_dir";
    }
    else if(this->Command.command == "delete_file" or this->Command.command == "delete_dir"){
        //do delete;
        this->__delete();
    }
    else if(this->Command.command == "goto"){
        this->goto_location();
    }
    else if(this->Command.command == "search"){
        //do search;
        cout<<"I am searching";
    }else if(this->Command.command == "snapshot"){
        //do snapshot;
        cout<<"I am snapshoting";
    }
}

void screen :: command_mode(){
    cout<<"\e["<<this->number_of_rows<<";1H";
    this->command_pos = 13;
    cout<<"Command Mode \e["<<this->number_of_rows<<";"<<this->command_pos<<"H"<<" :";
    this->command_pos += 2;
    if(this->Command.take_command_input(this->input, this->output, this->command_pos, this->number_of_rows) != 0){
        this->normal = true;
        this->fill_screen();
        return;
    }
    //debug2(this->Command.command,this->Command.arguments);
    this->execute_command();
    this->normal=true;
}

void screen :: flush(){
    cout<<"\e[2J";
    cout<<"\e[1;1H";
}

screen :: screen(FILE *in, FILE *out, string home=""){
    this->normal = true;
    this->HOME = home;
    this->input = in;
    this->output = out;
    this->command_pos = 1;
    if(home.empty()){
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        this->HOME = string(cwd);
    }
    this->x_pos = 2;
    this->current_top = 1;
    this->y_pos = 1;
    //debug(this->HOME);
    this->current_directory.open_directory(this->HOME);
    this->current_position_in_history = 0;
    //cout<<"reached herer";
    this->history.push_back(this->HOME);
    this->fill_screen();
}

void screen :: get_screen_size(){
    struct winsize size;
    ioctl(fileno(this->input), TIOCGWINSZ, &size);
    this->number_of_rows = (int)size.ws_row;
    this->number_of_columns = (int)size.ws_col;
    this->current_bottom = this->number_of_rows - 2;
    //cerr<<this->current_bottom<<" "<<this->number_of_rows<<" "<<this->current_directory.all_files_folder.size()<<endl;
    this->current_bottom = min(this->current_bottom ,(int) this->current_directory.all_files_folder.size()); 
    this->current_bottom += this->current_top;
}

void screen :: fill_screen(){
    this->flush();
    this->get_screen_size();
    cout<<"\e[1m"<<this->current_directory.current_directory<<"\e[0m"<<endl;
    //zcout<<this->current_bottom<<endl;
    for(int line = this->current_top;line < this->current_bottom; line++){
        file_folder file_or_folder = this->current_directory.all_files_folder[line-1];
        cout << file_or_folder.permissions << " " ;
        string x = file_or_folder.user_name;
        if (x.length() > 8 ){
            x = x.substr(0,6)+"..";
        }
        cout << std::left << setw(8) << x << " " ;
        x = file_or_folder.group_name;
        if(x.length()>8){
            x = x.substr(0,6)+"..";
        }
        cout << std::left << setw(8) << x << "  ";
        if(file_or_folder.unit == ""){
            cout<<left<<setw(6)<<file_or_folder.size<<" ";
        }
        else {
            cout<<left<<setw(5)<<fixed<<setprecision(1)<<file_or_folder.size;
            cout<<file_or_folder.unit<<" ";
        }
        cout<<file_or_folder.last_modified.substr(0,24)<<" ";
        if(file_or_folder.is_folder()){
            cout<<"\e[2m";
        }
        cout<<file_or_folder.name_of_file_or_folder<<endl;
        if(file_or_folder.is_folder()){
            cout<<"\e[0m";
        }
    }
    cout<<"\e["<<this->number_of_rows<<";1H";
    if(this->normal){
        cout<<"Normal Mode";
    }
    else {
        cout<<"\e["<<this->number_of_rows<<";1H";
        cout<<"Command Mode \e["<<this->number_of_rows<<";"<<this->command_pos<<"H"<<" :";
        this->command_pos += 2;
    }
    cout<<"\e["<<this->x_pos<<";"<<y_pos<<"H";
    //debug4(this->current_top,this->current_bottom,this->x_pos, this->current_directory.all_files_folder.size());
}

void screen :: change_directory(string new_path, int position){
    this->x_pos = 2;
    this->current_top = 1;
    this->y_pos = 1;
    this->current_directory.open_directory(new_path);
    this->current_position_in_history = position;
    if (position == this->history.size()){
        this->history.push_back(new_path);
    }
    this->fill_screen();
    if(this->history.size()>INF){
        history.pop_front();
        this->current_position_in_history--;
    }
}

void screen :: move_up(){
    if(this->x_pos == 2){
        this->current_top = max( this->current_top - 1 , 1 );
    }
    else{
        this->x_pos --;
    }
    this->normal = true;
    this->fill_screen();
}

void screen :: move_down(){
    if(this->current_top + this->x_pos > this->current_directory.all_files_folder.size() + 1){
        return ;
    }
    if(this->x_pos == this->number_of_rows - 1){
        if(this->current_top + this->x_pos  <= this->current_directory.all_files_folder.size() + 1){
            //debug4(this->x_pos,this->current_top,this->number_of_rows,this->current_directory.all_files_folder.size());
            this->current_top++;
        }
    }
    else{
        this->x_pos++;
    }
    this->normal = true;
    this->fill_screen();
}

void screen :: move_right(){
    if(this->current_position_in_history  == this->history.size()-1)return;
    this->normal = true;
    this->change_directory(this->history[current_position_in_history+1], this->current_position_in_history + 1 );
}

void screen :: move_left(){
    if(this->current_position_in_history  == 0)return;
    this->normal = true;
    this->change_directory(this->history[current_position_in_history-1] , this->current_position_in_history - 1 );
}

void screen :: move_home(){
    this->normal = true;
    this->change_directory(this->HOME , this->current_position_in_history + 1);
}

void screen :: move_back(){
    if(this->current_directory.current_directory == this->HOME)return;
    string new_path = this->current_directory.current_directory.substr(0, this->current_directory.current_directory.find_last_of("/"));
    this->normal = true;
    this->change_directory(new_path, this->current_position_in_history + 1);
}

void screen :: move_into(){
    //freopen("/home/aman/Documents/OS/file_explorer/error.txt", "w", stderr);
    if (this->current_directory.all_files_folder[ this->x_pos + this->current_top - 3 ].is_folder()){
        string path_name = this->current_directory.all_files_folder[ this->x_pos + this->current_top - 3 ].name_of_file_or_folder;
        cout<<path_name;
        if( path_name == "."){
            return;
        }
        else if(path_name == ".."){
            move_back();
            return;
        }
        path_name = this->current_directory.current_directory + "/" + path_name;
        this->normal = true;
        this->change_directory( path_name, this->current_position_in_history + 1);
    }
    else{
        pid_t pid;
        pid = fork();
        if(pid == 0){
            //Execute the file;
            string file_to_execute = this->current_directory.all_files_folder[this->x_pos + this->current_top - 3].name_of_file_or_folder_for_stat ;
            cerr<<"/usr/bin/xdg-open"<< file_to_execute<<endl;
            execl("/usr/bin/xdg-open", "xdg-open", file_to_execute.c_str(), NULL);
        }
        else if(pid < 0){
            cerr<<"Failed to fork"<<endl;
        }
        else {
            this->normal = true;
            this->fill_screen();
            return;
        }
    }
}

void screen :: goto_location(){
    if(this->Command.arguments[0] == "/"){
        this->normal = true;
        this->change_directory(this->HOME, this->current_position_in_history + 1);
        return;
    }
    //derr(this->Command.arguments);
    string destination = this->Command.arguments[0];
    destination = destination.substr(1,destination.size()-1);
    destination = this->HOME + destination;
    this->normal = true;
    this->change_directory(destination, this->current_position_in_history + 1);
    return;
}


int main(int argc, char* argv[]){
    freopen("/home/aman/Documents/OS/file_explorer/error.txt", "w", stderr);
    cout<<"\e[?1049h";
    FILE *input,*output;
    input = fopen("/dev/tty","r");
	output = fopen("/dev/tty","w");
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