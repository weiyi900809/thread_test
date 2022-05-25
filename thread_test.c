#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include <sys/shm.h>

#include "msg_process.h"

#define NUM_THREADS 5
char table[] = "{\"hello\",\"world\",\"hello\",\"world\"}";
tmsg_buffer* msg_buff[NUM_THREADS] ;
tmsg_element* event[NUM_THREADS] ;
int thread_command[NUM_THREADS]; //
int thread_transmit_target[NUM_THREADS][NUM_THREADS];
int thread_receive_signal[NUM_THREADS];
int thread_receive_from[NUM_THREADS];
char receive_data[NUM_THREADS][1024];
pthread_mutex_t mutex[NUM_THREADS];
pthread_cond_t  butter_empty[NUM_THREADS];//signal of  buffer is empty 
int buffer_not_empty[NUM_THREADS]={0,0,0,0};//signal of  buffer is not empty 


void *thread_func(void *threadid) {
    long tid;
    int i;
    tid = (long)threadid;
    msg_buff[tid] = msg_buffer_init();
    
     
	
 	

    printf("Hello There! I am thread %ld, my pthread ID - %lu\n", tid, pthread_self()-1);
    while(1){
		sleep(0.05);
		if(thread_receive_signal[tid] ==1 ){
			pthread_mutex_lock(&mutex[tid]);
			event[tid] = msg_buff[tid]->get(msg_buff[tid]); 
			
			printf("thread %ld I receive data from thread %d  :", tid,thread_receive_from[tid]);
			printf("%s \n",event[tid]->dt);
			
			msg_buff[tid]->clear(msg_buff[tid]);
			thread_receive_signal[tid] =0; 
			buffer_not_empty[tid] = 0;
			pthread_cond_signal(&butter_empty[tid]);
			pthread_mutex_unlock(&mutex[tid]);
			
		}
		switch(thread_command[tid]) {
			case 1:
				if(receive_data[tid] != NULL) {
	     			printf("thread %ld ,recv_data: %s\n", tid,receive_data[tid]);
	     			thread_command[tid]=-1;
			 	}
			 	
				break;
			case 2:
				if(receive_data[tid] != NULL) {
	     			printf("thread %ld ,recv_data: %s\n", tid,receive_data[tid]);
	     			thread_command[tid]=-1;
			 	}
			 	
				break;
			case 3:
				if(receive_data[tid] != NULL && thread_transmit_target[tid][0] != -1) {
	     			printf("thread %ld ,transmit_data: %s , transmit_target:", tid,receive_data[tid]);
	     			
	     			for(i=0;i<NUM_THREADS;i++)
	     				if(thread_transmit_target[tid][i]!= -1 )
	     					printf(" %d ", thread_transmit_target[tid][i]);
	     			puts("");
	     			
	     			for(i=0;i<NUM_THREADS;i++){
	     				if(thread_transmit_target[tid][i]!= -1 ){
	     					while(buffer_not_empty[thread_transmit_target[tid][i]] == 1){
	     						pthread_cond_wait(&butter_empty[thread_transmit_target[tid][i]],&mutex[thread_transmit_target[tid][i]]);	
	     					}
						
	     					msg_buff[thread_transmit_target[tid][i]]->sendmsg(msg_buff[thread_transmit_target[tid][i]],0,0,receive_data[tid],sizeof(receive_data[tid]));
	     					thread_receive_from[thread_transmit_target[tid][i]]=tid;
	     					buffer_not_empty[thread_transmit_target[tid][i]] = 1;
	     					thread_receive_signal[thread_transmit_target[tid][i]] =1 ;			
	     				}
	     				
	     				
	     			}
	     			
	     			thread_command[tid]=-1;
			 	}
				break;	
			case 4:
				
				break;
			case 0:
				break;
			case -1:
				break;	
		}
		if(thread_command[tid]==0) {
			printf(" thread %ld terminated !\n", tid);
			break;
		}
		
		
	}
    pthread_exit(NULL);
}


