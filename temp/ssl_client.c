#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "SSL_html_parser.c"
#include "SSL_word_tree.c"

#define PORT 443

int main(int argc,char* argv[])
{    char domain[100];
	char path[100];
           if(argc==2){
		printf("No path\n");
                 strcpy(domain,argv[1]);
	}
	if(argc==3){
	     strcpy(domain,argv[1]);
             strcpy(path,argv[2]);
	}
	
    int sock, iResult;
    char  *ip;
    struct sockaddr_in sin;
    struct hostent* hent;

    hent = gethostbyname(domain);
    if(hent == NULL)
    {
        printf("gethostbyname failed: %d\n", errno);
        return -1;
    }
    printf("gethostbyname succeeded\n");
    ip = inet_ntoa(*((struct in_addr*)hent->h_addr_list[0]));
    printf("Host IP: %s\n", ip);
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1)
    {
        printf("socket failed: %d\n", errno);
        return -1;
    }
    printf("socket created\n");
    
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(ip);
    sin.sin_port = htons(PORT);
    iResult = connect(sock, (struct sockaddr*)&sin, sizeof(sin));
    if(iResult < 0)
    {
        printf("connect failed: %d\n", errno);
        return -1;
    }
    printf("connect succeeded\n");
    
    iResult = SSL_library_init();
    if(iResult < 0)
    {
        printf("SSL failed\n");
        return -1;
    }
    printf("SSL library initialised\n");
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
    SSL_load_error_strings();
    SSL_CTX* ctx = SSL_CTX_new(TLSv1_2_client_method());
    if(ctx == NULL)
    {
        printf("ctx failed\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    printf("ctx loaded\n");
    SSL* ssl = SSL_new(ctx);
    if(ssl == NULL)
    {
        printf("ssl failed\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    printf("ssl loaded\n");
    SSL_set_fd(ssl, sock);
    SSL_connect(ssl);
   

       char request[1024];
	char response[9999];
        snprintf(request, sizeof(request), "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", path, domain);
          
    
	
    iResult = SSL_write(ssl, request, strlen(request));
    if(iResult <= 0)
    {
        printf("SSL write failed\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    printf("Byte(s) sent: %d\n", iResult);
    bzero(response, sizeof(response));
    /*do
    {
        iResult = SSL_read(ssl, response,9999);
        if(iResult < 0)
        {
            printf("error receiving data\n");
            break;
        }
        if(iResult == 0)
        {
            printf("host closed connection\n");
            break;
        }
        bytes+=iResult;
          fwrite(response,1,iResult,fp);
          printf("Bytes recieved: %d\n",bytes);
          
         //printf("%s\n", response);
    }while(iResult > 0);
    */
    
    char *p;
    FILE *fp=fopen("SSL_Rec_file.txt","wb");
    printf("Recieving data...\n\n");
   
   int bytes=0;
   char *ptr=response+1;
 
 //skip the status and other information
    while(iResult = SSL_read(ssl, ptr,1)){
        if(iResult==-1){
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

   while( iResult = SSL_read(ssl, response,sizeof(response))){
    if(iResult== -1 )
	{
		perror("receive");
		return(1);
	}
	    if(iResult<0)
	    break;
    bytes+=iResult;
    fwrite(response,1,iResult,fp);
     printf("Bytes recieved: %d\n",bytes);

   }
    printf("\nSuccessfully received\n");
    fclose(fp);
    
    
    iResult = SSL_shutdown(ssl);
    if(iResult == 0)
    {
        printf("SSL shutdown in progress...\n");
    }
    iResult = SSL_shutdown(ssl);
    if(iResult == 1)
    {
        printf("SSL shutdown complete!\n");
    }
    if(iResult == -1)
    {
        printf("SSL shutdown unsuccessful!\n");
    }
    SSL_CTX_free(ctx);
    
    iResult = shutdown(sock, SHUT_RDWR);
    if(iResult == -1)
    {
        printf("Socket shutdown failed: %d\n", errno);
        return -1;
    }
    printf("Socket shutdown succeeded\n");
    iResult = close(sock);
    if(iResult != 0)
    {
        printf("error closing socket: %d\n", errno);
        return -1;
    }
    printf("Socket closed\n");
    fn1();
	printf("Successfully parsed\n");
	maketree();
	printf("Successfully ranked\n");
	return 0;
}
