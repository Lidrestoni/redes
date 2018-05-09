#include <stdio.h>
#include <sys/types.h>

#define NROUTERS 7


void main(){
	char x[5], routers[5];
	sprintf(routers,"%d", NROUTERS+2);
	int i, userId = 1, serverId = 9;
	pid_t pid;
	char *argv[] = {"X","X",routers, NULL};
	for(i=1; i<=NROUTERS+2; i++){
		if((pid = fork())==0){
			sprintf(x,"%d", i);
			argv[1]=x;
			if(i==userId){
				printf("Please:  Open a new terminal window in this folder and execute the following command:\n./userserver %s %s\n", argv[1], argv[2]); return;
				//execvp("./userserver", argv);
			}
			else if(i==serverId)
				execvp("./server", argv);
			else
				execvp("./router",argv);
			printf("Err");
			return;
		}
	}
	int n = NROUTERS+2, idP;
	while(n){
		idP=wait(NULL);
		n--;
	}
	return;
}
