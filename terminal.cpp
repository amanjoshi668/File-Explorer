#include "terminal.h"
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
}