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
			/*int k, a;
			for(k=0; k<98765432; k++)
				a+=2-3;*/
			//printf("(%d:%d)Child\n",i, pid[i]);
			sprintf(x,"%d", i);
			argv[1]=x;
			//printf("%s %s=%s\n", argv[0], argv[1], x);
			if(i==userId)
				execvp("./user", argv);
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
		//printf("Child %d returned!\n", idP);
		n--;
	}
	return;
}
