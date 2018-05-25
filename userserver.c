/*
    Simple udp router
    Silver Moon (m00n.silv3r@gmail.com)
*/

#include "udp.h"

 
#define BUFLEN 512  //Max length of buffer

static time_t ROUTERDELAY = 1;

 
/*Argumentos: 1: Número do roteador, 2: Quantidade de roteadores*/
int main(int argc, char **argv)
{
	if(argc<2){
		printf("Too few arguments to start router!\n");
		exit(1);	
	}	
	configureAndBindMeStructures(argv[1], argv[2]);
	
	struct AdjList *graph[NIDS];
	int i;
	for(i=0; i<NIDS; i++)
		graph[i] = NULL;

	struct sockaddr_in si_dest;
	char *destIP;
	startGraphFromFile(graph);
	int destPORT,destID;
	
	int slen = sizeof(si_dest) , recv_len;
	char buf[BUFLEN];
		
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
		if ((recv_len = recvfrom(Socket, buf, BUFLEN, 0, (struct sockaddr *) &si_me, &slen)) == -1){
			die("recvfrom()");
		}
         	
		sleep(ROUTERDELAY);
		StrToUDPMessage (buf, &mes);
		resetupDestStructures(&destID,graph,mes.idDest,&destPORT, destIP, &si_dest);		
				
		//now send data forward
		printf("Nodo %d encaminhando mensagem #%d para o nodo %d, com destino final no nodo %d\n", meID,mes.idMes,destID, mes.idDest);
		if (sendto(Socket, buf, recv_len, 0, (struct sockaddr*) &si_dest, slen) == -1){
			die("sendto()");
		}
	}
 	close(Socket);
	return 0;
}
