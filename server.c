/*finshed product*/
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
#include <fcntl.h>
#define _OPEN_SYS_ITOA_EXT
#define SA struct sockaddr 
void build_header_m(char*, char*,int,char* path);
void error_message(int error_num,char* buf,char* path){
    if(error_num == 400){
        build_header_m(buf,"400 bad request",strlen("<HTML><HEAD><TITLE>400 Bad Request</TITLE></HEAD>\r\n<BODY><H4>400 Bad request</H4>\r\nBad Request.\r\n</BODY></HTML>\0"),path);
        strcat(buf,"<HTML><HEAD><TITLE>400 Bad Request</TITLE></HEAD>\r\n<BODY><H4>400 Bad request</H4>\r\nBad Request.\r\n</BODY></HTML>");
        return;
    }
    if(error_num ==501){
        build_header_m(buf,"501 Not supported",strlen("<HTML><HEAD><TITLE>501 Not supported</TITLE></HEAD>\r\n<BODY><H4>501 Not supported</H4>\r\nMethod is not supported.\r\n</BODY></HTML>\0"),path);
        strcat(buf,"<HTML><HEAD><TITLE>501 Not supported</TITLE></HEAD>\r\n<BODY><H4>501 Not supported</H4>\r\nMethod is not supported.\r\n</BODY></HTML>");
        return;
    }
    if(error_num == 500){
        build_header_m(buf,"500 Internal Server Error",strlen("<HTML><HEAD><TITLE>500 Internal Server Error</TITLE></HEAD>\r\n<BODY><H4>500 Internal Server Error</H4>\r\nSome server side error.\r\n</BODY></HTML>\0"),path);
        strcat(buf,"<HTML><HEAD><TITLE>500 Internal Server Error</TITLE></HEAD>\r\n<BODY><H4>500 Internal Server Error</H4>\r\nSome server side error.\r\n</BODY></HTML>");
        return;
    }
    if(error_num == 404){
        build_header_m(buf,"404 Not Found",strlen("<HTML><HEAD><TITLE>404 Not Found</TITLE></HEAD>\r\n<BODY><H4>404 Not Found</H4>\r\nFile not found.\r\n</BODY></HTML>\0"),path);
        strcat(buf,"<HTML><HEAD><TITLE>404 Not Found</TITLE></HEAD>\r\n<BODY><H4>404 Not Found</H4>\r\nFile not found.\r\n</BODY></HTML>");
        return;
    }
    if(error_num == 403){
        build_header_m(buf,"403 Forbidden",strlen("<HTML><HEAD><TITLE>403 Forbidden</TITLE></HEAD>\r\n<BODY><H4>403 Forbidden</H4>\r\nAccess denied.\r\n</BODY></HTML>\0"),path);
        strcat(buf,"<HTML><HEAD><TITLE>403 Forbidden</TITLE></HEAD>\r\n<BODY><H4>403 Forbidden</H4>\r\nAccess denied.\r\n</BODY></HTML>");
        return;
    }
    if(error_num == 302){
        build_header_m(buf,"302 Found",123,path);
        strcat(buf,"<HTML><HEAD><TITLE>302 Found</TITLE></HEAD>\r\n<BODY><H4>302 Found</H4>\r\nDirectories must end with a slash.\r\n</BODY></HTML>");
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
    if (strcmp(ext, ".mpeg") == 0 || strcmp(ext, ".mpg") == 0) return "video/mpeg";
    if (strcmp(ext, ".mp3") == 0) return "audio/mpeg";
    return NULL;
}
void split_str(char* request,char* split_by,char** split_request){
    int counter = 0;
    
    int i = 0;
    split_request[0] = request;
    while(request[i]!='\0'){
        if(request[i]==*split_by){
            request[i] = '\0';
            counter++;
            split_request[counter] = request+i+1;
        }
        i++;
    }
    request[i] = '\0';
}
int read_and_write_file(int fd_socket,char* path,off_t file_size,char* buf){
    int ptr;
    ptr = open(path,O_RDONLY); 
    int total_counter = 0;
    int write_val = 0;
    if (ptr == 0) {
        printf("Usage: open \n");
        return 1;
    }
    char ch;
    int check_read = 0;
    int start_length = strlen(buf);
    int counter =start_length ;
    while(total_counter!=file_size) {
        check_read = read(ptr,buf+start_length,512-start_length);
        if(check_read ==0){
            break;
        }
        if(check_read == -1){
            printf("Usage: read");
            return 1;
        }
        
      
        counter+=check_read;
        if(check_read+start_length == 512){
            write_val = write(fd_socket,buf,512);
            if(write_val == 0){
                printf("Usage: write");
                return 1;
            }
            bzero(buf,512);
            counter = 0;
            total_counter+=(512 - start_length);
            if(start_length!= 0){
                start_length = 0;
            }
        }
        
        // Checking if character is not EOF.
        // If it is EOF stop reading.
    }
    if(counter != 0){
        write_val = write(fd_socket,buf,counter);
        bzero(buf,counter);
        if(write_val == -1){
        perror("Usage: write\n");
        return 1;
    }
    }
  
   
    close(ptr);
    return 0 ;
}
int return_wrote_size(char* path,char* bigger_path){
    #define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT" 
    #define PATH_MAX        4096
    char absulute_path[PATH_MAX];
    getcwd(absulute_path,PATH_MAX);
    strcat(absulute_path,bigger_path);
    struct stat file_stats;
    stat(absulute_path,&file_stats);
    char timebuf_mtime[128];
    int size_of_in_buf = 0;
    char char_a_size[50];
    strftime(timebuf_mtime,sizeof(timebuf_mtime),RFC1123FMT,gmtime(&file_stats.st_mtime));
    sprintf(char_a_size,"%jd",file_stats.st_size);
    if(S_ISREG(file_stats.st_mode)){
        size_of_in_buf += strlen("<tr>\r\n<td><A HREF=\"\"></A></td><td></td>\r\n<td></td>\r\n</tr>\r\n");
        size_of_in_buf = size_of_in_buf+ strlen(path)+strlen(path)+strlen(timebuf_mtime)+strlen(char_a_size);
    }
    else{
        size_of_in_buf+= strlen("<tr>\r\n<td><A HREF=\"\"></A></td><td></td>\r\n</tr>\r\n");
        size_of_in_buf = size_of_in_buf +strlen(path)+strlen(path)+strlen(timebuf_mtime);
    }

    return size_of_in_buf;
}
int make_folder_file(char* path,char* buf,int fd ,char* bigger_path){
    /*gets the absulute path and makes the entry for it*/
    /*
        <tr>
        <td><A HREF="<entity-name>"><entity-name (file or sub-directory)></A></td><td><modification time></td>
        <td><if entity is a file, add file size, otherwise, leave empty></td>
        </tr>
    */

   //int fd
    #define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT" 
    #define PATH_MAX        4096
    char absulute_path[PATH_MAX];
    getcwd(absulute_path,PATH_MAX);
    strcat(absulute_path,bigger_path);
    struct stat file_stats;
    stat(absulute_path,&file_stats);

    int size_of_in_buf = 0;
    int counter_in_buf = 0;
    int counter_buf = strlen(buf);
    char in_buf[512];
    char timebuf_mtime[128];
    strftime(timebuf_mtime,sizeof(timebuf_mtime),RFC1123FMT,gmtime(&file_stats.st_mtime));
    if(S_ISREG(file_stats.st_mode)){
        sprintf(in_buf,"<tr>\r\n<td><A HREF=\"%s\">%s</A></td><td>%s</td>\r\n<td>%ld</td>\r\n</tr>\r\n",path,path,timebuf_mtime,file_stats.st_size);
        size_of_in_buf = strlen(in_buf);
    }
    else{
        sprintf(in_buf,"<tr>\r\n<td><A HREF=\"%s\">%s</A></td><td>%s</td>\r\n</tr>\r\n",path,path,timebuf_mtime);
        size_of_in_buf = strlen(in_buf);
    }
    while(size_of_in_buf!=counter_in_buf){
        if(counter_buf==512){
            write(fd,buf,512);
            bzero(buf,512);
            counter_buf = 0;
        }
        else{
            buf[counter_buf] = in_buf[counter_in_buf];
            counter_buf++;
            counter_in_buf++;
        }
    }
    write(fd,buf,counter_buf);
    bzero(buf,counter_buf);
    return counter_in_buf;
}

int create_ok(char* buf,char* path,int size_of_file){
    #define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT" 
    struct stat stats;
    if(stat(path,&stats)==1){
        perror("Usage: Stat");
        return 1;
    }
    if(size_of_file == -1){
        size_of_file = stats.st_size;
    }
    time_t now;
    char timebuf[128];
    char timebuf_mtime[128];
    now = time(NULL);
    char* type;
    strftime(timebuf_mtime,sizeof(timebuf_mtime),RFC1123FMT,gmtime(&stats.st_mtime));
    strftime(timebuf, sizeof(timebuf), RFC1123FMT, gmtime(&now));
    if((type = get_mime_type(path))==NULL && size_of_file != -1){
        type = "text/html";
    };
    sprintf(buf,"HTTP/1.1 %s\r\nServer: webserver/1.0\r\nDate: %s\nContent-Type: %s\r\nContent-Length: %d\r\nLast-Modified: %s\nConnection: close\r\n\r\n", "200 OK",timebuf,type,size_of_file,timebuf_mtime);
    return 0;
}
int accept_client(void* request,char* buf,int fd){
    char* split_request[3];
    #define PATH_MAX        4096
    split_str((char*)request," ",split_request);
    struct stat file_stats;
    char absulute_path[PATH_MAX];
    FILE* file_des ;
    getcwd(absulute_path,PATH_MAX);
    strcat(absulute_path,split_request[1]);
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
   if(stat(absulute_path,&file_stats) < 0){
    /*checks for permissions to the file we want to acess by the client*/
        if(errno==EACCES){
            error_message(403,buf,NULL);
            write(fd,buf,strlen(buf));
            return 1;
       }
       /*error of opening the file becuase it doesn't exits.*/
        if(errno==ENOENT){
            error_message(404,buf,NULL);
            write(fd,buf,strlen(buf));
            return 1;
        }
       
    };
    
    if(S_ISDIR(file_stats.st_mode)){
        /*check for if the file asked is a folder.*/
        if(split_request[1][strlen(split_request[1])-1]!='/'){
            /*check for if the file name contain a /
             at the end if not prints error for it is a dir and not contain a / at the end*/
            error_message(302,buf,split_request[1]);
            write(fd,buf,strlen(buf));
            /*this is commented becuase it is not askeed in this progect from college to do so.*/
            // char* new_request = (char*)malloc(strlen(request) +1);
            // char* new_split = (char*) malloc (strlen(split_request[1])+1);
            // sprintf(new_split,"%s/",split_request[1]);
            // printf("%s\n",new_split);
            // // split_request[1][len_absulute+1] = '\0';
            // sprintf(new_request,"GET %s HTTP/1.1\r\n",new_split);
            // bzero(buf,512);
            // printf("%s\n",new_request);
            // accept_client(new_request,buf,fd);
            return 0;        
            }
        else{
            /*it is a folder and contains a / at the end*/
            char* new_name = (char*)malloc(strlen(absulute_path) + strlen("index.html")+1);
            sprintf(new_name,"%s%s",absulute_path,"index.html");
            if((file_des = fopen(new_name,"r")) == NULL){
                /*check for if the file index.html doesn't exits inside of the folder
                if not print the content as an html file*/
                free(new_name);
                DIR *d;
                struct dirent *dir;
                int total_size_folder = 0;
                char temp_path[PATH_MAX];
                bzero(temp_path,PATH_MAX);
                d = opendir(absulute_path);
                char buf[512];
                char* type = get_mime_type(absulute_path);
                total_size_folder += strlen("<HTML>\r\n<HEAD><TITLE>index of</TITLE></HEAD>\r\n\r\n<BODY>\r\n<H4>index of</H4>\r\n\r\n<table CELLSPACING=8>\r\n<tr><th>Name</th><th>Last Modified</th><th>Size</th></tr>\r\n");
                total_size_folder += strlen(split_request[1])*2;
                /*a sum of the size of all the files inside the folder*/
                if (d) {
                    while ((dir = readdir(d)) != NULL) {
                            sprintf(temp_path,"%s%s",split_request[1],dir->d_name);
                            // strcat(temp_path,dir->d_name);
                            total_size_folder += return_wrote_size(dir->d_name,temp_path);
                            // printf("temp path is: %s\n", temp_path);
                        }
                    closedir(d);
                }
                /*insert the headers to the buffer*/
                total_size_folder += strlen("\r\n</table>\r\n\r\n<HR>\r\n\r\n<ADDRESS>webserver/1.0</ADDRESS>\r\n\r\n</BODY></HTML>");
                create_ok(buf,absulute_path,total_size_folder);
                sprintf(temp_path,"<HTML>\r\n<HEAD><TITLE>Index of %s</TITLE></HEAD>\r\n\r\n<BODY>\r\n<H4>Index of %s</H4>\r\n\r\n<table CELLSPACING=8>\r\n<tr><th>Name</th><th>Last Modified</th><th>Size</th></tr>\r\n",split_request[1],split_request[1]);
                
                strcat(buf,temp_path);
                /*opens the dir again and prints the values of the files to the client*/
                char* temp = absulute_path;
                for(int i = 0 ; i < strlen(absulute_path);i++){
                    temp[i] = absulute_path[i];
                }
                d = opendir(temp);
                if (d) {
                    while ((dir = readdir(d)) != NULL) {
                        bzero(temp_path,PATH_MAX);
                        // printf("absulute path is :%s\n",temp);
                        sprintf(temp_path,"%s%s",split_request[1],dir->d_name);
                        // strcat(temp_path,);   
                        // printf("tmp path is: %s\n",temp_path);     
                        make_folder_file(dir->d_name,buf,fd,temp_path);
                    }
                    closedir(d);
                }
                sprintf(buf,"</table>\r\n\r\n<HR>\r\n\r\n<ADDRESS>webserver/1.0</ADDRESS>\r\n\r\n</BODY></HTML>");
                write(fd,buf,strlen(buf));
                return 0;
            }
            else{
                fclose(file_des);
                /*if the file exists it prints it to the client as an html file*/
                stat(new_name,&file_stats);
                create_ok(buf,new_name,file_stats.st_size);
                read_and_write_file(fd,new_name,file_stats.st_size,buf);
                free(new_name);
                return 0;
            }
           
        }
    }
    if((!(file_stats.st_mode & S_IROTH))){
        /*check if the client has the premissions to see the file.*/
        error_message(403,buf,NULL);
        write(fd,buf,strlen(buf));
        return 1;
    }
    /*add a return file value*/
    else{
        /*makes the header*/
        create_ok(buf,absulute_path,-1);
        /*writes from the file to the client */
        read_and_write_file(fd,absulute_path,file_stats.st_size,buf);
    }
    return 0;
};
int client_read(void* arg){
    int* fd = (int*)arg;
    char buf[512];
    char returned_buf[512];
    int counter = 0;

    while(1){
        read(*fd,buf+counter,1);
        if(buf[counter]=='\n'){
            buf[counter-1] = '\0';
            break;
        }
        counter++;
    }
    
    accept_client(buf,returned_buf,*fd);
    // shutdown(*fd,SHUT_RD);
    // shutdown(*fd,SHUT_WR);
    close(*fd);
    return 0;
}
int create_server(int port,int number_of_request,threadpool* new_threadpool){
    /*creates the server and listens for clients.*/
    int sockfd, connfd;
    socklen_t len;
    struct sockaddr_in servaddr, cli;
    int counter_of_request = 0;
    int conns[number_of_request];
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Usage: socket creation failed...\n");
        return 0;
    }
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
   
    // Binding the socket created above.
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("Usgage: socket bind failed\n");
        return 1;
    }
   
    // setting the welcome socket to the server
    if ((listen(sockfd, 5)) != 0) {
        printf("Usage: Listen failed\n");
        return 1;
    }

    len = sizeof(cli);
    
    // Accept the data packet from client and verification
    while(counter_of_request < number_of_request){    
        conns[counter_of_request] = accept(sockfd, (SA*)&cli, &len);
        if (connfd < 0) {
            printf("Usage: server connection failed\n");
            return 1;
        }
        dispatch(new_threadpool,(dispatch_fn)client_read,(void*)&conns[counter_of_request]);
        counter_of_request++;
    }
    printf("after accepts\n");
    destroy_threadpool(new_threadpool);  
    free(new_threadpool);  
    // After chatting close the socket
    close(sockfd);
    return 0;
}

