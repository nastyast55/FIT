/*
** Author: Anastasiia Stoika
** Project: DNS Lookup
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
#include <stdint.h>
#include <inttypes.h>

#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

#define ERR_MAX 3 
#define DNS_PORT 53
#define MAX_PACKET 512						//max.size of UDP packet
#define MAX_DOMAIN_NAME 255
#define MAX_MSG 65535
#define IP4_SIZE 4								//32 bit IPv4 adresa
#define IP6_SIZE 16								//128 bit IPv6 adresa
#define DNS_HDR_SIZE sizeof(struct dns_header)
#define IN6ADDR_SIZE sizeof(struct in6_addr)

#define A 1
#define AAAA 28
#define CNAME 5
#define PTR 12
#define NS 2

#define STANDARD_QR 0							//standard query (0)
#define classIn_IN	1								// 0001 is mean for classIn IN

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

typedef struct params{
  char* server; 					//IPv4 adresa DNS serveru
  char* domain_name;				//prekladane domenove jmeno; pokud -t PTR, name = IPv4/IPv6 adr.
  bool ptr_ipv6_flag;				    //pokud hledany zaznam je typu PTR
  bool timeout_flag;				//pokud uzivatelem byl zadan jiny timeout
  int timeout;						//vychozi hodn. 5 sec
  bool record_flag;					//polud byl uzivatelem zadan typ zaznamu
  uint16_t record_type;					//implicit. podle A
  bool iterative_flag;				//vynucene iterativni hledani; implicit. rekurzivne
  string revers_ipv6_dest; 
 	string revers_ipv4_dest;
 // struct in_addr* ip_server;
}TParams;

typedef enum Error {
	NO_ERROR = 0,
	ERROR,
	ERROR_PARAMS
}Error;

struct dns_header{
	u_short id;						//16-bit ID
	
	uint16_t 	rd:1,					//query (0), response (1)
						tc:1,			//kind if query, 0 - Standard query
						aa:1,					//Authoritative Answer
						opcode:4,					//truncation
						qr:1,					//Recursion Derired, do recursively (0)
						rcode:4,					//Recursion Available - for server
						cd:1,					//reserved
						ad:1,
						z:1,
						ra:1;			//reply code, (0) no error
	
	u_short qdcount;			//no. of questions
	u_short ancount;			//no. of answers
	u_short nscount;			//no. of NS records
	u_short arcount;			//no. of Additional
};

//Our question to DNS server
struct question{
//	u_char* qname;					//dname represented as a sequence of labels, each label consists
												//of length octet followed by that number of octets
	u_short qtype;
	u_short qclassIn;
};

//For Answer, Authorities and Additional
struct resourceRecord{
	uint16_t type;
	uint16_t classIn;
	uint32_t ttl;
	uint16_t rdleghth;						//length in bytes of RDATA field
//	char *name;
	//const unsigned char* rdata;		//if the TYPE is A and the classIn is IN,
                								//the RDATA field is a 4 octet ARPA Internet address
};

const char *ERR_TABLE[ERR_MAX] = {
	"Successful.",
	"ERROR: Some error in program. Connection problems, or RCODE returned smth else than 0.",
	"ERROR_PARAMS: Wrong input params.\nUsage: ./ipk-lookup -s server [-T timeout] [-t type] [-i] name"
};

TParams getParams(int argc, char **argv);
void printErr(Error TypeError);
void printHelp();
void dns_name_format(u_char* dns, u_char* host);
int parse_dns_answer(u_char* packet, int size_to_answer, uint16_t type);
void convert_from_dns_name(u_char* dns);
int get_dns_name(u_char* pAnswer, char* name, u_char* packet);
string invertipv6 (const string &str);
string invertipv4(unsigned long int addr);

int main(int argc, char **argv)
{
	int sock;
 	u_char buffer[MAX_MSG];
 	struct sockaddr_in local, dns_server;
 	struct in_addr server_addr;
 	/* Get input parameters */
 	TParams param = getParams(argc, argv);

	if (inet_aton(param.server, &server_addr) == 0) {
    printErr(ERROR_PARAMS);
  }

	/*-- Connection configuration -- */
 	memset(&dns_server,0,sizeof(dns_server)); 					
 	memset(&local,0,sizeof(local));   					
 	//Server configuration
 	dns_server.sin_family = AF_INET;
 	dns_server.sin_port = htons(DNS_PORT);
 	dns_server.sin_addr = server_addr;

  /* Create client socket*/
  if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)  
   	printErr(ERROR);

  //-----------------Zaplneni hlavicky-------------------------
  struct dns_header *dns_hdr = NULL;
  dns_hdr = (struct dns_header *)&buffer;		//pretypovani

  dns_hdr->id = (u_short)htons(getpid());
  dns_hdr->qr = 0;				//it's a query
  dns_hdr->opcode = 0x0;		//standard query
  dns_hdr->aa = 0;				//--NULL ?
  dns_hdr->tc = 0;				//not truncated
  dns_hdr->rd = 1;				//do recursively
  dns_hdr->ra = 0;				//--NULL?
  dns_hdr->z = 0;
  dns_hdr->ad = 0;
  dns_hdr->cd = 0;
  dns_hdr->rcode = 0x0;		//--NULL ?
  dns_hdr->qdcount = htons(1);	//htons? ..
  dns_hdr->ancount = 0;
  dns_hdr->nscount = 0;
  dns_hdr->arcount = 0;

  u_char *qname = (u_char*)&buffer[sizeof(struct dns_header)];

  if(param.record_type == PTR){
  	if(param.ptr_ipv6_flag)
  		memcpy(qname, param.revers_ipv6_dest.c_str(), param.revers_ipv6_dest.size());
  	else
  		dns_name_format(qname, (u_char*)param.revers_ipv4_dest.c_str());
  }
  else	
  	dns_name_format(qname, (u_char*)param.domain_name);

	/* Make question */
  struct question *my_qst = NULL;
  my_qst = (struct question *)&buffer[sizeof(struct dns_header) + (strlen((const char*)qname) + 1)];		//preskocit dns hlavicku a zaplnit
  
  my_qst->qtype = htons(param.record_type);
  my_qst->qclassIn = htons(classIn_IN);

	int m = sendto(sock, (char *)buffer, sizeof(struct dns_header) 
																				+ (strlen((const char*)qname) + 1)
																				+ sizeof(struct question),0,(struct sockaddr*)&dns_server,sizeof(dns_server));
	if(m < 0)
		printErr(ERROR);

	u_char *packet = (u_char*) malloc(MAX_MSG * sizeof(unsigned char));
	if(packet == NULL)
		printErr(ERROR);
	memset(packet, '\0', MAX_MSG * sizeof(unsigned char));

	int len = sizeof(dns_server);
	int receive = recvfrom(sock, packet, MAX_MSG, 0, (struct sockaddr*)&dns_server, (socklen_t*)&len);
	if(receive < 0)
		printErr(ERROR);

	//Size of dns header and queries -- before Answer
	int size = sizeof(struct dns_header) + (strlen((const char*)qname) + 1) + sizeof(struct question);
	
	int exitCode = parse_dns_answer(packet, size, param.record_type);
	
  close(sock);

	exit(exitCode);
}

