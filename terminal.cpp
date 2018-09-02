#include "terminal.h"
#include "error.h"
#include "common.h"
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
