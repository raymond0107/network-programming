/**    
 *  This client.c file is as the client side.
 *  It could receive information from 4 servers.
 *	It could also calculate the topology of the network and the MST.
 *  It could send the topolopy information to every server.
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
#define REMOTE_PROT_A_UDP "21133"
#define REMOTE_PROT_B_UDP "22133"
#define REMOTE_PROT_C_UDP "23133"
#define REMOTE_PROT_D_UDP "24133"
#define LOCAL_PORT_TCP "25133"
#define MAXDATASIZE 1000
#define LOCALHOSTADDR "127.0.0.1"
#define BACKLOG 100
#define SERVERNUMBER 4

int main () {
	int i = 0;
	int j = 0;
	int sockfd_tcp; 
	int remotefd_tcp;        
	struct hostent *host_tcp;      
	struct sockaddr_in local_addr_tcp;      
	struct sockaddr_in remote_addr_tcp;     
	int buffer_send_tcp[MAXDATASIZE][MAXDATASIZE];
	char *buffer_receive_tcp[MAXDATASIZE];
	
	//Booting up the client.
	printf("The client has TCP port number %s, and IP address %s\n", LOCAL_PORT_TCP, LOCALHOSTADDR);
	
	/*
   		Part 1: 
   		Creat the client socket of tcp.
   	*/

	if ((sockfd_tcp = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		perror("error in create socket");
		exit(1); 
	}
	
	/*
		Part 2: 
		Bind the address. 
   	*/

   	struct addrinfo hints_tcp, *res_tcp;
   	memset(&hints_tcp, 0, sizeof hints_tcp);
   	hints_tcp.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
	hints_tcp.ai_socktype = SOCK_STREAM;
	getaddrinfo(LOCALHOSTADDR, LOCAL_PORT_TCP , &hints_tcp, &res_tcp);
   	if (bind(sockfd_tcp, res_tcp->ai_addr, res_tcp->ai_addrlen) == -1) {
   		perror("error in bind");
   		exit(1);
   	}

   	/*
   		Part 3: 
   		Listen the request from the server.
   	*/
   	
   	if (listen(sockfd_tcp, BACKLOG) == -1) {
  		perror("error in listen");
   		exit(1);
   	}

   	/*
   		Part 4  
   		Accpet multiple servers and reveice the informations.

   		Paramaters:
   		char *serverInf[][]: store the server's information including name and path cost.
   		char *serverName[][]: store the server's name.
   		char *serverCost[][]: store the server's path cost.
   		int server_cost[][]: change the path cost data type into INT.
   		int adjacency_matrix[][]: the adjacency matrix of all servers.
   	*/

   	char ans[SERVERNUMBER][MAXDATASIZE]; 
   	char *serverInf[SERVERNUMBER][SERVERNUMBER];
   	char *serverName[SERVERNUMBER][SERVERNUMBER];
   	char *serverCost[SERVERNUMBER][SERVERNUMBER];  
    int server_cost[SERVERNUMBER][SERVERNUMBER];
    int adjacency_matrix[SERVERNUMBER][SERVERNUMBER];
    int nums = 0;  
	for (i = 0; i < SERVERNUMBER; i++) {
		for (j = 0; j < SERVERNUMBER; j++) {
			server_cost[i][j] = 0;
			adjacency_matrix[i][j] = 0;
		}
	}
   	while (nums < SERVERNUMBER) {
   		socklen_t sin_size = sizeof(struct sockaddr_in);
   		if ((remotefd_tcp = accept(sockfd_tcp,(struct sockaddr*)&remote_addr_tcp ,&sin_size)) == -1) {
			perror("error in accept");
			exit(1);
		}
		char *strAddr_tcp = inet_ntoa(remote_addr_tcp.sin_addr);
		int remote_prot_tcp = remote_addr_tcp.sin_port;
		if (nums == 0) {
			printf("The Client receivers neighbor information from the Server A with TCP port number %d and IP address %s\n", remote_prot_tcp, strAddr_tcp);
			printf("The Server A has the following neighbor information:\n");
		}
		else if (nums == 1) {
			printf("The Client receivers neighbor information from the Server B with TCP port number %d and IP address %s\n", remote_prot_tcp, strAddr_tcp);
			printf("The Server B has the following neighbor information:\n");
		}
		else if (nums == 2) {
			printf("The Client receivers neighbor information from the Server C with TCP port number %d and IP address %s\n", remote_prot_tcp, strAddr_tcp);
			printf("The Server V has the following neighbor information:\n");
		}
		else {
			printf("The Client receivers neighbor information from the Server D with TCP port number %d and IP address %s\n", remote_prot_tcp, strAddr_tcp);
			printf("The Server D has the following neighbor information:\n");
		}	
		printf("Neighbor-Cost\n");

		// reveice the server's information.
		if (recv(remotefd_tcp, ans[nums], MAXDATASIZE, 0) == -1) {
			perror("error in receive");
			exit(1);
		}

		// show the server information.
		printf("%s\n", ans[nums]);
		if (nums == 0) {
			printf("For this connection with Server A, The Client has TCP port number %s and IP address %s.\n", LOCAL_PORT_TCP, LOCALHOSTADDR);
		}
		else if (nums == 1) {
			printf("For this connection with Server B, The Client has TCP port number %s and IP address %s.\n", LOCAL_PORT_TCP, LOCALHOSTADDR);
		}
		else if (nums == 2) {
			printf("For this connection with Server C, The Client has TCP port number %s and IP address %s.\n", LOCAL_PORT_TCP, LOCALHOSTADDR);
		}
		else {
			printf("For this connection with Server D, The Client has TCP port number %s and IP address %s.\n", LOCAL_PORT_TCP, LOCALHOSTADDR);
		}	
		char *token;
		char s[2] = "\n";
		token = strtok(ans[nums], s);

		// deal with the data.
		int k = 0;
		while (token) {
			serverInf[nums][k] = token;
	    	k++;
	    	token = strtok(NULL, s);	
		}
		char *token_child;
		char c[2] = " ";
		for (i = 0; i < k; i++) {
			token_child = strtok(serverInf[nums][i], c);
			serverName[nums][i] = token_child;
			token_child = strtok(NULL, c);
			serverCost[nums][i] = token_child;	
			if (strcmp(serverName[nums][i], "serverA") == 0) {
				server_cost[nums][0] = atoi(serverCost[nums][i]);
				adjacency_matrix[nums][0] = 1;
			}
			else if (strcmp(serverName[nums][i], "serverB") == 0) {
				server_cost[nums][1] = atoi(serverCost[nums][i]);
				adjacency_matrix[nums][1] = 1;
			}
			else if (strcmp(serverName[nums][i], "serverC") == 0) {
				server_cost[nums][2] = atoi(serverCost[nums][i]);
				adjacency_matrix[nums][2] = 1;
			}
			else {
				server_cost[nums][3] = atoi(serverCost[nums][i]);
				adjacency_matrix[nums][3] = 1;
			}
		}
		nums++;
		close(remotefd_tcp);
	}
	/*
		Part 6:
		Calculate each path and corrsponding cost.

		Paramaters:
   		char *start[]: store the start vertex of one side.
   		char *end[]: store the end vertex of one side.
   		char edge[][]: store the topology sides'name.
   		char edge_COST[][]: store the topolopy sides length.
   		int edge_NUM: represent the topology's side number.
	*/

	char *start[MAXDATASIZE];
	char *end[MAXDATASIZE];
	char edge[MAXDATASIZE][MAXDATASIZE];
	char edge_COST[MAXDATASIZE][MAXDATASIZE];
	int edge_NUM = 0;
	for (i = 0; i < SERVERNUMBER; i++) {
		for (j = i; j < SERVERNUMBER; j++) {
			if (server_cost[i][j] != 0) {
				if (i == 0) {
					start[edge_NUM] = "A";
				}
				else if (i == 1) {
					start[edge_NUM] = "B";
				}
				else if (i == 2) {
					start[edge_NUM] = "C";
				}
				else {
					start[edge_NUM] = "D";
				}
				strcpy(edge[edge_NUM],start[edge_NUM]);
				if (j == 0) {
					end[edge_NUM] = "A";
				}
				else if (j == 1) {
					end[edge_NUM] = "B";
				}
				else if (j == 2) {
					end[edge_NUM] = "C";
				}
				else {
					end[edge_NUM] = "D";
				}
				strcat(edge[edge_NUM],end[edge_NUM]); 
				sprintf(edge_COST[edge_NUM], "%d", server_cost[i][j]);
				edge_NUM++;
			}
		}
	}
	/*
		Part 7:
		Store the topology information into a string.

		Parameter:
		char topology[]: store the topology information.
	*/
	printf("--------test... %s\n",edge[0]);
	char topology[MAXDATASIZE];
	strcpy(topology,edge[0]);
	strcat(topology," "); 
	strcat(topology,edge_COST[0]); 
	strcat(topology,"\n"); 
	for (i = 1; i < edge_NUM; i++) {
		strcat(topology,edge[i]);
		strcat(topology," "); 
		strcat(topology,edge_COST[i]); 
		strcat(topology,"\n"); 
	}
	close(sockfd_tcp);

	/*
		Part 8:
		Creat the client socket of udp.
	*/

	int sockfd_udp;             
	struct sockaddr_in local_addr_udp;         
	struct sockaddr_in remote_addr_udp; 
	if ((sockfd_udp = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("error in create socket");
		exit(1); 
	}

	int status;
	struct addrinfo hints_udp, *res_udp;
   	memset(&hints_udp, 0, sizeof hints_udp);
   	hints_udp.ai_family = AF_UNSPEC; 
	hints_udp.ai_socktype = SOCK_DGRAM;

	struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    int getsock_check;
    char *client_udp_addr;
    int client_udp_port;
    
	/*
		Part 9:
		Send the topology to the server A.
		Get the client UDP port number.
	*/

	if ((status = getaddrinfo(LOCALHOSTADDR, REMOTE_PROT_A_UDP, &hints_udp, &res_udp)) == -1) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
 	}
 	socklen_t sin_size = sizeof(struct sockaddr_in);				
 	int result = sendto(sockfd_udp, topology, MAXDATASIZE, 0,res_udp->ai_addr, res_udp->ai_addrlen);
 	if (result == -1) {
 		perror("sendto failed\n");
 		exit(1);
 	}
 	printf("The Client has sent the network topology Server A with UDP port number %s and IP address %s as follows\n",REMOTE_PROT_A_UDP,  LOCALHOSTADDR);
	printf("Edge-Cost\n");
	printf("%s",topology);
	getsock_check = getsockname(sockfd_tcp, (struct sockaddr *)&client_addr,&addrlen);
    if (getsock_check == -1) {
        perror("getsockname");
        exit(1);
    }
    client_udp_addr = inet_ntoa(client_addr.sin_addr);
    client_udp_port = client_addr.sin_port;
    printf("For this connection with the Server A, the Client has UDP port number %d and IP address %s.\n",client_udp_port , LOCALHOSTADDR);

	
	/*
		Part 10:
		Send the topology to the server B.
		Get the client UDP port number.
	*/

 	if ((status = getaddrinfo(LOCALHOSTADDR, REMOTE_PROT_B_UDP, &hints_udp, &res_udp)) == -1) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
 	}
 	sin_size = sizeof(struct sockaddr_in);				
 	result = sendto(sockfd_udp, topology, MAXDATASIZE, 0,res_udp->ai_addr, res_udp->ai_addrlen);
 	if (result == -1) {
 		perror("sendto failed\n");
 		exit(1);
 	}
 	printf("The Client has sent the network topology Server B with UDP port number %s and IP address %s\n",REMOTE_PROT_B_UDP,  LOCALHOSTADDR);
 	printf("Edge-Cost\n");
	printf("%s",topology);
	getsock_check = getsockname(sockfd_tcp, (struct sockaddr *)&client_addr,&addrlen);
    if (getsock_check == -1) {
        perror("getsockname");
        exit(1);
    }
    client_udp_addr = inet_ntoa(client_addr.sin_addr);
    client_udp_port = client_addr.sin_port;
    printf("For this connection with the Server B, the Client has UDP port number %d and IP address %s.\n",client_udp_port , LOCALHOSTADDR);

 	/*
		Part 11:
		Send the topology to the server C.
		Get the client UDP port number.
	*/
 	
 	if ((status = getaddrinfo(LOCALHOSTADDR, REMOTE_PROT_C_UDP, &hints_udp, &res_udp)) == -1) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
 	}

 	sin_size = sizeof(struct sockaddr_in);				
 	result = sendto(sockfd_udp, topology, MAXDATASIZE, 0,res_udp->ai_addr, res_udp->ai_addrlen);
 	if (result == -1) {
 		perror("sendto failed\n");
 		exit(1);
 	}
 	printf("The Client has sent the network topology Server C with UDP port number %s and IP address %s\n",REMOTE_PROT_C_UDP,  LOCALHOSTADDR);
 	printf("Edge-Cost\n");
	printf("%s",topology);
	getsock_check = getsockname(sockfd_tcp, (struct sockaddr *)&client_addr,&addrlen);
    if (getsock_check == -1) {
        perror("getsockname");
        exit(1);
    }
    client_udp_addr = inet_ntoa(client_addr.sin_addr);
    client_udp_port = client_addr.sin_port;
    printf("For this connection with the Server C, the Client has UDP port number %d and IP address %s.\n",client_udp_port , LOCALHOSTADDR);

 	/*
		Part 12:
		Send the topology to the server D.
		Get the client UDP port number.
	*/
 	
 	if ((status = getaddrinfo(LOCALHOSTADDR, REMOTE_PROT_D_UDP, &hints_udp, &res_udp)) == -1) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
 	}
 	sin_size = sizeof(struct sockaddr_in);				
 	result = sendto(sockfd_udp, topology, MAXDATASIZE, 0,res_udp->ai_addr, res_udp->ai_addrlen);
 	if (result == -1) {
 		perror("sendto failed\n");
 		exit(1);
 	}
 	printf("The Client has sent the network topology Server D with UDP port number %s and IP address %s\n",REMOTE_PROT_D_UDP,  LOCALHOSTADDR);
	printf("Edge-Cost\n");
	printf("%s",topology);
	getsock_check = getsockname(sockfd_tcp, (struct sockaddr *)&client_addr,&addrlen);
    if (getsock_check == -1) {
        perror("getsockname");
        exit(1);
    }
    client_udp_addr = inet_ntoa(client_addr.sin_addr);
    client_udp_port = client_addr.sin_port;
    printf("For this connection with the Server D, the Client has UDP port number %d and IP address %s.\n",client_udp_port , LOCALHOSTADDR);

 	/* Part 13:
	   Calculate the MST.

	   Paramaters:
	   int mst[][]: store the information of the mst.
	   char *mst_INF： store the information of edge and their path cost.
	*/
	for (i = 0; i < SERVERNUMBER; i++) {
		for (j = i; j < SERVERNUMBER; j++) {
			server_cost[j][i] = server_cost[i][j];
		}
	}
	char *mst_INF[SERVERNUMBER - 1];
	int mst[SERVERNUMBER][SERVERNUMBER];
	int vertex[SERVERNUMBER];
	int vertex_Now = 0;
	int vertex_Num = 0;
	int k = 0, m = 0, min, flag;
	while (vertex_Num < SERVERNUMBER) {
		min = 100000;
		vertex[vertex_Num] = vertex_Now;
		for (i = 0; i < SERVERNUMBER; i++) {
			flag = 0;
			for (k = 0; k < vertex_Num + 1; k++) {
				if (i == vertex[k]) {
					flag = 1;
				}
			}
			k = 0;
			while (k < vertex_Num + 1 && flag == 0) {
				if (server_cost[i][vertex[k]] != 0 && server_cost[i][vertex[k]] < min) {
					min = server_cost[i][vertex[k]];
					j = i;
					m = vertex[k];
				}
				k++;
			}
		}
		mst[j][m] = server_cost[j][m];
		vertex_Now = j;
		vertex_Num++;
	}

	/*
		Part 14:
		Calculate each path and corrsponding cost.

		Paramaters:
   		char *start_mst[]: store the start vertex of one side in MST.
   		char *end_mst[]: store the end vertex of one side in MST.
   		char edge_mst[][]: store the topology sides'name in MST.
   		char edge_COST_mst[][]: store the MST sides length.
   		int edge_NUM_mst: represent the MST's side number.
	*/

	char *start_mst[MAXDATASIZE];
	char *end_mst[MAXDATASIZE];
	char edge_mst[SERVERNUMBER - 1][MAXDATASIZE];
	char edge_COST_mst[SERVERNUMBER - 1][MAXDATASIZE];
	int edge_NUM_mst = 0;
	for (i = 0; i < SERVERNUMBER; i++) {
		for (j = 0; j < SERVERNUMBER; j++) {
			if (mst[i][j] != 0) {
				if (j == 0) {
					start_mst[edge_NUM_mst] = "A";
				}
				else if (j == 1) {
					start_mst[edge_NUM_mst] = "B";
				}
				else if (j == 2) {
					start_mst[edge_NUM_mst] = "C";
				}
				else {
					start_mst[edge_NUM_mst] = "D";
				}
				strcpy(edge_mst[edge_NUM_mst],start_mst[edge_NUM_mst]);
				if (i == 0) {
					end_mst[edge_NUM_mst] = "A";
				}
				else if (i == 1) {
					end_mst[edge_NUM_mst] = "B";
				}
				else if (i == 2) {
					end_mst[edge_NUM_mst] = "C";
				}
				else {
					end_mst[edge_NUM_mst] = "D";
				}
				strcat(edge_mst[edge_NUM_mst],end_mst[edge_NUM_mst]); 
				sprintf(edge_COST_mst[edge_NUM_mst], "%d", mst[i][j]);
				edge_NUM_mst++;
			}
		}
	}

	/*
		Part 15:
		Store the MST information into a string.
		Print the MST information.

		Parameter:
		char mst_inf[]: store the MST information.
	*/

	int total_cost = 0;
	char mst_inf[MAXDATASIZE];
	strcpy(mst_inf,edge_mst[0]);
	strcat(mst_inf," "); 
	strcat(mst_inf,edge_COST_mst[0]); 
	total_cost += atoi(edge_COST_mst[0]);
	strcat(mst_inf,"\n"); 
	for (i = 1; i < edge_NUM_mst; i++) {
		strcat(mst_inf,edge_mst[i]);
		strcat(mst_inf," "); 
		strcat(mst_inf,edge_COST_mst[i]); 
		total_cost += atoi(edge_COST_mst[i]);
		strcat(mst_inf,"\n"); 
	}
	printf("The Client has calculated a tree. The tree cost is %d\n", total_cost);
	printf("Edge-Cost\n");
	printf("%s", mst_inf);
	close(sockfd_udp);
	return 0;
}
