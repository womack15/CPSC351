#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "msg.h"
#include <unistd.h>
#include <signal.h>

// The size of the shared memory chunk 
#define SHARED_MEMORY_CHUNK_SIZE 1000

// The ids for the shared memory segment and the message queue 
int shmid, msqid;
 
// The pointer to the shared memory 
char* sharedMemPtr;

//overwrite Ctrl-c handler

void init(int& shmid, int& msqid, char*& sharedMemPtr)
{     key_t key;
    if ((key = ftok("keyfile.txt", 'Z')) == -1) {
        perror("ftok");
        exit(1);}
	
	// TODO: Get the id of the shared memory segment. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE 
	// TODO: Attach to the shared memory 
	// TODO: Attach to the message queue 
	// Store the IDs and the pointer to the shared memory region in the corresponding parameters 
	    
    if ((shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }

    // attach to the shared memory 
    sharedMemPtr = (char*)shmat(shmid, (void *)0, 0);
    if (sharedMemPtr == (char *)(-1)) {
        perror("shmat");
        exit(1);
    }
  // Attach to the message queue 
    if ((msqid = msgget(key, 0666 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(1);
    }

}

void send(const char* fileName)
{
		// Open the file argv[1] for reading 
	FILE* fp = fopen(fileName, "r"); 
	int msgsize=0;

	// Was the file open? 
	if(!fp)	{
		perror("fopen");
		exit(-1);
	}

	  // A buffer to store message we will send to the receiver. 
    message sendMsg;   
    sendMsg.mtype = RECV_DONE_TYPE;  
    sendMsg.size = 1;   

    //send message
        if (msgsnd(msqid, &sendMsg, sizeof(message), 0) == -1) // +1 for '\0' 
            perror("msgsnd");


 while(!feof(fp) ){


    while(sendMsg.mtype == SENDER_DATA_TYPE){//to check if buf has something there
      msgrcv(msqid, &sendMsg, sizeof(message), 0, 0) ;  //receive message
    }  
printf("I am here, msg type: %ld size:%d\n", sendMsg.mtype, sendMsg.size);
sleep(1);

       //critical section
    if((sendMsg.size=fread(sharedMemPtr, 1, SHARED_MEMORY_CHUNK_SIZE, fp))<0){
			perror("fread");
			exit(-1);
		}   
    sendMsg.mtype = SENDER_DATA_TYPE;  
    msgsnd(msqid, &sendMsg, sizeof(message), 0)  ;  

//printf("I am here, msg type: %ld size:%d\n", sendMsg.mtype, sendMsg.size);
//for(;;);


     
}


fclose(fp);
//it's done
sendMsg.size = 0;  
msgsnd(msqid, &sendMsg, sizeof(message), 0)  ;


 }





int main(int argc, char *argv[])
{



	// Connect to shared memory and the message queue 
	init(shmid, msqid, sharedMemPtr);
	
	// Send the file 
	send(argv[1]);
	
	// Cleanup 
	//cleanUp(shmid, msqid, sharedMemPtr);

    
    
   



    return 0;
}
