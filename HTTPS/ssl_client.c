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
{    
       char domain[100];
       char path[100];
       if(argc==2)
        {                       //Urls with only domain name only
		printf("No path\n");
               strcpy(domain,argv[1]);
        }
	if(argc==3)
	 {                      //Urls with domain name and path
	       strcpy(domain,argv[1]);
               strcpy(path,argv[2]);
	 }
	
        int sock;
        int iResult;            //Declaring a socket descriptor
        char  *ip;              //Pointer to ip address
        struct sockaddr_in sin; //Structure to hold Ipv4 address
        struct hostent* hent;   //Structure to hold internet host

        hent = gethostbyname(domain);  //Converting the domain to ip address
        if(hent == NULL)
        {
              printf("gethostbyname failed: %d\n", errno);  
              return -1;
        }
        printf("gethostbyname succeeded\n");
       
        ip = inet_ntoa(*((struct in_addr*)hent->h_addr_list[0]));     //Function call to return the ip address in string format
        printf("Host IP: %s\n", ip);
    
        sock = socket(AF_INET, SOCK_STREAM, 0); //Function call to get the socket descriptor
        if(sock == -1)
        {
              printf("socket failed: %d\n", errno);
              return -1;
        }
        printf("socket created\n");
    
        sin.sin_family = AF_INET;                // Assigning address family
        sin.sin_addr.s_addr = inet_addr(ip);     // Assigning Ip address
        sin.sin_port = htons(PORT);              // Assigning Port number
         
        iResult = connect(sock, (struct sockaddr*)&sin, sizeof(sin));  //Function call to connect to host     
        if(iResult < 0)
        {
               printf("connect failed: %d\n", errno);
               return -1;
        }
        printf("connect succeeded\n");
    
        iResult = SSL_library_init();           //Initialize the available SSL/TLS ciphers and digests
        if(iResult < 0)
        {
               printf("SSL failed\n");
               return -1;
        }
        printf("SSL library initialised\n");
       
        OpenSSL_add_all_algorithms();          //Registers the available SSL/TLS ciphers and digests
        ERR_load_crypto_strings();             //Registers the error strings for all libcrypto functions.
        SSL_load_error_strings();              //Registers the libssl error strings
        SSL_CTX* ctx = SSL_CTX_new(TLSv1_2_client_method());         //Function call to create a new context for client application
       
        if(ctx == NULL)
        {
               printf("ctx failed\n");
               ERR_print_errors_fp(stderr);
               return -1;
        }
        printf("ctx loaded\n");
        SSL* ssl = SSL_new(ctx);               //Creates SSL structure to hold the data for a TLS/SSL connection.
        if(ssl == NULL)
        {
               printf("ssl failed\n");
               ERR_print_errors_fp(stderr);
               return -1;
        }
        printf("ssl loaded\n");
        SSL_set_fd(ssl, sock);                //Function call to assign a socket to SSL structure
        SSL_connect(ssl);                     //Initiates the TLS/SSL handshake with the server
   
        char request[1024];
        char response[9999];
        
        //Adding GET request inside the request buffer
        snprintf(request, sizeof(request), "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", path, domain);
          
        iResult = SSL_write(ssl, request, strlen(request));           //Function call to write the request buffer into the ssl connection
        if(iResult <= 0)
        {
              printf("SSL write failed\n");
              ERR_print_errors_fp(stderr);
              return -1;
        }
        printf("Byte(s) sent: %d\n", iResult);
        bzero(response, sizeof(response));    //Assigning the receiver buffer to zero
         
        char *p;
        FILE *fp=fopen("SSL_Rec_file.txt","wb");                     //Creating a file for saving response
        printf("Recieving data...\n\n");
   
        int bytes=0;
        char *ptr=response+1;
 
       //skip the status and other information
        while(iResult = SSL_read(ssl, ptr,1))
        {
              if(iResult==-1)
              {
                  perror("Parse Header");
                  exit(1);
              }
             if(
             (ptr[-3]=='\r')  && (ptr[-2]=='\n' ) &&
             (ptr[-1]=='\r')  && (*ptr=='\n' )
             )    break;

             ptr++;
        }
 
      //Saving html source code

       while( iResult = SSL_read(ssl, response,sizeof(response)))
       {
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
    
       iResult = SSL_shutdown(ssl);     //Shutdowns the SSL connection
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
        SSL_CTX_free(ctx);             //Removes the SSL_CTX object pointed to by ctx and frees up the memory
    
        iResult = shutdown(sock, SHUT_RDWR);
        if(iResult == -1)
        {
               printf("Socket shutdown failed: %d\n", errno);
               return -1;
        }
        printf("Socket shutdown succeeded\n");
        iResult = close(sock);        //Shutdowns the socket connection associated with socket descriptor
        if(iResult != 0)
        {
               printf("error closing socket: %d\n", errno);
               return -1;
        }
        printf("Socket closed\n");
        parser();                      //Function call to parse the html source code
	printf("Successfully parsed\n");
	ranker();                     //Function call to rank the words
	printf("Successfully ranked\n");
	return 0;
}
