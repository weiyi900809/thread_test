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


tmsg_buffer* receive_buff[NUM_THREADS][NUM_THREADS] ;
tmsg_element* receive_event[NUM_THREADS][NUM_THREADS] ;

typedef struct  transmit{

long from;
long to;
	
}Transmit; 
  

int thread_command[NUM_THREADS]; //
int send_command[NUM_THREADS][NUM_THREADS]; 

int thread_transmit_target[NUM_THREADS][NUM_THREADS];
int thread_peer_list[NUM_THREADS][NUM_THREADS];
int thread_receive_signal[NUM_THREADS];
int thread_send_signal[NUM_THREADS];

int receive_tunnel_ready_signal[NUM_THREADS][NUM_THREADS];
int send_tunnel_ready_signal[NUM_THREADS][NUM_THREADS];
//int thread_time_signal[NUM_THREADS];

int thread_send_to[NUM_THREADS];
int thread_receive_from[NUM_THREADS];
char send_message[NUM_THREADS][1024];
char receive_message[NUM_THREADS][1024];
char receive_func_message[NUM_THREADS][NUM_THREADS][1024];

char command_buff[NUM_THREADS][1024];

char file_data[10][1024]; 
int CPP_terminate_signal=0;
int tunnel_work_over[NUM_THREADS][NUM_THREADS];
int receive_work_over=0;
int request_work_over=0;
pthread_mutex_t mutex[NUM_THREADS][NUM_THREADS];

pthread_t receive[NUM_THREADS][NUM_THREADS];
pthread_t send[NUM_THREADS][NUM_THREADS];

pthread_cond_t  receive_butter_empty[NUM_THREADS][NUM_THREADS];//signal of  buffer is empty 
pthread_cond_t  request_empty[NUM_THREADS];

int buffer_not_empty[NUM_THREADS]={0,0,0,0};//signal of  buffer is not empty 

char make_peer_list_message(char message[],int tid){

   //char message[50];
   char text[50];
   
   int i;
 
   strcpy(message, "I am thread ");
   sprintf(text, "%d", tid);   
   strncat(message,text ,1);
   strcpy(text, " my peer list have: ");
   strncat(message,text ,strlen(text));
  
   for(i=0;i<NUM_THREADS;i++){
	   if(thread_peer_list[tid][i]!= -1 ){
		   sprintf(text, "%d", thread_peer_list[tid][i]);  
		   
		   strncat(message,text ,1);
		   if(thread_peer_list[tid][i+1]!= -1 ){
		   strcpy(text, ",");
		   strncat(message,text ,1);
		   }
		   
	   }
   
   }
   
	
   
}
   
   	


