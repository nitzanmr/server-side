#include "string.h"
#include "stdlib.h"

char* error_message(int error_num){
    if(error_num == 400){
        return("400 bad request");
    }
}
int file_exist(char* path){
    
}
int server(int argc, char* argv[]){
    /*argv == [port,pool_size,max number of requests]*/
    if(argc !=3){
        /*check the number of values inserted to the server function if it is less then needed print bad request*/
        perror(error_message(400));
        return 1;
    }
    if(strstr(argv[0],"GET")==NULL){
        /*checks if the first value contains any other option than GET becuase we only support GET*/
        perror(error_message(501));
        return 1;
    }
    

}