TParams getParams(int argc, char **argv){
	//flags na osetreni opakujicich parametru
	int s_flag = 0, t_flag = 0, time_flag = 0, i_flag = 0, s4 = 0, s6 = 0;
	int option = 0;
	TParams param;
	param.server = (char *)malloc(IP4_SIZE * sizeof(char));
	param.domain_name = (char *)malloc(MAX_DOMAIN_NAME * sizeof(char));
	param.ptr_ipv6_flag = false;
	param.timeout_flag = false;
	param.timeout = 5;
	param.record_flag = false;
	param.record_type = A;
	param.iterative_flag = false;

	if((argc == 2) && (strcmp("-h", argv[1]) == 0))
		printHelp();
	// argc < 4 ? ----- !!!
	else if(argc < 4 || argc > 9)		//min. 4 parametry, max. 9
		printErr(ERROR_PARAMS);

	while ((option = getopt(argc, argv, "s:t:T:i")) != -1) {
	    switch (option) {
	    case 's':							//DNS server
	    	if(s_flag >= 1)
	    		printErr(ERROR_PARAMS);
	      param.server = optarg;
	      s_flag++;
	      break;
	    case 't':							//Record type
	    	if(t_flag >= 1)
					printErr(ERROR_PARAMS);
				
				if(strcmp(optarg, "A") == 0)
					param.record_type = A;
				else if(strcmp(optarg, "AAAA") == 0)
					param.record_type = AAAA;
				else if(strcmp(optarg, "NS") == 0)
					param.record_type = NS;
				else if(strcmp(optarg, "PTR") == 0)
					param.record_type = PTR;
				else if(strcmp(optarg, "CNAME") == 0)
					param.record_type = CNAME;
				else
					printErr(ERROR_PARAMS);					//zadan nesmyslny typ

				t_flag++;								          //flag na zjisteni duplikace parametru, pokud byl zadan vice krat nez jedna --> chyba
	      break;
	    case 'T':							//Timeout
	    	if(time_flag >= 1)
	    		printErr(ERROR_PARAMS);
	    	if(atoi(optarg) < 0)
					printErr(ERROR_PARAMS);
				param.timeout = atoi(optarg);
				time_flag++;
	    	break;
	    case 'i':
	    	if(i_flag >= 1)
	    		printErr(ERROR_PARAMS);
	    	param.iterative_flag = true;

//	    	printf("ITERATIVE flag -- OK\n");
	    	
	    	i_flag++;
	    	break;
	    default:
	    	printErr(ERROR_PARAMS);
	    }
 	}
 //	printf("TIMEOUT: %d\n", param.timeout);
 	 /* Overeni jestli bylo zadano jmeno souboru */
  if(argv[optind] == NULL)
    printErr(ERROR_PARAMS);
  strcpy(param.domain_name, argv[optind]);


	char arr[IN6ADDR_SIZE];
	struct sockaddr_in ipv4_addr;

  if(param.record_type == PTR){
  	s4 = inet_pton(AF_INET, argv[optind], &(ipv4_addr.sin_addr));
  	s6 = inet_pton(AF_INET6, argv[optind], arr);
  	if(s4 <= 0 && s6 <= 0)
  		printErr(ERROR_PARAMS);
  }
  if(s6 > 0){
  	param.revers_ipv6_dest = invertipv6(param.domain_name);
  	param.ptr_ipv6_flag = 1;
  }
  else{
  	param.revers_ipv4_dest = invertipv4(ipv4_addr.sin_addr.s_addr);
  }

 	return param;
}

