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
#include<time.h>
#include"graph.h"
#include"udp.h"


static time_t TIMEOUT = 1; 
/*Estrutura utilizada para conferir o timeout*/
struct messConfs{
	int idMes;
	time_t time;
	char *arrayMes;
	struct messConfs *next, *prev;
};

struct messConfs *Confs;

void rmThisConfs(struct messConfs *node){
	if(Confs==NULL)
		return;
	if(node->prev==NULL&&node->next==NULL){
		free(node);
		Confs=NULL;	
		return;
	}
	if(node->prev==NULL)
		Confs=node->next;
	else
		node->prev->next = node->next;
	if(node->next!=NULL)
		node->next->prev = node->prev;
	free(node);
}

void rmThisId(int id){
	if(Confs==NULL)
		return;
	struct messConfs *it = Confs;
	while(it!=NULL&&it->idMes!=id)
		it=it->next;
	if(it!=NULL&&it->idMes==id)  ///Fix 1/3
		rmThisConfs(it);
}

void addConfs(int idMes, char *mes){
	if(Confs==NULL){
		Confs = malloc(sizeof(struct messConfs));
		Confs->idMes = idMes;
		Confs->time = time(NULL);
		Confs->arrayMes = malloc(sizeof(mes));
		strcpy(Confs->arrayMes,mes);
		Confs->prev = Confs->next = NULL;	
	}
	else{
		struct messConfs *iterator = Confs;
		while(iterator->next!=NULL)
			iterator = iterator->next;
		iterator->next = malloc(sizeof(struct messConfs));
		iterator->next->idMes = idMes;
		iterator->next->time = time(NULL);
		iterator->next->arrayMes = malloc(sizeof(mes));
		strcpy(iterator->next->arrayMes,mes);
		iterator->next->next=NULL;
		iterator->next->prev = iterator;
	}
}

struct messConfs* getConfsN(struct messConfs *it){
	if(it==NULL)
		return Confs;
	return it->next;
}

void printAllConfs(){
	if(Confs==NULL){
		printf("Sem confirmações pendentes\n");
		fflush(stdout);
	}
	else{
		struct messConfs *it = NULL;
		printf("\n\n\n-------------------------------\n");
		printf("Start from: %ld\n", (long)Confs);
		printf("-------------------------------\n");
		while((it=getConfsN(it))!=NULL){
			printf("Id: %d  Pos: %ld\nTime: %d\nMessage:\"%s\"\nFrom: %ld\nTo:%ld\n\n\n\n", it->idMes,(long)it, (int)it->time, it->arrayMes, (long)it->prev, (long)it->next);
			fflush(stdout);
		}
		printf("\n-------------------------------\n\n\n");
	}
}

int argc;
char **argv;
static size_t messageSize = 100;

int LastID=0, destServer = 9;
#define BUFLEN 512  //Max length of buffer
char *SERVER;
int PORT;   //The port on which to send data
 
void die(char *s){
	perror(s);
	exit(1);
}
 
/*Argumentos: 1: Número do cliente, 2: Quantidade de roteadores*/

int nuser, s;
struct sockaddr_in si_me;/*Fix 2/3 Único socket a ser compartilhado pelas threads*/
int main2(int id){
	struct AdjList *graph[NROUTERS];
	struct sockaddr_in si_other;
	int i;
	for(i=0; i<NROUTERS; i++)
		graph[i] = NULL;
		
	startGraphFromFile(graph);
	int destPORT,destROUTER;

	PORT = -1;
	PortsFromFile(&PORT, nuser, &SERVER);/*Fix 3/3*/

	
	int slen=sizeof(si_other), recv_len;
	char buf[BUFLEN];
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
   
     
    if (inet_aton(SERVER , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

	struct UDPMessage mes;
	char *message;
	char *destIP;
	while(1){

		destROUTER = dijkstra(graph, nuser, destServer);
		if(destROUTER==-1)
			exit(1);
		else if(!destROUTER)
			destROUTER=nuser;
		destPORT = -1;		
		PortsFromFile(&destPORT, destROUTER, &destIP);
		si_other.sin_port = htons(destPORT);

		if(id==1){
			while(1){
				memset(buf,'\0', BUFLEN);
				//try to receive some data, this is a blocking call
				if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1){
					die("recvfrom()");
				}
				StrToUDPMessage (buf, &mes);
				printf("\nSucesso! O pacote #%d voltou!\nMensagem do pacote: %s\n", mes.idMes, mes.mess);
				rmThisId(mes.idMes);
			}
		}
		else if(id==2){
			mes.idMes =  LastID++;
			mes.idOrig = nuser;
			mes.idDest = destServer;
			printf("Enter message : ");
			fflush(stdout);
			getline(&message, &messageSize, stdin);
			fflush(stdout);
			strcpy(mes.mess,message);
			message = UDPMessageToStr(mes);
         
			//send the message
			printf("Nodo %d encaminhando mensagem #%d para o nodo %d, com destino final no nodo %d\n(O caminho percorrido aparecerá no outro terminal)\n", nuser,mes.idMes,destROUTER, mes.idDest);
			addConfs(mes.idMes, message);
			if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1){
				die("sendto()");
			}
		}
		else{
			int l,m;
			while(1){
				struct messConfs *it = NULL;
				while((it=getConfsN(it))!=NULL){
					if(((int)time(NULL))>((int)it->time)+((int)TIMEOUT)){
						fprintf(stderr,"[%d]O pacote #%d deu timeout! Por isso será reenviado!\n",(int)time(NULL),it->idMes);
						fflush(stdout);					
						it->time = time(NULL);

						

						StrToUDPMessage (it->arrayMes, &mes);
						PortsFromFile(&destPORT, mes.idDest, &destIP);
						si_other.sin_port = htons(destPORT);

											         	
						//send the message
						printf("Nodo %d Reencaminhando mensagem #%d para o nodo %d, com destino final no nodo %d\n(O caminho percorrido aparecerá no outro terminal)\n", nuser,mes.idMes,destROUTER, mes.idDest);
						if (sendto(s, it->arrayMes, strlen(it->arrayMes) , 0 , (struct sockaddr *) &si_other, slen)==-1){
							die("sendto()");
						}




					}
				}
				sleep(2.5);
			}
		}		
	}
 
	close(s);
	return 0;
}

void *mythread(void *data);

#define N 3 // number of threads

/*Argumentos: 1: Número do cliente, 2: Quantidade de roteadores*/
int main(int argc2, char **argv2){
	argc = argc2;
	argv = argv2;
	pthread_t tids[N];
	int i, *ii = malloc(sizeof(int));
	
	if(argc<2){
		printf("Too few arguments to start router!\n");
		fflush(stdout);
		exit(1);	
	}
	else
		NROUTERS = atoi(argv[2])+1;
		
	nuser = atoi(argv[1]);
	PORT = -1;	
	PortsFromFile(&PORT, nuser, &SERVER);	

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
		fflush(stdout);
	}
	return(1);
}


void *mythread(void *data) {
	int pidThread = *((int *)data);
	main2(pidThread);
	pthread_exit(NULL);
}


