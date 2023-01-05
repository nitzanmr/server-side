/* write parsing function that takes argv given in the main to it and parase
   it and make the http command.*/
#include "string.h"
#include "stdlib.h"
#include <stdio.h>
#include <netdb.h>
struct Client
{
  int port;
  int text_length;
  char* url;
  char* path;
  char* parameters_of_r;
  char* text;
}typedef Client;

void client_init(Client* new_client){
  new_client->parameters_of_r;
  new_client->path = (char*)malloc(sizeof(char));
  new_client->path[0] = '\0';
  new_client->port = 80;
  new_client->text;
  new_client->url = (char*)malloc(sizeof(char));
  new_client->text_length = 0;
}
Client* http_parsing(int length_of_argv,char *kkkk[],Client* new_client){
  int flag_r = 0;
  int flag_p = 0;
  int flag_in_url = 0;
  int number_of_slash = 0;
  int check_next = 0;
  int number_of_parameters = 0;
  int size_of_text = 0;
  int size_of_parameters = 1;
  client_init(new_client);
  for (int i = 0; i<length_of_argv; i++) {
    if (flag_p == 1) {/*checks for the value of the flag of p and adds the text that comes afterwards to the */
      size_of_text = atoi(kkkk[i]);/*checks the value of the size of the text.*/
      i++;
      if(kkkk[i]== NULL){/*check if the value of the text is not NULL*/
        char* error_messeage;
        sprintf(error_messeage,"kkkk[%d] is NULL",i);
        perror(error_messeage);
        exit(1);
      }
      if(strlen(kkkk[i])<size_of_text){
        perror("The size specifed after -p is greater than the size of the text given");
        exit(1);
      }
      new_client->text = (char*)malloc(size_of_text);
      strncat(new_client->text,kkkk[i],size_of_text);
      new_client->text_length = size_of_text;
      flag_p=0;
    }
    if (flag_r == 1) {
      number_of_parameters = atoi(kkkk[i]);///enter the number of parameters into placeholder
      for(int k =i+1;k<=i+number_of_parameters;k++){
        for (int j = 0; kkkk[k][j] != NULL; j++){
          size_of_parameters++;
        }
      }
      new_client->parameters_of_r = (char*)malloc(size_of_parameters+(number_of_parameters)*sizeof(char));
      new_client->parameters_of_r[0] = '?';
      new_client->parameters_of_r[size_of_parameters+(number_of_parameters)] = "\0";
      flag_r = 0;
    } 
    else if (number_of_parameters >= 1) {
      char and_char = '&';
      strcat(new_client->parameters_of_r, kkkk[i]);
      number_of_parameters--;

      if(number_of_parameters >= 1)
       strncat(new_client->parameters_of_r ,&and_char,1);

      ///check for the parameters of -r
      if (number_of_parameters == 0)///for making it not possible to enter -r twice.
        number_of_parameters = -1;
    } 
    else {
      if((strstr(kkkk[i],"http://")!= NULL)){
        flag_in_url = 1;
        for (int j = 0; kkkk[i][j] != NULL; j++) {
          if (flag_in_url == 1) {
            if(kkkk[i][j]=='/'||kkkk[i][j]==':')number_of_slash++;
            if(number_of_slash==3){
              if(kkkk[i][j]!='/')
                strncat(new_client->url, &kkkk[i][j],1);
            }
          } 
          else {
            if (kkkk[i][j] == ':') { // check if there is a spacific port.
              char* temp_for_port = (char*)malloc(sizeof(char));
              while(kkkk[i][j] != '/'&&kkkk[i][j]!=NULL){
                if(kkkk[i][j]!=':'){
                  strncat(temp_for_port,&kkkk[i][j],1);
                }
                j++;
              }
              new_client->port = atoi(temp_for_port);
              strncat(new_client->path,&kkkk[i][j],1);/*adds the / to the path at place 0.*/
              free(temp_for_port);
            }
            else{/*adds the value of the path to the placeholder char* path*/
              strncat(new_client->path,&kkkk[i][j],1);
            }
          }
        }
        if(new_client->path[0]=='\0'){
          char temp = '/';
          strncat(new_client->path,&temp,1);
        }
      }
      else{
        for (int j = 0; j <= strlen(kkkk[i]); j++) {
          if (kkkk[i][j] == '-') {
            check_next = 1;
          } else if (check_next == 1) {
            if (kkkk[i][j] == 'r' && number_of_parameters != -1) {
              flag_r = 1;
            } 
            else if (kkkk[i][j] == 'p') {
              flag_p = 1;
            }
            check_next = 0;
          }
        }
      }
    }
  }
  if(new_client->url=="\0"||new_client->url==NULL){
    perror("the url is NULL");
    exit(1);
  }
  return new_client;
}
void make_http_request(Client* new_client){
  /*makes the http request and send it to the server*/
  struct sockaddr_in new_socket;
  struct hostent *hp;
  int nbytes;
  char buffer[512];
  char buffer_to_read[512];
  int chars_read = 0;
  int flag_p = 0;
  int size_of_buf = 0;
  int fd;
  if(new_client->parameters_of_r!=NULL)size_of_buf+2;
  if((hp = gethostbyname(new_client->url))==NULL){
    herror("gethostbyname");
    exit(1);
  };
  if((fd = socket(PF_INET,SOCK_STREAM,0)) < 0){
    perror("socket");
    exit(1);
  };
  new_socket.sin_addr.s_addr = ((struct in_addr*)(hp->h_addr_list[0]))->s_addr;
  new_socket.sin_family = PF_INET;
  if((new_socket.sin_port = htons(new_client->port))==0){
    perror("htons");
    exit(1);
  };
  if(connect(fd,(struct sockaddr*)&new_socket,sizeof(new_socket))<0)
    {
      perror("connect");
      exit(1);
    }
  if(flag_p ==1){
    strcpy(buffer,"POST ");
    size_of_buf+=5;
  }
  else{
    strcpy(buffer,"GET ");
    size_of_buf+=4;
  }
  strcat(buffer,new_client->path);
  size_of_buf+=strlen(new_client->path);
  strcat(buffer,new_client->parameters_of_r);
  size_of_buf+=strlen(new_client->parameters_of_r);
  strcat(buffer," HTTP/1.0\r\n");
  size_of_buf+=strlen(" HTTP/1.0\r\n");
  strcat(buffer,"HOST: ");
  size_of_buf+=strlen("HOST: ");
  strcat(buffer,new_client->url);
  size_of_buf+=strlen(new_client->url);
  if(new_client->text_length!=0&&new_client->text!=NULL){
    /*enter the content length option to the buf.*/
    /*checks if the value of the text is null and adss it to buff*/
    int text_length_integer = new_client->text_length;
    int size_of_text_length = snprintf(NULL,0,"%d",new_client->text_length);
    char* text_length[size_of_text_length+1];
    sprintf(text_length,"%d",new_client->text_length);
    strcat(buffer,"\r\n");
    size_of_buf+=strlen("\r\n");
    strcat(buffer,"content-length: ");
    size_of_buf+=strlen("content-length: ");
    strcat(buffer,text_length);
    size_of_buf+=strlen(text_length);
    strcat(buffer,"\r\n");
    size_of_buf+=strlen("\r\n");
    write(fd,buffer,size_of_buf);
    write(fd,new_client->text,size_of_text_length);
    write(fd,"\r\n\r\n",sizeof("\r\n\r\n"));
    printf("HTTP request =\n%s",buffer, strlen(buffer));
    printf(" %s",new_client->text);
    printf("\nLEN = %d\n",strlen(buffer)+strlen(new_client->text));
  }
  else{
    strcat(buffer,"\r\n\r\n");
    write(fd,buffer,strlen(buffer));
    printf("HTTP request =\n%s\nLEN = %d\n",buffer, strlen(buffer));
  } 
  printf("response: \n");
  int read_byts;
  while ((read_byts = read(fd,buffer_to_read,sizeof(buffer_to_read)))!=0)
  {
    chars_read+=read_byts;
    printf("%s",buffer_to_read);
  }
  printf("\n Total received response bytes: %d\n",chars_read);
}
void free_client(Client* client_to_free){
  /*
  frees the folowing malloced inside client:
  text
  url
  path
  parameters if not null
  client 
  */
 if(client_to_free!= NULL){
  if(client_to_free->text_length > 0){
    free(client_to_free->text);
  }
  if(client_to_free->parameters_of_r!= NULL){
    free(client_to_free->parameters_of_r);
  }
  free(client_to_free->url);
  free(client_to_free);
 }
}
int main(int argc,char* argv[]){
  /*a function to unite the other functions together and then free the malloced data.*/
    Client* client22 = (Client*)malloc(sizeof(Client));
    http_parsing(argc,argv,client22);
    make_http_request(client22);
    free_client(client22);
    return 0;
}