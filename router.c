/* 
 *	Simple udp client
 *  Silver Moon (m00n.silv3r@gmail.com)
 *	
 *	You can compile this program with:
 * gcc -o thread thread.c -lpthread */
/* We always need to include this header file for<\n>
 * the threads 







Tipos de Mensangens:

Atualização:
001 | ID do remetente |Número de colunas X | Cabeçalho 1 | Conteúdo 1 |Cabeçalho 2| Conteúdo 2  ... Cabeçalho X | Conteúdo X | 
*/

#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include "udp.h"
#define vtMAX 1123
#define totalMesLength 500

int vetdist[vtMAX][vtMAX], vetColumndistN=0, vetLinedistN=0, vetColumnLabels[vtMAX], vetLineLabels[vtMAX];

void printVetDist(){
	int i,j;
	printf("\n");
	printf("%4d  ", meID);
	for(i=0; i<vetColumndistN; i++)
		printf("%4d ", vetColumnLabels[i]);
	printf("\n");
	printf("%6s", " --- ");
	for(i=0; i<vetColumndistN; i++)
		printf("%4s", " --- ");
	printf("\n");
	
	for(j=0; j<vetLinedistN; j++){
		printf("%4d |", vetLineLabels[j]);
		for(i=0; i<vetColumndistN; i++)
			if(vetdist[j][i]<0)
				printf("%4c ", '*');
			else
				printf("%4d ", vetdist[j][i]);
		printf("\n");
	}
	printf("\n");
}

/*Atenção: Note que as chamadas para as funções getVetLabelSquareMatrix, getVetLineLabel e getVetColumnLable são perigosas.
	Caso uma dessas funções seja chamada mais de uma vez, por exemplo A = getVetLineLabel(X); B = getVetLineLabel(Y);
	O valor de A pode se tornar desatualizado, pois a função pode ter mexido na estrutura do vetor distâcia.
*/


int getVetLabel(int *vetLabels, int *vetDistN,int *vetDistN2, int value, int columnQ){
	int i,j,k;	
	for(i=0; i<*vetDistN;i++){
		if(value==vetLabels[i])
			return i;
		if((!i||value>vetLabels[i-1])&&value<vetLabels[i]){
			for(j=*vetDistN; j>i; j--){ 
				vetLabels[j]=vetLabels[j-1];
				for(k=0; k<(*vetDistN2); k++)
					if(columnQ)
						vetdist[k][j]=vetdist[k][j-1];
					else
						vetdist[j][k]=vetdist[j-1][k];				
				
			}
			vetLabels[i]=value;
			for(k=0; k<(*vetDistN2); k++)
				if(columnQ)
					vetdist[k][i]=-1;
				else
					vetdist[i][k]=-1;
			(*vetDistN)++;
			return i;
		}
	}
	vetLabels[(*vetDistN)]=value;
	(*vetDistN)++;
	for(i=0; i<(*vetDistN2); i++)
		if(columnQ)
			vetdist[i][(*vetDistN)-1]=-1;
		else
			vetdist[(*vetDistN)-1][i]=-1;
	return (*vetDistN)-1;
}

int getSameVetLabels(int value){
	int x,y;
	y=getVetLabel(vetColumnLabels, &vetColumndistN,&vetLinedistN, value, 1);
	x=getVetLabel(vetLineLabels, &vetLinedistN,&vetColumndistN, value, 0);
	return (x==y)? x:-1;
}

int getVetLineLabel(int value){ //Na prática não é necessário uma função que só crie uma linha. Essa função só foi mantida para manter uma nomeação de funções mais consistente.	
	return getSameVetLabels(value);
}

int validSum(int a, int b){
	if(a<0||b<0)
		return -1;
	return a+b;
}

int validSmaller(int a, int b){
	if(a<0&&b<0)
		return a<b? a:b;
	if(a<0)
		return b;
	if(b<0)
		return a;
	return a<b? a:b; 
}

void atualizaVD(){
	int i,j, min, meLineID = getVetLabel(vetLineLabels, &vetLinedistN,&vetColumndistN,meID , 1), meColumnID=getVetLabel(vetColumnLabels, &vetColumndistN,&vetLinedistN,meID , 1);
	for(i=0; i<vetColumndistN; i++){
		if(i==meColumnID)
			min=0;
		else{
			min=vetdist[meLineID][i];			
			for(j=0; j<vetLinedistN;j++)
				min=validSmaller(min, validSum(vetdist[meLineID][j],vetdist[j][i]));
		}
		vetdist[meLineID][i]=min;	
	}
}

