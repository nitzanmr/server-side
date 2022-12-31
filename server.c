#include "string.h"
#include "stdlib.h"
#include "threadpool.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

char* error_message(int error_num){
    if(error_num == 400){
        return("400 bad request");
    }
}
int file_exist(char* path){
    
}
char** split_str(void*request,char* split_by){
    char* split_request[] = strtok(request,split_by);
    if(split_request== NULL){
        perror("error in spliting the path\n");
    }
    return split_request;
}
int accept_client(void* request){
    char** split_request = split_str(request," ");
    struct stat file_stats;

    if(strstr(split_request[0],"GET")==NULL){
        /*checks if the first value contains any other option than GET becuase we only support GET*/
        perror(error_message(501));
        return 1;
    }
   
    if(stat(split_request[1],&file_stats) < 0){
        perror(error_message(404));
        return 1;
    };
    if(S_ISDIR(file_stats.st_mode)){
        perror(error_message(302));

    }
    if((file_stats.st_mode & S_IROTH)){
        perror(error_message(403));
        return 1;
    }
};
int server(int argc, char* argv[]){
    /*argv == [port,pool_size,max number of requests]*/
    int counter_of_request = 0;
    if(argc !=3){
        /*check the number of values inserted to the server function if it is less then needed print bad request*/
        perror(error_message(400));
        return 1;
    }
    threadpool* new_threadpool = create_threadpool(argv[1]);
    if(new_threadpool==NULL){
        perror("threadpool didnt create it self");
        exit(1);
    }
    while(counter_of_request<argv[2]){
        accept_client();
        counter_of_request++;
    }
    destroy_threadpool(new_threadpool);
    
    if(fopen(argv[1]))
    

}