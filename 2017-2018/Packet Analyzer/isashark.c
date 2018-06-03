/*
** Author: Anastasiia Stoika
** Project: packet analyzer - isashark
*/

#define _BSD_SOURCE
#define __FAVOR_BSD

#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h> 
#include <err.h>
#include <unistd.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip6.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>

#ifdef __linux__            // for Linux
#include <netinet/ether.h> 
#include <time.h>
#include <pcap/pcap.h>
#endif

#ifndef PCAP_ERRBUF_SIZE
#define PCAP_ERRBUF_SIZE (256)
#endif

#define SIZE_ETHERNET (14)       // offset of Ethernet header to L3 protocol
#define SIZE_IPV6 (40)

#define TCP_ID 6
#define UDP_ID 17
#define ICMPv4_ID 1
#define ICMPv6_ID 58
#define NO_PORT (-1)
/** Extend header ID */
#define ROUTE_HEADER_ID 43
#define HOP_HEADER_ID 0
#define DESTINATION_HEADER_ID 60
#define FRAGMENT_HEADER_ID 44
#define AUTH_HEADER_ID 51
#define ENCAP_HEADER_ID 50
#define NO_NEXT_HEADER_ID 59
#define QnQ_PROVIDER_BRIDGE 0x88a8  /* 802.1ad */

/* Additional flags in TCP */
#define TH_CWR 0x80
#define TH_ECE 0x40

/* For processinf ICMPv4, ICMPv6 messages - types and codes */
#define ERR_MAX 20
#define TYPE_ICMPv4_MAX 256
#define TYPE_ICMPv6_MAX 256
#define DEST_UNR_MAX_4 16
#define DEST_UNR_MAX_6 8
#define REDIRECT_MAX 4
#define ROUT_ADV_MAX 17
#define TIME_EX_MAX 2
#define P_PR_MAX 3
#define P_PR_MAX_6 4
#define PH_MAX 6
#define R_RENUMB_MAX 2
#define NODE_INF_MAX 3

typedef enum Error {
    ERROR_OK = 0,    
    ERROR_PARAM = 1,
    ERROR_FILE = 2,
    ERROR_UNDEF = 3,

    ERROR_TODO,
    ERROR_PARSE_EXTEND_HEADER,
    ERROR_MALLOC,
    ERROR_AGREGATION_KEY,
    ERROR_SORT,
    ERROR_WRONG_SORT_KEY,
    ERROR_UNKNOWN_PROTOCOL
} Error;
/* Structure for input parameters */
typedef struct params{
  char *aggr_key;
  bool aggr_flag;
  char *sort_key;
  bool sort_flag;
  int  limit;
  bool limit_flag;
  char *filter_expr;
  bool filter_flag;
  char *inpt_files[ERR_MAX];
  int number_files;
}TParams;


typedef struct element{
	u_char* dhost;
	u_char* shost;
	struct in_addr *ip_src, *ip_dst;
	struct in6_addr *ip6_src, *ip6_dst;
	u_short sport, dport;
	int length;
	int count;
	struct element * next;
}tElem;


typedef struct list{
	tElem *FIRST;
	tElem *LAST;
}tList;

struct  ether_header_802 {
  u_short ether_TPID;	
  u_short   vlan_id;
  u_short ether_type;
};

