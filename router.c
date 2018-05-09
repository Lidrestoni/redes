/*
    Simple udp server
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



void die(char *s)
{
    perror(s);
    exit(1);
}
 
/*Argumentos: 1: Número do roteador, 2: Quantidade de roteadores*/
int main(int argc, char **argv)
{
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

	struct sockaddr_in si_me, si_other, si_dest;
	int router = atoi(argv[1]);
	char *ipdest, *iprouter;
	PORT = -1;
	PortsFromFile(&PORT, router, &iprouter);
	startGraphFromFile(graph);
	int destPORT,destROUTER;
	
	int s, slen = sizeof(si_other) , recv_len;
	char buf[BUFLEN];
	
	//create a UDP socket
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
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

	memset((char *) &si_dest, 0, sizeof(si_dest));
	si_dest.sin_family= AF_INET;	
	
	si_dest.sin_addr.s_addr = htonl(INADDR_ANY);
	
	 struct UDPMessage mes;    
     
	//keep listening for data
	while(1){
		fflush(stdout);
		//clear the buffer by filling null, it might have previously received data
		memset(buf,'\0', BUFLEN);

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1){
			die("recvfrom()");
		}
         	
		int k, a;
			for(k=0; k<998765432; k++)
				a+=2-3;


		StrToUDPMessage (buf, &mes);
		destROUTER = dijkstra(graph, router, mes.idDest);
		if(destROUTER==-1)
			exit(1);
		else if(!destROUTER)
			destROUTER=router;
		destPORT  = -1;

		//ODfromFile(&PORT, &destPORT, router, destROUTER, ip);
		PortsFromFile(&destPORT, destROUTER, &ipdest);
		si_dest.sin_port = htons(destPORT);
		if (inet_aton(ipdest , &si_dest.sin_addr) == 0){
			fprintf(stderr, "inet_aton() failed\n");
			exit(1);
		}
	
		//now send data forward
		printf("Nodo %d encaminhando mensagem #%d para o nodo %d, com destino final no nodo %d\n", router,mes.idMes,destROUTER, mes.idDest);
		if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_dest, slen) == -1){
			die("sendto()");
		}
	}
 	close(s);
	return 0;
}