int main(int argc, char* argv[]){
    /*argv == [port,pool_size,max number of requests]*/
    int create_respone = 0;
    // printf("\n%s %d %s\n",argv[1],atoi(argv[2]),argv[3]);
    if(argc!=4||atoi(argv[1])<=0){
        printf("Usage: server <port> <pool-size> <number of requests>\n");
        return 0;
    }
    threadpool* new_threadpool = create_threadpool(atoi(argv[2]));
    if(new_threadpool==NULL){
        printf("Usage: threadpool create");
        return 1;
    }
    create_respone = create_server(atoi(argv[1]),atoi(argv[3]),new_threadpool);
    if(create_respone == 1){
        destroy_threadpool(new_threadpool);
        free(new_threadpool);
        return 1;
    }
    // printf("\nfinshed the reading from the clients\n");
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
    if(path!=NULL){
        char location[50];
        sprintf(location,"Location: %s/\r\n",path);
        sprintf(error_message,"HTTP/1.1 %s\r\nServer: webserver/1.0\r\nDate: %s\n%sContent-Type: text/html\r\nContent-Length: %d\r\nConnection: close\r\n\r\n", error_spciefed,timebuf,location,content_length);
    }
    else{
        sprintf(error_message,"HTTP/1.1 %s\r\nServer: webserver/1.0\r\nDate: %s\nContent-Type: text/html\r\nContent-Length: %d\r\nConnection: close\r\n\r\n", error_spciefed,timebuf,content_length);

    }

}