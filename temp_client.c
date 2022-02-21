#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<netdb.h>
int main(int argc,char* argv[])
{       char domain[100];
	char path[100];
        strcpy(domain,argv[1]);
	strcpy(path,argv[2]);

        if(argc<2){
		printf("Invalid input");
	}
        struct hostent *he;
	he = gethostbyname(domain); //changing the host domain to ip address
	if (he == NULL){
       herror("gethostbyname");
       exit(1);
        }

        int client_socket=socket(AF_INET,SOCK_STREAM,0); //creating a socket
        if(client_socket==-1){
		perror("Socket");
                exit(1);
	}
        struct sockaddr_in remote_address;
	remote_address.sin_family=AF_INET;  //Ipv4
	remote_address.sin_port=htons(80);  //TCP port
	remote_address.sin_addr = *((struct in_addr *)he->h_addr);
        bzero(&(remote_address.sin_zero),8); 


	if (connect(client_socket, (struct sockaddr *)&remote_address,sizeof(struct sockaddr)) == -1){ //Establishing a TCP connection
       perror("Connect");
       exit(1);
         }

	char request[1024];
	char response[9999];
        snprintf(request, sizeof(request), "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", path, domain);
        printf("%s",request);
        if(send(client_socket, request, strlen(request), 0)==-1){
        perror("send");
        exit(1); 
        }
    printf("Data sent.\n");  
    int bytes_received;
    char *p;
    FILE *fp=fopen("received_file.txt","wb");
    printf("Recieving data...\n\n");
   
   int bytes=0;
   char *ptr=response+1;
 
 //skip the status and other information
    while(bytes_received = recv(client_socket, ptr, 1, 0)){
        if(bytes_received==-1){
            perror("Parse Header");
            exit(1);
        }
        if(
            (ptr[-3]=='\r')  && (ptr[-2]=='\n' ) &&
            (ptr[-1]=='\r')  && (*ptr=='\n' )
        ) break;

        ptr++;
    }

 //Saving html source code

   while( bytes_received = recv(client_socket, response, sizeof(response), 0)){
    if( bytes_received== -1 )
	{
		perror("receive");
		return(1);
	}
    bytes+=bytes_received;
    fwrite(response,1,bytes_received,fp);
     printf("Bytes recieved: %d\n",bytes);
    if(bytes==-1)
	    break;
   }
    printf("\nSuccessfully received\n");
    close(client_socket);
	fclose(fp);
	return 0;
}
