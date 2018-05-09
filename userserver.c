/* You can compile this program with:
 * gcc -o thread thread.c -lpthread */
/* We always need to include this header file for<\n>
 * the threads */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<string.h> //memset
#include <sys/time.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include"graph.h"
#include"udp.h"

int argc;
char **argv;

int LastID=0, destServer = 9;
#define BUFLEN 512  //Max length of buffer
char *SERVER;
int PORT;   //The port on which to send data
 
void die(char *s){
	perror(s);
	exit(1);
}
 
/*Argumentos: 1: Número do cliente, 2: Quantidade de roteadores*/
struct sockaddr_in si_other, si_me;
int nuser, s;
char tc[10];
int main2(int id){
	struct AdjList *graph[NROUTERS];
	int i;
	for(i=0; i<NROUTERS; i++)
		graph[i] = NULL;
	//struct sockaddr_in si_me, si_other, si_dest;
		
	startGraphFromFile(graph);
	int destPORT,destROUTER;

	
	int slen=sizeof(si_other), recv_len;
	char buf[BUFLEN];
	//char message[BUFLEN];
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
   
     
    if (inet_aton(SERVER , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

	struct UDPMessage mes;
	char *message;
	while(1){

		destROUTER = dijkstra(graph, nuser, destServer);
		if(destROUTER==-1)
			exit(1);
		else if(!destROUTER)
			destROUTER=nuser;
		destPORT = -1;		
		PortsFromFile(&destPORT, destROUTER, tc);
		si_other.sin_port = htons(destPORT);

		if(id==1){
			//fflush(stdout);
			//receive a reply and print it
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
			
			//now reply the client with the same data
			if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1){
				die("sendto()");
			}

		}
		else{
			mes.idMes =  LastID++;
			mes.idOrig = nuser;
			mes.idDest = destServer;
			fflush(stdout);
			printf("Enter message : ");
			//gets(message);
			scanf("%s",mes.mess);
			message = UDPMessageToStr(mes);
         
			//StrToUDPMessage (message, &mes);
			//send the message
			printf("Nodo %d encaminhando mensagem #%d para o nodo %d, com destino final no nodo %d\n", nuser,mes.idMes,destROUTER, mes.idDest);
			if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1){
				die("sendto()");
			}
         
			//receive a reply and print it
			//clear the buffer by filling null, it might have previously received data
			memset(buf,'\0', BUFLEN);
			//try to receive some data, this is a blocking call
			if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1){
				die("recvfrom()");
			}
			puts(buf);
		}
	}
 
	close(s);
	return 0;
}







void *mythread(void *data);

#define N 2 // number of threads

/*Argumentos: 1: Número do cliente, 2: Quantidade de roteadores*/
int main(int argc2, char **argv2) {
	argc = argc2;
	argv = argv2;
	pthread_t tids[N];
	int i, *ii = malloc(sizeof(int));
	
	if(argc<2){
		printf("Too few arguments to start router!\n");
		exit(1);	
	}
	else
		NROUTERS = atoi(argv[2])+1;
		
	nuser = atoi(argv[1]);
	PORT = -1;	
	PortsFromFile(&PORT, nuser, tc);
	SERVER = tc;	

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

	for(i=0, *ii=i; i<N; i++, *ii=i) {
		pthread_create(&tids[i], 0, mythread, ii);
	}
	/* We will now wait for each thread to<\n>
	* terminate */
	for(i=0; i<N; i++) {
		pthread_join(tids[i], NULL);
		printf("Thread id %ld returned\n", tids[i]);
	}
	return(1);
}


void *mythread(void *data) {
	int pidThread = *((int *)data);
	main2(pidThread);
	pthread_exit(NULL);
}


