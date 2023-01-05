
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
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#define SA struct sockaddr 
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
char *get_mime_type(char *name) {
    char *ext = strrchr(name, '.');
    if (!ext) return NULL;
    if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) return "text/html";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".gif") == 0) return "image/gif";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".au") == 0) return "audio/basic";
    if (strcmp(ext, ".wav") == 0) return "audio/wav";
    if (strcmp(ext, ".avi") == 0) return "video/x-msvideo";
    if (strcmp(ext, ".mpeg") == 0 || strcmp(ext, ".mpg") == 0) return "video/mpeg"; if (strcmp(ext, ".mp3") == 0) return "audio/mpeg";
    return NULL;
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
int read_and_write_file(int fd_socket,char* path,int file_size,char* buf){
    FILE* ptr;
    ptr = open(path,"r");
    int read_val = 0;
    int write_val = 0;
    if (NULL == ptr) {
        printf("file can't be opened \n");
        return 1;
    }
    
    while(1){
            read_val=read(ptr,buf,512);
            
            if(read_val== -1){
                perror("read fiile error");
                return 1;
            }

            write_val = write(fd_socket,buf,512);
            if(write_val == -1){
                perror("write to server failed\n");
                return 1;
            }
            if(read_val == 0){
                return 0;
            }
    }

}
int accept_client(void* request,char* buf,int fd){
    char* split_by= " ";
    char* split_request[3];
    split_str((char*)request," ",split_request);
    struct stat file_stats;
    printf("made it here\n");
    if(split_request[0] == NULL || split_request[1] == NULL || split_request[2] == NULL){
        /*check the number of values inserted to the server function if it is less then needed print bad request*/
        error_message(400,buf,NULL);
        write(fd,buf,strlen(buf));
        return 1;
    }
    if(strstr(split_request[0],"GET")==NULL){
        /*checks if the first value contains any other option than GET becuase we only support GET*/
        error_message(501,buf,NULL);
        write(fd,buf,strlen(buf)); 
        return 1;
    }
   if(stat(split_request[1],&file_stats) < 0){
        error_message(404,buf,NULL);
        write(fd,buf,strlen(buf));
        return 1;
    };
    
    if(S_ISDIR(file_stats.st_mode)){
        if(split_request[1][strlen(split_request[1])]=='/'){
            error_message(302,buf,NULL);
            write(fd,buf,strlen(buf));        }
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
    if((!file_stats.st_mode & S_IROTH)){
        error_message(403,buf,NULL);
        write(fd,buf,strlen(buf));
        return 1;
    }
    /*add a return file value*/
    else{
        printf("entered the get file part\n");
        #define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT" 
        struct stat stats;
        if(stat(split_request[1],&stats)==1){
            perror("stat 200 failed");
            return 1;
        }
        time_t now;
        char timebuf[128];
        char timebuf_mtime[128];
        now = time(NULL);
        strftime(timebuf_mtime,sizeof(timebuf_mtime),RFC1123FMT,gmtime(&stats.st_mtime));
        strftime(timebuf, sizeof(timebuf), RFC1123FMT, gmtime(&now));
        char* type = get_mime_type(split_request[1]);
        sprintf(buf,"HTTP/1.1 %s\r\nServer: webserver/1.0\r\nDate: %sContent-Type: %s\r\nContent-Length: %d\r\nLast-Modified: %sConnection: closed\r\n\r\n", "200 OK",type,timebuf,stats.st_size*8,timebuf_mtime);
        write(fd,buf,strlen(buf));
        printf("after write to the client\n");
        read_and_write_file(fd,split_request[1],stats.st_size,buf);
    }
    return 0;
};
int client_read(void* arg){
    int* fd = (int*)arg;
    char buf[512];
    char returned_buf[512];
    int valread = 0;
    int counter = 0;
    while(1){
        valread = read(*fd,buf+counter,1);
        // printf("%c",)
        printf("valread is: %d\n",valread);
        if(buf[counter]=='\n'){
            buf[counter-1] = '\0';
            break;
        }
        printf("counter is: %d\n",counter);
        printf("read[%d]: %c\n",counter,buf[counter]);
        counter++;
    }
    printf("\nput on the eof\n");

    accept_client(buf,returned_buf,*fd);
    printf("finshed the client\n");
    return 0;
}
int create_server(int port,int number_of_request,threadpool* new_threadpool){
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
    int counter_of_request = 0;
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
   
    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
   
    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);
   
    // Accept the data packet from client and verification
    while(counter_of_request < number_of_request){    
        connfd = accept(sockfd, (SA*)&cli, &len);
        if (connfd < 0) {
            printf("server accept failed...\n");
            exit(0);
        }
        else
            printf("server accept the client...\n");
        // Function for chatting between client and server

        //write a function that sends the read from the client to the accept_client_func 
        //then send the returned value to the client.
        client_read((void*)&connfd);
        dispatch(new_threadpool,(dispatch_fn)client_read,(void*)&connfd);
        counter_of_request++;
    }
   
    // After chatting close the socket
    close(sockfd);
    return 0;
}

int server(int argc, char* argv[]){
    /*argv == [port,pool_size,max number of requests]*/
    int counter_of_request = 0;
    if(argc<3){
        perror("too little arguments entered to the server\n");
        return 1;
    }
    threadpool* new_threadpool = create_threadpool(atoi(argv[1]));
    if(new_threadpool==NULL){
        perror("threadpool didnt create it self");
        exit(1);
    }
    create_server(atoi(argv[0]),atoi(argv[2]),new_threadpool);
    destroy_threadpool(new_threadpool);    
    return 0;
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

    #define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT" 
    time_t now;
    char timebuf[128];
    now = time(NULL);
    strftime(timebuf, sizeof(timebuf), RFC1123FMT, gmtime(&now));
    //  sprintf(gmt_date, asctime(gmtime(epoch)));
   
    // printf("%jd seconds since the epoch began\n", (intmax_t)epoch);
    // printf("%s", asctime(gmtime(&epoch)));
    //  printf("\nsigsegv here\n");
    if(path!=NULL){
        char location[50];
        sprintf(location,"Location: %s\r\n",path);
        sprintf(error_message,"HTTP/1.1 %s\r\nServer: webserver/1.0\r\nDate: %s%sContent-Type: text/html\r\nContent-Length: %d\r\nConnection: closed\r\n\r\n", error_spciefed,timebuf,location,content_length);

    }
    else{
        sprintf(error_message,"HTTP/1.1 %s\r\nServer: webserver/1.0\r\nDate: %sContent-Type: text/html\r\nContent-Length: %d\r\nConnection: closed\r\n\r\n", error_spciefed,timebuf,content_length);

    }

    printf("\n%s\n",error_message);
}