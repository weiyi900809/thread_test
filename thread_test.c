#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/shm.h>

//
#define NUM_SERVENT_BOTS 4
#define NUM_CLIENT_BOTS 4
#define NUM_BOTS 4


typedef struct  transmit{

long from;
long to;
	
}Transmit; 
typedef struct  bot{

long id;
char ip[1024];
long port;
	
}Bot; 

  
Transmit servent_transmit_data[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
Transmit client_transmit_data[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
int servent_thread_work_over[NUM_SERVENT_BOTS/4];
int client_thread_work_over[NUM_SERVENT_BOTS/4];
char file_data[10][10][1024]; 

int socialnetwork_terminate_signal=0;
//servent----------------------------------
//-----------------------------------------
int servent_command[NUM_SERVENT_BOTS]; 
int servent_send_command[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS]; 
char servent_bot_command_buffer[NUM_SERVENT_BOTS][10][1024]; 
int servent_bot_command_buffer_pointer[NUM_SERVENT_BOTS];
char servent_bot_website_buffer[NUM_SERVENT_BOTS][10][1024]; 
int servent_bot_website_buffer_pointer[NUM_SERVENT_BOTS];
//-----------------------------------------
int servent_receive_tunnel_ready_signal[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
int servent_send_tunnel_ready_signal[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
char servent_send_message[NUM_SERVENT_BOTS][1024];
char servent_receive_message[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS][1024];
int servent_tunnel_work_over[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
int servent_work_over[NUM_SERVENT_BOTS];
//-----------------------------------------
int servent_peer_list[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
int servent_peer_num[NUM_SERVENT_BOTS];
//-----------------------------------------
int servent_client_list[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
int servent_client_num[NUM_SERVENT_BOTS];
//client-----------------------------------
//-----------------------------------------
int client_command[NUM_BOTS]; 
int client_send_command[NUM_BOTS][NUM_BOTS]; 
char client_bot_command_buffer[NUM_BOTS][1024]; 
int client_bot_command_buffer_pointer[NUM_BOTS];
char client_bot_website_buffer[NUM_BOTS][10][1024]; 
int client_bot_website_buffer_pointer[NUM_BOTS];
//-----------------------------------------
int client_receive_tunnel_ready_signal[NUM_BOTS][NUM_BOTS];
int client_send_tunnel_ready_signal[NUM_BOTS][NUM_BOTS];
char client_send_message[NUM_BOTS][1024];
char client_receive_message[NUM_BOTS][NUM_BOTS][1024];
int client_tunnel_work_over[NUM_BOTS][NUM_BOTS];
int client_work_over[NUM_BOTS];
//-----------------------------------------
int client_master[NUM_BOTS][NUM_BOTS];



pthread_mutex_t mutex[NUM_BOTS][NUM_BOTS];

pthread_t servent_receive[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
pthread_t servent_send[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
pthread_t client_receive[NUM_BOTS][NUM_BOTS];
pthread_t client_send[NUM_BOTS][NUM_BOTS];

pthread_cond_t  servent_receive_butter_empty[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];//signal of  buffer is empty 
pthread_cond_t  client_receive_butter_empty[NUM_BOTS][NUM_BOTS];//signal of  buffer is empty 

int sum_of_num_of_peerlist=0;
int client_transmit_times[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
int servent_transmit_times[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];



char make_peer_list_message(char message[],int tid){

   //char message[50];
   char text[50];
   
   int i;
 
   
   sprintf(message, "I am thread %d my peer list have: ", tid);   
  
   for(i=0;i<NUM_SERVENT_BOTS;i++){
	   if(servent_peer_list[tid][i]!= -1 ){
		   sprintf(text, "%d ", servent_peer_list[tid][i]);  
		   
		   strncat(message,text ,1);
		   if(servent_peer_list[tid][i+1]!= -1 ){
		   strcpy(text, ",");
		   strncat(message,text ,1);
		   }
		   
	   }
   
   }
   
	
   
}
   
   	


void *socialnetwork_func(){
    long time_counter =5;
    char path[1024];

    int i=0,j=0,f=1;
    FILE* file;
    printf("Hello There! I am socialnetwork read command/%d sec\n",time_counter);
    while(socialnetwork_terminate_signal != 1){
    	

	    if(time_counter == 5){

		    for(i=0,f=1;i<3,f<=3;i++,f++){
		    sprintf(path, "command%d.txt", f); 
		    file = fopen(path, "r");
		    if(!f){
			printf("data not exist");
			system("PAUSE");
			pthread_exit(NULL);
		    }
		    while(fgets(file_data[i][j], 1024, file) != NULL ){
		    
		    
		    printf("command%d from botmaster:",f);
		    puts(file_data[i][j]);
		    j++;
		    }
		    j=0;
		    fclose(file);
		    }
		    time_counter=0;
		    
		    
		    
		    
	    }
	    
	    sleep(1);
	    time_counter++;
    
    
    }
    printf(" socialnetwork terminated !\n");
    pthread_exit(NULL);
	
}

void *behavior_func(void *threadid){

    long tid;
    tid = (long)threadid;
    int behavior_num=0;
    int x =  rand()  % 4;
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
void *servent_handle_send_func(void *transmit_information){

    Transmit *information;
    information = (Transmit *)transmit_information;
   
    
    int i,j;
   
    char message[1024];
    while(servent_tunnel_work_over[information->from][information->to] != 1){
    	if(servent_send_tunnel_ready_signal[information->from][information->to]  == 1){
    	
    	switch(servent_send_command[information->from][information->to]) {
	    	
	    	case 1:
			
			
			strcpy(message,"return botmaster command");
			
		     	while(servent_receive_tunnel_ready_signal[information->from][information->to] ==1 ){
		     		
		     		
		     		pthread_cond_wait(&servent_receive_butter_empty[information->from][information->to],&mutex[information->from][information->to]);	
		     	}
							
		     			
		     	strcpy(servent_receive_message[information->from][information->to], message);
		     	
		     	servent_receive_tunnel_ready_signal[information->from][information->to] =1 ;
		     	
		     	
		
		     	
		     	
			break;
		case 2:
			make_peer_list_message(message,information->from);//----need review-----
						
		     					
		     	while(servent_receive_tunnel_ready_signal[information->from][information->to] ==1 ){
		     		
		     		
		     		pthread_cond_wait(&servent_receive_butter_empty[information->from][information->to],&mutex[information->from][information->to]);	
		     	}
							
		     			
		     	strcpy(servent_receive_message[information->from][information->to], message);
		     	
		     	servent_receive_tunnel_ready_signal[information->from][information->to] =1 ;
			break;
		case 3:
			strcpy(message,"return website");
			
		     	while(servent_receive_tunnel_ready_signal[information->from][information->to] ==1 ){
		     		
		     		
		     		pthread_cond_wait(&servent_receive_butter_empty[information->from][information->to],&mutex[information->from][information->to]);	
		     	}
							
		     			
		     	strcpy(servent_receive_message[information->from][information->to], message);
		     	
		     	servent_receive_tunnel_ready_signal[information->from][information->to] =1 ;
			break;	
					
	    }
	    
	    strcpy(message,"");
	    servent_send_tunnel_ready_signal[information->from][information->to] =0; 
	    
	    servent_send_command[information->from][information->to] = 0;
	         	
		     	
    	
    	}
    	
	    
	    
	    
    	
    }
    
    //printf(" servent_handle_send_func %ld-%ld terminated !\n", information->from,information->to);
    pthread_exit(NULL);	



}
void *servent_handle_receive_func(void *transmit_information){

    Transmit *information;
    information = (Transmit *)transmit_information;
   
    
    int i,j,b,command_comparison_result=1,website_comparison_result=1;
   
    int behavior_request=0;
    char func_message[1024];
    
    
    while(servent_tunnel_work_over[information->from][information->to] != 1){
    	
	
	if(servent_receive_tunnel_ready_signal[information->from][information->to] == 1 ){
			
	printf("servent %ld  receive message from servent %ld : %s\n", information->to , information->from, servent_receive_message[information->from][information->to]);
	
	strcpy(func_message,servent_receive_message[information->from][information->to]);
	
	
	
	if(strcmp(func_message,"return botmaster command") == 0 ){

	b=0;
		
	printf("command from servent %ld:\n",information->from);
	while(servent_bot_command_buffer[information->from][b][0]!= '\0'){
				
	for(i=0;i<servent_bot_command_buffer_pointer[information->to];i++){	
	command_comparison_result=strcmp(servent_bot_command_buffer[information->to][i],servent_bot_command_buffer[information->from][b]);
	if(command_comparison_result== 0){break;}
	}
				
	if(command_comparison_result== 0){
	printf(" servent %ld already have this command\n",information->to);
	}
	if(command_comparison_result != 0){
	strcpy(servent_bot_command_buffer[information->to][servent_bot_command_buffer_pointer[information->to]],servent_bot_command_buffer[information->from][b]);
	puts(servent_bot_command_buffer[information->to][servent_bot_command_buffer_pointer[information->to]]);
	servent_bot_command_buffer_pointer[information->to]++;
	}				
	b++;			
	}
	}
	
	
	if(strcmp(func_message,"return website") == 0 ){
	b=0;
		
	printf("website from servent %ld:\n",information->from);
	while(servent_bot_website_buffer[information->from][b][0]!= '\0'){
				
	for(i=0;i<servent_bot_website_buffer_pointer[information->to];i++){	
	website_comparison_result=strcmp(servent_bot_website_buffer[information->to][i],servent_bot_website_buffer[information->from][b]);
	if(website_comparison_result== 0){break;}
	}
				
	if(website_comparison_result== 0){
	printf(" servent %ld already have this website\n",information->to);
	}
	if(website_comparison_result != 0){
	strcpy(servent_bot_website_buffer[information->to][servent_bot_website_buffer_pointer[information->to]],servent_bot_website_buffer[information->from][b]);
	puts(servent_bot_website_buffer[information->to][servent_bot_website_buffer_pointer[information->to]]);
	servent_bot_website_buffer_pointer[information->to]++;
	}				
	b++;			
	}
	
	
	
	
	
	
	}
	//-------Need to deal with peer list specially---------
	if(func_message[0] == '*' ){
	
		behavior_request = func_message[1]-48;
		
	    	switch(behavior_request) {
				
				case 1:
					servent_send_command[information->to][information->from]=1;
					servent_send_tunnel_ready_signal[information->to][information->from]=1;

					break;
				case 2:
					
					servent_send_command[information->to][information->from]=2;
					servent_send_tunnel_ready_signal[information->to][information->from]=1;
				 	
					break;
				case 3:
					
					servent_send_command[information->to][information->from]=3;
					servent_send_tunnel_ready_signal[information->to][information->from]=1;
				 	
					break;	
					
				case 0:
					break;	
				
		}	 
    		
    	}
    	

	
	behavior_request=0;				
	strcpy(servent_receive_message[information->from][information->to],"");
	strcpy(func_message,"");
	servent_receive_tunnel_ready_signal[information->from][information->to] = 0 ; 
	pthread_cond_signal(&servent_receive_butter_empty[information->from][information->to]);
	servent_transmit_times[information->from][information->to]=0;
	//printf("receive:servent_transmit_times[%d][%d] :%d\n",information->to,information->from,servent_transmit_times[information->from][information->to]);
	
	}
	
	
	
	
 
    }
    
    //printf(" servent_handle_receive_func %ld-%ld terminated !\n", information->from,information->to);
    pthread_exit(NULL);	

}
void *client_handle_send_func(void *transmit_information){

    Transmit *information;
    information = (Transmit *)transmit_information;
   
    
    int i,j;
   
    char message_peer_list[1024];
    while(client_tunnel_work_over[information->from][information->to] != 1){
    	if(client_send_tunnel_ready_signal[information->from][information->to]  == 1){
    	
    	switch(client_send_command[information->from][information->to]) {
	    	
	    	case 1:
			
			
		     	
		     	
		     	
		
		     	
		     	
			break;
		case 2:
			make_peer_list_message(message_peer_list,information->from);
						
		     					
		     	while(client_receive_tunnel_ready_signal[information->from][information->to] ==1 ){
		     		
		     		
		     		pthread_cond_wait(&client_receive_butter_empty[information->from][information->to],&mutex[information->from][information->to]);	
		     	}
							
		     			
		     	strcpy(client_receive_message[information->from][information->to], message_peer_list);
		     	
		     	client_receive_tunnel_ready_signal[information->from][information->to] =1 ;
			break;
		case 0:
			break;	
					
	    }
	    
	    strcpy(message_peer_list,"");
	    client_send_tunnel_ready_signal[information->from][information->to] =0; 
	    
	    client_send_command[information->from][information->to] = 0;
	         	
		     	
    	
    	}
    	
	    
	    
	    
    	
    }
    
    printf(" client_handle_send_func %ld-%ld terminated !\n", information->from,information->to);
    pthread_exit(NULL);	



}
void *client_handle_receive_func(void *transmit_information){

    Transmit *information;
    information = (Transmit *)transmit_information;
   
    
    int i,j;
   
    int behavior_request=0;
    char func_message[1024];
    
    
    while(client_tunnel_work_over[information->from][information->to] != 1){
    	
	
	if(client_receive_tunnel_ready_signal[information->from][information->to] == 1 ){
			
	printf("client %ld  receive message from servent %ld : %s\n", information->to , information->from, client_receive_message[information->from][information->to]);
	
	strcpy(func_message,client_receive_message[information->from][information->to]);
	
	if(func_message[0] == '*' ){
	
		behavior_request = func_message[1]-48;
		
	    	switch(behavior_request) {
				
				case 1:
	
					

					break;
				case 2:
					
					client_send_command[information->to][information->from]=2;
					client_send_tunnel_ready_signal[information->to][information->from]=1;
				 	
					break;
				case 0:
					break;	
				
		}	 
    		
    	}
    	

	
	behavior_request=0;				
	strcpy(client_receive_message[information->from][information->to],"");
	strcpy(func_message,"");
	client_receive_tunnel_ready_signal[information->from][information->to] = 0 ; 
	pthread_cond_signal(&client_receive_butter_empty[information->from][information->to]);
	client_transmit_times[information->from][information->to]=0;
	printf("receive:client_transmit_times[%d][%d] :%d\n",information->to,information->from,client_transmit_times[information->from][information->to]);
	
	}
	
	
	
	
 
    }
    
    printf(" client_handle_receive_func %ld-%ld terminated !\n", information->from,information->to);
    pthread_exit(NULL);	

}
void servent_func(long servent_id){


    int rc,a,b,i,j,send_target,command_comparison_result=1,result,file_num;
    char text[1024];
  
		if(servent_work_over[servent_id] == 1){
			return;
		}
		servent_command[servent_id] =  rand()  % 4+1;
		printf(" servent_command[%ld] = %d !\n", servent_id,servent_command[servent_id]);
		switch(servent_command[servent_id]) {
			case 0:
				
				
				servent_command[servent_id]=99;
				break;
			case 1:
				file_num=rand()  % 3;
				switch(file_num){
				case 0:
					a=0;
					b=0;
					//printf(" I am servent[%ld] \n", servent_id);
					printf("command%d from botmaster:\n",a+1);
					while(file_data[a][b][0]!= '\0'){
					
					for(i=0;i<servent_bot_command_buffer_pointer[servent_id];i++){	
					command_comparison_result=strcmp(servent_bot_command_buffer[servent_id][i],file_data[a][b]);
					if(command_comparison_result== 0){break;}
					}
					
					if(command_comparison_result== 0){
					puts("already exist this command");
					
					}
					if(command_comparison_result != 0){
					strcpy(servent_bot_command_buffer[servent_id][servent_bot_command_buffer_pointer[servent_id]],file_data[a][b]);
					puts(servent_bot_command_buffer[servent_id][servent_bot_command_buffer_pointer[servent_id]]);
					servent_bot_command_buffer_pointer[servent_id]++;
					}
					
					
					b++;
					
					}
					break;
				case 1:
					a=1;
					b=0;
					//printf(" I am servent[%ld] \n", servent_id);
					printf("command%d from botmaster:\n",a+1);
					while(file_data[a][b][0]!= '\0'){
					
					for(i=0;i<servent_bot_command_buffer_pointer[servent_id];i++){	
					command_comparison_result=strcmp(servent_bot_command_buffer[servent_id][i],file_data[a][b]);
					if(command_comparison_result== 0){break;}
					}
					
					if(command_comparison_result== 0){
					puts("already exist this command");
					
					}
					if(command_comparison_result != 0){
					strcpy(servent_bot_command_buffer[servent_id][servent_bot_command_buffer_pointer[servent_id]],file_data[a][b]);
					puts(servent_bot_command_buffer[servent_id][servent_bot_command_buffer_pointer[servent_id]]);
					servent_bot_command_buffer_pointer[servent_id]++;
					}
					
					
					b++;
					
					}
					break;
				case 2:
					a=2;
					b=0;
					//printf(" I am servent[%ld] \n", servent_id);
					printf("command%d from botmaster:\n",a+1);
					while(file_data[a][b][0]!= '\0'){
					
					for(i=0;i<servent_bot_command_buffer_pointer[servent_id];i++){	
					command_comparison_result=strcmp(servent_bot_command_buffer[servent_id][i],file_data[a][b]);
					if(command_comparison_result== 0){break;}
					}
					
					if(command_comparison_result== 0){
					puts("already exist this command");
					
					}
					if(command_comparison_result != 0){
					strcpy(servent_bot_command_buffer[servent_id][servent_bot_command_buffer_pointer[servent_id]],file_data[a][b]);
					puts(servent_bot_command_buffer[servent_id][servent_bot_command_buffer_pointer[servent_id]]);
					servent_bot_command_buffer_pointer[servent_id]++;
					}
					
					
					b++;
					
					}
					break;
				}
				
				
				servent_command[servent_id]=99;
				
				break;
			case 2:
				
				servent_peer_num[servent_id]=0;
				for(i=0;i<NUM_SERVENT_BOTS;i++){
				
				if(servent_peer_list[servent_id][i]!= -1)
				servent_peer_num[servent_id]++;
					
				} 
				send_target = rand() % (servent_peer_num[servent_id]) ;
				while(send_target==servent_id){
    
				send_target = rand() % (servent_peer_num[servent_id]) ;
				    
				}
				while(servent_transmit_times[servent_id][send_target]==1){
					sleep(0.1);
				
				}
				while(servent_transmit_times[send_target][servent_id]==1){
					sleep(0.1);
				
				}
				servent_tunnel_work_over[servent_id][send_target]=0;        
				servent_transmit_data[servent_id][send_target].from = servent_id;
				servent_transmit_data[servent_id][send_target].to = send_target;
				rc = pthread_create(&servent_receive[servent_id][send_target], NULL, servent_handle_receive_func, &servent_transmit_data[servent_id][send_target]);
				rc = pthread_create(&servent_send[servent_id][send_target], NULL, servent_handle_send_func, &servent_transmit_data[servent_id][send_target]);  
				servent_receive_tunnel_ready_signal[servent_id][send_target]=0 ;
				servent_send_tunnel_ready_signal[servent_id][send_target]=0;
				
				servent_transmit_times[servent_id][send_target]=1;
				
				
				
				servent_tunnel_work_over[send_target][servent_id]=0;        
				servent_transmit_data[send_target][servent_id].from = send_target;
				servent_transmit_data[send_target][servent_id].to = servent_id;
				rc = pthread_create(&servent_receive[send_target][servent_id], NULL, servent_handle_receive_func, &servent_transmit_data[send_target][servent_id]);
				rc = pthread_create(&servent_send[send_target][servent_id], NULL, servent_handle_send_func, &servent_transmit_data[send_target][servent_id]);  
				servent_receive_tunnel_ready_signal[send_target][servent_id]=0 ;
				servent_send_tunnel_ready_signal[send_target][servent_id]=0;

				
				
				servent_transmit_times[send_target][servent_id]=1;
				
				strcpy(servent_send_message[servent_id],"*1");//
				printf("servent %ld , transmit_data: %s , target:%d", servent_id,servent_send_message[servent_id],send_target);
				
				
	     			
	     			
	     			
	     			puts("");
	     			
	     			
	     					
	     			while(servent_receive_tunnel_ready_signal[servent_id][send_target] ==1 ){
					pthread_cond_wait(&servent_receive_butter_empty[servent_id][send_target],&mutex[servent_id][send_target]);	
				}
						
	     					
	     			strcpy(servent_receive_message[servent_id][send_target], servent_send_message[servent_id]);//
	     					
	     			servent_receive_tunnel_ready_signal[servent_id][send_target] = 1 ;
	     			
	     					
	     			
	     			while(servent_transmit_times[servent_id][send_target] != 0  || servent_transmit_times[send_target][servent_id] != 0){
	     			
				sleep(0.1);
				}
				
				
				
				if(servent_transmit_times[servent_id][send_target]== 0 && servent_transmit_times[send_target][servent_id]==0){
				
				
				servent_tunnel_work_over[servent_id][send_target]=1;
				servent_tunnel_work_over[send_target][servent_id]=1;
						
				
				}
	     			servent_command[servent_id]=99;
			 	
				break;
			case 3:
				servent_peer_num[servent_id]=0;
				for(i=0;i<NUM_SERVENT_BOTS;i++){
				
				if(servent_peer_list[servent_id][i]!= -1)
				servent_peer_num[servent_id]++;
					
				} 
				send_target = rand() % (servent_peer_num[servent_id]) ;
				while(send_target==servent_id){
    
				send_target = rand() % (servent_peer_num[servent_id]) ;
				    
				}
				while(servent_transmit_times[servent_id][send_target]==1){
					sleep(0.1);
				
				}
				while(servent_transmit_times[send_target][servent_id]==1){
					sleep(0.1);
				
				}
				servent_tunnel_work_over[servent_id][send_target]=0;        
				servent_transmit_data[servent_id][send_target].from = servent_id;
				servent_transmit_data[servent_id][send_target].to = send_target;
				rc = pthread_create(&servent_receive[servent_id][send_target], NULL, servent_handle_receive_func, &servent_transmit_data[servent_id][send_target]);
				rc = pthread_create(&servent_send[servent_id][send_target], NULL, servent_handle_send_func, &servent_transmit_data[servent_id][send_target]);  
				servent_receive_tunnel_ready_signal[servent_id][send_target]=0 ;
				servent_send_tunnel_ready_signal[servent_id][send_target]=0;
				
				servent_transmit_times[servent_id][send_target]=1;
				
				
				
				servent_tunnel_work_over[send_target][servent_id]=0;        
				servent_transmit_data[send_target][servent_id].from = send_target;
				servent_transmit_data[send_target][servent_id].to = servent_id;
				rc = pthread_create(&servent_receive[send_target][servent_id], NULL, servent_handle_receive_func, &servent_transmit_data[send_target][servent_id]);
				rc = pthread_create(&servent_send[send_target][servent_id], NULL, servent_handle_send_func, &servent_transmit_data[send_target][servent_id]);  
				servent_receive_tunnel_ready_signal[send_target][servent_id]=0 ;
				servent_send_tunnel_ready_signal[send_target][servent_id]=0;

				
				
				servent_transmit_times[send_target][servent_id]=1;
				
				strcpy(servent_send_message[servent_id],"*2");//
				printf("servent %ld , transmit_data: %s , target:%d", servent_id,servent_send_message[servent_id],send_target);
				
				
	     			
	     			
	     			
	     			puts("");
	     			
	     			
	     					
	     			while(servent_receive_tunnel_ready_signal[servent_id][send_target] ==1 ){
					pthread_cond_wait(&servent_receive_butter_empty[servent_id][send_target],&mutex[servent_id][send_target]);	
				}
						
	     					
	     			strcpy(servent_receive_message[servent_id][send_target], servent_send_message[servent_id]);//
	     					
	     			servent_receive_tunnel_ready_signal[servent_id][send_target] = 1 ;
	     			
	     					
	     			
	     			while(servent_transmit_times[servent_id][send_target] != 0  || servent_transmit_times[send_target][servent_id] != 0){
	     			
				sleep(0.1);
				}
				
				
				
				if(servent_transmit_times[servent_id][send_target]== 0 && servent_transmit_times[send_target][servent_id]==0){
				
				
				servent_tunnel_work_over[servent_id][send_target]=1;
				servent_tunnel_work_over[send_target][servent_id]=1;
						
				
				}
	     			servent_command[servent_id]=99;
				break;	
				
			case 4:	
			        
			        
			        servent_peer_num[servent_id]=0;
				for(i=0;i<NUM_SERVENT_BOTS;i++){
				
				if(servent_peer_list[servent_id][i]!= -1)
				servent_peer_num[servent_id]++;
					
				} 
				send_target = rand() % (servent_peer_num[servent_id]) ;
				while(send_target==servent_id){
    
				send_target = rand() % (servent_peer_num[servent_id]) ;
				    
				}
				while(servent_transmit_times[servent_id][send_target]==1){
					sleep(0.1);
				
				}
				while(servent_transmit_times[send_target][servent_id]==1){
					sleep(0.1);
				
				}
				servent_tunnel_work_over[servent_id][send_target]=0;        
				servent_transmit_data[servent_id][send_target].from = servent_id;
				servent_transmit_data[servent_id][send_target].to = send_target;
				rc = pthread_create(&servent_receive[servent_id][send_target], NULL, servent_handle_receive_func, &servent_transmit_data[servent_id][send_target]);
				rc = pthread_create(&servent_send[servent_id][send_target], NULL, servent_handle_send_func, &servent_transmit_data[servent_id][send_target]);  
				servent_receive_tunnel_ready_signal[servent_id][send_target]=0 ;
				servent_send_tunnel_ready_signal[servent_id][send_target]=0;
				
				servent_transmit_times[servent_id][send_target]=1;
				
				
				
				servent_tunnel_work_over[send_target][servent_id]=0;        
				servent_transmit_data[send_target][servent_id].from = send_target;
				servent_transmit_data[send_target][servent_id].to = servent_id;
				rc = pthread_create(&servent_receive[send_target][servent_id], NULL, servent_handle_receive_func, &servent_transmit_data[send_target][servent_id]);
				rc = pthread_create(&servent_send[send_target][servent_id], NULL, servent_handle_send_func, &servent_transmit_data[send_target][servent_id]);  
				servent_receive_tunnel_ready_signal[send_target][servent_id]=0 ;
				servent_send_tunnel_ready_signal[send_target][servent_id]=0;

				
				
				servent_transmit_times[send_target][servent_id]=1;
				
				strcpy(servent_send_message[servent_id],"*3");//
				printf("servent %ld , transmit_data: %s , target:%d", servent_id,servent_send_message[servent_id],send_target);
				
				
	     			
	     			
	     			
	     			puts("");
	     			
	     			
	     					
	     			while(servent_receive_tunnel_ready_signal[servent_id][send_target] ==1 ){
					pthread_cond_wait(&servent_receive_butter_empty[servent_id][send_target],&mutex[servent_id][send_target]);	
				}
						
	     					
	     			strcpy(servent_receive_message[servent_id][send_target], servent_send_message[servent_id]);//
	     					
	     			servent_receive_tunnel_ready_signal[servent_id][send_target] = 1 ;
	     			
	     					
	     			
	     			while(servent_transmit_times[servent_id][send_target] != 0  || servent_transmit_times[send_target][servent_id] != 0){
	     			
				sleep(0.1);
				}
				
				
				
				if(servent_transmit_times[servent_id][send_target]== 0 && servent_transmit_times[send_target][servent_id]==0){
				
				
				servent_tunnel_work_over[servent_id][send_target]=1;
				servent_tunnel_work_over[send_target][servent_id]=1;
						
				
				}
	     			servent_command[servent_id]=99;
				break;	
			
			case (-1):
				printf(" servent %ld terminated !\n", servent_id);
				
				break;	
		}
		sleep(1);
		
		
	

}
void client_func(long client_id){


    int rc,a,b,i,j,send_target;
    
  
		if(client_work_over[client_id] == 1){
			return;
		}
		switch(client_command[client_id]) {
			case 0:
				
				
				
				client_command[client_id]=99;
				break;
			case 1:
				
				client_command[client_id]=99;
				
				break;
			case 2:
				

				
				//send_target = client_master[client_id][client_id] ;
				
				while(client_transmit_times[client_id][send_target]==1){
					sleep(0.1);
				
				}
				
				client_tunnel_work_over[client_id][send_target]=0;        
				client_transmit_data[client_id][send_target].from = client_id;
				client_transmit_data[client_id][send_target].to = send_target;
				rc = pthread_create(&client_receive[client_id][send_target], NULL, client_handle_receive_func, &client_transmit_data[client_id][send_target]);
				rc = pthread_create(&client_send[client_id][send_target], NULL, client_handle_send_func, &client_transmit_data[client_id][send_target]);  
				client_receive_tunnel_ready_signal[client_id][send_target]=0 ;
				client_send_tunnel_ready_signal[client_id][send_target]=0;
				
				client_transmit_times[client_id][send_target]=1;
				
				while(client_transmit_times[send_target][client_id]==1){
					sleep(0.1);
				
				}
				client_tunnel_work_over[send_target][client_id]=0;        
				client_transmit_data[send_target][client_id].from = send_target;
				client_transmit_data[send_target][client_id].to = client_id;
				rc = pthread_create(&client_receive[send_target][client_id], NULL, client_handle_receive_func, &client_transmit_data[send_target][client_id]);
				rc = pthread_create(&client_send[send_target][client_id], NULL, client_handle_send_func, &client_transmit_data[send_target][client_id]);  
				client_receive_tunnel_ready_signal[send_target][client_id]=0 ;
				client_send_tunnel_ready_signal[send_target][client_id]=0;

				
				
				client_transmit_times[send_target][client_id]=1;
				
				
				strcpy(client_send_message[client_id],"*2");//
				printf("client %ld , transmit_data: %s , target:%d", client_id,client_send_message[client_id],send_target);
				
				
	     			
	     			
	     			
	     			puts("");
	     			
	     			
	     					
	     			while(client_receive_tunnel_ready_signal[client_id][send_target] ==1 ){
					pthread_cond_wait(&client_receive_butter_empty[client_id][send_target],&mutex[client_id][send_target]);	
				}
						
	     					
	     			strcpy(client_receive_message[client_id][send_target], client_send_message[client_id]);//
	     					
	     			client_receive_tunnel_ready_signal[client_id][send_target] = 1 ;
	     			
	     					
	     			
	     			while(client_transmit_times[client_id][send_target] != 0  || client_transmit_times[send_target][client_id] != 0){
	     			
				sleep(0.1);
				}
				
				
				
				if(client_transmit_times[client_id][send_target]== 0 && client_transmit_times[send_target][client_id]==0){
				
				
				client_tunnel_work_over[client_id][send_target]=1;
				client_tunnel_work_over[send_target][client_id]=1;
						
				
				}
	     			
	     			
	     			
			 	
			 	client_command[client_id]=99;
			 	
				break;
			case 3:
				
	     			client_command[client_id]=99;
				break;	
				
				
			
			case (-1):
				printf(" client %ld terminated !\n", client_id);
				
				break;	
		}
		
		
		
	

}
void *servent_thread_func(void *threadid) {// 1 thread = 4 bots
    long tid;
    int i,j;
    int rc;
    
    tid = (long)threadid;
    
    while(servent_thread_work_over[tid] != 1){
		
		sleep(0.5);
		servent_func(4*tid);
		servent_func(4*tid+1);
		servent_func(4*tid+2);
		servent_func(4*tid+3);
	
	}
	
    printf(" servent_thread %ld terminated !\n", tid);
    pthread_exit(NULL);
}
void *client_thread_func(void *threadid) {// 1 thread = 4 bots
    long tid;
    int i,j;
    int rc;
    
    tid = (long)threadid;
    
    while(client_thread_work_over[tid] != 1){
		
		sleep(0.5);
		client_func(4*tid);
		client_func(4*tid+1);
		client_func(4*tid+2);
		client_func(4*tid+3);
	
	}
	
    printf(" client_thread %ld terminated !\n", tid);
    pthread_exit(NULL);
}
void program_over(int signal){
    int i=0;
    int j=0;
    if(signal==1){
    
	    for (i = 0; i < NUM_BOTS; i++) {
					
		servent_work_over[i]=1;
		for (j = 0; j < NUM_SERVENT_BOTS; j++){
			servent_tunnel_work_over[i][j]=1;
		}		
	    }
	    for (i = 0; i < (NUM_SERVENT_BOTS/4); i++) {
					
		servent_thread_work_over[i]=1;
	    	
	    }		
	    
	    socialnetwork_terminate_signal=1;	
    }

}
void init_servent_peer_list(){
	
    int i,j,a;
    for (i = 0; i < NUM_SERVENT_BOTS; i++) {
    for (j = 0; j < NUM_SERVENT_BOTS-1; j++) {
    
    
    servent_peer_list[i][j] = rand() % NUM_SERVENT_BOTS  ;
    
    
    while(servent_peer_list[i][j]==i){
    
    servent_peer_list[i][j] = rand() % NUM_SERVENT_BOTS ;
    
    }
    
    for (a = 0; a < j; a++) {
    
    while(servent_peer_list[i][a]==servent_peer_list[i][j]){
    
    servent_peer_list[i][j] = rand() % NUM_SERVENT_BOTS  ;
    
    while(servent_peer_list[i][j]==i){
    
    servent_peer_list[i][j] = rand() % NUM_SERVENT_BOTS ;
    a = 0;
    }
    a = 0;
    }
    
    }
    servent_peer_num[i]= (NUM_SERVENT_BOTS);
    
   
    }
    
    }
    
    for(i=0;i<NUM_SERVENT_BOTS;i++){
    puts("");
    printf("peer list of thread %ld have:", i);
    for (j = 0; j < NUM_SERVENT_BOTS; j++) {
    
    
    if(servent_peer_list[i][j]!= -1)
    	printf("%ld ", servent_peer_list[i][j]);
    
    }	
    
    } 
    puts("");

}
void init_servent_client_website(){
    int j=0,i=0;
    FILE* f;
    char website[10][1024];
    f = fopen("website.txt" , "r");
    if(!f){
    printf("data not exist");
    system("PAUSE");
    pthread_exit(NULL);
    }
    while(fgets(website[i], 1024, f) != NULL ){
		    
		    
    printf("website[%d]:\n",i);
    puts(website[i]);
    
    for(j=0;j<NUM_SERVENT_BOTS;j++){
    //printf("website in servent %d buffer:\n",j);
    strcpy(servent_bot_website_buffer[0][i], website[i]);//
    //puts(servent_bot_website_buffer[j][i]);
    }
    
    i++;
    }

    fclose(f);

}
int main() {
	
    srand(time(NULL) );
    pthread_t servent_threads[NUM_SERVENT_BOTS/4];
    
    pthread_t socialnetwork;
    int rc,return_data;
    int check=1;
    char data[1024]; 

    int master_command=-1;
   
    int i=0;
    int j=0;
    int a,b;
    long t;
    //FILE* f;

    rc = pthread_create(&socialnetwork, NULL, socialnetwork_func, NULL);  
    if (rc) {
            printf("ERORR; return code from pthread_create() is %d\n", rc);
            exit(EXIT_FAILURE);
    }
    
    for (t = 0; t < NUM_SERVENT_BOTS; t++) {
    	servent_command[t]=99;
        servent_bot_command_buffer_pointer[t]=0;
        
        for (i = 0; i < NUM_SERVENT_BOTS; i++){
		
		servent_peer_list[t][i] = -1;
		servent_tunnel_work_over[t][i]=1; 
		
	}

        
  
     }
     for (t = 0; t < (NUM_SERVENT_BOTS/4); t++) {
 	rc = pthread_create(&servent_threads[t], NULL, servent_thread_func, (void *)t);
        
        if (rc) {
            printf("ERORR; return code from pthread_create() is %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }
    
   
    init_servent_peer_list();
    init_servent_client_website();
   
    




					

    while(master_command != 0 ){
    	
    	sleep(1);
    	
    	printf("enter command \n");
    	printf("0:Exit 1.fetch command from social network  2:fetch command from peer list  3: request peer list from peer  4:request website from peer \n");
    		
    		
    		scanf("%d",&master_command);
    		
		
		if(master_command == 0 ) {
			program_over(1);
			break;
		}
		sleep(0.8);
		
		switch(master_command) {
			
		    	
			case 1:	
					
				
				for (t = 0; t < NUM_SERVENT_BOTS; t++) {							
					servent_command[0]=master_command;	
						
					}
				break;
				
			case 2:
					
				for (t = 0; t < NUM_SERVENT_BOTS; t++) {							
					
					servent_command[t]=master_command;	
				}
				break;				
			case 3:
				
					
				for (t = 0; t < NUM_SERVENT_BOTS; t++) {							
					servent_command[t]=master_command;		
						
					}	
				break;
			case 4:
				
					
				for (t = 0; t < NUM_SERVENT_BOTS; t++) {							
					servent_command[t]=master_command;		
						
					}	
				break;	
			default:
				sleep(10);
				break;
				
	    	
		}
		
		
		
		
		
		sleep(0.8);
		
    
	}
	for (t = 0; t < (NUM_SERVENT_BOTS/4); t++){
		pthread_join(servent_threads[t],NULL);	
	}
    
    
    pthread_exit(NULL);
	
    return 0; 
    
}
