/*
    Simple udp server
    Silver Moon (m00n.silv3r@gmail.com)
*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include<math.h> // ceil
 
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

#define NROUTERS 9+1
#define INF 11234567


struct AdjNode{
	int dest, cost;
	struct AdjNode *next;
};

struct AdjList{
	struct AdjNode *node;
};

void printGraph(struct AdjList *graph[]){
	if(graph==NULL)
		printf("Grafo vazio\n");
	else{
		int i;
		struct AdjNode *node;
		for(i=0; i<NROUTERS; i++){
			if(graph[i]!=NULL){
				node = graph[i]->node;
				printf("%d] ", i);
				do
					printf("| Dest: %d , Cost: %d |", node->dest, node->cost);
				while(node->next!=NULL, node = node->next);
				printf("\n");			
			}		
		}	
	}
		
}

void addToAdjList(struct AdjList **graph, int dest, int cost){
	if(*graph==NULL){
		*graph = malloc(sizeof(struct AdjList));
		(*graph)->node = malloc(sizeof(struct AdjNode));
		(*graph)->node->dest = dest;
		(*graph)->node->cost = cost;
		(*graph)->node->next = NULL;
	}
	else{
		struct AdjNode *node = (*graph)->node;
		while(node->next != NULL)
			node = node->next;
		node->next = malloc(sizeof(struct AdjNode));
		node->next->dest = dest;
		node->next->cost = cost;
		node->next->next = NULL;
	}
}

void addElementTo(int pos,int size, int *Costs[], int *Dests[],int *CompVect[], int elemC, int elemD){
	int i;
	for(i=size; i>pos; i--){
		*Dests[i]=*Dests[i-1];
		*Costs[i]=*Costs[i-1];
		*CompVect[*Dests[i]]=i;
	}
	*CompVect[*Dests[i]=elemD]=i;
	*Costs[i]=elemC;
}

void removeElementIn(int pos,int size, int *Costs[], int *Dests[],int *CompVect[]){
	int i;
	*CompVect[*Dests[pos]]=-1;
	for(i=pos; i<size; i++){
		*Dests[i]=*Dests[i+1];
		*Costs[i]=*Costs[i+1];
		*CompVect[*Dests[i]]=i;
		*CompVect[*Dests[i+1]]=i+1;
	}
	
}

int addOrderedCosts(int begin, int end, int *Costs[], int *Dests[], int *CompVect[], int elemC, int elemD){
/*Esse algoritmo utiliza busca binária para adicionar um elemento à uma lista ordenada em ordem crescente
 Foi criada uma variável nend para preservar o tamanho de end (já que precisamos saber a quantidade de índices utilizados para
roda o algoritmo addElementTo)
*/
	int mid, nend=end, ret = 1;
	if(*CompVect[elemD]>=0){
		ret = 0;
		if(elemC>=*Costs[*CompVect[elemD]])
			return ret;
		else{
			removeElementIn(*CompVect[elemD], end, Costs, Dests, CompVect);		
			end--;
		}
	}
	if(end<0)
		mid=0;
	else{
		while(mid = begin + (nend-begin)/2, nend!=begin)
			if(elemC<*Costs[mid]){
				if(nend==mid)
					mid++;
				nend = mid;
			}
			else{
				if(begin==mid)
					mid++;
				begin = mid;
			}
			/*Aqui em cima temos o problema da divisão de ímpares. Usamos esse mid++ para previnir um loop infinito*/
		if(elemC<*Costs[nend])
			mid = nend;
		else
			mid = nend+1;
	}
	addElementTo(mid,end+1, Costs, Dests, CompVect, elemC, elemD);
	return ret;

}

void addToGraph(struct AdjList **graph, int dest1, int dest2, int cost){
	addToAdjList(&graph[dest1], dest2, cost);
	addToAdjList(&graph[dest2], dest1, cost);
}

int dijkstra(struct AdjList *graph[], int s, int d){
	/*Nesse algoritmo selCosts and selDests representam todos os vizinhos que podemos visitar pelos caminhos até então percorridos, e selSize é a quantidade desses vizinhos. selCompVector é uma forma rápida de identificar qual o índice de certo nodo em selDests e SelCosts (-1 significa que está ausente) */
	int i,selSize=0, *selCosts[NROUTERS], *selDests[NROUTERS], *selCompVect[NROUTERS], originalS = s;
	
	for(i=0; i<NROUTERS; i++){
		selCosts[i] = malloc(sizeof(int));
		selDests[i] = malloc(sizeof(int));
		selCompVect[i] = malloc(sizeof(int));
		*selDests[i] = *selCosts[i] = 0;
		*selCompVect[i] = -1;
	}	

	struct AdjNode *node;
	int visited[NROUTERS],dist[NROUTERS], from[NROUTERS];
	for(i=0; i<NROUTERS; i++){
		visited[i]=0;
		dist[i]=INF;
		from[i]=-1;
	}
	dist[s]=0;
	
	do{
		visited[s]=1;
		node = graph[s]->node;
		do{
			if(!visited[node->dest]){			
				if(dist[node->dest]>dist[s]+node->cost){
					dist[node->dest]=dist[s]+node->cost;
					from[node->dest]=s;
				}
				selSize+= addOrderedCosts(0,selSize-1,selCosts, selDests, selCompVect, dist[node->dest], node->dest);
			}
		}
		while(node->next!=NULL, node = node->next);
		if(!selSize){
			printf("Erro: destino não encontrado!");
			return -1;
		}
		s = *selDests[0];
		removeElementIn(0, selSize-1, selCosts, selDests, selCompVect);
		selSize--;			
	}
	while(s!=d);
	int getX = from[d];
	if(getX!=originalS)
		while(from[getX]!=originalS)
			getX = from[getX];
	else
		getX = d;
	return getX;
	
	
}

void die(char *s)
{
    perror(s);
    exit(1);
}
 
/*Argumentos: 1: Número do roteador*/
int main(int argc, char **argv)
{	
	if(argc<2){
		printf("Too few arguments to start router!\n");
		exit(1);	
	}
	int i, t[3];	

	struct AdjList *graph[NROUTERS];
	for(i=0; i<NROUTERS; i++)
		graph[i] = NULL;

	struct sockaddr_in si_me, si_other;
	int router = atoi(argv[1]);
	char ip[10];
    FILE *fp = fopen("roteador.config", "r");
	if(fp==NULL){
		printf("Não foi possível abrir o arquivo \"roteador.config\"\n");
		exit(1);
	}
	do
		if(fscanf(fp,"%d %d %s ", &t[0], &t[1], ip)==EOF){
			printf("ERRO: O roteador %d não foi encontrado!", router);			
			exit(1);
		}
	while(t[0]!=router);
	fclose(fp);
	printf("%d %d %s\n", t[0], t[1], ip);

	fp = fopen("enlaces2.config", "r");
	if(fp==NULL){
		printf("Não foi possível abrir o arquivo \"enlaces2.config\"\n");
		exit(1);
	}
	while(fscanf(fp,"%d %d %d ", &t[0], &t[1], &t[2])!=EOF){
		addToGraph(graph, t[0],t[1],t[2]);
	}
	fclose(fp);	

	int to = dijkstra(graph, router, 9);
	printf("Mandar pacote para %d\n", to);
    int s, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];
     return 0;
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }
     
    //keep listening for data
    while(1)
    {
        printf("Waiting for data...");
        fflush(stdout);
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }
         
        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %s\n" , buf);
         
        //now reply the client with the same data
        if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
        {
            die("sendto()");
        }
    }
 
    close(s);
    return 0;
}
