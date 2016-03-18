
/*	g++ -o sender sender.c && ./sender  sender.txt  */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>


#include <unistd.h>
#include <signal.h>

// The size of the shared memory chunk 
#define SHARED_MEMORY_CHUNK_SIZE 1000

// The ids for the shared memory segment and the message queue 
int shmid;
 
// The pointer to the shared memory 
void* sharedMemPtr;

int pid,sendSize; //=11056;


 	FILE* fp = fopen("keyfile.txt", "r"); 


 
static void sig_handler(int signum) {
 printf("in sender signal\n");
}

void init(int& shmid,   void*& sharedMemPtr)
{     key_t key;
    if ((key = ftok("keyfile.txt", 'Z')) == -1) {
        perror("ftok");
        exit(1);}
 
	//  Get the id of the shared memory segment. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE 
	    
    if ((shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }

    // attach to the shared memory 
    sharedMemPtr = (void*)shmat(shmid, (void *)0, 0);
    if (sharedMemPtr == (char *)(-1)) {
        perror("shmat");
        exit(1);
    }
 }
void cleanUp(const int& shmid,  void* sharedMemPtr)
{
	/* TODO: Detach from shared memory */
	
	/* TODO: Deallocate the shared memory chunk */

    if (shmdt(sharedMemPtr) == -1) {
        perror("shmdt");
        exit(1);
    }
 
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {//remove a message queue 
        perror("shmctl");
        exit(1);
    }	
 
    /* detach from the segment: */
 

 
 
    //sharedMemPtr=NULL;
}
 
void send(const char* fileName){
   // Open the file argv[1] for reading 
   FILE* fp = fopen(fileName, "r"); 
 	 
   //*pidPtr=getpid();pid=*pidPtr;   //save pid of recv.c
   while(!feof(fp) ){
    
   if((sendSize=fread((char*)(sharedMemPtr)+4, 1, SHARED_MEMORY_CHUNK_SIZE-4, fp))<0){
	 	      perror("fread");
	      exit(1);
      }
      *((int*)sharedMemPtr) = sendSize;
   // snprintf((char*)sharedMemPtr, sizeof(char)*4,"%d",sendSize);  
   /* char b[5];
    snprintf(b, 5,"%d",sendSize);
   if((fread((char*)(sharedMemPtr), 1, 5, (char* b))<0){
	 	      perror("fread");
	      exit(1);
      }  */
  printf("shared memory size = %d \n", *((int*)sharedMemPtr));
 
   if( (kill(pid,SIGUSR1))==-1 ){printf( "SIGUSR1 cannot be signal");exit(1);}   
   sleep(2);
 

}
 
   if( (kill(pid,SIGUSR2))==-1 ){printf( "SIGUSR2 cannot be signal");exit(1);}  
 

fclose(fp); 
printf("file closed\n");  
if( (kill(pid,SIGINT))==-1 ){printf( "SIGINT cannot be signal");exit(1);}  
  
 }





int main(int argc, char *argv[])
{
   // signal(SIGUSR1, sig_handler); 
   // signal(SIGUSR2, sig_handler); 
   signal(SIGUSR1, SIG_IGN); 
  signal(SIGUSR2, SIG_IGN); 
      //sharedMemPtr=0;  
	// Connect to shared memory and the message queue 
	init(shmid, sharedMemPtr);
     //*pidPtr=getpid(); pid=*pidPtr;
 
 
 //char *c=(char*)(sharedMemPtr);

    pid = atoi((char*)sharedMemPtr);
   printf("receiver pid is:%d\n", pid);
 
 
	// Send the file 
	 send(argv[1]);
	
	// Cleanup 

	//cleanUp(shmid,   sharedMemPtr);
 

    return 0;
}