const char *ERR_TABLE[ERR_MAX] = {
    "V poradku.",
    "Chybne zadane vstupni parametry.",
    "Can't open file for reading.",
    "Unknown error.",
    "ERROR_TODO: Neco mas dodelat!!!.",
    "ERROR_PARSE_EXTEND_HEADER: Undefined header_id, i dont support that ID. Sorry bro!!!",
    /*<ERROR_MALLOC>*/
    "ERROR_MALLOC: To low memmory. Cant allocate mem.",
    /*ERROR_AGREGATION_KEY*/
    "ERROR_AGREGATION_KEY: I dont recognize that aggregation key.",
    /*ERROR_SORT*/
    "ERROR_SORT: Something is wrong with sort, LIST.FIRST and LIST.LAST must be NULL and sortList.FIRST and sortList.LAST != NULL",
    /*ERROR_WRONG_SORT_KEY*/
    "ERROR_WRONG_SORT_KEY: I dont support that sort_key, support only [bytes|packets]. Thank you!!!",
    "ERROR_UNKNOWN_PROTOCOL: Unknown protocol in captured packet."
};
/** Start of processing ICMPv4 messages **/
const char *TYPES_ICMPv4_TAB[TYPE_ICMPv4_MAX] = { //{[0] = "type1",[2] = "type2"};
  "Echo Reply",
  "Unassigned", 
  "Unassigned",  
  "Destination Unreachable",
  "Source Quench (Deprecated)",
  "Redirect",
  "Alternate Host Address (Deprecated)",
  "Unassigned",
  "Echo",
  "Router Advertisement",
  "Router Solicitation",
  "Time Exceeded",
  "Parameter Problem",
  "Timestamp",
  "Timestamp Reply",
  "Information Request (Deprecated)",
  "Information Reply (Deprecated)",
  "Address Mask Request (Deprecated)",
  "Address Mask Reply (Deprecated)",
  "Reserved (for Security)",
  "Reserved (for Robustness Experiment)",
  [30] = "Traceroute (Deprecated)",
  "Datagram Conversion Error (Deprecated)",
  "Mobile Host Redirect (Deprecated)",
  "IPv6 Where-Are-You (Deprecated)",
  "IPv6 I-Am-Here (Deprecated)",
  "Mobile Registration Request (Deprecated)",
  "Mobile Registration Reply (Deprecated)",
  "Domain Name Request (Deprecated)",
  "Domain Name Reply (Deprecated)",
  "SKIP (Deprecated)",
  "Photuris",
  "ICMP messages utilized by experimental mobility protocols such as Seamoby",
  "Unassigned",
  [253] = "RFC3692-style Experiment 1",
  "RFC3692-style Experiment 2",
  "Reserved"
};
/* Codes for specific types of ICMPv4 */
const char *DEST_UNR_TYPE_4[DEST_UNR_MAX_4] = {
  "Net Unreachable",
  "Host Unreachable",
  "Protocol Unreachable",
  "Port Unreachable",
  "Fragmentation Needed and Don't Fragment was Set",
  "Source Route Failed",
  "Destination Network Unknown",
  "Destination Host Unknown",
  "Source Host Isolated",
  "Communication with Destination Network is Administratively Prohibited",
  "Communication with Destination Host is Administratively Prohibited",
  "Destination Network Unreachable for Type of Service",
  "Destination Host Unreachable for Type of Service",
  "Communication Administratively Prohibited",
  "Host Precedence Violation",
  "Precedence cutoff in effect"
};
const char *REDIRECT_TYPE[REDIRECT_MAX] = {
  "Redirect Datagram for the Network (or subnet)",
  "Redirect Datagram for the Host",  
  "Redirect Datagram for the Type of Service and Network",
  "Redirect Datagram for the Type of Service and Host"
};
const char *ROUTER_ADVERT[ROUT_ADV_MAX] = {
  "Normal router advertisement",
  [16] = "Does not route common traffic"
};
const char *TIME_EXCEEDED_T[TIME_EX_MAX] = {
  "Time to Live exceeded in Transit",  
  "Fragment Reassembly Time Exceeded "
};
const char *PARAM_PROBLEM[P_PR_MAX] = {
  "Pointer indicates the error", 
  "Missing a Required Option",
  "Bad Length"
};
const char *PHOTURIS_T[PH_MAX] = {
  "Bad SPI",
  "Authentication Failed", 
  "Decompression Failed",  
  "Decryption Failed",
  "Need Authentication",
  "Need Authorization",
};
/** End of processing ICMPv4 messages **/
/** Processing ICMPv6 messages **/
const char *TYPES_ICMPv6_TAB[TYPE_ICMPv6_MAX] = {       
  "Reserved",
  "Destination Unreachable",
  "Packet Too Big",
  "Time Exceeded",
  "Parameter Problem",
  [100] = "Private experimentation",
  "Private experimentation",
  "Unassigned",
  [127] = "Reserved for expansion of ICMPv6 error messages",
  "Echo Request",
  "Echo Reply",
  "Multicast Listener Query",
  "Multicast Listener Report",
  "Multicast Listener Done",
  "Router Solicitation",
  "Router Advertisement",
  "Neighbor Solicitation",
  "Neighbor Advertisement",
  "Redirect Message",
  "Router Renumbering",                                             /* Type 138 */
  "ICMP Node Information Query",
  "ICMP Node Information Response",
  "Inverse Neighbor Discovery",
  "Inverse Neighbor Discovery",
  "Version 2 Multicast Listener Report",
  [144] = "Home Agent Address Discovery",
  "Home Agent Address Discovery",
  "Mobile Prefix Solicitation",
  "Mobile Prefix Advertisement",
  "Certification Path Solicitation Message",
  "Certification Path Advertisement Message", 
  "ICMP messages utilized by experimental mobility protocols such as Seamoby",
  "Multicast Router Advertisement", 
  "Multicast Router Solicitation",  
  "Multicast Router Termination",   
  "FMIPv6 Messages [RFC5568]", 
  "RPL Control Message",  
  "ILNPv6 Locator Update Message",  
  "Duplicate Address Request", 
  "Duplicate Address Confirmation",   
  "MPL Control Message", 
  [200] = "Private experimentation",
  "Private experimentation",
  [255] = "Reserved for expansion of ICMPv6 informational messages"
};
const char *DEST_UNR_TYPE_6[DEST_UNR_MAX_6] = {                     /* Type 1 */
  "no route to destination", 
  "communication with destination administratively prohibited",  
  "beyond scope of source address",
  "address unreachable", 
  "port unreachable",  
  "source address failed ingress/egress policy",
  "reject route to destination",
  "Error in Source Routing Header"
};
const char *TIME_EXCEEDED_T6[TIME_EX_MAX] = {                       /* Type 3 */
  "hop limit exceeded in transit",
  "fragment reassembly time exceeded"
};
const char *PARAM_PROBLEM_6[P_PR_MAX_6] = {                         /* Type 4 */
  "erroneous header field encountered",  
  "unrecognized Next Header type encountered", 
  "unrecognized IPv6 option encountered",  
  "IPv6 First Fragment has incomplete IPv6 Header Chain"
};
const char *ROUTER_RENUMB[R_RENUMB_MAX] = {                         /* Type 138 */
  "Router Renumbering Command",
  "Router Renumbering Result"
};
const char *NODE_INF_QUERY[NODE_INF_MAX] = {                        /* Type 139 */
  "The Data field contains an IPv6 address which is the Subject of this Query.",
  "The Data field contains a name which is the Subject of this Query, or is empty, as in the case of a NOOP.",
  "The Data field contains an IPv4 address which is the Subject of this Query."
};
const char *NODE_INF_RESP[NODE_INF_MAX] = {                         /* Type 140 */
  "A successful reply. The Reply Data field may or may not be empty.",
  "The Responder refuses to supply the answer. The Reply Data field will be empty.",
  "The Qtype of the Query is unknown to the Responder. The Reply Data field will be empty."
};

/* Prototypes of functions */
int parse_ext_header( u_char ** ext_header, int header_id);
void parse_icmp(u_char* packet2, int nxt_hdr, bool aggr_flag);
void parse_tcp(tList *LIST, u_char *packet_skip, char* aggr_key, int length, bool aggr_flag);
void parse_udp(tList *LIST, u_char *packet_skip, char* aggr_key, int length, bool aggr_flag);
void printErr(Error TypeError);
void printHelp();
TParams getParams(int argc, char **argv);
void parse_ipv4(tList * LIST, char* aggr_key, u_char* packet2, int length, bool aggr_flag);
void parse_ipv6(tList * LIST, tList * IPv6LIST, char* aggr_key, u_char* packet2, int length, bool aggr_flag);

void print_binnary(int n);


void sortList(tList * LIST, tList * IPv6LIST, char * sort_key);
void printList(tList * LIST, tList * IPv6LIST, TParams param);
void AgregateMac(tList *LIST, u_char* shost, u_char* dhost, int length);
void AgregateIPv4(tList *LIST, struct in_addr* ip_src, struct in_addr* ip_dst, int length);

