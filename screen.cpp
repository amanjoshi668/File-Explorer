#include "screen.h"

screen :: screen(FILE *in, FILE *out, string home=""){
    this->Error = error :: getInstance();
    this->normal = true;
    this->HOME = home;
    this->input = in;
    this->output = out;
    this->command_pos = 1;
    this->isSearch = false;
    if(home.empty()){
        char cwd[PATH_MAX];
        if(getcwd(cwd, sizeof(cwd)) == NULL){
            this->Error->is_error = true;
            this->Error->error_code += "can't open home directory";
            cerr << "Cant't open Home Directory " << endl;
            return;
        }
        this->HOME = string(cwd);
    }
    cerr<<"this is home "<<" "<<this->HOME;
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

void screen :: search(string source="", string file_name=""){
    if(source.empty()){
        source = this->current_directory.current_directory;
        this->search_elems.first = source;
    }
    if(file_name.empty()){
        file_name = this->Command.arguments[0];
        this->search_elems.second = file_name;
    }
    string new_name = "Search results of " + file_name + " in " + source;
    vector<string> search_results;
    //derr2(source,file_name);
    recursive_search(source, file_name, search_results);
    this->x_pos = 2;
    this->current_top = 1;
    this->y_pos = 1;
    this->search_title = new_name;
    this->current_directory.all_files_folder.clear();
    for(string file_folder_name : search_results){
        file_folder current(file_folder_name, 9, file_folder_name);
        current.get_stat();
        this->current_directory.all_files_folder.push_back(current);
    }
    sort(all(this->current_directory.all_files_folder));
    this->isSearch = true;
}

void screen :: recursive_search(string source, string file_to_search, vector<string> &search_results){
    //derr(source);
    struct stat tmp ={0};
    if(stat(source.c_str(), &tmp) == -1){
        this->Error->is_error = true;
        this->Error->error_code += "Cant't Open" + source;
        cerr<<"Error in opening "<<source<<endl;
        return ;
    }
    int x = stat(source.c_str(),&tmp);
    DIR *direct = opendir(source.c_str());
    if(direct == NULL){
        this->Error->is_error = true;
        this->Error->error_code += "Cant'Open Directory" + source;
        cerr<<"Can't open Directory "<<source<<endl;
        return ;
    }
    dirent *pDirent;
    while((pDirent = readdir(direct)) != NULL)if(string(pDirent->d_name)!="." and string(pDirent->d_name)!=".."){
        string file_name = "/" + string(pDirent->d_name);
        string source_file_name = source + file_name;
        if(string(pDirent->d_name) == file_to_search){
            search_results.push_back(source_file_name);
        }
        if(pDirent->d_type == 4) {
            recursive_search(source_file_name, file_to_search, search_results);
        }
    }
    return;
}

void screen :: flush(){
    cout<<"\e[2J";
    cout<<"\e[1;1H";
}

void screen :: get_screen_size(){
    struct winsize size;
    if(ioctl(fileno(this->input), TIOCGWINSZ, &size)!=0){
        this->Error->is_error = true;
        this->Error->error_code += "Can't fetch screen size";
    }
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
    //cerr<<"curDIR"<<this->current_directory.current_directory<<endl;
    if(!this->isSearch){
        cout<<"\e[1m"<<this->current_directory.current_directory<<"\e[0m"<<endl;
    }
    else{
        cout<<"\e[1m"<<this->search_title<<"\e[0m"<<endl;
    }
    int width = to_string(this->current_directory.all_files_folder.size()).length();
    //zcout<<this->current_bottom<<endl;
    for(int line = this->current_top;line < this->current_bottom; line++){
        file_folder file_or_folder = this->current_directory.all_files_folder[line-1];
        cout << std::left << setw(width) << line  << "  ";
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
        if(file_or_folder.name_of_file_or_folder.length()>this->number_of_columns-width-67){
            cout<<file_or_folder.name_of_file_or_folder.substr(0,this->number_of_columns-width-70)<<"..."<<endl;
        }
        else {
            cout<<file_or_folder.name_of_file_or_folder<<endl;
        }
        if(file_or_folder.is_folder()){
            cout<<"\e[0m";
        }
    }
    cout<<"\e["<<this->number_of_rows<<";1H";
    if(this->normal){
        if(this->Error->is_error){
            this->Error->print_error(true, this->number_of_rows);
        }
        else{
           cout<<"Normal Mode";
        }
    }
    else {
        cout<<"\e["<<this->number_of_rows<<";1H";
        cout<<"Command Mode :";
    }
    cout<<"\e["<<this->x_pos<<";"<<y_pos<<"H";
    //debug4(this->current_top,this->current_bottom,this->x_pos, this->current_directory.all_files_folder.size());
}

void screen :: change_directory(string new_path, int position){
    if(this->isSearch == true){
        return ;
    }
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
    this->change_directory(this->history[this->current_position_in_history+1], this->current_position_in_history + 1 );
}

void screen :: move_left(){
    if(this->isSearch){
        this->isSearch = false;
        this->change_directory(this->history[this->current_position_in_history],this->current_position_in_history);
        this->fill_screen();
        return;
    }
    if(this->current_position_in_history  == 0)return;
    this->normal = true;
    this->change_directory(this->history[current_position_in_history-1] , this->current_position_in_history - 1 );
}

void screen :: move_home(){
    this->normal = true;
    this->change_directory(this->HOME , this->current_position_in_history + 1);
}

void screen :: move_back(){
    if(this->isSearch){
        this->isSearch = false;
        this->change_directory(this->history[this->current_position_in_history],this->current_position_in_history);
        this->fill_screen();
        return;
    }
    if(this->current_directory.current_directory == this->HOME)return;
    string new_path = this->current_directory.current_directory.substr(0, this->current_directory.current_directory.find_last_of("/"));
    this->normal = true;
    this->change_directory(new_path, this->current_position_in_history + 1);
}

void screen :: move_into(){
    //freopen("/home/aman/Documents/OS/file_explorer/error.txt", "w", stderr);
    if (this->current_directory.all_files_folder[ this->x_pos + this->current_top - 3 ].is_folder()){
        string path_name = this->current_directory.all_files_folder[ this->x_pos + this->current_top - 3 ].name_of_file_or_folder;
        if( path_name == "."){
            return;
        }
        else if(path_name == ".."){
            move_back();
            return;
        }
        if(!isSearch)path_name = this->current_directory.current_directory + "/" + path_name;
        this->normal = true;
        this->isSearch = false;
        while(this->history.size() > this->current_position_in_history + 1){
            history.pop_back();
        }
        this->change_directory( path_name, this->current_position_in_history + 1);
    }
    else{
        pid_t pid;
        pid = fork();
        if(pid == 0){
            //Execute the file;
            string file_to_execute = this->current_directory.all_files_folder[this->x_pos + this->current_top - 3].name_of_file_or_folder_for_stat ;
            if(execl("/usr/bin/xdg-open", "xdg-open", file_to_execute.c_str(), NULL)!=0){
                this->Error->is_error = true;
                this->Error->error_code += "Cant't open " + file_to_execute;
                cerr << "Cant't open " << file_to_execute <<endl;
            }
        }
        else if(pid < 0){
            this->Error->is_error = true;
            this->Error->error_code += "Cant't fork";
        }
        else {
            this->normal = true;
            this->fill_screen();
            return;
        }
    }
}

void screen :: goto_location(){
    while(this->history.size() > this->current_position_in_history + 1){
        history.pop_back();
    }
    this->isSearch = false;
    if(this->Command.arguments[0] == "~/"){
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

void screen :: recursive_copy(string source, string destination){
    struct stat tmp ={0};
    if(stat(source.c_str(), &tmp) == -1){
        this->Error->is_error = true;
        this->Error->error_code += "Error in opening " + source;
        cerr<<"Error in opening copy "<<source<<endl;
        return ;
    }
    if (!S_ISDIR(tmp.st_mode)){
        this->copy_file(source, destination);
        return;
    }
    DIR *direct = opendir(source.c_str());
    if(direct == NULL){
        this->Error->is_error = true;
        this->Error->error_code += "Cant't Open File" + source;
        //cerr<<"Can't open Directory "<<source<<endl;
        cerr << "Cant't open Directory copy" << source <<endl;
        return ;
    }
    dirent *pDirent;
    this->create_dir(destination, 0700);
    while((pDirent = readdir(direct)) != NULL)if(string(pDirent->d_name)!="." and string(pDirent->d_name)!=".."){
        string file_name = "/" + string(pDirent->d_name);
        string source_file_name = source + file_name;
        string destination_file_name = destination + file_name;
        recursive_copy(source_file_name, destination_file_name);
    }
    chmod(destination.c_str(), tmp.st_mode);
    cerr<<"Copy Done";
    return ;
}

void screen :: snapshot(){
    if(this->Command.arguments.size()<2){
        this->Error->is_error = true;
        this->Error->error_code += "Too few arguments for snapshot";
        return;
    }
    string write_file = this->Command.arguments[1];
    if(write_file[0]=='~'){
        write_file = write_file.substr(2,write_file.size()-2);
        write_file = this->HOME + write_file;
    }
    else if(write_file[0] == '.'){
        write_file = write_file.substr(1,write_file.size()-1);
        write_file = this->current_directory.current_directory  + write_file;
    }
    else{
        write_file = this->current_directory.current_directory+"/"+write_file;
    }
    string source = this->Command.arguments[0];
    if(source[0]=='~'){
        source = source.substr(2,source.size()-2);
        source = this->HOME + source;
    }
    else if(source[0] == '.'){
        source = source.substr(1,source.size()-1);
        source = this->current_directory.current_directory  + source;
    }
    else{
        source = this->current_directory.current_directory+"/"+source;
    }
    struct stat tmp ={0};
    if(stat(write_file.c_str(), &tmp) == -1){
        int out_fd = creat(write_file.c_str(), __mode_t(0777));
        if(out_fd == -1){
            this->Error->is_error = true;
            this->Error->error_code = "Can't create"+write_file;
        }
    }
    recursive_snapshot(source, write_file, ".");
    return;
}

void screen :: recursive_snapshot(string source, string destination, string current){
    struct stat tmp ={0};
    if(stat(source.c_str(), &tmp) == -1){
        this->Error->is_error = true;
        this->Error->error_code += "Error in opening " + source;
        cerr<<"Error in opening "<<source<<endl;
        return ;
    }
    if (!S_ISDIR(tmp.st_mode)){
        return;
    }
    DIR *direct = opendir(source.c_str());
    if(direct == NULL){
        this->Error->is_error = true;
        this->Error->error_code += "Cant't Open File" + source;
        cerr<<"Can't open Directory "<<source<<endl;
        return ;
    }
    ofstream out(destination,ios :: app);
    out<<current<<" :"<<endl;
    dirent *pDirent;
    while((pDirent = readdir(direct)) != NULL)if(string(pDirent->d_name)!="." and string(pDirent->d_name)!=".."){
        out<<string(pDirent->d_name)<<" ";
    }
    out<<endl<<endl;
    closedir(direct);
    out.close();

    direct = opendir(source.c_str());
    if(direct == NULL){
        this->Error->is_error = true;
        this->Error->error_code += "Cant't Open File" + source;
        cerr<<"Can't open Directory "<<source<<endl;
        return ;
    }
    while((pDirent = readdir(direct)) != NULL)if(string(pDirent->d_name)!="." and string(pDirent->d_name)!=".."){
        string file_name = "/" + string(pDirent->d_name);
        string source_file_name = source + file_name;
        string current_file_name = current + file_name;
        recursive_snapshot(source_file_name, destination, current_file_name);
    }
    return ;
}

void screen :: copy(){
    string destination = this->Command.arguments.back();
    if(destination[0]=='~'){
        destination = destination.substr(2,destination.size()-2);
        destination = this->HOME + destination;
    }
    else if(destination[0] == '.'){
        destination = destination.substr(1,destination.size()-1);
        destination = this->current_directory.current_directory  + destination;
    }
    else{
        destination = this->current_directory.current_directory+"/"+destination;
    }
    struct stat tmp ={0};
    if(stat(destination.c_str(), &tmp) == -1){
        this->Error->is_error = true;
        this->Error->error_code += "Error in opening " + destination;
        cerr<<"Error in opening "<<destination<<endl;
        return ;
    }
    for(int i=0;i<this->Command.arguments.size()-1 ;i++){
        string source = this->Command.arguments[i];
        if(this->Command.arguments[i][0]=='~'){
            source = source.substr(2,source.size()-2);
            source = this->HOME + source;
        }
        else if(this->Command.arguments[i][0] == '.'){
            source = source.substr(1,source.size()-1);
            source = this->current_directory.current_directory  + source;
        }
        else{
            source = this->current_directory.current_directory+"/"+source;
        }
        string destination_file = destination + "/" + this->Command.arguments[i];
        //derr2(source,destination_file);
        if(source == destination_file){
            Error->is_error = true;
            Error->error_code = "Source and destination can't be same";
            continue;
        }
        recursive_copy(source, destination_file);
    }   
    return;
}

void screen :: __delete(){
    string source;
    if(this->Command.arguments[0][0]=='~'){
        source = this->Command.arguments[0];
        source = source.substr(2,source.size()-2);
        source = this->HOME + source;
    }
    else if(this->Command.arguments[0][0] == '.'){
        source = source.substr(1,source.size()-1);
        source = this->current_directory.current_directory + source;
    }
    else{
        source = this->current_directory.current_directory+"/"+this->Command.arguments[0];
    }
    if(source.find(this->current_directory.current_directory)== string :: npos){
        this->Error->is_error = true;
        this->Error->error_code += "Can't delete at this location move to parent of the path you entered";
        return;
    }
    recursive_delete(source);
    return;
}

void screen :: recursive_delete(string source){
    //derr(source);
    struct stat tmp ={0};
    if(stat(source.c_str(), &tmp) == -1){
        this->Error->is_error = true;
        this->Error->error_code += "Cant't open file1" + source;
        cerr<<"Error in opening I aM buf "<<source<<endl;
        return ;
    }
    if (!S_ISDIR(tmp.st_mode)){
        int x = unlink(source.c_str());
        if(x!=0){
            this->Error->is_error = true;
            this->Error->error_code += "Cant't Delete File2" + source;
            cerr<<"Can't delete file "<<source<<endl;
            return;
        }
        return;
    }
    DIR *direct = opendir(source.c_str());
    if(direct == NULL){
        this->Error->is_error = true;
        this->Error->error_code += "Cant't Open File3" + source;
        cerr<<"Can't open Directory "<<source<<endl;
        return ;
    }
    dirent *pDirent;
    while((pDirent = readdir(direct)) != NULL)if(string(pDirent->d_name)!="." and string(pDirent->d_name)!=".."){
        string file_name = "/" + string(pDirent->d_name);
        string source_file_name = source + file_name;
        recursive_delete(source_file_name);
    }
    int x = rmdir(source.c_str());
    if(x!=0){
        this->Error->is_error = true;
        this->Error->error_code += "Cant't Delete Folder4" + source;
        cerr<<"Can't open Directory "<<source<<endl;
        return ;
    }
    return;
}

void screen :: copy_file(string source, string destination){
    struct stat tmp ={0};
    stat(source.c_str(),&tmp);
    int in_fd = open(source.c_str(), O_RDONLY);
    int out_fd = creat(destination.c_str(), __mode_t(0700));
    if(in_fd ==-1){
        this->Error->is_error = true;
        this->Error->error_code += "Cant't Open" + source;
        cerr<<"Can't open files"<<source<<endl;
        return;
    }
    else if(out_fd==-1){
        this->Error->is_error = true;
        this->Error->error_code += "Cant't Open " + destination;
        cerr<<"Can't open files"<<destination<<endl;
        return;
    }
    char buff[BUFFERSIZE];
    int n_chars;
    while( (n_chars = read(in_fd, buff, BUFFERSIZE)) > 0 ){
        if( write(out_fd, buff, n_chars) != n_chars ){
            this->Error->is_error = true;
            this->Error->error_code += "Write error to " + destination;
            cerr<<"Write error to "<<destination<<endl;
            return;
        }    
        if( n_chars == -1 ){
            this->Error->is_error = true;
            this->Error->error_code += "Read error from " + source;
            cerr<<"Read error from "<<source<<endl;
            return;
        }
    }
    chmod(destination.c_str(), tmp.st_mode);
    return;
}

void screen :: create_dir(string directory="", __mode_t t = 0700){
    struct stat st = {0};
    string pathname = this->Command.arguments[0];
    if(pathname[0]=='~'){
        pathname = this->Command.arguments[0];
        pathname = pathname.substr(2,pathname.size()-2);
        pathname = this->HOME + pathname;
    }
    else if(pathname[0] == '.'){
        pathname = pathname.substr(1,pathname.size()-1);
        pathname = this->current_directory.current_directory + pathname;
    }
    else{
        pathname = this->current_directory.current_directory+"/"+this->Command.arguments[0];
    }
    if(!directory.empty()){
        pathname = directory;
    }
    if(mkdir(pathname.c_str(),t) == -1){
        this->Error->is_error = true;
        this->Error->error_code += "Cant't Create" + pathname;
        cerr<<"Error in Creating "<<pathname<<endl;
        return ;
    }
}

void screen :: create_file(){
    struct stat st = {0};
    string pathname = "";
    if(this->Command.arguments[1]=="."){
        pathname = this->current_directory.current_directory;
        //cerr<<"I am awesome fdjfkd"<<this->current_directory.current_directory<<endl;
    }
    else {
        pathname = this->HOME + this->Command.arguments[1].substr(1,this->Command.arguments[1].size()-1);
    }
    pathname = pathname + "/" + this->Command.arguments[0];
    //derr(pathname);
    int out_fd = creat(pathname.c_str(), __mode_t(0700));
    if(out_fd == -1){
        this->Error->is_error = true;
        this->Error->error_code += "Cant't open files" + pathname;
        cerr<<"Can't open files"<<endl;
        return;
    }
}
void screen :: move(){
    string destination = this->Command.arguments.back();
    if(destination[0]=='~'){
        destination = destination.substr(2,destination.size()-2);
        destination = this->HOME + destination;
    }
    else if(destination[0] == '.'){
        destination = destination.substr(1,destination.size()-1);
        destination = this->current_directory.current_directory  + destination;
    }
    else{
        destination = this->current_directory.current_directory+"/"+destination;
    }
    struct stat tmp ={0};
    if(stat(destination.c_str(), &tmp) == -1){
        this->Error->is_error = true;
        this->Error->error_code += "Error in opening " + destination;
        cerr<<"Error in opening "<<destination<<endl;
        return ;
    }
    for(int i=0;i<this->Command.arguments.size()-1 ;i++){
        string source = this->Command.arguments[i];
        if(source[0]=='~'){
            source = source.substr(2,source.size()-2);
            source = this->HOME + source;
        }
        else if(source[0] == '.'){
            source = source.substr(1,source.size()-1);
            source = this->current_directory.current_directory  + source;
        }
        else{
            source = this->current_directory.current_directory+"/"+this->Command.arguments[0];
        }
        string destination_file = destination + "/" + this->Command.arguments[i];
        //derr2(source,destination_file);
        recursive_copy(source, destination_file);
        recursive_delete(source);
    }   
    return;
}

void screen :: rename(){
    if(this->Command.arguments.size()<2){
        this->Error->is_error = true;
        this->Error->error_code += "Too few arguments for rename";
        return;
    }
    string destination = this->Command.arguments[1];
    if(destination[0]=='~'){
        destination = destination.substr(2,destination.size()-2);
        destination = this->HOME + destination;
    }
    else if(destination[0] == '.'){
        destination = destination.substr(1,destination.size()-1);
        destination = this->current_directory.current_directory  + destination;
    }
    else{
        destination = this->current_directory.current_directory+"/"+destination;
    }
    string source = this->Command.arguments[0];
    if(source[0]=='~'){
        source = source.substr(2,source.size()-2);
        source = this->HOME + source;
    }
    else if(source[0] == '.'){
        source = source.substr(1,source.size()-1);
        source = this->current_directory.current_directory  + source;
    }
    else{
        source = this->current_directory.current_directory+"/"+source;
    }
    recursive_copy(source,destination);
    recursive_delete(source);
}

void screen :: execute_command(){
    if(this->Command.command_type == "copy"){
        this->copy();
    }
    else if(this->Command.command_type == "move"){
        this->move();
    }
    else if(this->Command.command_type == "rename"){
        this->rename();
    }
    else if(this->Command.command_type == "create_file"){
        this->create_file();
        //cout<<"I am ‘:create_file";
    }
    else if(this->Command.command_type == "create_dir"){
        this->create_dir();
        //cout<<"I am ‘:create_dir";
    }
    else if(this->Command.command_type == "delete_file" or this->Command.command_type == "delete_dir"){
        //do delete;
        this->__delete();
    }
    else if(this->Command.command_type == "goto"){
        this->goto_location();
    }
    else if(this->Command.command_type == "search"){
        //do search;
        this->search();
    }else if(this->Command.command_type == "snapshot"){
        //do snapshot;
        this->snapshot();
    }
    else {
        this->Error->is_error = true;
        this->Error->error_code += "You have not entered a VALID Command";
    }
    if(!this->isSearch){
        this->change_directory(this->current_directory.current_directory, this->current_position_in_history);
    }
    else {
        this->search(this->search_elems.first, this->search_elems.second);
    }
    this->fill_screen();
    this->normal = false;
    this->command_mode();
}

void screen :: command_mode(){
    this->Command.command_type.clear();
    this->Command.arguments.clear();
    cout<<"\e["<<this->number_of_rows<<";1H";
    this->command_pos = 13;
    cout<<"Command Mode :";
    this->command_pos += 2;
    if(this->Error->is_error){
        this->Error->print_error(false, this->number_of_rows);
    }
    if(this->Command.take_command_input(this->input, this->output, this->command_pos, this->number_of_rows) != 0){
        this->normal = true;
        this->fill_screen();
        return ;
    }
    this->execute_command();
}
