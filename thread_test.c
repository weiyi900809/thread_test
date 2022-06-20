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
#define NUM_CLIENT_BOTS 8
#define NUM_BOTS 4


typedef struct  servent_transmit{

long from;
long to;
	
}Servent_Transmit; 
typedef struct  client_transmit{

long servent_id;
long client_id;
	
}Client_Transmit; 
typedef struct  bot{

long id;
char ip[1024];
long port;
	
}Bot; 

Bot servent[NUM_SERVENT_BOTS];
Bot client[NUM_CLIENT_BOTS];  

Servent_Transmit servent_transmit_data[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
Client_Transmit client_transmit_data[NUM_CLIENT_BOTS][NUM_SERVENT_BOTS];
int servent_thread_work_over[NUM_SERVENT_BOTS/4];
int client_thread_work_over[NUM_SERVENT_BOTS/4];
char file_data[10][10][1024]; 
char already_exist_ip[NUM_CLIENT_BOTS+NUM_SERVENT_BOTS][1024];
int already_exist_ip_num=0;
int already_exist_port[NUM_CLIENT_BOTS+NUM_SERVENT_BOTS];
int already_exist_port_num=0;
int relay_station_terminate_signal=0;
//servent----------------------------------
//-----------------------------------------
int servent_pattern[NUM_SERVENT_BOTS]; 
int servent_send_command[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS]; 
char servent_bot_command_buffer[NUM_SERVENT_BOTS][10][1024]; 
int servent_bot_command_buffer_pointer[NUM_SERVENT_BOTS];
char servent_bot_website_buffer[NUM_SERVENT_BOTS][10][1024]; 
int servent_bot_website_buffer_pointer[NUM_SERVENT_BOTS];
//-----------------------------------------
int servent_transmit_tunnel_ready_signal[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
char servent_send_message[NUM_SERVENT_BOTS][1024];
char servent_receive_message[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS][1024];
int servent_tunnel_work_over[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
int servent_work_over[NUM_SERVENT_BOTS];
//-----------------------------------------
int servent_peer_list[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
int servent_peer_num[NUM_SERVENT_BOTS];
//-----------------------------------------
/*int servent_client_list[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
int servent_client_num[NUM_SERVENT_BOTS];*/
//client-----------------------------------
//-----------------------------------------
int client_pattern[NUM_CLIENT_BOTS]; 
int client_send_command[NUM_CLIENT_BOTS][NUM_CLIENT_BOTS]; 
char client_bot_command_buffer[NUM_CLIENT_BOTS][10][1024]; 
int client_bot_command_buffer_pointer[NUM_CLIENT_BOTS];
char client_bot_website_buffer[NUM_CLIENT_BOTS][10][1024]; 
int client_bot_website_buffer_pointer[NUM_CLIENT_BOTS];
//-----------------------------------------

int servent_to_client_tunnel_ready_signal[NUM_CLIENT_BOTS][NUM_SERVENT_BOTS];
int client_to_servent_tunnel_ready_signal[NUM_CLIENT_BOTS][NUM_SERVENT_BOTS];
char client_send_message[NUM_CLIENT_BOTS][1024];
char client_receive_message[NUM_CLIENT_BOTS][NUM_SERVENT_BOTS][1024];
char servent_receive_message_from_client[NUM_CLIENT_BOTS][NUM_SERVENT_BOTS][1024];
int client_and_servent_tunnel_work_over[NUM_CLIENT_BOTS][NUM_SERVENT_BOTS];
int client_work_over[NUM_CLIENT_BOTS];
//-----------------------------------------
int client_master[NUM_CLIENT_BOTS][NUM_SERVENT_BOTS];
int client_master_num[NUM_CLIENT_BOTS];



pthread_mutex_t mutex[NUM_BOTS][NUM_BOTS];

pthread_t servent_transmit[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];

pthread_t client_receive[NUM_CLIENT_BOTS][NUM_SERVENT_BOTS];
pthread_t servent_handle_client_message[NUM_CLIENT_BOTS][NUM_SERVENT_BOTS];


pthread_cond_t  servent_receive_butter_empty[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];//signal of  buffer is empty 
pthread_cond_t  client_receive_butter_empty[NUM_CLIENT_BOTS][NUM_SERVENT_BOTS];//signal of  buffer is empty 


int servent_transmit_times[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];

int client_receive_times[NUM_CLIENT_BOTS][NUM_SERVENT_BOTS];
int servent_receive_times_from_client[NUM_CLIENT_BOTS][NUM_SERVENT_BOTS];



char make_peer_list_message(char message[],int tid){

   //char message[50];
   char text[50];
   
   int i;
 
   
   sprintf(message, "I am servent %d my peer list have: ", tid);   
  
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
   
   	


void *relay_station_func(){
    long time_counter =5;
    char path[1024];

    int i=0,j=0,f=1;
    FILE* file;
    printf("Hello There! I am relay station read command/%d sec\n",time_counter);
    while(relay_station_terminate_signal != 1){
    	

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
		    
		    
		    /*printf("command%d from botmaster:",f);
		    puts(file_data[i][j]);*/
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
    printf(" relay_station terminated !\n");
    pthread_exit(NULL);
	
}

void *servent_handle_transmit_func(void *transmit_information){

    Servent_Transmit *information;
    information = (Servent_Transmit *)transmit_information;
   
    
    int i,j,b,command_comparison_result=1,website_comparison_result=1;
   
    int behavior_request=0;
    char func_message[1024];
    char message[1024];
    
    
    while(servent_tunnel_work_over[information->from][information->to] != 1){
    	
	
	if(servent_transmit_tunnel_ready_signal[information->from][information->to] == 1 ){
			
	printf("servent %ld  ip:%s port:%d receive message from servent %ld ip:%s port:%d: %s\n", information->to , servent[information->to].ip,servent[information->to].port,information->from, 
	servent[information->from].ip,servent[information->from].port, servent_receive_message[information->from][information->to]);
	
	strcpy(func_message,servent_receive_message[information->from][information->to]);
	
	
	if(strcmp(func_message,"return peer list") == 0 ){
	make_peer_list_message(func_message,information->from);
	puts(func_message);
	
	
	
	
	
	}
	
	
	if(strcmp(func_message,"return botmaster command") == 0 ){

	b=0;
		
	printf("command from servent %ld:\n",information->from);
	while(servent_bot_command_buffer[information->from][b][0]!= '\0'){
				
	for(i=0;i<servent_bot_command_buffer_pointer[information->to];i++){	
	command_comparison_result=strcmp(servent_bot_command_buffer[information->to][i],servent_bot_command_buffer[information->from][b]);
	if(command_comparison_result== 0){break;}
	}
				
	if(command_comparison_result== 0){
	printf("%s servent %ld already have this command\n",servent_bot_command_buffer[information->from][b],information->to);
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
	printf("%s servent %ld already have this website\n",servent_bot_website_buffer[information->from][b],information->to);
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
					
					strcpy(message,"return botmaster command");
			
		     			while(servent_transmit_tunnel_ready_signal[information->to][information->from] ==1 ){
		     		
		     		
		     			pthread_cond_wait(&servent_receive_butter_empty[information->to][information->from],&mutex[information->to][information->from]);	
		     			}
							
		     			
		     			strcpy(servent_receive_message[information->to][information->from], message);
		     	
		     			servent_transmit_tunnel_ready_signal[information->to][information->from] =1 ;

					break;
				case 2:
					
					
				 	
					strcpy(message,"return peer list");	
		     					
		     			while(servent_transmit_tunnel_ready_signal[information->to][information->from] ==1 ){
		     		
		     		
		     			pthread_cond_wait(&servent_receive_butter_empty[information->to][information->from],&mutex[information->to][information->from]);	
		     			}
							
		     			
		     			strcpy(servent_receive_message[information->to][information->from], message);
		     	
		     			servent_transmit_tunnel_ready_signal[information->to][information->from] =1 ;
					break;
				case 3:
					
					
				 	strcpy(message,"return website");
			
		     			while(servent_transmit_tunnel_ready_signal[information->to][information->from] ==1 ){
		     		
		     		
		     			pthread_cond_wait(&servent_receive_butter_empty[information->to][information->from],&mutex[information->to][information->from]);	
		     			}
							
		     			
		     			strcpy(servent_receive_message[information->to][information->from], message);
		     	
		     			servent_transmit_tunnel_ready_signal[information->to][information->from] =1 ;
				 	
					break;	
					
				case 0:
					break;	
				
		}	 
    		
    	}
    	

	
	behavior_request=0;				
	strcpy(servent_receive_message[information->from][information->to],"");
	strcpy(func_message,"");
	servent_transmit_tunnel_ready_signal[information->from][information->to] = 0 ; 
	pthread_cond_signal(&servent_receive_butter_empty[information->from][information->to]);
	servent_transmit_times[information->from][information->to]=0;
	//printf("receive:servent_transmit_times[%d][%d] :%d\n",information->to,information->from,servent_transmit_times[information->from][information->to]);
	
	}
	
	
	
	
 
    }
    
    //printf(" servent_handle_transmit_func %ld-%ld terminated !\n", information->from,information->to);
    pthread_exit(NULL);	

}


void *client_handle_receive_func(void *transmit_information){

    Client_Transmit *information;
    information = (Client_Transmit *)transmit_information;
    
    int i,j,b,command_comparison_result=1,website_comparison_result=1;
   
    
    char func_message[1024];
    char message[1024];
    
    
    while(client_and_servent_tunnel_work_over[information->client_id][information->servent_id] != 1){
    	
	
	if(servent_to_client_tunnel_ready_signal[information->client_id][information->servent_id] == 1 ){
			
	printf("client %ld ip:%s port:%d receive message from servent %ld ip:%s port:%d : %s\n", information->client_id , client[information->client_id].ip, client[information->client_id].port, 
	information->servent_id,  servent[information->servent_id].ip,servent[information->servent_id].port, client_receive_message[information->client_id][information->servent_id]);
	
	strcpy(func_message,client_receive_message[information->client_id][information->servent_id]);
	
	if(strcmp(func_message,"return peer list") == 0 ){
	make_peer_list_message(func_message,information->servent_id);
	puts(func_message);

	
	}
	
	
	if(strcmp(func_message,"return botmaster command") == 0 ){

	b=0;
		
	printf("command from servent %ld:\n",information->servent_id);
	
	while(servent_bot_command_buffer[information->servent_id][b][0]!= '\0'){
				
	for(i=0;i<client_bot_command_buffer_pointer[information->client_id];i++){	
	command_comparison_result=strcmp(client_bot_command_buffer[information->client_id][i],servent_bot_command_buffer[information->servent_id][b]);
	if(command_comparison_result== 0){break;}
	}
				
	if(command_comparison_result== 0){
	printf("%s client %ld already have this command\n",servent_bot_command_buffer[information->servent_id][b],information->client_id);
	}
	if(command_comparison_result != 0){
	strcpy(client_bot_command_buffer[information->client_id][client_bot_command_buffer_pointer[information->client_id]],servent_bot_command_buffer[information->servent_id][b]);
	puts(client_bot_command_buffer[information->client_id][client_bot_command_buffer_pointer[information->client_id]]);
	client_bot_command_buffer_pointer[information->client_id]++;
	}				
	b++;			
	}
	}
	
	
	if(strcmp(func_message,"return website") == 0 ){
	b=0;
		
	printf("website from servent %ld:\n",information->servent_id);
	while(servent_bot_website_buffer[information->servent_id][b][0]!= '\0'){
				
	for(i=0;i<client_bot_website_buffer_pointer[information->client_id];i++){	
	website_comparison_result=strcmp(client_bot_website_buffer[information->client_id][i],servent_bot_website_buffer[information->servent_id][b]);
	if(website_comparison_result== 0){break;}
	}
				
	if(website_comparison_result== 0){
	printf("%s client %ld already have this website\n",servent_bot_website_buffer[information->servent_id][b],information->client_id);
	}
	if(website_comparison_result != 0){
	strcpy(client_bot_website_buffer[information->client_id][client_bot_website_buffer_pointer[information->client_id]],servent_bot_website_buffer[information->servent_id][b]);
	puts(client_bot_website_buffer[information->client_id][client_bot_website_buffer_pointer[information->client_id]]);
	client_bot_website_buffer_pointer[information->client_id]++;
	}				
	b++;			
	}

	}
	
	
	
					
	strcpy(client_receive_message[information->client_id][information->servent_id],"");
	strcpy(func_message,"");
	servent_to_client_tunnel_ready_signal[information->client_id][information->servent_id] = 0 ; 
	//pthread_cond_signal(&client_receive_butter_empty[information->client_id][information->servent_id]);
	client_receive_times[information->client_id][information->servent_id]=0;
	//printf("receive:servent_transmit_times[%d][%d] :%d\n",information->to,information->servent_id,servent_transmit_times[information->client_id][information->servent_id]);
	
	}
	
	
	
	
 
    }
    
    //printf(" client_handle_receive_func %ld-%ld terminated !\n", information->client_id,information->servent_id);
    pthread_exit(NULL);	

}
void *servent_handle_client_message_func(void *transmit_information){ 

    Client_Transmit *information;
    information = (Client_Transmit *)transmit_information;
    
    
    int i,j,b,command_comparison_result=1,website_comparison_result=1;
   
    int behavior_request=0;
    char func_message[1024];
    char message[1024];
    
    
    while(client_and_servent_tunnel_work_over[information->client_id][information->servent_id] != 1){
    	
	
	if(client_to_servent_tunnel_ready_signal[information->client_id][information->servent_id] == 1 ){
			
	printf("servent %ld ip:%s port:%d receive message from client %ld ip:%s port:%d : %s\n", information->servent_id , servent[information->servent_id].ip,servent[information->servent_id].port,
	information->client_id, client[information->client_id].ip, client[information->client_id].port, servent_receive_message_from_client[information->client_id][information->servent_id]);//
	
	strcpy(func_message,servent_receive_message_from_client[information->client_id][information->servent_id]);//
	
	if(func_message[0] == '*' ){
	
		behavior_request = func_message[1]-48;
		
	    	switch(behavior_request) {
				
				case 1:
					
					strcpy(message,"return botmaster command");
				
		     			
		     			strcpy(client_receive_message[information->client_id][information->servent_id], message);
		     	
		     			servent_to_client_tunnel_ready_signal[information->client_id][information->servent_id] =1 ;

					break;
				case 2:
					
					
				 	strcpy(message,"return peer list");
						
		     					
		     			
		     			
		     			strcpy(client_receive_message[information->client_id][information->servent_id], message);
		     	
		     			servent_to_client_tunnel_ready_signal[information->client_id][information->servent_id] =1 ;
					break;
				case 3:
					
					
				 	strcpy(message,"return website");
			
		     				
		     			
		     			strcpy(client_receive_message[information->client_id][information->servent_id], message);
		     	
		     			servent_to_client_tunnel_ready_signal[information->client_id][information->servent_id] =1 ;
				 	
					break;	
					
				case 0:
					break;	
				
		}	 
    		
    	}
	behavior_request=0;					
	strcpy(servent_receive_message_from_client[information->client_id][information->servent_id],"");
	strcpy(func_message,"");
	client_to_servent_tunnel_ready_signal[information->client_id][information->servent_id] = 0 ; 
	servent_receive_times_from_client[information->client_id][information->servent_id]=0;
	
	
	}
	
	
	
	
 
    }
    
    //printf(" servent_handle_client_message_func %ld-%ld terminated !\n", information->client_id,information->servent_id);
    pthread_exit(NULL);	

}
void client_func(long c_id){


    int rc,a,b,i,j,target_servent;
    
  
		if(client_work_over[c_id] == 1){
			return;
		}
		switch(client_pattern[c_id]) {
			case 0:
	
				client_pattern[c_id]=99;
				break;
			case 1:
				
				client_master_num[c_id]=0;
				for(i=0;i<NUM_CLIENT_BOTS;i++){
				
				if(client_master[c_id][i]!= -1)
				client_master_num[c_id]++;
					
				} 
				target_servent = rand() % (client_master_num[c_id]) ;
				
				
				
				while(client_receive_times[c_id][target_servent]==1){
					sleep(0.1);
				
				}
				while(servent_receive_times_from_client[c_id][target_servent]==1){
					sleep(0.1);
				
				}
				client_and_servent_tunnel_work_over[c_id][target_servent]=0;        
				client_transmit_data[c_id][target_servent].client_id = c_id;
				client_transmit_data[c_id][target_servent].servent_id = target_servent;
				 
				rc = pthread_create(&client_receive[c_id][target_servent], NULL, client_handle_receive_func, &client_transmit_data[c_id][target_servent]);
				rc = pthread_create(&servent_handle_client_message[c_id][target_servent], NULL, servent_handle_client_message_func, &client_transmit_data[c_id][target_servent]);
				
				client_to_servent_tunnel_ready_signal[c_id][target_servent]=0 ;
				servent_to_client_tunnel_ready_signal[c_id][target_servent] = 0 ; 
				
				
				servent_receive_times_from_client[c_id][target_servent]=1;
				client_receive_times[c_id][target_servent]=1;
				
				strcpy(client_send_message[c_id],"*1");//
				printf("client %ld , send_data: %s , target_servent:%d", c_id,client_send_message[c_id],target_servent);

	     			puts("");
	
	     			/*while(client_and_servent_tunnel_ready_signal[c_id][target_servent] ==1 ){
					pthread_cond_wait(&client_receive_butter_empty[c_id][target_servent],&mutex[c_id][target_servent]);	
				}*/
	
	     			strcpy(servent_receive_message_from_client[c_id][target_servent], client_send_message[c_id]);//
	     					
	     			client_to_servent_tunnel_ready_signal[c_id][target_servent] = 1 ;

	     			while(client_receive_times[c_id][target_servent] != 0  || servent_receive_times_from_client[c_id][target_servent] != 0){
	     			
				sleep(0.1);
				}

				if(client_receive_times[c_id][target_servent]== 0 && servent_receive_times_from_client[c_id][target_servent]==0){

				client_and_servent_tunnel_work_over[c_id][target_servent]=1;
	
				}
	     			client_pattern[c_id]=99;
				
				break;
			case 2:

				client_master_num[c_id]=0;
				for(i=0;i<NUM_CLIENT_BOTS;i++){
				
				if(client_master[c_id][i]!= -1)
				client_master_num[c_id]++;
					
				} 
				target_servent = rand() % (client_master_num[c_id]) ;
				
				
				
				while(client_receive_times[c_id][target_servent]==1){
					sleep(0.1);
				
				}
				while(servent_receive_times_from_client[c_id][target_servent]==1){
					sleep(0.1);
				
				}
				client_and_servent_tunnel_work_over[c_id][target_servent]=0;        
				client_transmit_data[c_id][target_servent].client_id = c_id;
				client_transmit_data[c_id][target_servent].servent_id = target_servent;
				 
				rc = pthread_create(&client_receive[c_id][target_servent], NULL, client_handle_receive_func, &client_transmit_data[c_id][target_servent]);
				rc = pthread_create(&servent_handle_client_message[c_id][target_servent], NULL, servent_handle_client_message_func, &client_transmit_data[c_id][target_servent]);
				
				client_to_servent_tunnel_ready_signal[c_id][target_servent]=0 ;
				servent_to_client_tunnel_ready_signal[c_id][target_servent] = 0 ; 
				
				
				servent_receive_times_from_client[c_id][target_servent]=1;
				client_receive_times[c_id][target_servent]=1;
				
				strcpy(client_send_message[c_id],"*2");//
				printf("client %ld , send_data: %s , target_servent:%d", c_id,client_send_message[c_id],target_servent);

	     			puts("");
	
	     			/*while(client_and_servent_tunnel_ready_signal[c_id][target_servent] ==1 ){
					pthread_cond_wait(&client_receive_butter_empty[c_id][target_servent],&mutex[c_id][target_servent]);	
				}*/
	
	     			strcpy(servent_receive_message_from_client[c_id][target_servent], client_send_message[c_id]);//
	     					
	     			client_to_servent_tunnel_ready_signal[c_id][target_servent] = 1 ;

	     			while(client_receive_times[c_id][target_servent] != 0  || servent_receive_times_from_client[c_id][target_servent] != 0){
	     			
				sleep(0.1);
				}

				if(client_receive_times[c_id][target_servent]== 0 && servent_receive_times_from_client[c_id][target_servent]==0){

				client_and_servent_tunnel_work_over[c_id][target_servent]=1;
	
				}
	     			client_pattern[c_id]=99;
			 	
				break;
			case 3:
				
	     			client_master_num[c_id]=0;
				for(i=0;i<NUM_CLIENT_BOTS;i++){
				
				if(client_master[c_id][i]!= -1)
				client_master_num[c_id]++;
					
				} 
				target_servent = rand() % (client_master_num[c_id]) ;
				
				
				
				while(client_receive_times[c_id][target_servent]==1){
					sleep(0.1);
				
				}
				while(servent_receive_times_from_client[c_id][target_servent]==1){
					sleep(0.1);
				
				}
				client_and_servent_tunnel_work_over[c_id][target_servent]=0;        
				client_transmit_data[c_id][target_servent].client_id = c_id;
				client_transmit_data[c_id][target_servent].servent_id = target_servent;
				 
				rc = pthread_create(&client_receive[c_id][target_servent], NULL, client_handle_receive_func, &client_transmit_data[c_id][target_servent]);
				rc = pthread_create(&servent_handle_client_message[c_id][target_servent], NULL, servent_handle_client_message_func, &client_transmit_data[c_id][target_servent]);
				
				client_to_servent_tunnel_ready_signal[c_id][target_servent]=0 ;
				servent_to_client_tunnel_ready_signal[c_id][target_servent] = 0 ; 
				
				
				servent_receive_times_from_client[c_id][target_servent]=1;
				client_receive_times[c_id][target_servent]=1;
				
				strcpy(client_send_message[c_id],"*3");//
				printf("client %ld , send_data: %s , target_servent:%d", c_id,client_send_message[c_id],target_servent);

	     			puts("");
	
	     			/*while(client_and_servent_tunnel_ready_signal[c_id][target_servent] ==1 ){
					pthread_cond_wait(&client_receive_butter_empty[c_id][target_servent],&mutex[c_id][target_servent]);	
				}*/
	
	     			strcpy(servent_receive_message_from_client[c_id][target_servent], client_send_message[c_id]);//
	     					
	     			client_to_servent_tunnel_ready_signal[c_id][target_servent] = 1 ;

	     			while(client_receive_times[c_id][target_servent] != 0  || servent_receive_times_from_client[c_id][target_servent] != 0){
	     			
				sleep(0.1);
				}

				if(client_receive_times[c_id][target_servent]== 0 && servent_receive_times_from_client[c_id][target_servent]==0){

				client_and_servent_tunnel_work_over[c_id][target_servent]=1;
	
				}
	     			client_pattern[c_id]=99;
				break;	
				
				
			
			case (-1):
				printf(" client %ld terminated !\n", c_id);
				
				break;	
		}
		
		
		
	

}
void servent_func(long servent_id){


    int rc,a,b,i,j,send_target,command_comparison_result=1,result,file_num;
    char text[1024];
  
		if(servent_work_over[servent_id] == 1){
			return;
		}
		/*servent_pattern[servent_id] =  rand()  % 4+1;
		printf(" servent_pattern[%ld] = %d !\n", servent_id,servent_pattern[servent_id]);*/
		switch(servent_pattern[servent_id]) {
			case 0:
				
				
				servent_pattern[servent_id]=99;
				break;
			case 1:
				file_num=rand()  % 3;
				switch(file_num){
				case 0:
					a=0;
					b=0;
					
					printf("servent %ld receive command%d from relay station:\n",servent_id,a+1);
					while(file_data[a][b][0]!= '\0'){
					
					for(i=0;i<servent_bot_command_buffer_pointer[servent_id];i++){	
					command_comparison_result=strcmp(servent_bot_command_buffer[servent_id][i],file_data[a][b]);
					if(command_comparison_result== 0){break;}
					}
					
					if(command_comparison_result== 0){
					printf("servent %ld already have this command\n",servent_id);
					
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
					
					printf("servent %ld receive command%d from relay station:\n",servent_id,a+1);
					while(file_data[a][b][0]!= '\0'){
					
					for(i=0;i<servent_bot_command_buffer_pointer[servent_id];i++){	
					command_comparison_result=strcmp(servent_bot_command_buffer[servent_id][i],file_data[a][b]);
					if(command_comparison_result== 0){break;}
					}
					
					if(command_comparison_result== 0){
					printf("servent %ld already have this command\n",servent_id);
					
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
					printf("servent %ld receive command%d from relay station:\n",servent_id,a+1);
					while(file_data[a][b][0]!= '\0'){
					
					for(i=0;i<servent_bot_command_buffer_pointer[servent_id];i++){	
					command_comparison_result=strcmp(servent_bot_command_buffer[servent_id][i],file_data[a][b]);
					if(command_comparison_result== 0){break;}
					}
					
					if(command_comparison_result== 0){
					printf("servent %ld already have this command\n",servent_id);
					
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
				
				
				servent_pattern[servent_id]=99;
				
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
				rc = pthread_create(&servent_transmit[servent_id][send_target], NULL, servent_handle_transmit_func, &servent_transmit_data[servent_id][send_target]);
				servent_transmit_tunnel_ready_signal[servent_id][send_target]=0 ;
				
				
				servent_transmit_times[servent_id][send_target]=1;
				
				
				
				servent_tunnel_work_over[send_target][servent_id]=0;        
				servent_transmit_data[send_target][servent_id].from = send_target;
				servent_transmit_data[send_target][servent_id].to = servent_id;
				rc = pthread_create(&servent_transmit[send_target][servent_id], NULL, servent_handle_transmit_func, &servent_transmit_data[send_target][servent_id]);
				servent_transmit_tunnel_ready_signal[send_target][servent_id]=0 ;

				
				
				servent_transmit_times[send_target][servent_id]=1;
				
				strcpy(servent_send_message[servent_id],"*1");//
				printf("servent %ld , send_data: %s , target servent:%d", servent_id,servent_send_message[servent_id],send_target);
				
				
	     			
	     			
	     			
	     			puts("");
	     			
	     			
	     					
	     			while(servent_transmit_tunnel_ready_signal[servent_id][send_target] ==1 ){
					pthread_cond_wait(&servent_receive_butter_empty[servent_id][send_target],&mutex[servent_id][send_target]);	
				}
						
	     					
	     			strcpy(servent_receive_message[servent_id][send_target], servent_send_message[servent_id]);//
	     					
	     			servent_transmit_tunnel_ready_signal[servent_id][send_target] = 1 ;
	     			
	     					
	     			
	     			while(servent_transmit_times[servent_id][send_target] != 0  || servent_transmit_times[send_target][servent_id] != 0){
	     			
				sleep(0.1);
				}
				
				
				
				if(servent_transmit_times[servent_id][send_target]== 0 && servent_transmit_times[send_target][servent_id]==0){
				
				
				servent_tunnel_work_over[servent_id][send_target]=1;
				servent_tunnel_work_over[send_target][servent_id]=1;
						
				
				}
	     			servent_pattern[servent_id]=99;
			 	
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
				rc = pthread_create(&servent_transmit[servent_id][send_target], NULL, servent_handle_transmit_func, &servent_transmit_data[servent_id][send_target]); 
				servent_transmit_tunnel_ready_signal[servent_id][send_target]=0 ;
				
				servent_transmit_times[servent_id][send_target]=1;
				
				
				
				servent_tunnel_work_over[send_target][servent_id]=0;        
				servent_transmit_data[send_target][servent_id].from = send_target;
				servent_transmit_data[send_target][servent_id].to = servent_id;
				rc = pthread_create(&servent_transmit[send_target][servent_id], NULL, servent_handle_transmit_func, &servent_transmit_data[send_target][servent_id]);
				servent_transmit_tunnel_ready_signal[send_target][servent_id]=0 ;

				
				
				servent_transmit_times[send_target][servent_id]=1;
				
				strcpy(servent_send_message[servent_id],"*2");//
				printf("servent %ld , send_data: %s , target servent:%d", servent_id,servent_send_message[servent_id],send_target);
				
				
	     			
	     			
	     			
	     			puts("");
	     			
	     			
	     					
	     			while(servent_transmit_tunnel_ready_signal[servent_id][send_target] ==1 ){
					pthread_cond_wait(&servent_receive_butter_empty[servent_id][send_target],&mutex[servent_id][send_target]);	
				}
						
	     					
	     			strcpy(servent_receive_message[servent_id][send_target], servent_send_message[servent_id]);//
	     					
	     			servent_transmit_tunnel_ready_signal[servent_id][send_target] = 1 ;
	     			
	     					
	     			
	     			while(servent_transmit_times[servent_id][send_target] != 0  || servent_transmit_times[send_target][servent_id] != 0){
	     			
				sleep(0.1);
				}
				
				
				
				if(servent_transmit_times[servent_id][send_target]== 0 && servent_transmit_times[send_target][servent_id]==0){
				
				
				servent_tunnel_work_over[servent_id][send_target]=1;
				servent_tunnel_work_over[send_target][servent_id]=1;
						
				
				}
	     			servent_pattern[servent_id]=99;
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
				rc = pthread_create(&servent_transmit[servent_id][send_target], NULL, servent_handle_transmit_func, &servent_transmit_data[servent_id][send_target]);
				servent_transmit_tunnel_ready_signal[servent_id][send_target]=0 ;
				
				servent_transmit_times[servent_id][send_target]=1;
				
				
				
				servent_tunnel_work_over[send_target][servent_id]=0;        
				servent_transmit_data[send_target][servent_id].from = send_target;
				servent_transmit_data[send_target][servent_id].to = servent_id;
				rc = pthread_create(&servent_transmit[send_target][servent_id], NULL, servent_handle_transmit_func, &servent_transmit_data[send_target][servent_id]);
				servent_transmit_tunnel_ready_signal[send_target][servent_id]=0 ;

				
				
				servent_transmit_times[send_target][servent_id]=1;
				
				strcpy(servent_send_message[servent_id],"*3");//
				printf("servent %ld , send_data: %s , target servent:%d", servent_id,servent_send_message[servent_id],send_target);
				
				
	     			
	     			
	     			
	     			puts("");
	     			
	     			
	     					
	     			while(servent_transmit_tunnel_ready_signal[servent_id][send_target] ==1 ){
					pthread_cond_wait(&servent_receive_butter_empty[servent_id][send_target],&mutex[servent_id][send_target]);	
				}
						
	     					
	     			strcpy(servent_receive_message[servent_id][send_target], servent_send_message[servent_id]);//
	     					
	     			servent_transmit_tunnel_ready_signal[servent_id][send_target] = 1 ;
	     			
	     					
	     			
	     			while(servent_transmit_times[servent_id][send_target] != 0  || servent_transmit_times[send_target][servent_id] != 0){
	     			
				sleep(0.1);
				}
				
				
				
				if(servent_transmit_times[servent_id][send_target]== 0 && servent_transmit_times[send_target][servent_id]==0){
				
				
				servent_tunnel_work_over[servent_id][send_target]=1;
				servent_tunnel_work_over[send_target][servent_id]=1;
						
				
				}
	     			servent_pattern[servent_id]=99;
				break;	
			
			case (-1):
				printf(" servent %ld terminated !\n", servent_id);
				
				break;	
		}
		sleep(1);
		
		
	

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
    
	    for (i = 0; i < NUM_SERVENT_BOTS; i++) {
					
		servent_work_over[i]=1;
		for (j = 0; j < NUM_SERVENT_BOTS; j++){
			servent_tunnel_work_over[i][j]=1;
		}		
	    }
	    
	    for (i = 0; i < NUM_CLIENT_BOTS; i++) {
					
		client_work_over[i]=1;
		for (j = 0; j < NUM_CLIENT_BOTS; j++){
			client_and_servent_tunnel_work_over[i][j]=1;
		}		
	    }
	    
	    for (i = 0; i < (NUM_SERVENT_BOTS/4); i++) {
					
		servent_thread_work_over[i]=1;
	    	
	    }		
	    for (i = 0; i < (NUM_CLIENT_BOTS/4); i++) {
					
		client_thread_work_over[i]=1;
	    	
	    }
	    relay_station_terminate_signal=1;	
    }

}


void num2ip(char bufX[], int ip)
{
	int b[4];
	
 
	b[0] = (int)((ip >> 24) & 0xff);
	b[1] = (int)((ip >> 16) & 0xff);
	b[2] = (int)((ip >> 8) & 0xff);
	b[3] = (int)(ip & 0xff);
 
	sprintf(bufX, "%d.%d.%d.%d", b[0], b[1], b[2], b[3]);
 
	
}
void getRandomIp(char ip[]){
	
	int range[][2] = { { 607649792, 608174079 },//36.56.0.0-36.63.255.255
	{ 1038614528, 1039007743 },//61.232.0.0-61.237.255.255
	{ 1783627776, 1784676351 },//106.80.0.0-106.95.255.255
	{ 2035023872, 2035154943 },//121.76.0.0-121.77.255.255
	{ 2078801920, 2079064063 },//123.232.0.0-123.235.255.255
	{ -1950089216, -1948778497 },//139.196.0.0-139.215.255.255
	{ -1425539072, -1425014785 },//171.8.0.0-171.15.255.255
	{ -1236271104, -1235419137 },//182.80.0.0-182.92.255.255
	{ -770113536, -768606209 },//210.25.0.0-210.47.255.255
	{ -569376768, -564133889 }, //222.16.0.0-222.95.255.255
	};
 
 
	int index = rand() % 10;
	num2ip(ip,range[index][0] + rand() % (range[index][1] - range[index][0]));
	
}
int getRandomPort(){
	int port = rand() % 6976+1024;
	return port;
}
void init_client_master(){
	
    int i,j,a;
    for (i = 0; i < NUM_CLIENT_BOTS; i++) {
    for (j = 0; j < NUM_SERVENT_BOTS; j++) {
    
    
    client_master[i][j] = rand() % NUM_CLIENT_BOTS  ;
    
    
   
    
    for (a = 0; a < j; a++) {
    
    while(client_master[i][a]==client_master[i][j]){
    
    client_master[i][j] = rand() % NUM_CLIENT_BOTS  ;
    
    
    a = 0;
    }
    
    }

    
   
    }
    
    }
    
    for(i=0;i<NUM_CLIENT_BOTS;i++){
    puts("");
    printf("master of client %ld have:", i);
    for (j = 0; j < NUM_SERVENT_BOTS; j++) {
    
    
    if(client_master[i][j]!= -1)
    	printf("%ld ", client_master[i][j]);
    
    }	
    
    } 
    puts("");

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
    
    
   
    }
    
    }
    
    for(i=0;i<NUM_SERVENT_BOTS;i++){
    puts("");
    printf("peer list of servent %ld have:", i);
    for (j = 0; j < NUM_SERVENT_BOTS; j++) {
    
    
    if(servent_peer_list[i][j]!= -1)
    	printf("%ld ", servent_peer_list[i][j]);
    
    }	
    
    } 
    puts("");

}
void init_servent_website(){
    int j=0,i=0,random,row;
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

    i++;
    }
    row = i;
    for(i=0;i<NUM_SERVENT_BOTS;i++){
    //printf("website in servent %d buffer:\n",j);
    
    random=rand() % row ;
    strcpy(servent_bot_website_buffer[i][0], website[random]);//
    //puts(servent_bot_website_buffer[i][j]);
    }
    

    fclose(f);

}
void init_servent_and_client_information(){
    int i=0,j=0;
    
    for (i = 0; i < NUM_SERVENT_BOTS; i++){
    getRandomIp(servent[i].ip);
    
	    for(j=0;j<already_exist_ip_num;j++){
	    	
		    while(strcmp(servent[i].ip,already_exist_ip[j])==0){
		    getRandomIp(servent[i].ip);
		    j=0;
		    }
	    
	    }
	       
    strcpy(already_exist_ip[already_exist_ip_num],servent[i].ip);
    already_exist_ip_num++;
    }
    for (i = 0; i < NUM_SERVENT_BOTS; i++){
    servent[i].port = getRandomPort();
	    for(j=0;j<already_exist_port_num;j++){
	    	
		    while(servent[i].port==already_exist_port[j]){
		    servent[i].port = getRandomPort();
		    j=0;
		    }
	    
	    }
    already_exist_port[already_exist_port_num] = servent[i].port;
    already_exist_port_num++;
    }
    
    for (i = 0; i < NUM_CLIENT_BOTS; i++){
    getRandomIp(client[i].ip);
    
	    for(j=0;j<already_exist_ip_num;j++){
	    	
		    while(strcmp(client[i].ip,already_exist_ip[j])==0){
		    getRandomIp(client[i].ip);
		    j=0;
		    }
	    
	    }
	       
    strcpy(already_exist_ip[already_exist_ip_num],client[i].ip);
    already_exist_ip_num++;
    }
    for (i = 0; i < NUM_CLIENT_BOTS; i++){
    client[i].port = getRandomPort();
	    for(j=0;j<already_exist_port_num;j++){
	    	
		    while(client[i].port==already_exist_port[j]){
		    client[i].port = getRandomPort();
		    j=0;
		    }
	    
	    }
    already_exist_port[already_exist_port_num] = client[i].port;
    already_exist_port_num++;
    }
    		
    for (i = 0; i < NUM_SERVENT_BOTS; i++){
    printf("servent[%d]->ip\n",i);
    puts(servent[i].ip);
    printf("servent[%d]->port: %d\n",i,servent[i].port);
    }
    for (i = 0; i < NUM_CLIENT_BOTS; i++){
    printf("client[%d]->ip\n",i);
    puts(client[i].ip);
    printf("client[%d]->port: %d\n",i,client[i].port);
    }
   
 
}
int main() {
	
    srand(time(NULL) );
    pthread_t servent_threads[NUM_SERVENT_BOTS/4];
    pthread_t client_threads[NUM_CLIENT_BOTS/4];
    
    pthread_t relay_station;
    int rc,return_data;
    int check=1;
    char data[1024]; 

    int master_command=-1;
   
    long i=0;
    long j=0;
    int a,b;
    long t;
    //FILE* f;

    rc = pthread_create(&relay_station, NULL, relay_station_func, NULL);  
    if (rc) {
            printf("ERORR; return code from pthread_create() is %d\n", rc);
            exit(EXIT_FAILURE);
    }
    
    for (i = 0; i < NUM_SERVENT_BOTS; i++) {
    	servent_pattern[i]=99;
        servent_bot_command_buffer_pointer[i]=0;
        servent_bot_website_buffer_pointer[i]=0;
        for (j = 0; j < NUM_SERVENT_BOTS; j++){
		
		servent_peer_list[i][j] = -1;
		servent_tunnel_work_over[i][j]=1; 
		
	}

     }
     for (i = 0; i < NUM_CLIENT_BOTS; i++) {
    	client_pattern[i]=99;
        client_bot_command_buffer_pointer[i]=0;
        client_bot_website_buffer_pointer[i]=0;
        for (j = 0; j < NUM_CLIENT_BOTS; j++){
		
		client_master[i][j] = -1;
		client_and_servent_tunnel_work_over[i][j]=1; 
		
	}

     }
     for (i = 0; i < (NUM_SERVENT_BOTS/4); i++) {
 	rc = pthread_create(&servent_threads[i], NULL, servent_thread_func, (void *)i);
        
        if (rc) {
            printf("ERORR; return code from pthread_create() is %d\n", rc);
            exit(EXIT_FAILURE);
        }
     }
     
     for (i = 0; i < (NUM_CLIENT_BOTS/4); i++) {
 	rc = pthread_create(&client_threads[i], NULL, client_thread_func, (void *)i);
        
        if (rc) {
            printf("ERORR; return code from pthread_create() is %d\n", rc);
            exit(EXIT_FAILURE);
        }
     }
   
    init_servent_peer_list();
    init_servent_website();
    init_client_master();
    init_servent_and_client_information();
    




					

    while(master_command != 0 ){
    	
    	sleep(1);
    	
    	printf("enter command \n");
    	printf("0:Exit \n");
    	printf("1. servent:fetch command from social network   client:fetch command from servent  \n");	
    	printf("2. servent:fetch command from peer   client:request peer list from servent  \n");
    	printf("3. servent:request peer list from peer   client:request website from servent  \n");
    	printf("4. servent:request website from peer   client:NULL \n");	
    		scanf("%d",&master_command);
    		
		
		if(master_command == 0 ) {
			program_over(1);
			break;
		}
		sleep(0.8);
		
		switch(master_command) {
			
		    	
			case 1:	
					
				
				for (t = 0; t < NUM_SERVENT_BOTS; t++) {servent_pattern[t]=master_command;}	
				//for (t = 0; t < NUM_CLIENT_BOTS; t++) {client_pattern[t]=master_command;}
				
				break;
				
			case 2:
					
				for (t = 0; t < NUM_SERVENT_BOTS; t++) {servent_pattern[t]=master_command;}	
				//for (t = 0; t < NUM_CLIENT_BOTS; t++) {client_pattern[t]=master_command;}
				
				break;				
			case 3:
				
					
				for (t = 0; t < NUM_SERVENT_BOTS; t++) {servent_pattern[t]=master_command;}	
				//for (t = 0; t < NUM_CLIENT_BOTS; t++) {client_pattern[t]=master_command;}	
						
				break;
			case 4:
				
					
				for (t = 0; t < NUM_SERVENT_BOTS; t++) {servent_pattern[t]=master_command;}	
				//for (t = 0; t < NUM_CLIENT_BOTS; t++) {client_pattern[t]=master_command;}
					
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
	for (t = 0; t < (NUM_CLIENT_BOTS/4); t++){
		pthread_join(client_threads[t],NULL);	
	}
    
    
    pthread_exit(NULL);
	
    return 0; 
    
}