void AgregateIPv6(tList *LIST, struct in6_addr* ip6_src, struct in6_addr* ip6_dst, int length);
void AgregatePort(tList *LIST, int srcport, int dstport, int length);

u_short reverse(u_short x);







int main(int argc, char *argv[]){

	tList LIST;
	LIST.FIRST = NULL;	
	LIST.LAST = NULL;

	tList IPv6LIST;
	IPv6LIST.FIRST = NULL;
	IPv6LIST.LAST = NULL;


  int packet_number = 0, i = 0;
  char errbuf[PCAP_ERRBUF_SIZE];                  // constant defined in pcap.h
  const u_char *packet;
  struct pcap_pkthdr header;  
  struct ether_header *eptr;
  pcap_t *handle;                                  // file/device handler
  u_char *packet_skip;
  struct ether_header_802 *header802;

  /* Get input parameters */
  TParams param = getParams(argc, argv);

while(i < param.number_files){
  /* open capture file for offline processing */
  if ((handle = pcap_open_offline(param.inpt_files[i],errbuf)) == NULL)
      printErr(ERROR_FILE);

  /* Filtering */
  struct bpf_program fp;    /* The compiled filter */

  if (pcap_compile(handle, &fp, param.filter_expr, 0, PCAP_NETMASK_UNKNOWN) == -1) {
    fprintf(stderr, "Couldn't parse filter %s: %s\n", param.filter_expr, pcap_geterr(handle));
    return(2);
   }
   if (pcap_setfilter(handle, &fp) == -1) {
    fprintf(stderr, "Couldn't install filter %s: %s\n", param.filter_expr, pcap_geterr(handle));
    return(2);
   }

   
  /* Read packets from the file */
  while ((packet = pcap_next(handle,&header)) != NULL){
    packet_number++;
    
    /* Read the Ethernet header */
    eptr = (struct ether_header *) packet;

    /* Aggregation */
    if(strcmp(param.aggr_key, "srcmac") == 0){
    	AgregateMac(&LIST, eptr->ether_shost, NULL, header.len);
    }else if(strcmp(param.aggr_key, "dstmac") == 0){
    	AgregateMac(&LIST, NULL, eptr->ether_dhost, header.len);
    }

    /* Kontrola pro vypisy -- jestli bola zadana agregacia v parametroch, tak se vypise, jinak normalni vypis */
    if(!param.aggr_flag){
      /* Print the packet header data - cislo paketu, casova znacka v microsekundach, dlzka paketu */
      printf("%d: ",packet_number); //
      printf("%ld %d",((header.ts.tv_sec*1000000) + header.ts.tv_usec), header.len);  
      /* Printing Ethernet frame information */
      printf(" | Ethernet: %02x:%02x:%02x:%02x:%02x:%02x", eptr->ether_shost[0], eptr->ether_shost[1],
                                                           eptr->ether_shost[2], eptr->ether_shost[3],
                                                           eptr->ether_shost[4], eptr->ether_shost[5]); 
      printf(" %02x:%02x:%02x:%02x:%02x:%02x", eptr->ether_dhost[0], eptr->ether_dhost[1],
                                                           eptr->ether_dhost[2], eptr->ether_dhost[3],
                                                           eptr->ether_dhost[4], eptr->ether_dhost[5]);
    }

    //============================================================================
    /* SME V ETHERNET HLAVICKE A ROZPOZNAVAME JAKY TYP PAKETU ZAHRNUJE */
    switch (ntohs(eptr->ether_type)){               // see /usr/include/net/ethernet.h for types
      /* IPv4 packet == 0x0800 */

      case ETHERTYPE_IP:
      {
        u_char *packet2 = (u_char*) packet;
        packet2 += SIZE_ETHERNET; /* (packet + SIZE_ETHERNET) */
        
        parse_ipv4(&LIST, param.aggr_key, packet2, header.len, param.aggr_flag);
      
        break; 
      }  
      case ETHERTYPE_IPV6: 
      {
        u_char *packet2 = (u_char*) packet;
        packet2 += SIZE_ETHERNET; /* (packet + SIZE_ETHERNET) */

        parse_ipv6(&LIST, &IPv6LIST, param.aggr_key, packet2, header.len, param.aggr_flag);
      
        break;
      }
      case ETHERTYPE_VLAN:
      {
        header802 = (struct ether_header_802*) (packet + SIZE_ETHERNET - 2);

        if(!param.aggr_flag) printf(" %d", header802->vlan_id >> 8);
        

        if(ntohs(header802->ether_type) == 0x88a8){     /* Jestli nasledujici po VLAN je QnQ */
          header802 = (struct ether_header_802*) (packet + SIZE_ETHERNET + 2);
          if(!param.aggr_flag) printf(" %d", header802->vlan_id >> 8);
          /* Osetrenie na dalsi hlavicky */
          if(ntohs(header802->ether_type) == ETHERTYPE_IP){
            u_char *packet2 = (u_char*) packet;
            packet2 += SIZE_ETHERNET + 8;
            parse_ipv4(&LIST, param.aggr_key, packet2, header.len, param.aggr_flag);
          }
          else if(ntohs(header802->ether_type) == ETHERTYPE_IPV6){
          //  printf(" HERE %04x\n", ntohs(header802->ether_type));
            u_char *packet2 = (u_char*) packet;
            packet2 += SIZE_ETHERNET + 8;
            parse_ipv4(&LIST, param.aggr_key, packet2, header.len, param.aggr_flag);
          }
          else{
            /* Neznamy protokol - vypise chybove hlaseni na stderr a preskoci packet */
            fprintf(stderr, "ERROR: %s\n", ERR_TABLE[ERROR_UNKNOWN_PROTOCOL]);
          }
        }
        /*Jestli nasledujici po VLAN je IPv6*/
        else if(ntohs(header802->ether_type) == ETHERTYPE_IPV6){
          u_char *packet2 = (u_char*) packet;
            packet2 += SIZE_ETHERNET + 4;
            parse_ipv6(&LIST, &IPv6LIST, param.aggr_key, packet2, header.len, param.aggr_flag);
        }
        /*Jestli nasledujici po VLAN je IPv4*/
        else if(ntohs(header802->ether_type) == ETHERTYPE_IP){
          u_char *packet2 = (u_char*) packet;
          packet2 += SIZE_ETHERNET + 4;
          parse_ipv4(&LIST, param.aggr_key, packet2, header.len, param.aggr_flag);
        }
        }
        break;

      case QnQ_PROVIDER_BRIDGE:
      {
        header802 = (struct ether_header_802*) (packet + SIZE_ETHERNET - 2);
        /*Vypiseme VLAN ID do Ethernet polozky*/
        if(!param.aggr_flag) printf(" %d", header802->vlan_id >> 8);
        /* Jestli nasledujici po QnQ je VLAN */
        if(ntohs(header802->ether_type) == 0x8100){
        	header802 = (struct ether_header_802*) (packet + SIZE_ETHERNET + 2);
          if(!param.aggr_flag) printf(" %d", header802->vlan_id >> 8);
          /* Osetrenie na dalsi hlavicky */
          if(ntohs(header802->ether_type) == ETHERTYPE_IP){
            u_char *packet2 = (u_char*) packet;
            packet2 += SIZE_ETHERNET + 8;
            parse_ipv4(&LIST, param.aggr_key, packet2, header.len, param.aggr_flag);
          }
          else if(ntohs(header802->ether_type) == ETHERTYPE_IPV6){
            u_char *packet2 = (u_char*) packet;
            packet2 += SIZE_ETHERNET + 8;
            parse_ipv6(&LIST, &IPv6LIST, param.aggr_key, packet2, header.len, param.aggr_flag);
          }
          else{
            /* Neznamy protokol - vypise chybove hlaseni na stderr a preskoci packet */
            fprintf(stderr, "ERROR: %s\n", ERR_TABLE[ERROR_UNKNOWN_PROTOCOL]);
          }
        }
        /*Jestli je nasledujici po Q-n-Q je IPv6*/
        else if(ntohs(header802->ether_type) == ETHERTYPE_IPV6){
          u_char *packet2 = (u_char*) packet;
            packet2 += SIZE_ETHERNET + 4;
            parse_ipv6(&LIST, &IPv6LIST, param.aggr_key, packet2, header.len, param.aggr_flag);
        }
        /* Jestli nasledujici po Q-n-Q je IPv4*/ 
        else if(ntohs(header802->ether_type) == ETHERTYPE_IP){
          u_char *packet2 = (u_char*) packet;
          packet2 += SIZE_ETHERNET + 4;
          parse_ipv4(&LIST, param.aggr_key, packet2, header.len, param.aggr_flag);
        }
        
        break;
      }
      default:
        fprintf(stderr, "ERROR: %s\n", ERR_TABLE[ERROR_UNKNOWN_PROTOCOL]);
      } 
      if(!param.aggr_flag)
        printf("\n");

      if(param.limit_flag){
        if (packet_number == param.limit)
          break; /* end while cyklus */
      }
  }
  /* Vypis agregace */
  if(param.aggr_flag && i == param.number_files - 1)
    printList(&LIST, &IPv6LIST, param);
  
  /* close the capture device and deallocate resources */
  pcap_close(handle);
  i++;

} // konec while - pri vice souborech
  return 0;
}
/** ----------------------------- Funkce pro parsovani hlavicek ruznych protokolu-------------------------------------------- **/

