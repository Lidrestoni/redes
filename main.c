/* You can compile this program with:
 * gcc -o thread thread.c -lpthread */
/* We always need to include this header file for<\n>
 * the threads */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

//void *mythread(void *data);

#define N 3 // number of threads
#define PROCC 2

/*int x = 0;
int main(void) {
   pthread_t tids[N];
   int i;
     for(i=0; i<N; i++) {
      pthread_create(&tids[i], NULL, mythread, NULL);
   }
   * We will now wait for each thread to<\n>
    * terminate *
   for(i=0; i<N; i++) {
      pthread_join(tids[i], NULL);
      printf("Thread id %ld returned\n", tids[i]);
   }
   return(1);
}*/


//pid_t pid;
void main(){
	int pid,i;
	char *argv[] = {"/router","1", NULL};
	for(i=1; i<=PROCC; i++){
		pid=(int)fork();
		if(pid!=0){
			printf("\n I am the father of (%d)%d\n", i,pid);
			waitpid((pid_t)pid,NULL,0);
			printf("%d returned!\n", i);
			return;
		}
		else{
			int k, a;
			char x[5];
			for(k=0; k<98765432; k++)
				a+=2-3;
			printf("(%d)Child\n",i);
			sprintf(x,"%d", i);
			argv[1]=x;
			printf("%s %s=%s\n", argv[0], argv[1], x);
			/*if(execv("./router",argv)==-1)
				printf("Err");*/
		}
		printf("->%d<-\n",i);
	}
	/*if(pid==0){
		int k, a;
			for(k=0; k<98765432; k++)
				a+=2-3;
		printf("(%d)Child\n",i);
		//execv("./router",argv);
	}*/
}

/*int main(){
	int proc, i;
	for(i=0; i<PROCC; i++){
		proc=fork(tick[i]);
		if(proc==0){
			printf(1,"Waiting for process %d to terminate (%d)\n", i, uptime());
			int k, a;
			for(k=0; k<98765432; k++)
				a+=2-3;
			printf(1,"Process %d has ended at %d\n", i, uptime());
			break;	
		}
	}

	if(proc!=0)
		for(i=0; i<PROCC; i++)
			wait();
	return 0;
}*/

/*void *mythread(void *data) {
struct timeval tv;
struct timezone tz;
struct tm *tm;
   while(x < 10) {
      x++;
      gettimeofday(&tv, &tz);
      tm=localtime(&tv.tv_sec);
      printf("Thread ID%ld: x is now %d (%d:%02d:%02d:%d).\n",pthread_self(), x, tm->tm_hour, tm->tm_min, tm->tm_sec, (int)tv.tv_usec);
      usleep(2000000);
   }
   * We can return a pointer. Whatever pointer
    * we return can later be retrieved using the
    * pthread_join function *
   pthread_exit(NULL);
}*/