void printHelp(){
	printf("USAGE: ... TODO\n");
	exit(NO_ERROR);
}

void printErr(Error TypeError){
  if(TypeError != 0){ 
      fprintf(stderr, "%s\n", ERR_TABLE[TypeError]);
      exit(TypeError);
  }
  return;
}

void dns_name_format(u_char* dns, u_char* host){
	int i, count_label = 0;
	int len = strlen((char*)host);
	for(i = 0; i < len; i++){
		if(*(host + i) != '.'){
			dns[i + 1] = *(host + i);
			count_label++;

			if(i == len-1)
				dns[i - count_label + 1] = count_label;
		}
		else if(*(host + i) == '.'){
			dns[i - count_label] = count_label;
			count_label = 0;
		}
		else
			printErr(ERROR);
	}
	dns[++i] = '\0';
}

void convert_from_dns_name(u_char* dns){
	int i = 0, j = 0, len = strlen((char*)dns);
	int lbl_cnt = 0;
	if(len == 0) 
		dns[0] = '.';
	else{ 
		for(i = 0; i < len; i++){
    	lbl_cnt = (int)dns[i];
    	for(j = 0; j < lbl_cnt ; j++) 
    	{
      	dns[i] = dns[i + 1];
    		i++;
    	}
    	dns[i] = '.';
  	}
  	dns[i] = '\0';
  }
  dns[i + 1] = '\0';
  printf("%s", dns);
}