/** parse_ext_header
  -> roll all extend header, @out and move [ext_header] to right pointer
  @in   - [ext_header] - pointer to pointer to packet
        - [header_id] - id of expected header what i need parse
  @out  - return header ID after all extend header [TCP_ID|UDP_ID|ICMPv6_ID|NO_NEXT_HEADER_ID]
  @err  - if i dont support expected header ID
*/
int parse_ext_header(u_char ** ext_header, int header_id){

  /*if this was last extend header and next header is TCP,UDP,ICMPv6 or isnt exist next header return ID*/
  if(header_id == TCP_ID || header_id == UDP_ID || header_id == ICMPv6_ID || header_id == NO_NEXT_HEADER_ID)
    return header_id;
  /*if i need roll extend header then check type next extend header and roll pointer to right position*/
  else if(header_id == HOP_HEADER_ID || header_id == DESTINATION_HEADER_ID || header_id == ROUTE_HEADER_ID){
    struct ip6_rthdr *ext_rout = (struct ip6_rthdr *)(*ext_header);
    *ext_header += 8 + ext_rout->ip6r_len*8;                        //<-- roll position
    return parse_ext_header( ext_header, ext_rout->ip6r_nxt);       //<-- recursive call parse function
  /*if i have extend header, which is fixed size 8 then roll*/
  }else if(header_id == FRAGMENT_HEADER_ID || header_id == AUTH_HEADER_ID ){
    struct ip6_rthdr *ext_rout = (struct ip6_rthdr *)(*ext_header);
    *ext_header += 8;
    return parse_ext_header( ext_header, ext_rout->ip6r_nxt);
  }else if(header_id == ENCAP_HEADER_ID){
    /*pretypovat, vytiahnut delku, roll, rekurzivne zavolat funkciu*/
    printErr(ERROR_TODO);
  } 
  /*if i have header id which i dont support, then error and exit*/
  else
    printErr(ERROR_PARSE_EXTEND_HEADER);
}