int main() {
    pthread_t threads[NUM_THREADS];
    int rc;
    int check=1;
    char data[1024]; 
    char file_data[1024]; 
    int bot_command=-1;
    int receive_ID[NUM_THREADS];
    int i;

    long t;
    FILE* f;

    
    for (t = 0; t < NUM_THREADS; t++) {
        rc = pthread_create(&threads[t], NULL, thread_func, (void *)t);
        thread_command[t]=-1;
        for (i = 0; i < NUM_THREADS; i++)
		thread_transmit_target[t][i]= -1;	
        if (rc) {
            printf("ERORR; return code from pthread_create() is %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }
    
  

    f = fopen("command.txt" , "r");
    if(!f){
        printf("data not exist");
        system("PAUSE");
        return 0;
    }
   	
    
    
    

    
    
    
    
    while(bot_command != 0 ){
    	sleep(1);
    	
    	printf("enter command \n");
    	printf("0:Exit   1:output data   2:appoint thread receive data   3:appoint thread transmit data\n");
    		fgets(file_data, 1024, f);
    		bot_command=file_data[0]-48;
    		if(bot_command < 0 || bot_command > 3){
			printf("error: only accept 0,1,2,3\n");
			break;	
		}
    		printf("bot_command :%d \n",bot_command);
		/*scanf("%d",&bot_command);
		while(bot_command < 0 || bot_command > 3){
			printf("error: only accept 0,1,2,3\n");
			scanf("%d",&bot_command);
			
		} */
		if(bot_command == 0 ) {
			for (t = 0; t < NUM_THREADS; t++) 
				thread_command[t]=bot_command;	
			break;
		}
		sleep(0.8);
		
		switch(bot_command) {
			
	    	case 1: 
	    		
	        	printf("enter receive data\n");
	        	
			
			/*getchar();
			fgets(data, 1024, stdin);*/ 
			fgets(file_data, 1024, f);
			if(file_data[strlen(file_data) - 1] == '\n'){
				file_data[strlen(file_data) - 1] = '\0';
			}
			strcpy(data,file_data);
			puts(data);

			for (t = 0; t < NUM_THREADS; t++) {
				thread_command[t]=bot_command;
				strcpy(receive_data[t],data);
			}
	        	break;
	        	
	        case 2: 
	        	
	        	printf("enter ID of thread(s) and input -1 to terminate\n");
	        	for (t = 0; t < NUM_THREADS; t++) {
				fgets(file_data, 1024, f);
				if((file_data[0]-48) != -3 ){

					receive_ID[t] = file_data[0] -48;
					printf("%d\n", receive_ID[t]);
				}
				if((file_data[0]-48) == -3 ){
					
					receive_ID[t] = file_data[0] -46;
					printf("%d\n", receive_ID[t]);
					break;
				}
				if((file_data[0]-48) > (NUM_THREADS-1) || (file_data[0]-48) < (-1)){
					printf("input erroe not exist this thread id\n");
					break;
				}
	        	}
	        	for (t = 0; t < NUM_THREADS; t++) {
	        		
	        		//scanf("%d",&receive_ID[t]);
	        		
	        		/*while(receive_ID[t] > (NUM_THREADS-1) || receive_ID[t] < (-1)){
	        			printf("input erroe not exist this thread id\n");
	        			scanf("%d",&receive_ID[t]);
	        		}	*/
	        		if(receive_ID[t] == -1) {
						for (i = t; i < NUM_THREADS; i++) 
							receive_ID[i] = -1;
						break;
					}
				}
				
	        	printf("enter receive data\n");
			/*getchar();
			fgets(data, 1024, stdin);*/
			fgets(file_data, 1024, f);
			if(file_data[strlen(file_data) - 1] == '\n'){
				file_data[strlen(file_data) - 1] = '\0';
			}
			strcpy(data,file_data);
			puts(data);
				
			for (t = 0; t < NUM_THREADS; t++) {
				if(receive_ID[t]!=-1) {
					
					thread_command[receive_ID[t]]=bot_command;
					strcpy(receive_data[receive_ID[t]],data);
				}
					
			}
				/*for (t = 0; t < NUM_THREADS; t++) {
					printf("thread_command: %d\n", thread_command[t]);
				}*/
				
				
	        	break;
	        case 3:
	        	printf("enter ID of thread\n");
	        	fgets(file_data, 1024, f);
			if((file_data[0]-48) != -3 ){
				receive_ID[0] = file_data[0] -48;
				printf("%d\n", receive_ID[0]);
			}
			if((file_data[0]-48) == -3 ){
				receive_ID[0] = file_data[0] -46;
				printf("%d\n", receive_ID[0]);
				break;
			}
			if((file_data[0]-48) > (NUM_THREADS-1) || (file_data[0]-48) < (-1)){
				printf("input erroe not exist this thread id\n");
				break;
			}
			
	        	
	        	/*scanf("%d",&receive_ID[0]);
	        	while(receive_ID[0]>(NUM_THREADS-1) || receive_ID[0]< (-1)){
	        		printf("input erroe not exist this thread id\n");
	        		scanf("%d",&receive_ID[0]);
	        	}*/
	        		
	        	
	        	
	        	printf("enter ID of target thread and input -1 to terminate\n");
	        	for (t = 0; t < NUM_THREADS; t++) {
				fgets(file_data, 1024, f);
				if((file_data[0]-48) != -3 ){
					thread_transmit_target[receive_ID[0]][t] = file_data[0] -48;
					printf("%d\n", thread_transmit_target[receive_ID[0]][t]);
				}
				if((file_data[0]-48) == -3 ){
					thread_transmit_target[receive_ID[0]][t] = file_data[0] -46;
					printf("%d\n", thread_transmit_target[receive_ID[0]][t]);
					break;
				}
				if((file_data[0]-48) > (NUM_THREADS-1) || (file_data[0]-48) < (-1)){
					printf("input erroe not exist this thread id\n");
					break;
				}
	        	}
	        	
	        	for (t = 0; t < NUM_THREADS; t++) {
	        		/*scanf("%d",&thread_transmit_target[receive_ID[0]][t]);
	        		while(thread_transmit_target[receive_ID[0]][t]>(NUM_THREADS-1) || thread_transmit_target[receive_ID[0]][t]< (-1)){
	        			printf("input erroe not exist this thread id\n");
	        			scanf("%d",&thread_transmit_target[receive_ID[0]][t]);
	        		}*/
	        				
	        		if(thread_transmit_target[receive_ID[0]][t] == -1) {
						for (i = 0; i < NUM_THREADS; i++) 
							thread_transmit_target[receive_ID[0]][t] = -1;
						break;
					}
			}
			
			printf("enter receive data\n");
		
			/*getchar();
			fgets(data, 1024, stdin);*/
			fgets(file_data, 1024, f);
			if(file_data[strlen(file_data) - 1] == '\n'){
				file_data[strlen(file_data) - 1] = '\0';
			}
    			
			strcpy(data,file_data);
			puts(data);
			
			strcpy(receive_data[receive_ID[0]],data);
			thread_command[receive_ID[0]]=bot_command;
	        	
	        			        
	        	break;	
	        	
	    	
		}
		
		
		
		
		
		sleep(0.8);
		
    
	}
	for (t = 0; t < NUM_THREADS; t++){
		pthread_join(threads[t],NULL);	
	}
    
    fclose(f);
    pthread_exit(NULL);
	
    return 0; 
    
}
