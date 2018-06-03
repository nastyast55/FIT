/*  
* Server
*
* Autor: Anastasiia Stoika, xstoik00
* Subject: IPK
* Project: Client-server for File transfering
* Usage: ./ipk-server -p port 
*/

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <ctype.h>
#include <arpa/inet.h>

#define BUFFER 4096         // max buffer length for incoming messages
#define QUEUE 1             
#define RESERVED_PORT 1024
#define ERR_MAX 20

typedef struct params{
  int port;
}TParams;

typedef enum Error {
    ERROR_OK = 0,    
    ERROR_UNDEF = 1,
    ERROR_TODO,
    ERROR_MALLOC,
    ERROR_PORT_NUMBER,
    ERROR_PARAM_USAGE,
    ERROR_FILE_WRITE,
    ERROR_HOST,
    ERROR_SOCKET,
    ERROR_CONNECT,
    ERROR_SIGACTION,
    ERROR_BIND,
    ERROR_LISTEN,
    ERROR_ACCEPT,
    ERROR_FORK,
    ERROR_WRITE,
    ERROR_RECV
} Error;

const char *ERR_TABLE[ERR_MAX] = {
    "NO_ERROR: Without errors.",
    "ERROR_UNDEF: Unknown error.",
    "ERROR_TODO: Neco mas dodelat!!!.",
    "ERROR_MALLOC: Too low memory. Cant allocate mem.",
    "ERROR_PORT_NUMBER: Port numbers till 1024 are reserved or another error.",
    "ERROR_PARAM_USAGE: ./ipk-server -p port",
    "ERROR_FILE_WRITE: Can't open file for write.",
    "ERROR_HOST: Error connection to host. host (IP address or fully-qualified DNS name).",
    "ERROR_SOCKET: Error while create server socket.",
    "ERROR_CONNECT: Error connect(). Connection to server refused.",
    "ERROR_SIGACTION: sigaction() failed.",
    "ERROR_BIND: bind() failed.",
    "ERROR_LISTEN: listen() failed.",
    "ERROR_ACCEPT: accept() failed.",
    "ERROR_FORK: fork() failed.",
    "ERROR_WRITE: write() failed.",
    "ERROR_RECV: recv() failed."
};

TParams getParams(int argc, char **argv);
void printErr(Error TypeError);\
void service(int newsock);

int main(int argc, char **argv)
{
	int fd, newsock, len;
	struct sockaddr_in server, from;   
	pid_t pid; 
  long p;           
	struct sigaction sa;         

	/* Get input params */
	TParams param = getParams(argc, argv);
	/* Handling SIG_CHILD for concurrent processes */
  	sa.sa_handler = SIG_IGN;      
  	sa.sa_flags = 0;
  	sigemptyset(&sa.sa_mask);     

  	if (sigaction(SIGCHLD, &sa ,NULL) == -1)
    	printErr(ERROR_SIGACTION);

    /* Create server socket */
    if ((fd = socket(AF_INET , SOCK_STREAM , 0)) == -1)
    	printErr(ERROR_SOCKET);

    server.sin_family = AF_INET;            
  	server.sin_addr.s_addr = INADDR_ANY;      
  	server.sin_port = htons(param.port);

  	/* Bind server socket to the port */
  	if(bind(fd, (struct sockaddr *)&server, sizeof(server)) < 0)  
  		printErr(ERROR_BIND);
 	  /* Setting a queue and waiting for incoming connections */
  	if (listen(fd,QUEUE) != 0)
    	printErr(ERROR_LISTEN); 
  	
  	printf("...Waiting for incoming connections...\n");

  	/* Processing incoming connections */
  	len = sizeof(from);
  	while(1){
	    /* Accept new connection */
	    if ((newsock = accept(fd, (struct sockaddr *)&from, (socklen_t*)&len)) == -1)
	      printErr(ERROR_ACCEPT);
	  	/* Create process for new connection */
	    if ((pid = fork()) > 0){     // its parent process
	      close(newsock);
	    }
	    else if (pid == 0){          // its a child process
	      p = (long) getpid();       // child's PID
	      close(fd);

	      printf("A new connection accepted from %s, port %d (%d) by process %ld\n", inet_ntoa(from.sin_addr),ntohs(from.sin_port), from.sin_port,p);

	      /* Communication with client */
	     	service(newsock);

	      // no other data from the client -> close the client and wait for another
	     	printf("...Close newsock...\n");
	      close(newsock);                          
	      exit(0);                                 
	    } 
	    else
	      	printErr(ERROR_FORK);
  } 
  
  	// close server 
  	printf("Close an original socket\n");
  	close(fd);  

	return 0;
}

/* --- Function for parsing input parameters for server ---
**	./ipk-server -p port
*/
TParams getParams(int argc, char **argv){
	TParams param = {
		.port = 0
	};

	if(argc != 3)
		printErr(ERROR_PARAM_USAGE);

	if((strcmp("-p", argv[1]) == 0)){
		if(atoi(argv[2]) > RESERVED_PORT){
			param.port = atoi(argv[2]);
		}
		else
			printErr(ERROR_PORT_NUMBER);
	}
	else
		printErr(ERROR_PARAM_USAGE);

	return param;
}