void parse_ipv4(tList * LIST, char *aggr_key, u_char* packet2, int length, bool aggr_flag){
  u_int size_ip;
 // u_char *packet_skip;
  struct ip *my_ip = (struct ip*) (packet2);        /*skip Ethernet header --> bude ukazovat na zaciatok hlavicky ipv4*/
  size_ip = my_ip->ip_hl*4;                                      /*length of IP header*/
  /* Vypis IPv4 hlavicky */
  if(!aggr_flag){
    printf(" | IPv4: %s ",inet_ntoa(my_ip->ip_src));
    printf("%s ",inet_ntoa(my_ip->ip_dst));
    printf("%d", my_ip->ip_ttl);
  }
  switch (my_ip->ip_p){
    case ICMPv4_ID:
      packet2 += size_ip;
     // packet_skip = (u_char*) packet;
      parse_icmp(packet2, ICMPv4_ID, aggr_flag);
      break;
    case TCP_ID:
      packet2 += size_ip;
    //  packet_skip = (u_char*) packet;
      parse_tcp(LIST, packet2, aggr_key, length, aggr_flag);
      break;
    case UDP_ID: // UDP protocol
      packet2 += size_ip;
    //  packet_skip = (u_char*) packet;
      parse_udp(LIST, packet2, aggr_key, length, aggr_flag);
      break;
    default: 
      fprintf(stderr, "ERROR: %s\n", ERR_TABLE[ERROR_UNKNOWN_PROTOCOL]);
  }
  /* Agregace */
  if(strcmp(aggr_key, "srcip") == 0){
    AgregateIPv4(LIST, &my_ip->ip_src, NULL, length);
  }else if(strcmp(aggr_key, "dstip") == 0){
    AgregateIPv4(LIST, NULL, &my_ip->ip_dst, length);
  }
  return;
}
void parse_ipv6(tList *LIST, tList * IPv6LIST, char* aggr_key, u_char* packet2, int length, bool aggr_flag){
    /* Skipp to ipv6 header */
  struct ip6_hdr *my_ip6 = (struct ip6_hdr*) (packet2);        
  char str[128];

  if(!aggr_flag){
    printf(" | IPv6: %s ", inet_ntop(AF_INET6,&(my_ip6->ip6_src), str, 128));
    printf("%s ", inet_ntop(AF_INET6,&(my_ip6->ip6_dst), str, 128));
    printf("%d", my_ip6->ip6_ctlun.ip6_un1.ip6_un1_hlim);
  }

  /*Check next header if TCP,UDP.. or roll extend header*/
  int next_header = my_ip6->ip6_ctlun.ip6_un1.ip6_un1_nxt;
  packet2 += SIZE_IPV6;
       
  //u_char* packet2 = (u_char*) packet;
  if(next_header == TCP_ID)
    parse_tcp(LIST, packet2, aggr_key, length, aggr_flag);
  else if(next_header == UDP_ID)
    parse_udp(LIST, packet2, aggr_key, length, aggr_flag);
  else{   
  /* Next header code after all extended headers */
    int hdr_after_extend = parse_ext_header(&packet2, next_header);
    if(hdr_after_extend == ICMPv6_ID)
      parse_icmp(packet2, hdr_after_extend, aggr_flag);
    else if(hdr_after_extend == TCP_ID){
      parse_tcp(LIST, packet2, aggr_key, length, aggr_flag);
    }else if(hdr_after_extend == UDP_ID){
      parse_udp(LIST, packet2, aggr_key, length, aggr_flag);
    }else{
      fprintf(stderr, "ERROR: %s\n", ERR_TABLE[ERROR_UNKNOWN_PROTOCOL]);
    }
  }
  /* Agregace */
  if(strcmp(aggr_key, "srcip") == 0){
    AgregateIPv6(IPv6LIST, &my_ip6->ip6_src, NULL, length);
  }else if(strcmp(aggr_key, "dstip") == 0){
    AgregateIPv6(IPv6LIST, NULL, &my_ip6->ip6_dst, length);
  }
  return;
}

void parse_icmp(u_char* packet2, int nxt_hdr, bool aggr_flag){
  struct icmp *my_icmp = (struct icmp *)(packet2);

  if(nxt_hdr == ICMPv4_ID){
    if(!aggr_flag){
      printf(" | ICMPv4: %u %u", my_icmp->icmp_type, my_icmp->icmp_code);
      if(my_icmp->icmp_type >= 20 && my_icmp->icmp_type <=29) printf(" %s", TYPES_ICMPv4_TAB[20]);
      else if(my_icmp->icmp_type >= 42 && my_icmp->icmp_type <=252) printf(" %s", TYPES_ICMPv4_TAB[42]);
      else printf(" %s", TYPES_ICMPv4_TAB[my_icmp->icmp_type]);

      if (my_icmp->icmp_type == 3) 
        printf(" %s", DEST_UNR_TYPE_4[my_icmp->icmp_code]);
      else if (my_icmp->icmp_type == 5)        
        printf(" %s", REDIRECT_TYPE[my_icmp->icmp_code]);
      else if (my_icmp->icmp_type == 9)
        printf(" %s", ROUTER_ADVERT[my_icmp->icmp_code]);
      else if (my_icmp->icmp_type == 11)
        printf(" %s", TIME_EXCEEDED_T[my_icmp->icmp_code]);
      else if (my_icmp->icmp_type == 12)
        printf(" %s", PARAM_PROBLEM[my_icmp->icmp_code]);
      else if (my_icmp->icmp_type == 40)
        printf(" %s", PHOTURIS_T[my_icmp->icmp_code]);
    }
  }
  else if(nxt_hdr == ICMPv6_ID){
    if(!aggr_flag){
      printf(" | ICMPv6: %u %u", my_icmp->icmp_type, my_icmp->icmp_code); 
      if((my_icmp->icmp_type >= 102 && my_icmp->icmp_type <=126) || (my_icmp->icmp_type >= 160 && my_icmp->icmp_type<=199)) printf(" %s", TYPES_ICMPv6_TAB[102]);
      else printf(" %s", TYPES_ICMPv6_TAB[my_icmp->icmp_type]);
      
      if (my_icmp->icmp_type == 1)
        printf(" %s", DEST_UNR_TYPE_6[my_icmp->icmp_code]);
      else if (my_icmp->icmp_type == 3)
        printf(" %s", TIME_EXCEEDED_T6[my_icmp->icmp_code]);
      else if (my_icmp->icmp_type == 4)
        printf(" %s", PARAM_PROBLEM_6[my_icmp->icmp_code]);
      else if (my_icmp->icmp_type == 138)
        printf(" %s", ROUTER_RENUMB[my_icmp->icmp_code]);
      else if (my_icmp->icmp_type == 139)
        printf(" %s", NODE_INF_QUERY[my_icmp->icmp_code]);
      else if (my_icmp->icmp_type == 140)
        printf(" %s", NODE_INF_RESP[my_icmp->icmp_code]);
    }
  }
  return;
}

