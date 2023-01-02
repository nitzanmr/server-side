
#include "string.h"
#include "stdlib.h"
#include <stdio.h>
#include "threadpool.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
char* error_message(int error_num){
    if(error_num == 400){
        return("400 bad request");
    }
}

void split_str(char* request,char* split_by,char** split_request){
    // printf("\n%s\n",*request);
    int counter = 0;
    // char* token = strtok(request," ");
    // printf("\n%s\n",token);
    int start_of_word = 0;
    int i = 0;
    split_request[0] = request;
    printf("\n%s\n",split_request[counter]);
    while(request[i]!='\0'){
        if(request[i]==*split_by){
            printf("\ni is:%d request[%d] is: /%c/\n",i,i,request[i]);
            request[i] = '\0';
            printf("\n%s\n",request);
            counter++;
            split_request[counter] = request+i+1;
        }
        i++;
    }
    request[i] = '\0';
    for(int j = 0;j<3;j++){
        printf("\nthe split[%d] is:%s\n",j,split_request[j]);
    }
    printf("\n made it to the end\n");
}
int accept_client(void* request){
    char* split_by= " ";
    char* split_request[3];
    split_str((char*)request," ",split_request);
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
        if(split_request[1][strlen(split_request[1])]=='/'){
            perror(error_message(302));
        }
        else{
            char* new_name = (char*)malloc(strlen(split_request[1]) + strlen("index.html"));
            sprintf(new_name,"%s%s",split_request[1],"index.html");
            if(fopen(new_name,"r") < 0){
                DIR *d;
                struct dirent *dir;
                d = opendir(split_request[1]);
                if (d) {
                    while ((dir = readdir(d)) != NULL) {
                        printf("%s\n", dir->d_name);
                    }
                    closedir(d);
                }
            };
        }
    }
    if((file_stats.st_mode & S_IROTH)){
        perror(error_message(403));
        return 1;
    }
    return 0;
};
int server(int argc, char* argv[]){
    /*argv == [port,pool_size,max number of requests]*/
    // int counter_of_request = 0;
    // if(argc !=3){
    //     /*check the number of values inserted to the server function if it is less then needed print bad request*/
    //     perror(error_message(400));
    //     return 1;
    // }
    // threadpool* new_threadpool = create_threadpool(argv[1]);
    // if(new_threadpool==NULL){
    //     perror("threadpool didnt create it self");
    //     exit(1);
    // }
    // while(counter_of_request<argv[2]){
    //     // accept_client();
    //     counter_of_request++;
    // }
    // destroy_threadpool(new_threadpool);    

}