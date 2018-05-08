/*
    Simple udp client
    Silver Moon (m00n.silv3r@gmail.com)
*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include"graph.h"
#include"udp.h"
 

#define BUFLEN 512  //Max length of buffer
char *SERVER;
int PORT;   //The port on which to send data
 
void die(char *s)
{
    perror(s);
    exit(1);
}
 
/*Argumentos: 1: Número do cliente, 2: Quantidade de roteadores*/
int main(int argc, char **argv)
{
	//int nuser = atoi(argv[1]);
	//char tc[10];


	if(argc<2){
		printf("Too few arguments to start router!\n");
		exit(1);	
	}
	else
		NROUTERS = atoi(argv[2])+1;
	int i, t[3];	
	
	struct AdjList *graph[NROUTERS];
	for(i=0; i<NROUTERS; i++)
		graph[i] = NULL;

	//struct sockaddr_in si_me, si_other, si_dest;
	int nuser = atoi(argv[1]);
	char tc[10];
	
	startGraphFromFile(graph);
	int destPORT,destROUTER = dijkstra(graph, nuser, 9);


	if(destROUTER==-1)
		exit(1);
	else if(!destROUTER)
		destROUTER=nuser;
	destPORT = PORT = -1;
	printf("Router %d: Mandar pacote para %d\n", nuser, destROUTER);
	ODfromFile(&PORT, &destPORT, nuser, destROUTER, tc);

	


	SERVER = tc;	
	printf("\n%d %d %s\n\n", nuser,PORT, SERVER);
	struct sockaddr_in si_other, si_me;
	int s, slen=sizeof(si_other), recv_len;
	char buf[BUFLEN];
	char message[BUFLEN];
 
	if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket");
	}
 

	// zero out the structure
	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind socket to port
	if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1){
		die("bind");
	}




    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(destPORT);
     
    if (inet_aton(SERVER , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
 int router = 9;
while(1){
		printf("Router %d] Waiting for data...\n", router);
		fflush(stdout);
		//receive a reply and print it
		//clear the buffer by filling null, it might have previously received data
		memset(buf,'\0', BUFLEN);

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1){
			die("recvfrom()");
		}
         
		//print details of the client/peer and the data received
		printf("Router %d] Received packet from %s:%d\n", router,inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("Router %d] Data: %s\n" , router,buf);
         	
		int k, a;
			for(k=0; k<998765432; k++)
				a+=2-3;
	
		//now reply the client with the same data
		/*if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_dest, slen) == -1){
			die("sendto()");
		}*/
	}
 
    close(s);
    return 0;
}