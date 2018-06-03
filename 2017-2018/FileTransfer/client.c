/* 
*	Client
*
*	Autor: Anastasiia Stoika, xstoik00
*	Subject: IPK
*	Project: Client-server for File transfering
*	Usage: ./ipk-client -h host -p port [-r|-w] file
*/

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <getopt.h> 
#include <stdbool.h> 
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <libgen.h>

#define BUFFER 4096             //max buffer length for incoming messages
#define RESERVED_PORT 1024
#define ERR_MAX 30

typedef struct params{
  int port;
  char* host; 
  bool read_flag;				        // read file flag
  bool write_flag;				      // write file flag
  char* filename;
}TParams;

typedef enum Error {
    ERROR_OK = 0,    
    ERROR_PARAM = 1,
    ERROR_FILE_READ = 2,
    ERROR_UNDEF = 3,
    ERROR_TODO,
    ERROR_MALLOC,
    ERROR_PORT_NUMBER,
    ERROR_RW_PARAM,
    ERROR_PARAM_USAGE,
    ERROR_FILE_WRITE,
    ERROR_HOST,
    ERROR_SOCKET,
    ERROR_CONNECT,
    ERROR_GETSOCKNAME,
    ERROR_READ,
    ERROR_WRITE
} Error;

const char *ERR_TABLE[ERR_MAX] = {
    "NO_ERROR: Without errors.",
    "ERROR_PARAM: Wrong input params.",
    "ERROR_FILE_READ: Can't open file for reading.",
    "ERROR_UNDEF: Unknown error.",
    "ERROR_TODO: Neco mas dodelat!!!.",
    "ERROR_MALLOC: To low memmory. Cant allocate mem.",
    "ERROR_PORT_NUMBER: Port numbers till 1024 are reserved or another error.",
    "ERROR_RW_PARAM: Please choose only one operation: -r for reading file or -w for writing.",
    "ERROR_PARAM_USAGE: ./ipk-client -h host -p port [-r|-w] file",
    "ERROR_FILE_WRITE: Can't open file for write.",
    "ERROR_HOST: Error connection to host. host (IP address or fully-qualified DNS name).",
    "ERROR_SOCKET: Error while create client socket.",
    "ERROR_CONNECT: Error connect(). Connection to server refused.",
    "ERROR_GETSOCKNAME: getsockname() failed.",
    "ERROR_READ: read() failed.",
    "ERROR_WRITE: write() failed."
};

TParams getParams(int argc, char **argv);
void printErr(Error TypeError);
void service(int sock, TParams params);

//-------------------------------------------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	int sock;
  	socklen_t len;
  	struct sockaddr_in local, server;
  	struct hostent *servaddr;      					 	  
	/* Get input parameters */
  	TParams param = getParams(argc, argv);
/* -- Connection configuration -- */
  	memset(&server,0,sizeof(server)); 					
  	memset(&local,0,sizeof(local));   					

  	server.sin_family = AF_INET;

  	/* Make DNS resolution using gethostbyname() */
  	if ((servaddr = gethostbyname(param.host)) == NULL) // check the first parameter
    	printErr(ERROR_HOST);

    memcpy(&server.sin_addr, servaddr->h_addr, servaddr->h_length); 
  	server.sin_port = htons(param.port);
  	/* Create client socket*/
  	if ((sock = socket(AF_INET , SOCK_STREAM , 0)) == -1)  
    	printErr(ERROR_SOCKET);

  	/* Connect to remote server */
  	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) == -1)
    	printErr(ERROR_CONNECT);

  	len = sizeof(local);
  	if (getsockname(sock,(struct sockaddr *) &local, &len) == -1)
    	printErr(ERROR_GETSOCKNAME);

  	printf("Client successfully connected from %s, port %d (%d) to %s, port %d (%d)\n", inet_ntoa(local.sin_addr),ntohs(local.sin_port),local.sin_port,inet_ntoa(server.sin_addr),ntohs(server.sin_port), server.sin_port);
     
  	/* Communication with server */
 	  service(sock, param);
  	
    printf("closing client socket ...\n");
  	close(sock);

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------

/* --- Function for parsing input parameters for client ---
**	./ipk-client -h host -p port [-r|-w] file
*/
TParams getParams(int argc, char **argv){
	int option = 0, h_flag = 0, p_flag = 0;
	TParams param = {
		.host = malloc(50 * sizeof(char)),
		.port = 0,
		.read_flag = false,
		.write_flag = false,
		.filename = malloc(50 * sizeof(char)),
	};

	if(argc != 7)
		printErr(ERROR_PARAM_USAGE);

	while ((option = getopt(argc, argv, "h:p:r:w:")) != -1) {
	    switch (option) {
	    case 'h':
	    	if(h_flag >= 1)
	    		printErr(ERROR_PARAM_USAGE);
	        param.host = optarg;
	        h_flag += 1;
	        break;
	    case 'p':
	    	if(p_flag >= 1)
				printErr(ERROR_PARAM_USAGE);
			if(atoi(optarg) <= RESERVED_PORT)
				printErr(ERROR_PORT_NUMBER);
			param.port = atoi(optarg);
			p_flag += 1;								                //flag na zjisteni duplikace parametru, pokud byl zadan vice krat nez jedna --> chyba
	        break;
	    case 'r':
	    	if(param.write_flag || param.read_flag)		//byl-li zadan krome parametru -r taky i parametr -w; || dochazi-li k duplikaci parametru -r
	    		printErr(ERROR_RW_PARAM);
	    	param.read_flag = true;
	    	param.filename = optarg;  
	    	break;
	    case 'w':
	    	if(param.read_flag || param.write_flag)
	    		printErr(ERROR_RW_PARAM);
	    	param.write_flag = true;
	    	param.filename = optarg;
	    	break;
	    default:
	    	printErr(ERROR_PARAM_USAGE);
	    }
 	}
 	return param;
}