void printErr(Error TypeError){
  if(TypeError != 0){
    fprintf(stderr, "%s\n", ERR_TABLE[TypeError]);
    exit(TypeError);
  }
  return;
}

void service(int newsock){
  	int msg_size, i, fileSize, data_blocks, last_block, fw = 0, fr, n = 0, cnt = 0;
  	char buffer[BUFFER], size_char[BUFFER], inpt_file_buf[BUFFER];
    char *filename = malloc(50 * sizeof(char));
    FILE *fp;
	/* Processing incoming messages from client */
	while((msg_size = recv(newsock, buffer, BUFFER, 0)) > 0)
	{
    buffer[msg_size] = '\0';
    msg_size += 1; 
    /* Processing reading file from server */
    if(strncmp("READ/", buffer, 5) == 0){
      
      strcpy(filename, buffer+strlen("READ/"));        // extracting name of file from message
      if((fp = fopen(filename, "rb")) == NULL)
      {
        strcpy(buffer, "ERROR_READ\0");                // neexistuje subor - odeslat klientu zpravu o chybe
        send(newsock, buffer, strlen(buffer), 0);      // client ukonci cinnost s chybou
        break;  //ukoncit obsluhu klientskeho procesu
      }
      /* Sending to client accept for reading file and SIZE of file*/
      fseek(fp, 0, SEEK_END);                          // finding out size of file
      fileSize = ftell(fp);
      fseek(fp, 0, SEEK_SET);

      strcpy(buffer, "READ_OK/");
      sprintf(size_char, "%d", fileSize);              //convert size of file to char
      strcat(buffer, size_char);

      send(newsock, buffer, strlen(buffer), 0); 
    }
    else if(strncmp("SIZE_OK", buffer, 7) == 0){
      printf("...Prepare sending file... \n");
    /* ***** SENDING DATA TO SERVER ***** */
      while((fr = fread(inpt_file_buf, sizeof(char), BUFFER, fp)) > 0){
        n = send(newsock, inpt_file_buf, fr, 0);
        usleep(5000);                                   //sleep client between sending data
        printf("--- Precteno z souboru: %d, -- ODESLANO: %d\n",fr, n);
        printf("BLOCK: %d\n", cnt);
        if(fr < BUFFER){
          inpt_file_buf[fr + 1] = '\0';
        }
        cnt++;
      }
      printf("Complete.\n");
      fclose(fp);
    }
    /* Processing writing file to server */
    else if(strncmp("WRITE/", buffer, 6) == 0){
      /* Extracting name of file from message */
      strcpy(filename, buffer+strlen("WRITE/"));

      if((fp = fopen(filename, "wb")) == NULL)
        printErr(ERROR_FILE_WRITE);

      /* Sending to client accept for writing file */
      strcpy(buffer, "WRITE_OK\0");
      i = send(newsock, buffer, strlen(buffer), 0);
      if (i == -1)                                      // check if data was successfully sent out
        printErr(ERROR_WRITE);
    }
    /* Only in case when client want to WRITE file on server */
    else if(strncmp("SIZE/", buffer, 5) == 0){
      strcpy(size_char, buffer+strlen("SIZE/"));
      fileSize = atoi(size_char);

      /* Osetreni jestli soubor je prazdny - v takovem pripade se ulozi prazndy a ukonci program */
      (fileSize > 0) ? strcpy(buffer, "SIZE_OK\0") : strcpy(buffer, "WRITE_END\0");
      send(newsock, buffer, strlen(buffer), 0);

  /* --------------- Saving input data to file ------------------- */
      if(fileSize <= BUFFER){
        recv(newsock, inpt_file_buf, BUFFER, 0);
        fwrite(inpt_file_buf, sizeof(char), fileSize, fp);
      }
      else{
        data_blocks = fileSize / BUFFER;
        last_block = fileSize % BUFFER;
        
        for(int n = 0; n < data_blocks; n++){
          recv(newsock, inpt_file_buf, BUFFER, 0);
          fw = fwrite(inpt_file_buf, sizeof(char), BUFFER, fp);
          printf("Block: %d, -- zapsano: %d\n", n, fw);
        }
        /* If exists last block less than BUFFER 4096, write last block */
        if(last_block > 0){
          recv(newsock, inpt_file_buf, last_block, 0);
          inpt_file_buf[last_block + 1] = '\0';
          fw = fwrite(inpt_file_buf, sizeof(char), last_block, fp);
          printf("Zapis zbytku: %d\n", fw);
        }
      }
      fclose(fp);
      printf("Complete.\n");

      strcpy(buffer, "WRITE_END\0");
      send(newsock, buffer, strlen(buffer), 0);
    }
	}
}