void parse_tcp(tList *LIST, u_char *packet_skip, char* aggr_key, int length, bool aggr_flag){
 /*skip ip header -> pointer to tcp header*/
  const struct tcphdr *my_tcp = (struct tcphdr *) (packet_skip);

  if(!aggr_flag){
    printf(" | TCP: %d %d %u %u",ntohs(my_tcp->th_sport), ntohs(my_tcp->th_dport), ntohl(my_tcp->th_seq), ntohl(my_tcp->th_ack));        
   /* Printing flags */ 
    (my_tcp->th_x2 & TH_CWR)        ?   printf(" C") : printf(" .");
    (my_tcp->th_x2 & TH_ECE)        ?   printf("E") : printf(".");
    (my_tcp->th_flags & TH_URG)     ?   printf("U") : printf(".");
    (my_tcp->th_flags & TH_ACK)     ?   printf("A") : printf(".");
    (my_tcp->th_flags & TH_PUSH)    ?   printf("P") : printf(".");
    (my_tcp->th_flags & TH_RST)     ?   printf("R") : printf(".");
    (my_tcp->th_flags & TH_SYN)     ?   printf("S") : printf(".");
    (my_tcp->th_flags & TH_FIN)     ?   printf("F") : printf(".");
  }
  /*Agregace*/
  if(strcmp(aggr_key, "srcport") == 0){
    AgregatePort(LIST, my_tcp->th_sport, NO_PORT, length);
  }else if(strcmp(aggr_key, "dstport") == 0){
    AgregatePort(LIST, NO_PORT, my_tcp->th_dport, length);
  }
  return;
}

void parse_udp(tList *LIST, u_char *packet_skip, char* aggr_key, int length, bool aggr_flag){
  const struct udphdr *my_udp = (struct udphdr *) (packet_skip); // pointer to the UDP header

  if(!aggr_flag)
    //printf(" | UDP: %d %d %d",ntohs(my_udp->uh_sport), ntohs(my_udp->uh_dport), ntohs(my_udp->uh_ulen));
      printf(" | UDP: %d %d",ntohs(my_udp->uh_sport), ntohs(my_udp->uh_dport));

  /*Agregace*/
  if(strcmp(aggr_key, "srcport") == 0){
    AgregatePort(LIST, my_udp->uh_sport, NO_PORT, length);
  }else if(strcmp(aggr_key, "dstport") == 0){
    AgregatePort(LIST, NO_PORT, my_udp->uh_dport, length);
  }
  return;
}

//TParams getParams(int argc, char **argv){
TParams getParams(int argc, char **argv){
  int option = 0;

  TParams param = {
    .aggr_key = malloc(50 * sizeof(char)),
    .aggr_flag = false,
    .sort_key = malloc(50 * sizeof(char)),
    .sort_flag = false,
    .limit = 0,
    .limit_flag = false,
    .filter_expr = malloc(50 * sizeof(char)),
    .filter_flag = false,
    .inpt_files = malloc(50 * sizeof(char)),
    .number_files = 0,
  };
  while ((option = getopt(argc, argv, "ha:s:l:f:")) != -1) {
    switch (option) {
      case 'h':
        printHelp();
        break;
      case 'a':
        sprintf(param.aggr_key, "%s", optarg);
        /* Osetreni jestli agregacni kluc bol spravne zadan */
        if((strcmp(param.aggr_key, "srcmac") == 0) ||(strcmp(param.aggr_key, "dstmac") == 0) 
            || (strcmp(param.aggr_key, "srcip") == 0) || (strcmp(param.aggr_key, "dstip") == 0)
            || (strcmp(param.aggr_key, "srcport") == 0) ||(strcmp(param.aggr_key, "dstport") == 0) ){
          param.aggr_flag = true;
        }
        else{
          printErr(ERROR_AGREGATION_KEY);
        } 
        break;
      case 's':
        /* !!!TODO - Osetreni je ve funkci Sort */
        param.sort_flag = true;
        sprintf(param.sort_key, "%s", optarg);
        break;
      case 'l':
        param.limit_flag = true;
        if(atoi(optarg) == 0 || atoi(optarg) < 0)
          printErr(ERROR_PARAM);
        param.limit = atoi(optarg);
        break;
      case 'f':
        param.filter_flag = true;
        sprintf(param.filter_expr, "%s", optarg);
        break;
      default:
        printErr(ERROR_PARAM);
      }
  }
  /* Overeni jestli bylo zadano jmeno souboru */
  if(argv[optind] == NULL)
    printErr(ERROR_PARAM);
 
  /** Prijimani vice souboru *********/
  int i = 0;
  while(argv[optind+i]){
    param.inpt_files[i] = argv[optind+i];
    param.number_files++;
    i++;
  }
  
  return param;
}
/** Konec funkci pro parsovani hlavicek protokolu **/

void printErr(Error TypeError){
  if(TypeError != 0){ // 0 nebo ERROR_OK
      fprintf(stderr, "ERROR: %s\n", ERR_TABLE[TypeError]);
      exit(TypeError);
  }
  return;
}

void printHelp(){
  printf("Napoveda bla bla.\n");
  exit(EXIT_SUCCESS);
  //return;
}

void print_binnary(int n){

  for(int i = 12; i; i--, n>>=1) {
      if (n & 1)
          printf("1");
      else
          printf("0");
  }
  printf("\n");
}
/** -------------------------------------------------Funkce pro agregace--------------------------------------------------------- **/
void AgregateMac(tList *LIST, u_char* shost, u_char* dhost, int length){

	bool add_flag = true;
	tElem * temp;
	if(LIST->FIRST != NULL){

		for(temp = LIST->FIRST; temp != NULL; temp = temp->next){

			if(shost != NULL){
				if(strncmp(temp->shost, shost, 6) == 0){
					temp->length += length;
					temp->count ++;
					add_flag = false;
					break;
				}
			}
			else if(dhost != NULL){
				if(strncmp(temp->dhost, dhost, 6) == 0){
					temp->length += length;
					temp->count ++;
					add_flag = false;
					break;
				}
			}
		}
	}
	if(add_flag){
		tElem * n = NULL;
		n = malloc(sizeof(tElem));
		if(n == NULL)
			printErr(ERROR_MALLOC);

		if(shost != NULL){
			n->shost = malloc(6*sizeof(u_char));
			memcpy(n->shost, shost, 6*sizeof(u_char));
		}else if(dhost != NULL){
			n->dhost = malloc(6*sizeof(u_char));
			memcpy(n->dhost, dhost, 6*sizeof(u_char));
		}
		n->next = NULL;
		n->length = length;
		n->count = 1;

		if(LIST->FIRST == NULL){
			LIST->FIRST = n;
			LIST->LAST = n;
		}else{
			LIST->LAST->next = n;
			LIST->LAST = n;
		}
	}
	return;
}

