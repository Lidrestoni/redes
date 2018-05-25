/* 
 *	Simple udp client
 *  Silver Moon (m00n.silv3r@gmail.com)
 *	
 *	You can compile this program with:
 * gcc -o thread thread.c -lpthread */
/* We always need to include this header file for<\n>
 * the threads */
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include "udp.h"

int destFinalID;

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


#define BUFLEN 512  //Max length of buffer

 
/*Argumentos: 1: Número do cliente, 2: Quantidade de roteadores*/


int main2(int id){
	struct AdjList *graph[NIDS];
	struct sockaddr_in si_dest;
	int destPORT,destNextID;
	int slen=sizeof(si_dest), recv_len;
	char buf[BUFLEN];
	static size_t messageSize = 100;

	int i;
	for(i=0; i<NIDS; i++)
		graph[i] = NULL;
		
	startGraphFromFile(graph);
 
    memset((char *) &si_dest, 0, sizeof(si_dest));
    si_dest.sin_family = AF_INET;
   

	struct UDPMessage mes;
	char *message,*destIP;
	int LastMessID=0;
	
	if(id==1)
		while(1){
			memset(buf,'\0', BUFLEN);
			//try to receive some data, this is a blocking call
			if (recvfrom(Socket, buf, BUFLEN, 0, (struct sockaddr *) &si_me, &slen) == -1){
				die("recvfrom()");
			}
			StrToUDPMessage (buf, &mes);
			if(mes.idDest==meID)
				printf("\nSucesso! O pacote #%d voltou!\nMensagem do pacote: %s\n", mes.idMes, mes.mess);
			else
				printf("XXX");
			rmThisId(mes.idMes);
		}
	else if(id==2)
		while(1){
			printf("Para qual roteador mandar a mensagem? (# de 1 a %d, exceto %d): ", NIDS, meID);
			do{
				scanf("%d", &destFinalID);
			}
			while(destFinalID==meID||destFinalID<1||destFinalID>NIDS);
			getchar();
			resetupDestStructures(&destNextID,graph,destFinalID,&destPORT, destIP, &si_dest);
			mes.idMes =  LastMessID++;
			mes.idOrig = meID;
			mes.idDest = destFinalID;
			printf("Enter message : ");
			//fflush(stdout);
			getline(&message, &messageSize, stdin);
			fflush(stdout); 
			strcpy(mes.mess,message);
			message = UDPMessageToStr(mes);
       	 
			//send the message
			printf("Nodo %d encaminhando mensagem #%d para o nodo %d, com destino final no nodo %d\n(O caminho percorrido aparecerá no outro terminal)\n", meID,mes.idMes,destNextID, mes.idDest);
			addConfs(mes.idMes, message);
			if (sendto(Socket, message, strlen(message) , 0 , (struct sockaddr *) &si_dest, slen)==-1){
				die("sendto()");
			}else printf("[%d]", si_dest.sin_port);
		}
	else{return 0;
		int l,m;
		while(1){
			resetupDestStructures(&destNextID,graph,destFinalID,&destPORT, destIP, &si_dest);
			struct messConfs *it = NULL;
			while((it=getConfsN(it))!=NULL){
				if(((int)time(NULL))>((int)it->time)+((int)TIMEOUT)){
					fprintf(stderr,"[%d]O pacote #%d deu timeout! Por isso será reenviado!\n",(int)time(NULL),it->idMes);
					fflush(stdout);					
					it->time = time(NULL);
					StrToUDPMessage (it->arrayMes, &mes);
																         	
					//send the message
					printf("Nodo %d Reencaminhando mensagem #%d para o nodo %d, com destino final no nodo %d\n(O caminho percorrido aparecerá no outro terminal)\n", meID,mes.idMes,destNextID, mes.idDest);
					if (sendto(Socket, it->arrayMes, strlen(it->arrayMes) , 0 , (struct sockaddr *) &si_dest, slen)==-1){
						die("sendto()");
					}
				}
			}
			sleep(2.5);
		}
	}		
	close(Socket);
	return 0;
}

void *mythread(void *data);

#define N 3 // number of threads

/*Argumentos: 1: Número do cliente, 2: Quantidade de roteadores*/
int main(int argc2, char **argv2){
	int i;
	char x[10];
	FILE *fp = fopen("roteador.config", "r");
	NIDS=0;
	if(fp==NULL){
		printf("Não foi possível abrir o arquivo \"roteador.config\"\n");		
		return;
	}
	while(fscanf(fp,"%d %d %s ", &i, &i, x)!=EOF)
		NIDS++;
	fclose(fp);
	if(NIDS<3){
		printf("O arquivo \"roteador.config\" possui muitos poucos IDs\n");		
		return;
	}

	argc = argc2;
	argv = argv2;
	pthread_t tids[N];
	int *ii = malloc(sizeof(int));
	
	if(argc<1){
		printf("Specify which router is being started!\n");
		fflush(stdout);
		exit(1);	
	}
	
	configureAndBindMeStructures(argv[1]);
	
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
