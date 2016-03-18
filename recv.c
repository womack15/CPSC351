/*	g++ -o recv recv.c && ./recv */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>


#include <unistd.h>
#include <signal.h>

/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid,sendSize;

/* The pointer to the shared memory */
void *sharedMemPtr;

/* The name of the received file */
const char recvFileName[] = "recvfile.txt";
 	/* Open the file for writing */
	FILE* fp = fopen(recvFileName, "w+");
	
	void cleanUp(const int& shmid,  void* sharedMemPtr);
static void sig_handler(int signum) {

   if(signum == SIGUSR1)
   {
      printf("Receive SIGUSR==1==%d!\n",signum);

     sendSize= *((int*)sharedMemPtr);
     printf("send shared memory size:%d \n", sendSize);
      if(fwrite((char*)sharedMemPtr+4,  sizeof(char), sendSize ,fp) < 0)	{
         perror("fwrite");
      }

   }
   if(signum == SIGUSR2)
   {      printf("Receive SIGUSR==2==%d!\n",signum);
      /* Close the file*/
      fclose(fp);
   }
   if(signum == SIGINT){
         printf("Receive SIGINT==^c==%d!\n",signum);
      cleanUp(shmid,  sharedMemPtr);
      exit(0);
   }
}

void init(int& shmid,  void*& sharedMemPtr)
{
	
	/* TODO:  1. Create a file called keyfile.txt containing string "Hello world".
            2. Use ftok("keyfile.txt", 'a') in order to generate the key.
    		    3. Use the key in the below. 	 */
    key_t key;
    if ((key = ftok("keyfile.txt", 'Z')) == -1) {
        perror("ftok");
        exit(1);
    }
 

   if ((shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }

     /* attach to the shared memory */
    sharedMemPtr = (char*)shmat(shmid, (void *)0, 0);
    if (sharedMemPtr == (char *)(-1)) {
        perror("shmat");
        exit(1);
    }
     
}

 


/**
 * Perfoms the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */

void cleanUp(const int& shmid,  void* sharedMemPtr)
{
	/* TODO: Detach from shared memory */
 
	/* TODO: Deallocate the shared memory chunk */
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {//remove a message queue 
        perror("shmctl");
        exit(1);
    }	
 
    /* detach from the segment: */
    if (shmdt(sharedMemPtr) == -1) {
        perror("shmdt");
        exit(1);
    } 
    sharedMemPtr=NULL;
}

 

void mainLoop(){ 
      for(;;); 
  
 }
int main(void)
{

   // Overide the default signal handler for the SIGINT signal with signalHandlerFunc
   signal(SIGUSR1, sig_handler); 
   signal(SIGUSR2, sig_handler); 
   signal(SIGINT, sig_handler); 

   /* Initialize */
   init(shmid,   sharedMemPtr);	
   
    int pid=getpid();  
    
    char b[20];
    snprintf(b, 20,"%d",pid);  
     
    strncpy((char*)sharedMemPtr,b,1000);  
    printf("my pid is:%s\n", (char*)sharedMemPtr);
   /* Go to the main loop */
   mainLoop();
 
    return 0;
}