void printErr(Error TypeError){
  if(TypeError != 0){
    fprintf(stderr, "%s\n", ERR_TABLE[TypeError]);
    exit(TypeError);
  }
  return;
}

void service(int sock, TParams params){
	int msg_size, fileSize, fr, fw, n = 0, cnt = 0, data_blocks, last_block;                                 //block counter while sending file
	char buffer[BUFFER], size_char[BUFFER], file_buf[BUFFER];
	FILE *fp;

	/* Read file from server -- write to file on client */
	if(params.read_flag){
		strcpy(buffer, "READ/");
		strcat(buffer, basename(params.filename));
	}
  /* Write file to server - open for read on client */
  else if(params.write_flag){
    if((fp = fopen(params.filename, "rb")) == NULL)
      printErr(ERROR_FILE_READ);
    strcpy(buffer, "WRITE/");
    strcat(buffer, basename(params.filename));
  }
  send(sock, buffer, strlen(buffer), 0);

  /* Communication with server */
  while(1){
    if((msg_size = recv(sock, buffer, BUFFER, 0)) == -1)
      printErr(ERROR_READ);

    /* Obdrzeni potvrzeni o cteni a velikost suboru - READ_OK/file_size */
    if(strncmp("READ_OK/", buffer, 8) == 0){
      if((fp = fopen(params.filename, "wb")) == NULL)    //prepise obsah jestli uz existuje subor s takovym jmenem
      printErr(ERROR_FILE_WRITE);

      strcpy(size_char, buffer+strlen("READ_OK/"));      //get size of file
      fileSize = atoi(size_char);
      printf("Size of file: %d\n", fileSize);

      /* Osetreni jestli soubor je prazdny, pokud ano, ulozi se prazdny soubor a program ukonci */
      if(fileSize > 0){
        strcpy(buffer, "SIZE_OK\0");
        send(sock, buffer, strlen(buffer), 0);    

      /* -------- RECEIVING FILE FROM SERVER ----------- */
        if(fileSize <= BUFFER){
          recv(sock, file_buf, BUFFER, 0);
          fwrite(file_buf, sizeof(char), fileSize, fp);
        }
        else{
          data_blocks = fileSize / BUFFER;
          last_block = fileSize % BUFFER;
          
          for(int n = 0; n < data_blocks; n++){
            recv(sock, file_buf, BUFFER, 0);
            fw = fwrite(file_buf, sizeof(char), BUFFER, fp);
            printf("Block: %d, -- zapsano: %d\n", n, fw);
          }
          /* If exists last block less than BUFFER 4096, write last block */
          if(last_block > 0){
            recv(sock, file_buf, last_block, 0);
            file_buf[last_block + 1] = '\0';
            fw = fwrite(file_buf, sizeof(char), last_block, fp);
            printf("Zabis zbytku: %d\n", fw);
          }
        }
        fclose(fp);
        printf("Complete.\n");
        exit(0);
      }
      else{
        fclose(fp);                                 
        printf("Receiving zero file. Complete.\n");   //if file has zero length, "upload" zero file
        exit(0);
      }
    }
    else if(strncmp("ERROR_READ", buffer, msg_size) == 0){
      printf("ERROR_READ received.\n");
      //fclose(fp);
      printErr(ERROR_FILE_READ);
    }
    else if(strncmp("WRITE_OK", buffer, msg_size) == 0){  
    /* Find out size of file and send to server */
      fseek(fp, 0, SEEK_END);
      fileSize = ftell(fp);
      fseek(fp, 0, SEEK_SET);                       //turn back to the beginning of file

      strcpy(buffer, "SIZE/");
      sprintf(size_char, "%d", fileSize);           //convert size of file to char
      strcat(buffer, size_char);
      send(sock, buffer, strlen(buffer), 0);        //send size to server
    }
    else if(strncmp("SIZE_OK", buffer, msg_size) == 0){
      printf("...Prepare sending file... \n");
    /* ***** SENDING DATA TO SERVER ***** */
      while((fr = fread(file_buf, sizeof(char), BUFFER, fp)) > 0){
        n = send(sock, file_buf, fr, 0);
        usleep(5000);                               //sleep client between sending data
        printf("--- Precteno z souboru: %d, -- ODESLANO: %d\n",fr, n);
        printf("BLOCK: %d\n", cnt);
        if(fr < BUFFER){
          file_buf[fr + 1] = '\0';
        }
        cnt++;
      }
      printf("Complete.\n");
    }
    else if(strncmp("WRITE_END", buffer, msg_size) == 0){
      printf("-------- TRANSMISSION COMPLETE -----------\n");
      fclose(fp);
      exit(0);
    }
  } //end of protocol
}