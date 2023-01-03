
#include "string.h"
#include "stdlib.h"
#include <stdio.h>
#include "threadpool.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <stdint.h>
void build_header_m(char*, char*,int,char* path);
void error_message(int error_num,char* buf,char* path){
    if(error_num == 400){
        build_header_m(buf,"400 bad request",strlen("<HTML><HEAD><TITLE>400 Bad Request</TITLE></HEAD>\r\n<BODY><H4>400 Bad request</H4>\r\nBad Request.\r\n</BODY></HTML>\0"),path);
        sprintf(buf,"<HTML><HEAD><TITLE>400 Bad Request</TITLE></HEAD>\r\n<BODY><H4>400 Bad request</H4>\r\nBad Request.\r\n</BODY></HTML>");
        return;
    }
    if(error_num ==501){
        build_header_m(buf,"501 Not supported",strlen("<HTML><HEAD><TITLE>501 Not supported</TITLE></HEAD>\r\n<BODY><H4>501 Not supported</H4>\r\nMethod is not supported.\r\n</BODY></HTML>\0"),path);
        sprintf(buf,"<HTML><HEAD><TITLE>501 Not supported</TITLE></HEAD>\r\n<BODY><H4>501 Not supported</H4>\r\nMethod is not supported.\r\n</BODY></HTML>");
        return;
    }
    if(error_num == 500){
        build_header_m(buf,"500 Internal Server Error",strlen("<HTML><HEAD><TITLE>500 Internal Server Error</TITLE></HEAD>\r\n<BODY><H4>500 Internal Server Error</H4>\r\nSome server side error.\r\n</BODY></HTML>\0"),path);
        sprintf(buf,"<HTML><HEAD><TITLE>500 Internal Server Error</TITLE></HEAD>\r\n<BODY><H4>500 Internal Server Error</H4>\r\nSome server side error.\r\n</BODY></HTML>");
        return;
    }
    if(error_num == 404){
        build_header_m(buf,"404 Not Found",strlen("<HTML><HEAD><TITLE>404 Not Found</TITLE></HEAD>\r\n<BODY><H4>404 Not Found</H4>\r\nFile not found.\r\n</BODY></HTML>\0"),path);
        sprintf(buf,"<HTML><HEAD><TITLE>404 Not Found</TITLE></HEAD>\r\n<BODY><H4>404 Not Found</H4>\r\nFile not found.\r\n</BODY></HTML>");
        return;
    }
    if(error_num == 403){
        build_header_m(buf,"403 Forbidden",strlen("<HTML><HEAD><TITLE>403 Forbidden</TITLE></HEAD>\r\n<BODY><H4>403 Forbidden</H4>\r\nAccess denied.\r\n</BODY></HTML>\0"),path);
        sprintf(buf,"<HTML><HEAD><TITLE>403 Forbidden</TITLE></HEAD>\r\n<BODY><H4>403 Forbidden</H4>\r\nAccess denied.\r\n</BODY></HTML>");
        return;
    }
    if(error_num == 302){
        build_header_m(buf,"302 Found",strlen("<HTML><HEAD><TITLE>302 Found</TITLE></HEAD>\r\n<BODY><H4>302 Found</H4>\r\nDirectories must end with a slash.\r\n</BODY></HTML>\0"),path);
        sprintf(buf,"<HTML><HEAD><TITLE>302 Found</TITLE></HEAD>\r\n<BODY><H4>302 Found</H4>\r\nDirectories must end with a slash.\r\n</BODY></HTML>");
        return;
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
        char error[500];
        error_message(501,error);
        perror(error);
        return 1;
    }
   
    if(stat(split_request[1],&file_stats) < 0){
        char error[500];
        error_message(404,error,NULL);
        perror(error);
        return 1;
    };
    if(S_ISDIR(file_stats.st_mode)){
        if(split_request[1][strlen(split_request[1])]=='/'){
             char error[500];
            error_message(302,error,split_request[1]);
            perror(error);
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
        char error[500];
        error_message(403,error,NULL);
        perror(error);
        return 1;
    }
    return 0;
};
int server(int argc, char* argv[]){
    /*argv == [port,pool_size,max number of requests]*/
    int counter_of_request = 0;
    if(argc !=3){
        /*check the number of values inserted to the server function if it is less then needed print bad request*/
        char error[500];
        error_message(400,error,argv[1]);
        perror(error);
        return 1;
    }
    threadpool* new_threadpool = create_threadpool(argv[1]);
    if(new_threadpool==NULL){
        perror("threadpool didnt create it self");
        exit(1);
    }
    while(counter_of_request<argv[2]){
        // accept_client();
        counter_of_request++;
    }
    destroy_threadpool(new_threadpool);    

}
void build_header_m(char* error_message ,char* error_spciefed,int content_length,char* path){
    /* format: this is only the format for 400.
    HTTP/1.1 400 Bad Request
    Server: webserver/1.0
    Date: Fri, 05 Nov 2010 13:50:33 GMT
    Content-Type: text/html
    Content-Length: 113
    Connection: close

    <HTML><HEAD><TITLE>400 Bad Request</TITLE></HEAD>
    <BODY><H4>400 Bad request</H4>
    Bad Request.
    </BODY></HTML> */

    char* gmt_date;
    char location[50];
    if(path!=NULL){
         sprintf(location,"Location: %s",path);
    }
    else{
        location[0] = "\0";
    }
    //  sprintf(gmt_date, asctime(gmtime(epoch)));
      time_t epoch = 0;
      time(&epoch);
    // printf("%jd seconds since the epoch began\n", (intmax_t)epoch);
    // printf("%s", asctime(gmtime(&epoch)));
    //  printf("\nsigsegv here\n");


    sprintf(error_message,"HTTP/1.1 %s\r\nServer: webserver/1.0\r\nDate: %s\r\n%sContent-Type: text/html\r\nContent-Length: %d\r\nConnection: closed\r\n\r\n", error_spciefed,asctime(gmtime(&epoch)),location,content_length);
    printf("\n%s\n",error_message);
}