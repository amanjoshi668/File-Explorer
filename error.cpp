#include "common.h"
#include "error.h"
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
}