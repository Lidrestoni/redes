#include <stdio.h>
#include <sys/types.h>


void main(){
	char x[5], NIDSString[5], serverIdString[5], temp[30];
	int i, userId, serverId, NIDS;
	pid_t pid;


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
	sprintf(NIDSString,"%d", NIDS);
	
	printf("Escolha o id do usuário (# de 1 a %d): ", NIDS);
	fflush(stdout);
	do
		scanf("%d", &userId);
	while(userId<1||userId>NIDS);
	printf("Escolha o id do servidor (# de 1 a %d, exceto %d): ", NIDS, userId);
	fflush(stdout);
	do
		scanf("%d", &serverId);
	while(serverId==userId||serverId<1||serverId>NIDS);
	sprintf(serverIdString,"%d", serverId);

	char *argv[] = {"X","X",NIDSString,serverIdString, NULL};
	for(i=1; i<=NIDS; i++){
		if((pid = fork())==0){
			sprintf(x,"%d", i);
			argv[1]=x;
			/*if(i==userId){
				printf("Please:  Open a new terminal window in this folder and execute the following command:\n./userserver %s %s %s\n", argv[1], argv[2], argv[3]); return;
				//execvp("./userserver", argv);
			}argv
			else if(i==serverId)
				execvp("./server", argv);
			else*/
				//execvp("./router",argv);
				sprintf(temp,"gnome-terminal 'sh -c \"echo 'saaaa ';\"'");// ./router %s %s %s;\"'", argv[1], argv[2], argv[3]);
				//sprintf(temp,"gnome-terminal -e 'sh -c \"echo \"saaaa \"; ./router %s %s %s; exec bash\"'", argv[1], argv[2], argv[3]);
				system(temp);
				printf("[%s]\n", temp);
			//printf("Err");
			//return;
			//while(1);
		}
	}
	int n = NIDS, idP;
	while(n){
		idP=wait(NULL);
		n--;
	}
	return;
}
