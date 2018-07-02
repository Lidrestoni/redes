#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#define INF 11234567

//int NIDS;//Total number of routers+users+servers
char *meIP;
int meID,mePORT, Socket; 
struct sockaddr_in si_me;

/*struct AdjNode{
	int dest, cost;
	struct AdjNode *next;
};

struct AdjList{
	struct AdjNode *node;
};*/

void die(char *s){
	perror(s);
	exit(1);
}


static int messageTotalLen = 3*sizeof(int)+101*sizeof(char);
struct UDPMessage{
	int idMes,idOrig,idDest;
	char mess[101];
};

char* UDPMessageToStr (struct UDPMessage m){
	char *apstr = malloc(messageTotalLen);
	snprintf (apstr, messageTotalLen+1, "%d;%d;%d;%s;", m.idMes,m.idOrig, m.idDest, m.mess); 
	return apstr;
}
void StrToUDPMessage (char *a, struct UDPMessage *m){
	int i=0, x[3], v=3;
	char *p, temp[103];
	while(v--){
		p = &a[i];
		while(a[i]!=';')
			i++;
		a[i]='\0';
		x[v] = atoi(p);
		a[i]=';';
		i++;
	}
	m->idMes=x[2]; m->idOrig = x[1]; m->idDest = x[0];
	int j=0;
	while(a[i]!=';'&&a[i]!='\0')
		temp[j++]=a[i++];
	temp[j]='\0';
	strcpy(m->mess,temp);
}

void PortsFromFile( int *PORT, int id, char **ip){
	int t[2];
	*ip = malloc(sizeof(char)*10);
	FILE *fp = fopen("roteador.config", "r");
	if(fp==NULL){
		printf("Não foi possível abrir o arquivo \"roteador.config\"\n");		
		exit(1);
	}
	do{
		if(fscanf(fp,"%d %d %s ", &t[0], &t[1], *ip)==EOF){
			printf("ERRO: O roteador %d não foi encontrado!", id);			
			exit(1);
		}
		if(t[0]==id){
			*PORT = t[1];		
		}
	}
	while(*PORT==-1);
	fclose(fp);
}

void configureAndBindMeStructures(char *id){
	//NIDS = atoi(nids)+1;
	meID = atoi(id);
	mePORT = -1;	
	PortsFromFile(&mePORT, meID, &meIP);	//printf("<<%d>>", mePORT);

	if ( (Socket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket");
	}

	// zero out the structure
	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(mePORT); 
	//si_me.sin_addr.s_addr = htonl(atoi(meIP));//INADDR_ANY);//printf("{{{{%d:%d(%d)}}}}",si_me.sin_port, mePORT,si_me.sin_addr.s_addr);
	si_me.sin_addr.s_addr = inet_addr(meIP);
	//bind socket to port
	if( bind(Socket , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1){
		die("bind");
	}
}

/*void printGraph(struct AdjList *graph[]){
	if(graph==NULL)
		printf("Grafo vazio\n");
	else{
		int i;
		struct AdjNode *node;
		for(i=0; i<NIDS; i++){
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
		
}*/

void getDestIPandPort(int destID,int *destPORT, char *destIP,struct sockaddr_in *si_dest){
	*destPORT = -1;		
	PortsFromFile(destPORT, destID, &destIP);
	si_dest->sin_port = htons(*destPORT);//printf("\n{Port:%d-%d IP: %s}", *destPORT,si_dest->sin_port, destIP);
	//printf("?{%d}-%d", si_dest->sin_port, *destPORT);
	if (inet_aton(destIP , &si_dest->sin_addr) == 0){	
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}
}

/*void resetupDestStructures(int *destNextID, struct AdjList *graph[], int destFinalID, int *destPORT, char *destIP,struct sockaddr_in *si_dest){
	*destNextID = dijkstra(graph, meID, destFinalID);
	if(*destNextID==-1)
		exit(1);
	else if(!*destNextID)
		*destNextID=meID;
	*destPORT = -1;		
	PortsFromFile(destPORT, *destNextID, &destIP);
	si_dest->sin_port = htons(*destPORT);
	if (inet_aton(destIP , &si_dest->sin_addr) == 0){	
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

}*/

/*void addToAdjList(struct AdjList **graph, int dest, int cost){
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
}*/

/*void addElementTo(int pos,int size, int *Costs[], int *Dests[],int *CompVect[], int elemC, int elemD){
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
	
}*/

/*int addOrderedCosts(int begin, int end, int *Costs[], int *Dests[], int *CompVect[], int elemC, int elemD){
Esse algoritmo utiliza busca binária para adicionar um elemento à uma lista ordenada em ordem crescente
 Foi criada uma variável nend para preservar o tamanho de end (já que precisamos saber a quantidade de índices utilizados para
roda o algoritmo addElementTo)

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
			Aqui em cima temos o problema da divisão de ímpares. Usamos esse mid++ para previnir um loop infinito
		if(elemC<*Costs[nend])
			mid = nend;
		else
			mid = nend+1;
	}
	addElementTo(mid,end+1, Costs, Dests, CompVect, elemC, elemD);
	return ret;

}*/

/*void addToGraph(struct AdjList **graph, int dest1, int dest2, int cost){
	addToAdjList(&graph[dest1], dest2, cost);
	addToAdjList(&graph[dest2], dest1, cost);
}*/

/*int dijkstra(struct AdjList *graph[], int s, int d){
	Nesse algoritmo selCosts and selDests representam todos os vizinhos que podemos visitar pelos caminhos até então percorridos, e selSize é a quantidade desses vizinhos. selCompVector é uma forma rápida de identificar qual o índice de certo nodo em selDests e SelCosts (-1 significa que está ausente) 
Retorno: Em caso de erro retorna -1, em caso do destino e origem serem iguais retorna 0, demais casos retorna o próximo s que irá para o caminho mais curto.


	
	if (s==d)
		return 0;	
	int i,selSize=0, *selCosts[NIDS], *selDests[NIDS], *selCompVect[NIDS], originalS = s;
	
	for(i=0; i<NIDS+1; i++){
		selCosts[i] = malloc(sizeof(int));
		selDests[i] = malloc(sizeof(int));
		selCompVect[i] = malloc(sizeof(int));
		*selDests[i] = *selCosts[i] = 0;
		*selCompVect[i] = -1;
	}	

	struct AdjNode *node;
	int visited[NIDS+1],dist[NIDS+1], from[NIDS+1];
	for(i=0; i<NIDS+1; i++){
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
			printf("Erro: Não existe caminho do roteador %d para o roteador %d! Último roteador visitado: %d\n", originalS, d, s);
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
	
	
}*/

/*void startGraphFromFile(struct AdjList *graph[]){
	int t[3];
	FILE *fp = fopen("enlaces2.config", "r");
	if(fp==NULL){
		printf("Não foi possível abrir o arquivo \"enlaces2.config\"\n");
		exit(1);
	}
	while(fscanf(fp,"%d %d %d ", &t[0], &t[1], &t[2])!=EOF)
		addToGraph(graph, t[0],t[1],t[2]);
	fclose(fp);
}*/
