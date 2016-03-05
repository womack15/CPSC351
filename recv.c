#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>

#include "msg.h"
#include <unistd.h>
#include <signal.h>
/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
char *sharedMemPtr;

/* The name of the received file */
const char recvFileName[] = "recvfile.txt";



void init(int& shmid, int& msqid, char*& sharedMemPtr)
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
 

    if ((msqid = msgget(key, 0644)) == -1) { /* connect to the queue */
        perror("msgget");
        exit(1);
    }



}


/**
 * The main loop
 */
void mainLoop()
{
	/* The size of the mesage */
	int msgSize = 1;
	
	/* Open the file for writing */
	FILE* fp = fopen(recvFileName, "w+");

	/* Error checks */
	if(!fp)	{		perror("fopen");			exit(-1);	}
      else {printf("%s file is open\n",recvFileName);}
	  /* A buffer to store message we will send to the receiver. */
    message revMsg;  revMsg.mtype=RECV_DONE_TYPE;  revMsg.size=1;   







 msgSize=revMsg.size;


while(msgSize!= 0)	{	   
    while(revMsg.mtype!=SENDER_DATA_TYPE){  //if size=0, wait here
    /*recv message*/
         if (msgrcv(msqid, &revMsg, sizeof(message), 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }     
  }
 msgSize=revMsg.size;
printf("shared memory data size:\n %d\n",msgSize);

if(msgSize!=0){
// Save the shared memory to file */
	if(fwrite(sharedMemPtr,  sizeof(char), revMsg.size ,fp) < 0)	{
				perror("fwrite");
	}



		/* We are done */
    revMsg.mtype = RECV_DONE_TYPE;  
    msgsnd(msqid, &revMsg, sizeof(message), 0)  ;   
}

}
/* Close the file */
fclose(fp);


}



/**
 * Perfoms the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */

void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	/* TODO: Detach from shared memory */
	
	/* TODO: Deallocate the shared memory chunk */
	
	/* TODO: Deallocate the message queue */

    if (msgctl(msqid, IPC_RMID, NULL) == -1) {//remove a message queue 
        perror("msgctl");
        exit(1);
    }

    /* detach from the segment: */
    if (shmdt(sharedMemPtr) == -1) {
        perror("shmdt");
        exit(1);
    }

    sharedMemPtr=NULL;
}

/**
 * Handles the exit signal
 * @param signal - the signal type
 */

void signalHandlerFunc(int signo){


printf("i am signal handle\n");  
	/* let Ctrl-C deallocate memory and message queue */
   	cleanUp(shmid, msqid, sharedMemPtr);
    exit(0);

}

int main(void)
{

	// Overide the default signal handler for the SIGINT signal with signalHandlerFunc
	 

				
	/* Initialize */
	init(shmid, msqid, sharedMemPtr);
	
	/* Go to the main loop */
	mainLoop();

	signal(SIGINT, signalHandlerFunc); 

for(;;);



    return 0;
}
