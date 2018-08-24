#include "common.h"
#include <termios.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <pwd.h>
#include <limits.h>
#include <grp.h>
using namespace std;

struct terminal{
    termios initial_settings, new_settings;
    FILE *input,*output;
    void switch_to_non_canonical_mode();
    int switch_to_canonical_mode();
    terminal (FILE *in, FILE *out);
};
terminal :: terminal(FILE *in, FILE *out){
	tcgetattr(fileno(input), &this->initial_settings);
    this->new_settings = this->initial_settings;
    this->input = in;
    this->output = out;
}
int terminal :: switch_to_canonical_mode(){
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ISIG;
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
    int d_type;
    struct stat sb;
    string user_name,group_name;
    string permissions, last_modified;
    double size;
    string unit;
    file_folder (string name, int type);
    bool is_file();
    bool is_folder();
    int get_stat();
};
file_folder :: file_folder(string name, int type){
    this->name_of_file_or_folder = name;
    this->d_type = type;
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
    if(lstat(this->name_of_file_or_folder.c_str(), &(this->sb)) == -1){
        cerr<<"Cant't execute stat"<<this->name_of_file_or_folder<<endl;
        cerr<<this->name_of_file_or_folder.c_str()<<endl;
        return 1;
    }
    //cout << "Debug " << endl;
    group *grp;
    passwd *pwd;
    grp = getgrgid(this->sb.st_gid);
    this->group_name = grp->gr_name;
    //cout << grou
    pwd = getpwuid(this->sb.st_uid);
    this->user_name = pwd->pw_name;
    //cout << user_name << " Debuf" << endl;
    int x = this->sb.st_mode;
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
    this->permissions = convert_octal_permission[temp[2]]+convert_octal_permission[temp[1]]+convert_octal_permission[temp[0]];
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
    this->last_modified = ctime(&this->sb.st_mtime);
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
    while((this->pDirent = readdir(this->direct)) != NULL)if(this->pDirent->d_name!=NULL){
        string file_name (this->pDirent->d_name);
        file_folder current(file_name, (int)this->pDirent->d_type);
        current.get_stat();
        this->all_files_folder.push_back(current);
    }
    return 0;
}
struct screen{
    int x_pos, y_pos;
    int number_of_rows, number_of_columns;
    int current_top, current_bottom;
    int current_position_in_history;
    bool normal;
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
};
void screen :: flush(){
    cout<<"e[2J";
    cout<<endl;
    cout<<"\e[1;1H";
    this->x_pos = this->y_pos = 1;
}
screen :: screen(FILE *in, FILE *out, string home){
    this->normal = true;
    this->HOME = home;
    this->input = in;
    this->output = out;
    if(home.empty()){
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        this->HOME = string(cwd);
    }
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
    this->current_top = 1;
    this->current_bottom = this->number_of_rows - 3;
    //debug4(this->current_bottom,this->current_directory.all_files_folder.size(),this->number_of_columns,this->number_of_rows);
    this->current_bottom = min(this->current_bottom ,(int) this->current_directory.all_files_folder.size()); 
    this->current_bottom += this->current_top;
}
void screen :: fill_screen(){
    this->flush();
    this->get_screen_size();
    cout<<"\e[1m"<<this->current_directory.current_directory<<"\e[0m"<<endl;
    cout<<this->current_directory.all_files_folder.size()<<endl;
    for(int line = current_top;line <= current_bottom ; line++){
        file_folder file_or_folder = this->current_directory.all_files_folder[line];
        cout<<file_or_folder.permissions<<" ";
        string x = file_or_folder.user_name;
        if(x.length()>8)x = x.substr(0,6)+"..";
        cout<<std::left<<setw(8)<<x<<" ";
        x = file_or_folder.group_name;
        if(x.length()>8)x = x.substr(0,6)+"..";
        cout<<std::left<<setw(8)<<x<<" ";
        if(file_or_folder.unit == "")cout<<left<<setw(6)<<file_or_folder.size<<" ";
        else {
            cout<<left<<setw(5)<<fixed<<setprecision(1)<<file_or_folder.size;
            cout<<file_or_folder.unit<<" ";
        }
        cout<<file_or_folder.last_modified.substr(0,24)<<" ";
        if(file_or_folder.is_folder())cout<<"\e[2m";
        cout<<file_or_folder.name_of_file_or_folder<<endl;
        if(file_or_folder.is_folder())cout<<"\e[0m";
    }
    cout<<endl;
}
int main(int argc, char* argv[]){
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
    int x;
    cin>>x;
    Terminal.switch_to_non_canonical_mode();
    cout<<"\e[?1049l";
    return 0;
}