void AgregateIPv4(tList *LIST, struct in_addr* ip_src, struct in_addr* ip_dst, int length){

	bool add_flag = true;
	tElem * temp;
	if(LIST->FIRST != NULL){

		for(temp = LIST->FIRST; temp != NULL; temp = temp->next){
			if(ip_src != NULL){
				if(strncmp((const char *)temp->ip_src, (const char*)ip_src, sizeof(struct in_addr)) == 0){
					temp->length += length;
					temp->count ++;
					add_flag = false;
					break;
				}
			}
			else if(ip_dst != NULL){
				if(strncmp((const char *)temp->ip_dst, (const char *)ip_dst, sizeof(struct in_addr)) == 0){
					temp->length += length;
					temp->count ++;
					add_flag = false;
					break;
				}
			}
		}
	}
	if(add_flag){
		tElem * n = NULL;
		n = malloc(sizeof(tElem));
		if(n == NULL)
			printErr(ERROR_MALLOC);

		n->ip6_src = NULL;
		n->ip6_dst = NULL;

		if(ip_src != NULL){
			n->ip_src = malloc(sizeof(struct in_addr));
			memcpy(n->ip_src, ip_src, sizeof(struct in_addr));
		}else if(ip_dst != NULL){
			n->ip_dst = malloc(sizeof(struct in_addr));
			memcpy(n->ip_dst, ip_dst, sizeof(struct in_addr));
		}
		n->next = NULL;
		n->length = length;
		n->count = 1;
		
		if(LIST->FIRST == NULL){
			LIST->FIRST = n;
			LIST->LAST = n;
		}else{
			LIST->LAST->next = n;
			LIST->LAST = n;
		}
	}
	return;
}

void AgregateIPv6(tList *LIST, struct in6_addr* ip6_src, struct in6_addr* ip6_dst, int length){

	bool add_flag = true;
	tElem * temp;
	if(LIST->FIRST != NULL){
		char str[128];
		char str2[128];

		for(temp = LIST->FIRST; temp != NULL; temp = temp->next){

			if(ip6_src != NULL){
				if(strcmp(inet_ntop(AF_INET6, temp->ip6_src, str, 128), inet_ntop(AF_INET6, ip6_src, str2, 128)) == 0){
					temp->length += length;
					temp->count ++;
					add_flag = false;
					break;
				}
			}
			else if(ip6_dst != NULL){
				if(strcmp(inet_ntop(AF_INET6, temp->ip6_dst, str, 128), inet_ntop(AF_INET6, ip6_dst, str2, 128)) == 0){
					temp->length += length;
					temp->count ++;
					add_flag = false;
					break;
				}
			}
		}
	}
	if(add_flag){
		tElem * n = NULL;
		n = malloc(sizeof(tElem));
		if(n == NULL)
			printErr(ERROR_MALLOC);

		n->ip_src = NULL;
		n->ip_dst = NULL;

		if(ip6_src != NULL){
			n->ip6_src = malloc(sizeof(struct in6_addr));
			memcpy(n->ip6_src, ip6_src, sizeof(struct in6_addr));
		}else if(ip6_dst != NULL){
			n->ip6_dst = malloc(sizeof(struct in6_addr));
			memcpy(n->ip6_dst, ip6_dst, sizeof(struct in6_addr));
		}
		n->next = NULL;
		n->length = length;
		n->count = 1;

		if(LIST->FIRST == NULL){
			LIST->FIRST = n;
			LIST->LAST = n;
		}else{
			LIST->LAST->next = n;
			LIST->LAST = n;
		}
	}
	return;
}

void AgregatePort(tList *LIST, int srcport, int dstport, int length){
  bool add_flag = true;
  tElem * temp;
  if(LIST->FIRST != NULL){

    for(temp = LIST->FIRST; temp != NULL; temp = temp->next){
      if(srcport != NO_PORT){
        if(temp->sport == srcport){
          temp->length += length;
          temp->count ++;
          add_flag = false;
          break;
        }
      }
      else if(dstport != NO_PORT){
        if(temp->dport == dstport){
          temp->length += length;
          temp->count ++;
          add_flag = false;
          break;
        }
      }
    }
  }
/* Pokud dany port se vyskytuje v listu poprve --> pridat novu polozku v listu */
  if(add_flag){
    tElem * n = NULL;
    n = malloc(sizeof(tElem));
    if(n == NULL)
      printErr(ERROR_MALLOC);
    /* Umisteni do listu src port z analyzovaneho paketu */
    if(srcport != NO_PORT)
      n->sport = srcport;
    else
      n->dport = dstport;

    n->next = NULL;
    n->length = length;
    n->count = 1;
    
    if(LIST->FIRST == NULL){
      LIST->FIRST = n;
      LIST->LAST = n;
    }else{
      LIST->LAST->next = n;
      LIST->LAST = n;
    }
  }
  return;
}
/** ----------------------------------------------------SORT------------------------------------------------------------ **/