int parse_dns_answer(u_char* packet, int size_to_answer, uint16_t origType){
	struct dns_header *received_hdr;
	struct resourceRecord *answer;
	u_char *pAnswer;								//pointer to start of answer --- Name
	char *name = (char *)malloc(MAX_DOMAIN_NAME * sizeof(char));
	int posuv = 0, flagType = 0, zero_name = 0;

	received_hdr = (struct dns_header *)packet;

/* ---------------------- Parsovani odpovedi ---------------------------------------- */
	pAnswer = packet + size_to_answer;											//ukazatel na zacatek odpovedi

	for(int i = 0; i < ntohs(received_hdr->ancount); i++){
		if(*pAnswer == 0x00 && *(pAnswer+1) == 0x00){
			pAnswer += 1;
			zero_name += 1;
		}
		else{
			posuv = get_dns_name(pAnswer, name, packet);
			pAnswer = pAnswer + posuv;
		}

		answer = (struct resourceRecord *)pAnswer;
		if(htons(answer->type) == A)
			printf(" IN A ");
		else if(htons(answer->type) == AAAA)
			printf(" IN AAAA ");
		else if(htons(answer->type) == PTR)
			printf(" IN PTR ");
		else if(htons(answer->type) == CNAME)
			printf(" IN CNAME ");
		else if(htons(answer->type) == NS){
			if(zero_name) printf(".");
			printf(" IN NS ");
			zero_name = 0;
		}
		/* je - li iny zaznam --------- PRESKOCIT ? */
		if(origType == htons(answer->type)){
			flagType = 1;
		}
		
		pAnswer = pAnswer + sizeof(struct resourceRecord) - 2;
		
		char str[128];
		switch(htons(answer->type)){
			case A:
			{
				struct sockaddr_in ipv4;
				long *p;
	      p=(long*)pAnswer;
	      ipv4.sin_addr.s_addr=(*p); //working without ntohl
	      printf("%s\n",inet_ntoa(ipv4.sin_addr));	
	      pAnswer += 4;
				break;
			}
			case CNAME:
				posuv = get_dns_name(pAnswer, name, packet);
				pAnswer += posuv;
				printf("\n");
				break;
			case PTR:
				posuv = get_dns_name(pAnswer, name, packet);
				printf("\n");
				pAnswer += posuv;
				break;
			case AAAA:
				printf("%s\n", inet_ntop(AF_INET6, pAnswer, str, 128));
				pAnswer += posuv;
				break;
			case NS:
				posuv = get_dns_name(pAnswer, name, packet);
				pAnswer = pAnswer + posuv + 1;
				printf("\n");
				break;
			default:
				printErr(ERROR);
		}
	}
	return (flagType == 1) ? 0 : 1;
}
/* Do 'name' se zapise vysledek */
int get_dns_name(u_char* pAnswer, char* name, u_char* packet){
	int posuv = 0, i = 0, flag = 0;
	uint16_t offset;
	uint8_t first_byte_offset;

	while(*pAnswer != 0){
		if(CHECK_BIT(*pAnswer, 7) && CHECK_BIT(*pAnswer, 6))
		{
			first_byte_offset = *pAnswer & 0x3F;					//vynulovat prvni dva bity maskou & 0011 1111;
			/* Write first two bytes that points into name to offset */
			offset = ((uint16_t)first_byte_offset << 8) | *(pAnswer + 1);
			pAnswer = packet + offset - 1;															//ukazatel na meno -- OK
			flag = 1;
		}
		else{
			name[i] = *pAnswer;
			i++;
		}
		pAnswer += 1;

		if(!flag){
			posuv++;
		}
	}
	name[i] = '\0';
	
	if(flag)
		posuv += 2;

	convert_from_dns_name((u_char*)name);
	return posuv;
}


string invertipv6 (const string &str) {
  struct in6_addr addr;
  inet_pton(AF_INET6,str.c_str(),&addr);
  char str2[48];
  string PTR_IPv6;

  sprintf(str2,"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
  addr.s6_addr[15], addr.s6_addr[14],
  addr.s6_addr[13], addr.s6_addr[12],
  addr.s6_addr[11], addr.s6_addr[10],
  addr.s6_addr[9], addr.s6_addr[8],
  addr.s6_addr[7], addr.s6_addr[6],
  addr.s6_addr[5], addr.s6_addr[4],
  addr.s6_addr[3], addr.s6_addr[2],
  addr.s6_addr[1], addr.s6_addr[0]);
	
	PTR_IPv6.push_back(0x01);
  
  for(int i = 0, x = 0; i < 64; i += 4, x += 2){
   	PTR_IPv6.push_back(str2[x + 1]);
   	PTR_IPv6.push_back(0x01);
   	PTR_IPv6.push_back(str2[x]);
		
   	(i == 60) ? PTR_IPv6.push_back(0x03) : PTR_IPv6.push_back(0x01);
	/*	if(i == 60)
			PTR_IPv6.push_back(0x03);    
		else
			PTR_IPv6.push_back(0x01);
	*/
	}
	PTR_IPv6.append("ip6");
	PTR_IPv6.push_back(0x04);
	PTR_IPv6.append("arpa");

	return PTR_IPv6;
}

string invertipv4(unsigned long int addr){
	string PTR_IPv4;

	char reversed_ip[INET_ADDRSTRLEN];

	addr =
    ((addr & 0xff000000) >> 24) |
    ((addr & 0x00ff0000) >>  8) |
    ((addr & 0x0000ff00) <<  8) |
    ((addr & 0x000000ff) << 24);

	inet_ntop(AF_INET, &addr, reversed_ip, sizeof(reversed_ip));

	PTR_IPv4 = reversed_ip;
	PTR_IPv4.append(".in-addr.arpa");

	return PTR_IPv4;
}