void *CPP_func(){
    long time_counter =5;	
    int bot_command;
    int i=0;
    FILE* f;
    printf("Hello There! I am CPP\n");
    //while(CPP_terminate_signal != 1){
    	

	   // if(time_counter == 5){
		    f = fopen("command.txt" , "r");
		    if(!f){
			printf("data not exist");
			system("PAUSE");
			pthread_exit(NULL);
		    }
		    while(fgets(file_data[i], 1024, f) != NULL ){
		    
		    
		    printf("command from botmaster:");
		    puts(file_data[i]);
		    i++;
		    }
		   /* time_counter=0;
		    i=0;
		    fclose(f);*/
	    //}
	    
	    sleep(1);
	    time_counter++;
    
    
    //}
    printf(" CPP terminated !\n");
    pthread_exit(NULL);
	
}
void *behavior_func(void *threadid){

    long tid;
    tid = (long)threadid;
    int behavior_num=0;
    switch(behavior_num) {
    	case 0:
		break;	
    	case 1:
		break;
	case 2:
		
		break;
	case 3:
		break;	
				
    }
    


}
void *handle_send_func(void *transmit_information){

    Transmit *information;
    information = (Transmit *)transmit_information;
   
    
    int i,j;
   
    char message_peer_list[1024];
    while(tunnel_work_over[information->from][information->to] != 1){
    	if(send_tunnel_ready_signal[information->from][information->to]  == 1){
    	
    	switch(send_command[information->from][information->to]) {
	    	
	    	case 1:
			
			make_peer_list_message(message_peer_list,information->from);
						
		     					
		     	while(receive_tunnel_ready_signal[information->from][information->to] ==1 ){
		     		
		     		
		     		pthread_cond_wait(&receive_butter_empty[information->from][information->to],&mutex[information->from][information->to]);	
		     	}
							
		     			
		     	strcpy(receive_func_message[information->from][information->to], message_peer_list);
		     	
		     	receive_tunnel_ready_signal[information->from][information->to] =1 ;
		     	
		     	
		     	
		     	//pthread_cond_signal(&request_empty[tid]);
		     	
		     	
			break;
		case 2:
			/*while(thread_receive_signal[thread_receive_from[tid]] ==1  ){
	     			printf("waiting..\n");
	     			pthread_cond_wait(&butter_empty[thread_receive_from[tid]],&mutex[thread_receive_from[tid]]);	
	     		}
						
	     			
	     		strcpy(receive_message[thread_receive_from[tid], send_message[tid]);
	     		thread_receive_from[thread_receive_from[tid]]=tid;
	     		thread_receive_signal[thread_receive_from[tid]] =1 ;
	     		request_command[tid] = 0;
	     		*/
			break;
		case 0:
			break;	
					
	    }
	    
	    strcpy(message_peer_list,"");
	    send_tunnel_ready_signal[information->from][information->to] =0; 
	    send_command[information->from][information->to] = 0;
		     	
		     	
    	
    	}
    	
	    
	    
	    
    	
    }
    
    printf(" handle_send_func %ld-%ld terminated !\n", information->from,information->to);
    pthread_exit(NULL);	



}
void *handle_receive_func(void *transmit_information){

    Transmit *information;
    information = (Transmit *)transmit_information;
   
    
    int i,j;
   
    int behavior_request=0;
    char func_message[1024];
    
    
    while(tunnel_work_over[information->from][information->to] != 1){
    	
	
	if(receive_tunnel_ready_signal[information->from][information->to] == 1 ){
			
	printf("thread %ld  receive message from thread %ld : %s\n", information->to , information->from, receive_func_message[information->from][information->to]);
	
	strcpy(func_message,receive_func_message[information->from][information->to]);
	//printf("func_message:%s\n", func_message);
	//printf("func_message[0]:%c\n", func_message[0]);
	if(func_message[0] == '*' ){
	
		behavior_request = func_message[1]-48;
		
	    	switch(behavior_request) {
				
				case 1:
					
					
					send_command[information->to][information->from]=1;
					send_tunnel_ready_signal[information->to][information->from]=1;
				
	     				
					break;
				case 2:
					
					//send_command[tid]=2;
				 	
					break;
				case 0:
					break;	
				
		}	 
    		
    	}
    	

	sleep(1);
	behavior_request=0;				
	strcpy(receive_func_message[information->from][information->to],"");
	strcpy(func_message,"");
	receive_tunnel_ready_signal[information->from][information->to] = 0 ; 
	pthread_cond_signal(&receive_butter_empty[information->from][information->to]);
	
	}
	
	
	
	
 
    }
    
    printf(" handle_receive_func %ld-%ld terminated !\n", information->from,information->to);
    pthread_exit(NULL);	

}
void *thread_func(void *threadid) {
    long tid;
    int i,j;
    int rc;
    
    tid = (long)threadid;
    
    
    
    printf("Hello There! I am thread %ld, my pthread ID - %lu\n", tid, pthread_self()-1);
    while(1){
		
		sleep(0.5);
		
		
		switch(thread_command[tid]) {
			
			case 1:
				printf("peer list of thread %ld have:", tid);
				for(i=0;i<NUM_THREADS;i++){
					if(thread_peer_list[tid][i]!= -1)
						printf("%ld ", thread_peer_list[tid][i]);
				}
				puts("");
				thread_command[tid]=99;
				
				break;
			case 2:
				
				if(send_message[tid] != NULL && thread_peer_list[tid][0] != -1 ) {
				
	     			printf("thread %ld , transmit_data: %s , peerlist:", tid,send_message[tid]);
	     			
	     			for(i=0;i<NUM_THREADS;i++){
	     				if(thread_peer_list[tid][i]!= -1 ){
	     					printf(" %d ", thread_peer_list[tid][i]);
	     				}
	     			}
	     			puts("");
	     			
	     			for(i=0;i<NUM_THREADS;i++){
	     				
	     				if(thread_peer_list[tid][i]!= -1  ){ 
	     					
	     					while(receive_tunnel_ready_signal[tid][thread_peer_list[tid][i]] ==1 ){
					     		pthread_cond_wait(&receive_butter_empty[tid][thread_peer_list[tid][i]],&mutex[tid][thread_peer_list[tid][i]]);	
					     	}
						
	     					
	     					strcpy(receive_func_message[tid][thread_peer_list[tid][i]], send_message[tid]);//
	     					
	     					receive_tunnel_ready_signal[tid][thread_peer_list[tid][i]] = 1 ;
	     					
	     							
	     				}
	     				
	     				
	     			}
	     			
	     			
	     			
			 	}
			 	thread_command[tid]=99;
			 	
				break;
			case 3:
				
	     			thread_command[tid]=99;
				break;	
				
			case 4:
				
	     			thread_command[tid]=99;
	     			
				break;		
			case 0:
				
				break;
			case (-1):
				break;	
		}
		if(thread_command[tid]==-1) {
			
			printf(" thread %ld terminated !\n", tid);
			for (i = 0; i < NUM_THREADS; i++){
	    		for (j = 0; j < NUM_THREADS; j++){
	    		tunnel_work_over[i][j]=1;
	    		}
	    		}
			break;
		}
		
		
	}
    pthread_exit(NULL);
}


