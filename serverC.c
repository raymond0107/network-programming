/**    
 *  This serverC.c file is as the serverC side.
 *  It could read the information of network from a file called serverC.txt.
 *	It could send the information to the client.
 *  It could receive the topology information of the network from client.
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#define CLIENT_PORT_TCP "25133"
#define SERVER_C_PORT_UDP "23133"
#define MAXDATASIZE 100
#define BACKLOG 100
#define LOCALHOSTADDR "127.0.0.1"
#define PORTYNUMBER 4

int main() {

	printf("The Server C is up and running.\n");
	
	/*
		Part 1:
		Read a file
	*/

	FILE *fp;
	char fp_str[PORTYNUMBER][MAXDATASIZE];
	int length, row = 0;
	fp = fopen("serverC.txt", "r");
	if (fp == NULL) {
		perror("Error opening file");
		return(-1);
	}
	printf("The Server C has the following neighbor information:\n");
	printf("Neighbor-Cost\n");
	while (fgets(fp_str[row], MAXDATASIZE, fp) != NULL) {
		length = strlen(fp_str[row]);
		printf("%s",fp_str[row]);
		row++;
	}
	printf("\n");
	fclose(fp);
	fp = NULL;

	/* 
    	Part 2:
   		connect to the client using tcp. Now the server plays as a client.
	*/

   	int status_tcp;
   	struct addrinfo hints_tcp, *res_tcp;
   	memset(&hints_tcp, 0, sizeof hints_tcp);
   	hints_tcp.ai_family = AF_UNSPEC;
   	hints_tcp.ai_socktype = SOCK_STREAM;
   	if ((status_tcp = getaddrinfo(LOCALHOSTADDR,CLIENT_PORT_TCP, &hints_tcp, &res_tcp)) == -1) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status_tcp));
		exit(1);
 	}
    int sockfd_tcp = socket(res_tcp->ai_family, res_tcp->ai_socktype, res_tcp->ai_protocol);
    if (connect(sockfd_tcp, res_tcp->ai_addr, res_tcp->ai_addrlen) == -1) {
		perror("error in connect"); 
		exit(1);
	}

	/*
		Part 3:
		Send path information to the client.
		And print the serverC itself TCP port number and the client TCP port number.
	*/

	int len;
	char inf[MAXDATASIZE];
	if (row == 1) {
		strcpy(inf,fp_str[0]);
	}
	else {
		strcpy (inf,fp_str[0]);
		int i = 1;
		while (i < row ) {
			strcat(inf,fp_str[i]);
			i++;
		}
	}
	if (send(sockfd_tcp, inf, MAXDATASIZE, 0) == -1) {
		perror("error in send");
		exit(1);
	}
	printf("The Server C finishes sending its neighbor information to the Client with TCP port number %s and IP address %s (Client's TCP port number and IP address).\n",CLIENT_PORT_TCP, LOCALHOSTADDR);
    struct sockaddr_in my_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    int getsock_check = getsockname(sockfd_tcp, (struct sockaddr *)&my_addr,&addrlen);
    if (getsock_check == -1) {
        perror("getsockname");
        exit(1);
    }
    char *my_tcp_addr = inet_ntoa(my_addr.sin_addr);
    int my_tcp_port = my_addr.sin_port;
    printf("For this connection with the Client, the Server C has TCP port number %d and IP address %s.\n",my_tcp_port , LOCALHOSTADDR);
	close(sockfd_tcp);

	/*
		Part 4:
		Creat a udp socket.
	*/

	int sockfd_udp;             
	struct sockaddr_in local_addr_udp;         
	struct sockaddr_in remote_addr_udp;
	socklen_t addr_len;
 	if ((sockfd_udp = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("error in create socket");
		exit(1); 
	}
 	int status_udp;
   	struct addrinfo hints_udp, *res_udp;
   	memset(&hints_udp, 0, sizeof hints_udp);
   	hints_udp.ai_family = AF_INET;
   	hints_udp.ai_socktype = SOCK_DGRAM;
   	hints_udp.ai_flags = AI_PASSIVE;
   	if ((status_udp = getaddrinfo(NULL,SERVER_C_PORT_UDP, &hints_udp, &res_udp)) == -1) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status_udp));
		exit(1);
 	}

 	/*
 		Part 5:
 		Bind.
 	*/

 	if (bind(sockfd_udp, res_udp->ai_addr, res_udp->ai_addrlen) == -1) {
		close(sockfd_udp);
		perror("bind error");
		exit(1);
	}
	freeaddrinfo(res_udp);
	
	/*
		Part 6:
		Reveive the topology information from the client.
		Print the client UDP port number and the serverC itself UDP port number.

		Parameter:
		char topology[]: store the topology information of the netword from the client.
	*/

	char topology[MAXDATASIZE];
	addr_len = sizeof(remote_addr_udp);				
 	int result = recvfrom(sockfd_udp, topology, MAXDATASIZE - 1, 0,(struct sockaddr *)&remote_addr_udp, &addr_len);
 	if (result == -1) {
 		perror("receive failed");
 		exit(1);
 	}
 	char *client_addr_udp = inet_ntoa(remote_addr_udp.sin_addr);
    int client_udp_port = remote_addr_udp.sin_port;
    printf("The server C has received the network topology from the Client with UDP port number %d and IP address %s "
                   "(Client's UDP port number and IP address) as follows:\n",client_udp_port,client_addr_udp);
    printf("Edge-Cost\n" );
 	printf("%s", topology);
 	printf("For this connection with Client, The ServerC has UDP port number %s and IP address %s\n",SERVER_C_PORT_UDP,LOCALHOSTADDR);
 	close(sockfd_udp);
	return 0;
}