void readMessage001(char *vet){ //Lê o vetor mandado por src e atualiza o vetor distância na linha correspondente.
	if(vet[0]=='0'&&vet[1]=='0'&&vet[2]=='1'){
		int i=4, xx=0, n, src, dst, k, srcID;
		char x[20];
		for(k=0; k<2; k++,xx=0){
			while(vet[i]!='|')
				x[xx++]=vet[i++];
			x[xx]='\0';
			if(k)
				n=atoi(x);
			else{								
				srcID = atoi(x);
				getVetLineLabel(srcID);
			}
			i++; 
		}		
		while(n--){
			for(k=0; k<2; k++){
				xx=0;
				while(vet[i]!='|')
					x[xx++]=vet[i++];
				x[xx]='\0';
				i++;
				if(!k)
					dst = getVetLabel(vetColumnLabels, &vetColumndistN,&vetLinedistN, atoi(x), 1);//getVetColumnLabel(atoi(x));
				else
					vetdist[getVetLineLabel(srcID)][dst]=atoi(x);
			}
		}
		
	}
	else{
		printf("Warning: Erro de tratamento de mensagem! Uma mensagem do tipo %c%c%c está solicitando tratamento do tipo 001\n", vet[0], vet[1], vet[2]);
	}
}

void readMessage(char *mes){
	if(mes[0]=='0'&&mes[1]=='0'&&mes[2]=='1')
		readMessage001(mes);
	else
		printf("Warning: Foi recebida mensagem do tipo %c%c%c, que é desconhecido.\n", mes[0], mes[1], mes[2]);
}


char* createMessage001(){ //Mensagem utilizada para atualizar o vetor distância dos vizinhos.
	int i, meLine = getVetLineLabel(meID), j, k, kk;
	char *mystr = malloc(totalMesLength), x[50];
	mystr[0]=mystr[1]='0';
	mystr[2]='1';
	mystr[3]='|';
	i=4;
	for(kk=0; kk<2; kk++){
		snprintf(x, 50, "%d", kk? vetColumndistN:meID);
		for(j=0; j<strlen(x); j++)
			mystr[i++]=x[j];
		mystr[i++]='|';
	}
	for(k=0; k<vetColumndistN; k++)
		for(kk=0; kk<2; kk++){
			snprintf(x, 50, "%d", kk? vetdist[meLine][k] : vetColumnLabels[k]);
			for(j=0; j<strlen(x); j++)
				mystr[i++]=x[j];
			mystr[i++]='|';
		}
	mystr[i]='\0';
	return mystr;
	
}


void addEdgeToVD(int from, int to, int value){
	getSameVetLabels(from);//getVetLabelSquareMatrix(from);
	to = getSameVetLabels(to);//getVetLabelSquareMatrix(to);
	from = getSameVetLabels(from);//getVetLabelSquareMatrix(from); 
	//vetdist[from][to]=vetdist[to][from]=value;
	printVetDist();
	vetdist[from][to]=value;
	printVetDist();
}

void startVetDistFromFile(){
	int t[3];	
	FILE *fp = fopen("enlaces3.config", "r");
	if(fp==NULL){
		printf("Não foi possível abrir o arquivo \"enlaces3.config\"\n");
		exit(1);
	}
	while(fscanf(fp,"%d %d %d ", &t[0], &t[1], &t[2])!=EOF){
		if(t[0]==meID)
			addEdgeToVD(t[0], t[1], t[2]);
		else if(t[1]==meID)
			addEdgeToVD(t[1], t[0], t[2]);
		
	}
	addEdgeToVD(meID, meID,0);
}


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


