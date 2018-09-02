#include "command.h"

/*
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
command :: command(){
    this->Error = error :: getInstance();
}
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
}