int main() {
    pthread_t threads[NUM_THREADS];
    
    pthread_t CPP;
    int rc,return_data;
    int check=1;
    char data[1024]; 

    int bot_command=-1;
    int receive_command_ID[NUM_THREADS];
    int i=0;
    int j=0;
    long t;
    //FILE* f;

    Transmit transmit_data[NUM_THREADS][NUM_THREADS];
    for (i = 0; i < NUM_THREADS; i++){
	    for (j = 0; j < NUM_THREADS; j++){
	    transmit_data[i][j].from = i;
	    //printf("transmit_data-> from :%ld \n",transmit_data.from);
	    transmit_data[i][j].to = j;
	    //printf("transmit_data-> to :%ld \n",transmit_data.to);
	    
	    rc = pthread_create(&receive[i][j], NULL, handle_receive_func, &transmit_data[i][j]);
	    if (rc) {
			    printf("ERORR; return code from pthread_create() is %d\n", rc);
			    pthread_exit(NULL);
		    }
	    rc = pthread_create(&send[i][j], NULL, handle_send_func, &transmit_data[i][j]);  
	    if (rc) {
			    printf("ERORR; return code from pthread_create() is %d\n", rc);
			    pthread_exit(NULL);
		    }      
	    }
    }
    
    
    //rc = pthread_create(&send[tid], NULL, handle_send_func, (void *)tid);
    
    
    
    
    rc = pthread_create(&CPP, NULL, CPP_func, NULL);  
    if (rc) {
            printf("ERORR; return code from pthread_create() is %d\n", rc);
            exit(EXIT_FAILURE);
    }
    
    for (t = 0; t < NUM_THREADS; t++) {
    	thread_command[t]=99;
        thread_receive_signal[t]=0;
        buffer_not_empty[t]=0;
        for (i = 0; i < NUM_THREADS; i++){
		thread_transmit_target[t][i]= -1;
		thread_peer_list[t][i] = -1;
		
	}
	
        rc = pthread_create(&threads[t], NULL, thread_func, (void *)t);
        
        if (rc) {
            printf("ERORR; return code from pthread_create() is %d\n", rc);
            exit(EXIT_FAILURE);
        }
        
        
    
        
  
    }

    i=0;
    j=0;

    while(bot_command != 0 ){
    	
    	sleep(1);
    	
    	printf("enter command \n");
    	printf("0:Exit  1: add thread id to peer list  2:send message to peer list \n");
    		
    		if(file_data[i][j]=='-') {
    		scanf("%d",&bot_command);
    		while(bot_command == 1){
			printf("auto not manual\n");
			scanf("%d",&bot_command);
			
		}
    		while(bot_command < 0 || bot_command > 2){
			printf("error: only accept 0,1,2\n");
			scanf("%d",&bot_command);
			
		}
		
    		//sleep(20);
    		}
    		
    		else{
    		while(file_data[i][j] != '|' && file_data[i][j] !='-'){
    		 
    		printf("file_data[i][j] :%c \n",file_data[i][j]);
    		bot_command = file_data[i][j]-48;		
    		j++;
    		}
    		if(file_data[i][j] == '|')j++;

    		if(bot_command < 0 || bot_command > 2){
			printf("error: only accept 0,1,2\n");
			
			break;	
		}
    		printf("bot_command :%d \n",bot_command);
    		
    		}
		/*scanf("%d",&bot_command);
		 */
		if(bot_command == 0 ) {
			for (t = 0; t < NUM_THREADS; t++) 
				thread_command[t]=-1;	
			CPP_terminate_signal=1;	
			break;
		}
		sleep(0.8);
		
		switch(bot_command) {
			
		    	
			case 1:	
				
				printf("enter ID of thread\n");
				while(file_data[i][j] != '|'){
    		 
		    		printf("file_data[i][j] :%c \n",file_data[i][j]);
		    		
		    		receive_command_ID[0] = file_data[i][j] -48;
		    		
				printf("%d\n", receive_command_ID[0]);		
		    		j++;
		    		}
		    		if(receive_command_ID[0]>(NUM_THREADS-1) || receive_command_ID[0]< (-1)){
					printf("input error not exist this thread id\n");
					break;
				}
		    		if(file_data[i][j] == '|')j++;
		    		
	
				
				printf("add ID of thread to peer list and input -1 to terminate\n");
				
				t=0;
				int k;
				while(file_data[i][j] != '|'){

			    		
			    		if((file_data[i][j]-48) >= 0){
			    			if((file_data[i][j]-48) > (NUM_THREADS-1) || (file_data[i][j]-48) < (-1)){
							printf("input error not exist this thread id\n");
							break;
						}
						if((file_data[i][j]-48) == receive_command_ID[0]){
							printf("input error cannt input receive thread id\n");
							break;
						}
						
				    		thread_peer_list[receive_command_ID[0]][t] = file_data[i][j]-48;
				    		printf("thread_peer_list :%d\n",thread_peer_list[receive_command_ID[0]][t]);
				    		t++;
			    		}
			    		if((file_data[i][j]-48) == -3){
			    		thread_peer_list[receive_command_ID[0]][t] = -1;
			    		}
			
			    		j++;
		    		}
		    		
		    		
		    		
		    			

				thread_command[receive_command_ID[0]]=bot_command;
				i++;
				j=0;
				break;
				
				
			case 2:
				printf("1:all 0:appoint \n");
				int all_signal;
				scanf("%d",&all_signal);
				while(all_signal < 0 || all_signal > 1){
					printf("error: only accept 0,1\n");
					scanf("%d",&all_signal);
				
				} 
				
				if(all_signal == 0){
					printf("enter ID of thread(s) and input -1 to terminate\n");
					
					for (t = 0; t < NUM_THREADS; t++) {
						
						scanf("%d",&receive_command_ID[t]);
						
						while(receive_command_ID[t] > (NUM_THREADS-1) || receive_command_ID[t] < (-1)){
							printf("input error not exist this thread id\n");
							scanf("%d",&receive_command_ID[t]);
						}	
						if(receive_command_ID[t] == -1) {
								for (k = t; k < NUM_THREADS; k++) 
									receive_command_ID[k] = -1;
								break;
							}
					}
					
					printf("enter receive data\n");
					getchar();
					fgets(data, 1024, stdin);
					data[strlen(data) - 1] = '\0';
					
					for (t = 0; t < NUM_THREADS; t++) {
						if(receive_command_ID[t]!=-1) {
							
							thread_command[receive_command_ID[t]]=bot_command;
							strcpy(send_message[receive_command_ID[t]],data);//strcpy(message[receive_command_ID[t]],data);
						}
							
					}
					
				
				}
				else{
					printf("enter receive data\n");
					getchar();
					fgets(data, 1024, stdin);
					data[strlen(data) - 1] = '\0';
										
					for (t = 0; t < NUM_THREADS; t++) {							
						thread_command[t]=bot_command;
						strcpy(send_message[t],data);		
					}
				
				}
				
				break;
			default:
				sleep(10);
				break;
				
	    	
		}
		
		
		
		
		
		sleep(0.8);
		
    
	}
	for (t = 0; t < NUM_THREADS; t++){
		pthread_join(threads[t],NULL);	
	}
    
    
    pthread_exit(NULL);
	
    return 0; 
    
}