int main2(int id){//printf("main2<%d>", id);
	//struct AdjList *graph[NIDS];
	struct sockaddr_in si_dest;
	int destPORT,destID;
	int slen=sizeof(si_dest), recv_len;
	char buf[BUFLEN];
	static size_t messageSize = 100;

	int i;
	/*for(i=0; i<NIDS; i++)
		graph[i] = NULL;
		
	startGraphFromFile(graph);*/
 
    memset((char *) &si_dest, 0, sizeof(si_dest));
    si_dest.sin_family = AF_INET;
   

	struct UDPMessage mes;
	char *message,*destIP;
	int LastMessID=0;
	if(id==0&&id==meID-1){ printf("I'm one!!!");//Thread responsável por receber mensagens dos vizinhos
		struct sockaddr_in si_other;
		int recv_len, other_len = sizeof(si_other);
		while(1){
			memset(buf,'\0', BUFLEN);
			//try to receive some data, this is a blocking call
			if ((recv_len=recvfrom(Socket, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &other_len)) == -1)
				die("recvfrom()");
			//printf("Recebi \"%s\"\n", buf);
			//printf("Received packet{\"%s\"} from %s-%d:%d\n", buf,inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port),si_other.sin_port);
			readMessage(buf);
			printVetDist();
			atualizaVD();
			printVetDist();
		}
	}
	else if(id==1&&id==meID-1)
		while(1){
			printf("Para qual roteador mandar a mensagem? ");
			do{
				scanf("%d", &destFinalID);
			}
			while(destFinalID==meID||destFinalID<1);
			getchar();
			//resetupDestStructures(&destNextID,graph,destFinalID,&destPORT, destIP, &si_dest);
			mes.idMes =  LastMessID++;
			mes.idOrig = meID;
			mes.idDest = destFinalID;
			getDestIPandPort(vetLineLabels[i],&destPORT, destIP,&si_dest);
			printf("Enter message : ");
			//fflush(stdout);
			getline(&message, &messageSize, stdin);
			fflush(stdout); 
			strcpy(mes.mess,message);
			message = UDPMessageToStr(mes);
       	 
			//send the message
			printf("Nodo %d encaminhando mensagem #%d para o nodo %d, com destino final no nodo %d\n(O caminho percorrido aparecerá no outro terminal)\n", meID,mes.idMes,destID, mes.idDest);
			addConfs(mes.idMes, message);
			if (sendto(Socket, message, strlen(message) , 0 , (struct sockaddr *) &si_dest, slen)==-1){
				die("sendto()");
			}//else printf("[%d]", si_dest.sin_port);
		}
	else if(id==2&&meID-1==id){ printf("I'm three!");	/*Thread responsável por dar sinal de vida aos vizinhos*/
		char *m;
		while(1){
			sleep(2.5);
			m = createMessage001();
			for(i=0; i<vetLinedistN; i++){ 
				if(vetLineLabels[i]==meID)//getVetLabels(meID))
					continue;
				getDestIPandPort(vetLineLabels[i],&destPORT, destIP,&si_dest);
				//printf("\n(%d Meid:%d Meport:%d{%d:%d})Mandando \"%s\" para id %d <%d>\n", vetLinedistN,meID,mePORT,si_me.sin_port,si_me.sin_addr.s_addr,m, vetLineLabels[i],si_dest.sin_port);
				if(sendto(Socket, m, strlen(m) , 0 , (struct sockaddr *) &si_dest, sizeof(si_dest))==-1){
							die("sendto()");
				}//else printf("[%d %d]Sucesso: Mensagem mandada para %d %d\n",mePORT, htons(mePORT), si_dest.sin_port,si_dest.sin_port);
			}
		}
	}
	else{ printf("[%d:%d]", meID,id);return 0;
		int l,m;
		while(1){
			//resetupDestStructures(&destNextID,graph,destFinalID,&destPORT, destIP, &si_dest);
			struct messConfs *it = NULL;
			while((it=getConfsN(it))!=NULL){
				if(((int)time(NULL))>((int)it->time)+((int)TIMEOUT)){
					fprintf(stderr,"[%d]O pacote #%d deu timeout! Por isso será reenviado!\n",(int)time(NULL),it->idMes);
					fflush(stdout);					
					it->time = time(NULL);
					StrToUDPMessage (it->arrayMes, &mes);
																         	
					//send the message
					printf("Nodo %d Reencaminhando mensagem #%d para o nodo %d, com destino final no nodo %d\n(O caminho percorrido aparecerá no outro terminal)\n", meID,mes.idMes,destID, mes.idDest);
					if (sendto(Socket, it->arrayMes, strlen(it->arrayMes) , 0 , (struct sockaddr *) &si_dest, slen)==-1){
						die("sendto()");
					}
				}
			}
			sleep(2.5);
		}
	}		
	//close(Socket);
	return 0;
}

void *mythread(void *data);

#define N 4 // number of threads

/*Argumentos: 1: Número do cliente, 2: Quantidade de roteadores*/

int main(int argc2, char **argv2){
	int i;
	char x[10];
	
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
	printVetDist();
	startVetDistFromFile();
	printVetDist();
	/*printf("\n>%s<\n",createMessage001());
	readMessage001("001|10|6|1|0|2|20|3|20|4|20|5|10|9|100|");
	printVetDist();*/
	
	/*for(i=0; i<N; i++){
		pthread_create(&tids[i], 0, mythread, (void*)i);
	}*/

	int id[10];
	pthread_t thread_tid[10];

	for(i = 0; i < 10; i++) {
		id[i] = i;
		pthread_create(&tids[i], NULL, mythread, (void*)(id + i));
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
	int *pidThread = (int *)data;//*((int *)data);
	printf("pidThread<%d>", *pidThread);main2(*pidThread);
	pthread_exit(NULL);
}