void sortList(tList * LIST, tList * IPv6LIST, char * sort_key){

	if(LIST->FIRST == NULL || LIST->LAST == NULL)
		return;

	tList sortList;
	sortList.FIRST = NULL;
	sortList.LAST = NULL;

	tElem * temp = LIST->FIRST;
	tElem * before = LIST->FIRST;

	int length = 0;
	int packets = 0;
	tElem * maxPoint = NULL;
	tElem * beforeMaxPoint = NULL;

	/*Check if i need sort IPv6*/
	if(IPv6LIST != NULL){
		if(IPv6LIST->FIRST != NULL){
			LIST->LAST->next = IPv6LIST->FIRST;
			LIST->LAST = IPv6LIST->LAST;
		}
	}

	/*while if i something change*/
	while(LIST->FIRST != NULL){

		if(LIST->FIRST == NULL)
			break;
		/* Search the max element for bytes*/
		if(strcmp(sort_key, "bytes") == 0){
			for(; temp != NULL; before = temp, temp = temp->next){
        /* Pocatecni hodnota length pro porovnani je 0 */
				if(temp->length > length){
					length = temp->length;
					maxPoint = temp;
					beforeMaxPoint = before;
				}
			}
		}
		/* Search the max element for packets*/
		else if(strcmp(sort_key, "packets") == 0){
			for(; temp != NULL; before = temp, temp = temp->next){
				if(temp->count > packets){
					packets = temp->count;
					maxPoint = temp;
					beforeMaxPoint = before;
				}
			}
		}else{
			printErr(ERROR_WRONG_SORT_KEY);
		}
		/*Cut element from LIST*/
		if(maxPoint == LIST->FIRST){
			if(LIST->FIRST == LIST->LAST) {
				LIST->FIRST = NULL;
				LIST->LAST = NULL;
			}else{
				LIST->FIRST = maxPoint->next;
			}
		}else if(LIST->LAST == maxPoint)
			LIST->LAST = beforeMaxPoint;
		else
			beforeMaxPoint->next = maxPoint->next;
		
		maxPoint->next = NULL;

		/*move max element from LIST to sortList*/
		if(sortList.FIRST == NULL){
			sortList.FIRST = maxPoint;
			sortList.LAST = maxPoint;
		}else{
			sortList.LAST->next = maxPoint;
			sortList.LAST = maxPoint;
		}

		/*Initializaction for next search*/
		temp = LIST->FIRST;
		before = LIST->FIRST;
		length = 0;
		packets = 0;
	}

	LIST->FIRST = sortList.FIRST;
	LIST->LAST = sortList.LAST;

	return;
}


void printList(tList * LIST, tList * IPv6LIST, TParams param){

	if(LIST->FIRST == NULL || LIST->LAST == NULL)
		return;

	tElem * temp = LIST->FIRST;
	char str[128];

	if(param.sort_flag && (0 == strcmp(param.aggr_key, "srcip"))){
		sortList(LIST, IPv6LIST, param.sort_key);
		temp = LIST->FIRST;
		for(; temp != NULL; temp = temp->next){
			/*print ipv4*/
			if(temp->ip6_dst == NULL && temp->ip6_src == NULL){
				printf("%s: %d %d\n", inet_ntoa(*(temp->ip_src)), temp->count, temp->length);
			}
			/*print ipv6*/
			else{
				printf("%s: %d %d\n", inet_ntop(AF_INET6,temp->ip6_src, str, 128), temp->count, temp->length);
			}
		}
		return;
		
	}else if(param.sort_flag && (0 == strcmp(param.aggr_key, "dstip"))){
		sortList(LIST, IPv6LIST, param.sort_key);
		temp = LIST->FIRST;
		for(; temp != NULL; temp = temp->next){
			/*print ipv4*/
			if(temp->ip6_dst == NULL && temp->ip6_src == NULL)
				printf("%s: %d %d\n", inet_ntoa(*(temp->ip_dst)), temp->count, temp->length);
			/*print ipv6*/
			else
				printf("%s: %d %d\n", inet_ntop(AF_INET6,temp->ip6_dst, str, 128), temp->count, temp->length);
		}
		return;
	}


	if(param.sort_flag){ sortList(LIST, NULL, param.sort_key); temp = LIST->FIRST;}


	if(strcmp(param.aggr_key, "srcmac") == 0){
		for(; temp != NULL; temp = temp->next){
      printf("%02x:%02x:%02x:%02x:%02x:%02x", temp->shost[0], temp->shost[1], temp->shost[2], temp->shost[3], temp->shost[4], temp->shost[5]); 
      printf(": %d %d\n", temp->count, temp->length);
    }	
  }else if(strcmp(param.aggr_key, "dstmac") == 0){
		for(; temp != NULL; temp = temp->next){
      printf("%02x:%02x:%02x:%02x:%02x:%02x", temp->dhost[0], temp->dhost[1], temp->dhost[2], temp->dhost[3], temp->dhost[4], temp->dhost[5]); 
      printf(": %d %d\n", temp->count, temp->length);
    }
  /* Agregation with src ipv4 */
	}else if(strcmp(param.aggr_key, "srcip") == 0){
		for(; temp != NULL; temp = temp->next)
			printf("%s: %d %d\n", inet_ntoa(*(temp->ip_src)), temp->count, temp->length);

		if(IPv6LIST->FIRST == NULL)
			return;
    /* Agregation with src ipv6 */
		temp = IPv6LIST->FIRST;
		for(; temp != NULL; temp = temp->next)
			printf("%s: %d %d\n", inet_ntop(AF_INET6,temp->ip6_src, str, 128), temp->count, temp->length);
  /* Agregation with dst ipv4 */
	}else if(strcmp(param.aggr_key, "dstip") == 0){
		for(; temp != NULL; temp = temp->next)
			printf("%s: %d %d\n", inet_ntoa(*(temp->ip_dst)), temp->count, temp->length);
  /* Agregation with dst ipv6 */
		if(IPv6LIST->FIRST == NULL)
			return;
		temp = IPv6LIST->FIRST;
		for(; temp != NULL; temp = temp->next)
			printf("%s: %d %d\n", inet_ntop(AF_INET6,temp->ip6_dst, str, 128), temp->count, temp->length);

	}
  /* Agregation with srcport */
  else if(strcmp(param.aggr_key, "srcport") == 0){
    for(; temp != NULL; temp = temp->next)
      printf("%d: %d %d\n", ntohs(temp->sport), temp->count, temp->length);
  }
  /* Agregation with dstport */
  else if(strcmp(param.aggr_key, "dstport") == 0){
    for(; temp != NULL; temp = temp->next)
      printf("%d: %d %d\n", ntohs(temp->dport), temp->count, temp->length);
  }
	else{
		printErr(ERROR_AGREGATION_KEY);
	}
}

