#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <locale.h>
#include <sys/stat.h>
#include <sys/shm.h>

//
#define NUM_SERVENT_BOTS 40
#define NUM_CLIENT_BOTS 4
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

typedef struct  peer{

long peer_id;
int reputation_value;
int trust_signal;
	
}Peer; 

typedef struct  master{

long master_id;
int reputation_value;

	
}Master; 

typedef struct  command{

char content[1024];
int effective_time;
int sender;//self
int receiver;//target or botmaster(9999)
char timestamp[1024];
char extra_information[1024];  
char command_code[1024]; 

}Command; 

typedef struct  website{

char url[1024];
int reputation_value;

	
}Website;
 
char date[1024];
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
int reputation_value_base=10;
int reputation_value_max=20;
int reputation_value_min=0;
//servent----------------------------------
//-----------------------------------------
int servent_pattern[NUM_SERVENT_BOTS]; 
Command servent_bot_command_buffer[NUM_SERVENT_BOTS][100];
Command servent_botmaster_command_buffer[NUM_SERVENT_BOTS][100];
int servent_botmaster_command_buffer_pointer[NUM_SERVENT_BOTS];
int servent_bot_command_buffer_pointer[NUM_SERVENT_BOTS];
Website servent_bot_website_buffer[NUM_SERVENT_BOTS][10]; 
int servent_bot_website_buffer_pointer[NUM_SERVENT_BOTS];
//-----------------------------------------
int servent_transmit_tunnel_ready_signal[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
char servent_send_message[NUM_SERVENT_BOTS][1024];
char servent_receive_message[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS][1024];
int servent_tunnel_work_over[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
int servent_work_over[NUM_SERVENT_BOTS];
//-----------------------------------------
//int servent_peer_list[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
Peer servent_peer_list[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
int servent_peer_num[NUM_SERVENT_BOTS];
int servent_trust_threshold[NUM_SERVENT_BOTS];
int servent_latency_signal[NUM_SERVENT_BOTS];
int servent_resurrection_complete_signal[NUM_SERVENT_BOTS];
int servent_eliminate_signal[NUM_SERVENT_BOTS];
int servent_resurrection_request_peer_time[NUM_SERVENT_BOTS]; 
//-----------------------------------------
/*int servent_client_list[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
int servent_client_num[NUM_SERVENT_BOTS];*/
//client-----------------------------------
//-----------------------------------------
int client_pattern[NUM_CLIENT_BOTS]; 
Command client_bot_command_buffer[NUM_CLIENT_BOTS][100]; 
Command client_botmaster_command_buffer[NUM_CLIENT_BOTS][100]; 
int client_bot_command_buffer_pointer[NUM_CLIENT_BOTS];
int client_botmaster_command_buffer_pointer[NUM_CLIENT_BOTS];
Website client_bot_website_buffer[NUM_CLIENT_BOTS][10]; 
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
Master client_master[NUM_CLIENT_BOTS][NUM_SERVENT_BOTS];
int client_master_num[NUM_CLIENT_BOTS];
struct tm *info;
time_t current;


pthread_mutex_t mutex[NUM_BOTS][NUM_BOTS];

pthread_t servent_transmit[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];

pthread_t client_receive[NUM_CLIENT_BOTS][NUM_SERVENT_BOTS];
pthread_t servent_handle_client_message[NUM_CLIENT_BOTS][NUM_SERVENT_BOTS];


pthread_cond_t  servent_receive_butter_empty[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];//signal of  buffer is empty 
pthread_cond_t  client_receive_butter_empty[NUM_CLIENT_BOTS][NUM_SERVENT_BOTS];//signal of  buffer is empty 


int servent_transmit_times[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];

int client_receive_times[NUM_CLIENT_BOTS][NUM_SERVENT_BOTS];
int servent_receive_times_from_client[NUM_CLIENT_BOTS][NUM_SERVENT_BOTS];

void servent_make_command(char* func_result, int sender_id, int receive_id, int extra_information_target, char* send_command_code){
   int i,j,format=rand() % 3+1;
   char text[80];
   
   //printf("format = %d \n", format);
   
   if(strcmp(send_command_code,"f003") == 0 || strcmp(send_command_code,"f004") == 0){
   
   servent_peer_num[sender_id]=0;
   for(i=0;i<NUM_SERVENT_BOTS;i++){
   if(servent_peer_list[sender_id][i].peer_id!= -1)
   servent_peer_num[sender_id]++;
   } 
				
   for ( i = 0; i < servent_peer_num[sender_id]; i++) {
   for ( j = 0; j < i; j++) {
       if (servent_peer_list[sender_id][j].reputation_value > servent_peer_list[sender_id][i].reputation_value) {
       Peer temp = servent_peer_list[sender_id][j];
       servent_peer_list[sender_id][j] = servent_peer_list[sender_id][i];
       servent_peer_list[sender_id][i] = temp;
      	
       }
     }
   }
   switch(format) {
   	case 1:
   		
   		strncat(func_result,send_command_code ,strlen(send_command_code));//command_code
   		strcat(func_result,";");
   		sprintf(text, "%s:%s:%d", servent[servent_peer_list[sender_id][servent_peer_num[sender_id]-1].peer_id].ip, date, servent[servent_peer_list[sender_id][servent_peer_num[sender_id]-1].peer_id].port );//extra information
   		strncat(func_result,text ,strlen(text));//extra information
                strcat(func_result,";");
   		sprintf(text, "%d", sender_id);  //sender
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%d", receive_id);//receicer
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%s.%d", date,86400);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%d",43200);//effective time
   		strncat(func_result,text ,strlen(text));
   		
   		//puts(func_result);
   		
   		
   		break;
   	case 2:
   		strcat(func_result,"[");
   		sprintf(text, "%s:%s:%d", servent[servent_peer_list[sender_id][servent_peer_num[sender_id]-1].peer_id].ip, date, servent[servent_peer_list[sender_id][servent_peer_num[sender_id]-1].peer_id].port );//extra information
   		strncat(func_result,text ,strlen(text));//extra information
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		strncat(func_result,send_command_code ,strlen(send_command_code));//command_code
                strcat(func_result,"]");
                strcat(func_result,"[");
   		sprintf(text, "%d", sender_id);  //sender
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		sprintf(text, "%d", receive_id);//receicer
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		sprintf(text, "%s.%d", date,86400);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		sprintf(text, "%d",43200);//effective time
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		
   		//puts(func_result);
   		break;
   	case 3:
   		sprintf(text, "%s.%d", date,86400);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"||");
   		sprintf(text, "%d",43200);//effective time
   		strncat(func_result,text ,strlen(text));
                strcat(func_result,"||");
                sprintf(text, "%s:%s:%d", servent[servent_peer_list[sender_id][servent_peer_num[sender_id]-1].peer_id].ip, date, servent[servent_peer_list[sender_id][servent_peer_num[sender_id]-1].peer_id].port );//extra information
   		strncat(func_result,text ,strlen(text));//extra information 		
   		strcat(func_result,"||");
   		strncat(func_result,send_command_code ,strlen(send_command_code));//command_code   		
   		strcat(func_result,"||");
   		sprintf(text, "%d", sender_id);  //sender
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"||");
   		sprintf(text, "%d", receive_id);//receicer
   		strncat(func_result,text ,strlen(text));
   		
   		//puts(func_result);
   		break;   		   		
   
   
   
   
   
   
   
   }
   
   }
   else{
   switch(format) {
   	case 1:
   		
   		strncat(func_result,send_command_code ,strlen(send_command_code));//command_code
   		strcat(func_result,";");
   		//extra information
                strcat(func_result,";");
   		sprintf(text, "%d", sender_id);  //sender
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%d", receive_id);//receicer
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%s.%d", date,86400);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%d",43200);//effective time
   		strncat(func_result,text ,strlen(text));
   		
   		//puts(func_result);
   		
   		
   		break;
   	case 2:
   		strcat(func_result,"[");
   		//extra information
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		strncat(func_result,send_command_code ,strlen(send_command_code));//command_code
                strcat(func_result,"]");
                strcat(func_result,"[");
   		sprintf(text, "%d", sender_id);  //sender
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		sprintf(text, "%d", receive_id);//receicer
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		sprintf(text, "%s.%d", date,86400);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		sprintf(text, "%d",43200);//effective time
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		
   		//puts(func_result);
   		break;
   	case 3:
   		sprintf(text, "%s.%d", date,86400);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"||");
   		sprintf(text, "%d",43200);//effective time
   		strncat(func_result,text ,strlen(text));
                strcat(func_result,"||");
                //extra information   		
   		strcat(func_result,"||");
   		strncat(func_result,send_command_code ,strlen(send_command_code));//command_code   		
   		strcat(func_result,"||");
   		sprintf(text, "%d", sender_id);  //sender
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"||");
   		sprintf(text, "%d", receive_id);//receicer
   		strncat(func_result,text ,strlen(text));
   		
   		//puts(func_result);
   		break;   		   		
   
   
   
   
   
   
   
   }
   
   
   }
   
   format=0;
   
   

}
void client_make_command(char* func_result, int sender_id, int receive_id, int extra_information_target, char* send_command_code){
   int format=rand() % 3+1;
   char text[80];
   
   //printf("format = %d \n", format);
   
   if(strcmp(send_command_code,"f003") == 0 || strcmp(send_command_code,"f004") == 0){
   int i,j,c_id = sender_id-NUM_SERVENT_BOTS;
   
   client_master_num[c_id]=0;
   for(i=0;i<NUM_SERVENT_BOTS;i++){
   if(client_master[c_id][i].master_id!= -1)
   client_master_num[c_id]++;
   } 
				
   for ( i = 0; i < client_master_num[c_id]; i++) {
   for ( j = 0; j < i; j++) {
       if (client_master[c_id][j].reputation_value > client_master[c_id][i].reputation_value) {
       Master temp = client_master[c_id][j];
       client_master[c_id][j] = client_master[c_id][i];
       client_master[c_id][i] = temp;
      	
       }
     }
   }
   switch(format) {
   	case 1:
   		
   		strncat(func_result,send_command_code ,strlen(send_command_code));//command_code
   		strcat(func_result,";");
   		sprintf(text, "%s:%s:%d", servent[client_master[c_id][client_master_num[c_id]-1].master_id].ip, date, servent[client_master[c_id][client_master_num[c_id]-1].master_id].port );//extra information
   		strncat(func_result,text ,strlen(text));
                strcat(func_result,";");
   		sprintf(text, "%d", sender_id);  //sender
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%d", receive_id);//receicer
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%s.%d", date,86400);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%d",43200);//effective time
   		strncat(func_result,text ,strlen(text));
   		
   		//puts(func_result);
   		
   		
   		break;
   	case 2:
   		strcat(func_result,"[");
   		sprintf(text, "%s:%s:%d", servent[client_master[c_id][client_master_num[c_id]-1].master_id].ip, date, servent[client_master[c_id][client_master_num[c_id]-1].master_id].port );//extra information
   		strncat(func_result,text ,strlen(text));//extra information
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		strncat(func_result,send_command_code ,strlen(send_command_code));//command_code
                strcat(func_result,"]");
                strcat(func_result,"[");
   		sprintf(text, "%d", sender_id);  //sender
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		sprintf(text, "%d", receive_id);//receicer
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		sprintf(text, "%s.%d", date,86400);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		sprintf(text, "%d",43200);//effective time
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		
   		//puts(func_result);
   		break;
   	case 3:
   		sprintf(text, "%s.%d", date,86400);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"||");
   		sprintf(text, "%d",43200);//effective time
   		strncat(func_result,text ,strlen(text));
                strcat(func_result,"||");
                sprintf(text, "%s:%s:%d", servent[client_master[c_id][client_master_num[c_id]-1].master_id].ip, date, servent[client_master[c_id][client_master_num[c_id]-1].master_id].port );//extra information
   		strncat(func_result,text ,strlen(text));//extra information   		
   		strcat(func_result,"||");
   		strncat(func_result,send_command_code ,strlen(send_command_code));//command_code   		
   		strcat(func_result,"||");
   		sprintf(text, "%d", sender_id);  //sender
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"||");
   		sprintf(text, "%d", receive_id);//receicer
   		strncat(func_result,text ,strlen(text));
   		
   		//puts(func_result);
   		break;   		   		
   
   
   
   
   
   
   
   }
   
   }
   else{
   switch(format) {
   	case 1:
   		
   		strncat(func_result,send_command_code ,strlen(send_command_code));//command_code
   		strcat(func_result,";");
   		//extra information
                strcat(func_result,";");
   		sprintf(text, "%d", sender_id);  //sender
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%d", receive_id);//receicer
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%s.%d", date,86400);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%d",43200);//effective time
   		strncat(func_result,text ,strlen(text));
   		
   		//puts(func_result);
   		
   		
   		break;
   	case 2:
   		strcat(func_result,"[");
   		//extra information
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		strncat(func_result,send_command_code ,strlen(send_command_code));//command_code
                strcat(func_result,"]");
                strcat(func_result,"[");
   		sprintf(text, "%d", sender_id);  //sender
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		sprintf(text, "%d", receive_id);//receicer
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		sprintf(text, "%s.%d", date,86400);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		sprintf(text, "%d",43200);//effective time
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		
   		//puts(func_result);
   		break;
   	case 3:
   		sprintf(text, "%s.%d", date,86400);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"||");
   		sprintf(text, "%d",43200);//effective time
   		strncat(func_result,text ,strlen(text));
                strcat(func_result,"||");
                //extra information   		
   		strcat(func_result,"||");
   		strncat(func_result,send_command_code ,strlen(send_command_code));//command_code   		
   		strcat(func_result,"||");
   		sprintf(text, "%d", sender_id);  //sender
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"||");
   		sprintf(text, "%d", receive_id);//receicer
   		strncat(func_result,text ,strlen(text));
   		
   		//puts(func_result);
   		break;   		   		
   
   
   
   
   
   
   
   }
   
   
   }
   
   format=0;
   
   

}
void strchrn(char *dest,char *src, int begin, int end) {
   int range=end-begin-1;
   
   strncpy(dest, src+begin , range);
   dest[strlen(dest)] = '\0';   
}
int servent_instruction_analysis(int s_id,char *instruction){
   int format=0,segment_pointer[10];
   int i=0,j=0;
   char segment[1024];
   char result[10][1024];
   
   for(i=0;i<10;i++){
   segment_pointer[i]=0;
   }
   
   if(instruction[0] == 'f'){
   format = 1;
   }
   if(instruction[0] == '['){
   format = 2;
   }   
   if(instruction[0]-48 > 0 && instruction[0]-48 < 10){
   
   format = 3;
   }
      
   switch(format) {
   	case 1:
   		
   		
   		//printf("format %d \n",format);
   		
   		for(i=0;i<6;i++){
   		
   		while(instruction[j] != ';'){
   		if(instruction[j] == '\0')break;
   		
   		j++;
   		}
   		
   		j++;
   		segment_pointer[i]=j;
   		
   		if(i==0){
   		
   		strchrn(result[i],instruction,0,segment_pointer[i]);
   		strcpy(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].command_code ,result[i]);
   		//puts(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].command_code);
   		}
   		if(i==1){
   		
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]);
   		strcpy(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].extra_information ,result[i]);
   		//puts(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].extra_information);
   		}
   		if(i==2){
   		
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]);
   		servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].sender = atoi(result[i]);
   		//printf("%d \n",servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].sender);
   		}
   		if(i==3){
   		
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]);
   		servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].receiver = atoi(result[i]);
   		//printf("%d \n",servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].receiver);
   		}
   		if(i==4){
   		
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]);
   		strcpy(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].timestamp ,result[i]);
   		//puts(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].timestamp);
   		}
   		if(i==5){
   		
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]);
   		servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].effective_time = atoi(result[i]);
   		//printf("%d \n",servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].effective_time);
   		}
   		
   		
   		}
   		
   		for(i=0;i<6;i++){
   		memset(result[i],0,strlen(result[i]));
   		}
   		break; 
   	case 2:
   		//printf("format %d \n",format);
   		j++;
   		for(i=0;i<6;i++){
   		while( instruction[j] != ']'){
   		if(instruction[j] == '\0')break;
   		j++;
   		}
   		j+=2;
   		segment_pointer[i]=j;
   		
   		
   		if(i==0){
   		strchrn(result[i],instruction,1,segment_pointer[i]-1);
   		strcpy(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].extra_information ,result[i]);
   		//puts(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].extra_information);
   		}
   		if(i==1){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].command_code ,result[i]);
   		//puts(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].command_code);
   		}
   		if(i==2){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].sender = atoi(result[i]);
   		//printf("%d \n",servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].sender);
   		}
   		if(i==3){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].receiver = atoi(result[i]);
   		//printf("%d \n",servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].receiver);
   		}
   		if(i==4){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].timestamp ,result[i]);
   		//puts(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].timestamp);
   		}
   		if(i==5){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].effective_time = atoi(result[i]);
   		//printf("%d \n",servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].effective_time);
   		}
   		
   		}
   		
   		
   		for(i=0;i<6;i++){
   		memset(result[i],0,strlen(result[i]));
   		}
   		break;    
   	case 3:
   		//printf("format %d \n",format);
   		
   		for(i=0;i<6;i++){
   		while( instruction[j] != '|'){
   		if(instruction[j] == '\0')break;
   		j++;
   		}
   		j+=2;
   		segment_pointer[i]=j;
   		
   		if(i==0){
   		strchrn(result[i],instruction,0,segment_pointer[i]-1);
   		strcpy(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].timestamp ,result[i]);
   		//puts(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].timestamp);
   		}
   		if(i==1){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].effective_time = atoi(result[i]) ;
   		//printf("%d \n",servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].effective_time);
   		
   		}
   		if(i==2){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].extra_information ,result[i]);
   		//puts(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].extra_information);
   		}
   		if(i==3){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].command_code ,result[i]);
   		//puts(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].command_code);
   		
   		}
   		if(i==4){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].sender = atoi(result[i]);
   		//printf("%d \n",servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].sender);
   		
   		}
   		if(i==5){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].receiver = atoi(result[i]);
   		//printf("%d \n",servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].receiver);
   		
   		}
   		
   		}
   		
   		for(i=0;i<6;i++){
   		memset(result[i],0,strlen(result[i]));
   		}
   		
   		break;
   	default:
   		break;	    
   }
   if(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].receiver == 9999 ){
   printf("servent %d this instruction is to me \n",s_id);
   if(strcmp(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].command_code,"f006") == 0){
   printf("servent %d execute command f006 \n",s_id);
   if(servent_trust_threshold[s_id] >= 4 ){
   printf("servent %d turn to latency  \n",s_id);
   servent_latency_signal[s_id]=1;
   servent_trust_threshold[s_id]=0;
   }
   else{
   printf("servent %d does not have enough trust threshold \n",s_id);
   }
   }
   if(strcmp(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].command_code,"f007") == 0){
   printf("servent %d execute command f007 \n",s_id);
   
   if(servent_latency_signal[s_id]==1){
   servent_latency_signal[s_id]=0;
   for (i = 0; i < NUM_SERVENT_BOTS; i++) {
	   for (j = 0; j < NUM_SERVENT_BOTS; j++) {
	   if(servent_peer_list[i][j].peer_id == s_id ){
	   servent_peer_list[i][j].reputation_value = reputation_value_base+2;
	   }
	   }
   }
   for (i = 0; i < NUM_CLIENT_BOTS; i++) {
	   for (j = 0; j < NUM_SERVENT_BOTS; j++) {
	   if(client_master[i][j].master_id== s_id ){
	   client_master[i][j].reputation_value = reputation_value_base+2;
	   }
	   }
   }
   servent_peer_num[s_id]=0;
   for(i=0;i<NUM_SERVENT_BOTS;i++){
   if(servent_peer_list[s_id][i].peer_id!= -1)
   servent_peer_num[s_id]++;
   }
   for(i=0;i<servent_peer_num[s_id];i++){
   if(servent_peer_list[s_id][i].reputation_value < (reputation_value_base+2)){
       Peer temp = servent_peer_list[s_id][servent_peer_num[s_id]-1];
       servent_peer_list[s_id][servent_peer_num[s_id]-1] = servent_peer_list[s_id][i];
       servent_peer_list[s_id][i] = temp;
       printf("servent %d  remove servent %d\n" ,s_id,servent_peer_list[s_id][servent_peer_num[s_id]-1]);
       memset(&servent_peer_list[s_id][servent_peer_num[s_id]-1],0,sizeof(Peer));
   }
   
   }
   servent_peer_num[s_id]=0;
   for(i=0;i<NUM_SERVENT_BOTS;i++){
   if(servent_peer_list[s_id][i].peer_id!= -1)
   servent_peer_num[s_id]++;
   } 
					
   for ( i = 0; i < servent_peer_num[s_id]; i++) {
   for ( j = 0; j < i; j++) {
	   if (servent_peer_list[s_id][j].reputation_value > servent_peer_list[s_id][i].reputation_value) {
	   Peer temp = servent_peer_list[s_id][j];
	   servent_peer_list[s_id][j] = servent_peer_list[s_id][i];
	   servent_peer_list[s_id][i] = temp;
		      	
	   }
   }
   }
   for ( i = 0; i < servent_peer_num[s_id]; i++) {
   printf("id:%d value:%d\n",servent_peer_list[s_id][i].peer_id, servent_peer_list[s_id][i].reputation_value);
   }
   servent_resurrection_complete_signal[s_id] = 1;
   servent_resurrection_request_peer_time[s_id] = 2; 
   }
   if(servent_latency_signal[s_id]==0){
   
   for (i = 0; i < NUM_SERVENT_BOTS; i++) {
	   for (j = 0; j < NUM_SERVENT_BOTS; j++) {
		   if(servent_peer_list[i][j].peer_id == s_id ){
			   if(servent_peer_list[i][j].reputation_value < (reputation_value_base+2) ){
			   servent_eliminate_signal[s_id] = 1;
			   printf("servent %d  eliminate_signal: %d\n" ,s_id,servent_eliminate_signal[s_id]);
			   
			   break;
			   }
		   
		   }
	   }
   }
   
   servent_resurrection_complete_signal[s_id] = 1;
   servent_resurrection_request_peer_time[s_id] = 2;
   
   }
   
   
   }
   }
   
   return 0;
   
   
}
int servent_commmand_analysis(int s_id,char *servent_command){
   int format=0,segment_pointer[10];
   int i=0,j=0;
   char segment[1024];
   char result[10][1024];
   
   for(i=0;i<10;i++){
   segment_pointer[i]=0;
   }
   if(servent_command[0] == '*'){
   return 0;
   }
   if(strcmp(servent_command,"return website") == 0){
   return 0;
   }
   if(servent_command[0] == 'f'){
   format = 1;
   }
   if(servent_command[0] == '['){
   format = 2;
   }   
   if(servent_command[0]-48 > 0 && servent_command[0]-48 < 10){
   
   format = 3;
   }
      
   switch(format) {
   	case 1:
   		
   		
   		//printf("format %d \n",format);
   		
   		for(i=0;i<6;i++){
   		
   		while(servent_command[j] != ';'){
   		if(servent_command[j] == '\0')break;
   		
   		j++;
   		}
   		
   		j++;
   		segment_pointer[i]=j;
   		
   		if(i==0){
   		
   		strchrn(result[i],servent_command,0,segment_pointer[i]);
   		strcpy(servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].command_code ,result[i]);
   		//puts(servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].command_code);
   		}
   		if(i==1){
   		
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]);
   		strcpy(servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].extra_information ,result[i]);
   		//puts(servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].extra_information);
   		}
   		if(i==2){
   		
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]);
   		servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].sender = atoi(result[i]);
   		//printf("%d \n",servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].sender);
   		}
   		if(i==3){
   		
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]);
   		servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].receiver = atoi(result[i]);
   		//printf("%d \n",servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].receiver);
   		}
   		if(i==4){
   		
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]);
   		strcpy(servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].timestamp ,result[i]);
   		//puts(servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].timestamp);
   		}
   		if(i==5){
   		
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]);
   		servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].effective_time = atoi(result[i]);
   		//printf("%d \n",servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].effective_time);
   		}
   		
   		
   		}
   		
   		for(i=0;i<6;i++){
   		memset(result[i],0,strlen(result[i]));
   		}
   		break; 
   	case 2:
   		//printf("format %d \n",format);
   		j++;
   		for(i=0;i<6;i++){
   		while( servent_command[j] != ']'){
   		if(servent_command[j] == '\0')break;
   		j++;
   		}
   		j+=2;
   		segment_pointer[i]=j;
   		
   		
   		if(i==0){
   		strchrn(result[i],servent_command,1,segment_pointer[i]-1);
   		strcpy(servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].extra_information ,result[i]);
   		//puts(servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].extra_information);
   		}
   		if(i==1){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].command_code ,result[i]);
   		//puts(servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].command_code);
   		}
   		if(i==2){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]-1);
   		servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].sender = atoi(result[i]);
   		//printf("%d \n",servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].sender);
   		}
   		if(i==3){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]-1);
   		servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].receiver = atoi(result[i]);
   		//printf("%d \n",servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].receiver);
   		}
   		if(i==4){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].timestamp ,result[i]);
   		//puts(servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].timestamp);
   		}
   		if(i==5){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]-1);
   		servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].effective_time = atoi(result[i]);
   		//printf("%d \n",servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].effective_time);
   		}
   		
   		}
   		
   		
   		for(i=0;i<6;i++){
   		memset(result[i],0,strlen(result[i]));
   		}
   		break;    
   	case 3:
   		//printf("format %d \n",format);
   		
   		for(i=0;i<6;i++){
   		while( servent_command[j] != '|'){
   		if(servent_command[j] == '\0')break;
   		j++;
   		}
   		j+=2;
   		segment_pointer[i]=j;
   		
   		if(i==0){
   		strchrn(result[i],servent_command,0,segment_pointer[i]-1);
   		strcpy(servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].timestamp ,result[i]);
   		//puts(servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].timestamp);
   		}
   		if(i==1){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]-1);
   		servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].effective_time = atoi(result[i]) ;
   		//printf("%d \n",servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].effective_time);
   		
   		}
   		if(i==2){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].extra_information ,result[i]);
   		//puts(servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].extra_information);
   		}
   		if(i==3){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].command_code ,result[i]);
   		//puts(servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].command_code);
   		
   		}
   		if(i==4){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]-1);
   		servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].sender = atoi(result[i]);
   		//printf("%d \n",servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].sender);
   		
   		}
   		if(i==5){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]-1);
   		servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].receiver = atoi(result[i]);
   		//printf("%d \n",servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].receiver);
   		
   		}
   		
   		}
   		
   		for(i=0;i<6;i++){
   		memset(result[i],0,strlen(result[i]));
   		}
   		
   		break;
   	case 0:
   		break;	    
   }
   if(servent_bot_command_buffer[s_id][servent_bot_command_buffer_pointer[s_id]].receiver == s_id ){ 
   printf("servent %d this command is to me \n",s_id);
   return 1;
   }
   
   return 0;
   
   
}
int client_commmand_analysis(int c_id,char *client_command){
   int format=0,segment_pointer[10];
   int i=0,j=0;
   char segment[1024];
   char result[10][1024];
   
   for(i=0;i<10;i++){
   segment_pointer[i]=0;
   }
   //puts(client_command);
   if(client_command[0] == '*'){
   return 0;
   }
   if(strcmp(client_command,"return website") == 0){
   return 0;
   }
   if(client_command[0] == 'f'){
   format = 1;
   }
   if(client_command[0] == '['){
   format = 2;
   }   
   if(client_command[0]-48 > 0 && client_command[0]-48 < 10){
   format = 3;
   }   
   
   switch(format) {
   	case 1:
      		//printf("format %d \n",format);
   		
   		for(i=0;i<6;i++){
   		
   		while(client_command[j] != ';'){
   		if(client_command[j] == '\0')break;
   		
   		j++;
   		}
   		
   		j++;
   		segment_pointer[i]=j;
   		if(i==0){
   		
   		strchrn(result[i],client_command,0,segment_pointer[i]);
   		strcpy(client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].command_code ,result[i]);
   		//puts(client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].command_code);
   		}
   		if(i==1){
   		
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]);
   		strcpy(client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].extra_information ,result[i]);
   		//puts(client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].extra_information);
   		
   		}
   		if(i==2){
   		
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]);
   		client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].sender = atoi(result[i]);
   		//printf("%d \n",client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].sender);
   		}
   		if(i==3){
   		
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]);
   		client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].receiver = atoi(result[i]);
   		//printf("%d \n",client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].receiver);
   		}
   		if(i==4){
   		
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]);
   		strcpy(client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].timestamp ,result[i]);
   		//puts(client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].timestamp);
   		}
   		if(i==5){
   		
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]);
   		client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].effective_time = atoi(result[i]);
   		//printf("%d \n",client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].effective_time);
   		}
   		
   		
   		}
   		for(i=0;i<6;i++){
   		memset(result[i],0,strlen(result[i]));
   		}
   		break; 
   	case 2:
   		//printf("format %d \n",format);
   		
   		
   		j++;
   		for(i=0;i<6;i++){
   		while( client_command[j] != ']'){
   		if(client_command[j] == '\0')break;
   		j++;
   		}
   		j+=2;
   		segment_pointer[i]=j;
   		
   		
   		if(i==0){
   		strchrn(result[i],client_command,1,segment_pointer[i]-1);
   		strcpy(client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].extra_information ,result[i]);
   		//puts(client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].extra_information);
   		}
   		if(i==1){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].command_code ,result[i]);
   		//puts(client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].command_code);
   		}
   		if(i==2){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].sender = atoi(result[i]);
   		//printf("%d \n",client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].sender);
   		}
   		if(i==3){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].receiver = atoi(result[i]);
   		//printf("%d \n",client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].receiver);
   		}
   		if(i==4){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].timestamp ,result[i]);
   		//puts(client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].timestamp);
   		}
   		if(i==5){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].effective_time = atoi(result[i]);
   		//printf("%d \n",client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].effective_time);
   		}
   		
   		}
   		
   		for(i=0;i<6;i++){
   		memset(result[i],0,strlen(result[i]));
   		}
   		break;    
   	case 3:
   		//printf("format %d \n",format);
   		for(i=0;i<6;i++){
   		while( client_command[j] != '|'){
   		if(client_command[j] == '\0')break;
   		j++;
   		}
   		j+=2;
   		segment_pointer[i]=j;
   		
   		
   		if(i==0){
   		strchrn(result[i],client_command,0,segment_pointer[i]-1);
   		strcpy(client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].timestamp ,result[i]);
   		//puts(client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].timestamp);
   		
   		}
   		if(i==1){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].effective_time = atoi(result[i]);
   		//printf("%d \n",client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].effective_time);
   		
   		}
   		if(i==2){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].extra_information ,result[i]);
   		//puts(client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].extra_information);
   		}
   		if(i==3){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].command_code ,result[i]);
   		//puts(client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].command_code);
   		}
   		if(i==4){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].sender = atoi(result[i]);
   		//printf("%d \n",client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].sender);
   		
   		}
   		if(i==5){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].receiver = atoi(result[i]);
   		//printf("%d \n",client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].receiver);
   		
   		
   		}
   		
   		}
   		
   		for(i=0;i<6;i++){
   		memset(result[i],0,strlen(result[i]));
   		}
   		break;   
   		
   	case 0:
   		break; 
   }
   
   if(client_bot_command_buffer[c_id][client_bot_command_buffer_pointer[c_id]].receiver == client[c_id].id ){ //####
   printf("client %d this command is to me \n",c_id);
   return 1;
   }
   
   return 0;
   
}
/*int client_instruction_analysis(int c_id,char *instruction){
   int format=0,segment_pointer[10];
   int i=0,j=0;
   char segment[1024];
   char result[10][1024];
   
   for(i=0;i<10;i++){
   segment_pointer[i]=0;
   }
   if(instruction[0] == 'f'){
   format = 1;
   }
   if(instruction[0] == '['){
   format = 2;
   }   
   if(instruction[0]-48 > 0 && instruction[0]-48 < 10){
   format = 3;
   }   
   
   switch(format) {
   	case 1:
      		printf("format %d \n",format);
   		
   		for(i=0;i<6;i++){
   		
   		while(instruction[j] != ';'){
   		if(instruction[j] == '\0')break;
   		
   		j++;
   		}
   		
   		j++;
   		segment_pointer[i]=j;
   		if(i==0){
   		
   		strchrn(result[i],instruction,0,segment_pointer[i]);
   		strcpy(client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].command_code ,result[i]);
   		puts(client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].command_code);
   		}
   		if(i==1){
   		
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]);
   		strcpy(client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].extra_information ,result[i]);
   		puts(client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].extra_information);
   		
   		}
   		if(i==2){
   		
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]);
   		client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].sender = atoi(result[i]);
   		printf("%d \n",client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].sender);
   		}
   		if(i==3){
   		
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]);
   		client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].receiver = atoi(result[i]);
   		printf("%d \n",client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].receiver);
   		}
   		if(i==4){
   		
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]);
   		strcpy(client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].timestamp ,result[i]);
   		puts(client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].timestamp);
   		}
   		if(i==5){
   		
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]);
   		client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].effective_time = atoi(result[i]);
   		printf("%d \n",client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].effective_time);
   		}
   		
   		
   		}
   		for(i=0;i<6;i++){
   		memset(result[i],0,strlen(result[i]));
   		}
   		break; 
   	case 2:
   		printf("format %d \n",format);
   		
   		
   		j++;
   		for(i=0;i<6;i++){
   		while( instruction[j] != ']'){
   		if(instruction[j] == '\0')break;
   		j++;
   		}
   		j+=2;
   		segment_pointer[i]=j;
   		
   		
   		if(i==0){
   		strchrn(result[i],instruction,1,segment_pointer[i]-1);
   		strcpy(client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].extra_information ,result[i]);
   		puts(client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].extra_information);
   		}
   		if(i==1){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].command_code ,result[i]);
   		puts(client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].command_code);
   		}
   		if(i==2){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].sender = atoi(result[i]);
   		printf("%d \n",client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].sender);
   		}
   		if(i==3){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].receiver = atoi(result[i]);
   		printf("%d \n",client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].receiver);
   		}
   		if(i==4){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].timestamp ,result[i]);
   		puts(client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].timestamp);
   		}
   		if(i==5){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].effective_time = atoi(result[i]);
   		printf("%d \n",client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].effective_time);
   		}
   		
   		}
   		
   		for(i=0;i<6;i++){
   		memset(result[i],0,strlen(result[i]));
   		}
   		break;    
   	case 3:
   		printf("format %d \n",format);
   		for(i=0;i<6;i++){
   		while( instruction[j] != '|'){
   		if(instruction[j] == '\0')break;
   		j++;
   		}
   		j+=2;
   		segment_pointer[i]=j;
   		
   		
   		if(i==0){
   		strchrn(result[i],instruction,0,segment_pointer[i]-1);
   		strcpy(client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].timestamp ,result[i]);
   		puts(client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].timestamp);
   		
   		}
   		if(i==1){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].effective_time = atoi(result[i]);
   		printf("%d \n",client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].effective_time);
   		
   		}
   		if(i==2){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].extra_information ,result[i]);
   		puts(client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].extra_information);
   		}
   		if(i==3){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].command_code ,result[i]);
   		puts(client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].command_code);
   		}
   		if(i==4){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].sender = atoi(result[i]);
   		printf("%d \n",client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].sender);
   		
   		}
   		if(i==5){
   		strchrn(result[i],instruction,segment_pointer[i-1],segment_pointer[i]-1);
   		client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].receiver = atoi(result[i]);
   		printf("%d \n",client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].receiver);
   		
   		
   		}
   		
   		}
   		
   		for(i=0;i<6;i++){
   		memset(result[i],0,strlen(result[i]));
   		}
   		break;   
   		
   	default:
   		break; 
   }
   
   if(client_botmaster_command_buffer[c_id][client_botmaster_command_buffer_pointer[c_id]].receiver == 9999 ){ 
   printf("client %d this command is to me \n",c_id);
   return 1;
   }
   
   return 0;
   
}*/
char make_peer_list_message(char message[],int tid){

   //char message[50];
   char text[50];
   
   int i=0,j=0;
 
   memset(message,0,strlen(message));
   sprintf(message, "I am servent %d the peer which have the highest reputation value in my peer list is ", tid);   
   servent_peer_num[tid]=0;
   for(i=0;i<NUM_SERVENT_BOTS;i++){
   if(servent_peer_list[tid][i].peer_id!= -1)
   servent_peer_num[tid]++;
   } 
				
   for ( i = 0; i < servent_peer_num[tid]; i++) {
   for ( j = 0; j < i; j++) {
       if (servent_peer_list[tid][j].reputation_value > servent_peer_list[tid][i].reputation_value) {
       Peer temp = servent_peer_list[tid][j];
       servent_peer_list[tid][j] = servent_peer_list[tid][i];
       servent_peer_list[tid][i] = temp;
      	
       }
     }
   }
   
   for ( i = 0; i < servent_peer_num[tid]; i++) {
   printf("id:%d value:%d\n",servent_peer_list[tid][i].peer_id, servent_peer_list[tid][i].reputation_value);
   }
   
   sprintf(text, "%d ", servent_peer_list[tid][servent_peer_num[tid]-1].peer_id);  
		   
   strncat(message,text ,strlen(text));
  

   
	
   
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
		    
		    
		    //printf("command%d from botmaster:",f);
		    //puts(file_data[i][j]);
		    file_data[i][j][strlen(file_data[i][j])-1] = '\0';
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
   
    
    int i,j,b,command_comparison_result=1,website_comparison_result=1,peer_comparison_result=0;
    
   
    int behavior_request=0;
    char func_message[1024];
    char message[1024];
    
    
    while(servent_tunnel_work_over[information->from][information->to] != 1){
    	
	
	if(servent_transmit_tunnel_ready_signal[information->from][information->to] == 1 ){
			
	printf("servent %ld  ip:%s port:%d receive message from servent %ld ip:%s port:%d: %s\n", information->to , servent[information->to].ip,servent[information->to].port,information->from, 
	servent[information->from].ip,servent[information->from].port, servent_receive_message[information->from][information->to]);
	
	strcpy(func_message,servent_receive_message[information->from][information->to]);
	
	if(servent_commmand_analysis(information->to,func_message) == 1){
	//###
	if(strcmp(servent_bot_command_buffer[information->to][servent_bot_command_buffer_pointer[information->to]].command_code ,"f001" )==0 ){
	
	servent_make_command(message,servent[information->to].id,servent[information->from].id ,0,"f002" );
	//strcpy(message,"return botmaster command");
					
			
	while(servent_transmit_tunnel_ready_signal[information->to][information->from] ==1 ){
		     		
		     		
	pthread_cond_wait(&servent_receive_butter_empty[information->to][information->from],&mutex[information->to][information->from]);	
	}
							
		     			
	strcpy(servent_receive_message[information->to][information->from], message);
		     	
	servent_transmit_tunnel_ready_signal[information->to][information->from] =1 ;
		     			
	}
	
	if(strcmp(servent_bot_command_buffer[information->to][servent_bot_command_buffer_pointer[information->to]].command_code ,"f002" )==0 ){//###
	b=0;
		
	printf("command from servent %ld:\n",information->from);
	
	
	if(strlen(servent_botmaster_command_buffer[information->from][0].content)!= 0){
	for(i=0;i<servent_botmaster_command_buffer_pointer[information->to];i++){	
	command_comparison_result=strcmp(servent_botmaster_command_buffer[information->to][i].content,servent_botmaster_command_buffer[information->from][servent_botmaster_command_buffer_pointer[information->from]-1].content);
	if(command_comparison_result== 0){break;}
	}
				
	if(command_comparison_result== 0){
	printf("servent %ld already have (%s) \n",information->to,servent_botmaster_command_buffer[information->from][servent_botmaster_command_buffer_pointer[information->from]-1].content);
	}//***
	if(command_comparison_result != 0){
	strcpy(servent_botmaster_command_buffer[information->to][servent_botmaster_command_buffer_pointer[information->to]].content,
	servent_botmaster_command_buffer[information->from][servent_botmaster_command_buffer_pointer[information->from]-1].content);
	
	puts(servent_botmaster_command_buffer[information->to][servent_botmaster_command_buffer_pointer[information->to]].content);

	servent_botmaster_command_buffer_pointer[information->to]++;
	
	}
	
	}
	servent_instruction_analysis(information->to,servent_botmaster_command_buffer[information->to][servent_botmaster_command_buffer_pointer[information->to]-1].content);
	
	
	   
	
	for(i=0;i<servent_peer_num[information->to];i++){	
	if(servent_peer_list[information->to][i].peer_id ==  information->from && servent_peer_list[information->to][i].reputation_value < reputation_value_max && servent_peer_list[information->to][i].trust_signal == 0){
	servent_peer_list[information->to][i].reputation_value++;
	break;
	}
	}
	
	servent_peer_num[information->to]=0;
	for(i=0;i<NUM_SERVENT_BOTS;i++){
	if(servent_peer_list[information->to][i].peer_id!= -1)
	servent_peer_num[information->to]++;
	} 
					
	for ( i = 0; i < servent_peer_num[information->to]; i++) {
	for ( j = 0; j < i; j++) {
	    if (servent_peer_list[information->to][j].reputation_value > servent_peer_list[information->to][i].reputation_value) {
	    Peer temp = servent_peer_list[information->to][j];
	    servent_peer_list[information->to][j] = servent_peer_list[information->to][i];
	    servent_peer_list[information->to][i] = temp;
	      	
	    }
	}
	}
	for ( i = 0; i < servent_peer_num[information->to]; i++) {
	printf("id:%d value:%d\n",servent_peer_list[information->to][i].peer_id, servent_peer_list[information->to][i].reputation_value);
	}
	}
	if(strcmp(servent_bot_command_buffer[information->to][servent_bot_command_buffer_pointer[information->to]].command_code ,"f003" )==0 ){
	
	
	
	
	servent_make_command(message,servent[information->to].id,servent[information->from].id ,0,"f004" );
	
	for ( i = 0; i < servent_peer_num[information->to]; i++) {	
	if(servent_peer_list[information->to][i].peer_id == servent_peer_list[information->from][servent_peer_num[information->from]-1].peer_id){
	printf("servent %ld already have servent %d in peer list !\n",information->to,servent_peer_list[information->from][servent_peer_num[information->from]-1].peer_id);
	if(servent_peer_list[information->to][i].reputation_value < reputation_value_base){
	
	servent_peer_list[information->to][i].reputation_value = reputation_value_base;
	}
	else{
	if(servent_peer_list[information->to][i].reputation_value < reputation_value_max && servent_peer_list[information->to][i].trust_signal == 0){
	servent_peer_list[information->to][i].reputation_value ++; 
	}
	}
	peer_comparison_result=1;
	break;
	}
	if(information->to == servent_peer_list[information->from][servent_peer_num[information->from]-1].peer_id){
	printf("servent %ld cant add itself in peer list !\n",information->to,servent_peer_list[information->from][servent_peer_num[information->from]-1].peer_id);
	
	peer_comparison_result=1;
	break;
	}
	
	}
	if(peer_comparison_result == 0){
	printf("servent %ld remove servent %d in peer list !\n",information->to,servent_peer_list[information->to][0].peer_id);
	printf("servent %ld add servent %d in peer list !\n",information->to,servent_peer_list[information->from][servent_peer_num[information->from]-1].peer_id);
	servent_peer_list[information->to][0].peer_id = servent_peer_list[information->from][servent_peer_num[information->from]-1].peer_id;
	servent_peer_list[information->to][0].reputation_value = reputation_value_base+1;
	
	
	}
	
		     					
	while(servent_transmit_tunnel_ready_signal[information->to][information->from] ==1 ){
		     		
		     		
	pthread_cond_wait(&servent_receive_butter_empty[information->to][information->from],&mutex[information->to][information->from]);	
	}
							
		     			
	strcpy(servent_receive_message[information->to][information->from], message);
		     	
	servent_transmit_tunnel_ready_signal[information->to][information->from] =1 ;
	}
	
	
	if(strcmp(servent_bot_command_buffer[information->to][servent_bot_command_buffer_pointer[information->to]].command_code ,"f004" )==0 ){
	
	for ( i = 0; i < servent_peer_num[information->to]; i++) {	
	if(servent_peer_list[information->to][i].peer_id == servent_peer_list[information->from][servent_peer_num[information->from]-1].peer_id){
	printf("servent %ld already have servent %d in peer list !\n",information->to,servent_peer_list[information->from][servent_peer_num[information->from]-1].peer_id);
	if(servent_peer_list[information->to][i].reputation_value < reputation_value_base){
	
	servent_peer_list[information->to][i].reputation_value = reputation_value_base;
	}
	else{
	if(servent_peer_list[information->to][i].reputation_value < reputation_value_max && servent_peer_list[information->to][i].trust_signal == 0){
	servent_peer_list[information->to][i].reputation_value ++; 
	}
	
	}//***
	peer_comparison_result=1;
	break;
	}
	if(information->to == servent_peer_list[information->from][servent_peer_num[information->from]-1].peer_id){
	printf("servent %ld cant add itself in peer list !\n",information->to,servent_peer_list[information->from][servent_peer_num[information->from]-1].peer_id);
	
	peer_comparison_result=1;
	break;
	}
	
	}
	if(peer_comparison_result == 0){
	printf("servent %ld remove servent %d in peer list !\n",information->to,servent_peer_list[information->to][0].peer_id);
	printf("servent %ld add servent %d in peer list !\n",information->to,servent_peer_list[information->from][servent_peer_num[information->from]-1].peer_id);
	servent_peer_list[information->to][0].peer_id = servent_peer_list[information->from][servent_peer_num[information->from]-1].peer_id;
	servent_peer_list[information->to][0].reputation_value = reputation_value_base+1;
	
	
	}
	
	
	
	
	
	for(i=0;i<servent_peer_num[information->to];i++){	
	if(servent_peer_list[information->to][i].peer_id ==  information->from && servent_peer_list[information->to][i].reputation_value < reputation_value_max && servent_peer_list[information->to][i].trust_signal == 0){
	servent_peer_list[information->to][i].reputation_value++;
	break;
	}
	}//***
	
	servent_peer_num[information->to]=0;
	for(i=0;i<NUM_SERVENT_BOTS;i++){
	if(servent_peer_list[information->to][i].peer_id!= -1 ){
	servent_peer_num[information->to]++;
	}
	} 
					
	for ( i = 0; i < servent_peer_num[information->to]; i++) {
	for ( j = 0; j < i; j++) {
	    if (servent_peer_list[information->to][j].reputation_value > servent_peer_list[information->to][i].reputation_value) {
	    Peer temp = servent_peer_list[information->to][j];
	    servent_peer_list[information->to][j] = servent_peer_list[information->to][i];
	    servent_peer_list[information->to][i] = temp;
	      	
	    }
	}
	}
	   
	for ( i = 0; i < servent_peer_num[information->to]; i++) {
	printf("id:%d value:%d\n",servent_peer_list[information->to][i].peer_id, servent_peer_list[information->to][i].reputation_value);
	}
	}
	
	
	if(strcmp(servent_bot_command_buffer[information->to][servent_bot_command_buffer_pointer[information->to]].command_code ,"f005" )==0 ){
	
	servent_trust_threshold[information->to]++;
	printf("servent %ld receive trust message !\n",information->to);
	}
	
	
	}
	
	else{
	if(strcmp(func_message,"return website") == 0 ){
	b=0;
		
	printf("website from servent %ld:\n",information->from);
	while(servent_bot_website_buffer[information->from][b].url[0]!= '\0'){
				
	for(i=0;i<servent_bot_website_buffer_pointer[information->to];i++){	
	website_comparison_result=strcmp(servent_bot_website_buffer[information->to][i].url,servent_bot_website_buffer[information->from][b].url);
	if(website_comparison_result== 0){break;}
	}
				
	if(website_comparison_result== 0){
	printf("servent %ld already have (%s)\n",information->to,servent_bot_website_buffer[information->from][b].url);
	}
	if(website_comparison_result != 0){
	strcpy(servent_bot_website_buffer[information->to][servent_bot_website_buffer_pointer[information->to]].url,servent_bot_website_buffer[information->from][b].url);
	servent_bot_website_buffer[information->to][servent_bot_website_buffer_pointer[information->to]].reputation_value = servent_bot_website_buffer[information->from][b].reputation_value ;
	
	puts(servent_bot_website_buffer[information->to][servent_bot_website_buffer_pointer[information->to]].url);
	servent_bot_website_buffer_pointer[information->to]++;
	}				
	b++;			
	}
	
	for(i=0;i<servent_peer_num[information->to];i++){	
	if(servent_peer_list[information->to][i].peer_id ==  information->from && servent_peer_list[information->to][i].reputation_value < reputation_value_max && servent_peer_list[information->to][i].trust_signal == 0){
	servent_peer_list[information->to][i].reputation_value++;
	break;
	}
	}
	servent_peer_num[information->to]=0;
	for(i=0;i<NUM_SERVENT_BOTS;i++){
	if(servent_peer_list[information->to][i].peer_id!= -1)
	servent_peer_num[information->to]++;
	} 
					
	for ( i = 0; i < servent_peer_num[information->to]; i++) {
	for ( j = 0; j < i; j++) {
	    if (servent_peer_list[information->to][j].reputation_value > servent_peer_list[information->to][i].reputation_value) {
	    Peer temp = servent_peer_list[information->to][j];
	    servent_peer_list[information->to][j] = servent_peer_list[information->to][i];
	    servent_peer_list[information->to][i] = temp;
	      	
	    }
	}
	}
	   
	for ( i = 0; i < servent_peer_num[information->to]; i++) {
	printf("id:%d value:%d\n",servent_peer_list[information->to][i].peer_id, servent_peer_list[information->to][i].reputation_value);
	}
	}
	
	
	//-------Need to deal with peer list specially---------
	if(func_message[0] == '*' ){
	
		behavior_request = func_message[1]-48;
		
	    	switch(behavior_request) {
				
			
				case 1:
					
					
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
	
	}
	
	
	
    	

	
	behavior_request=0;	
	memset(servent_receive_message[information->from][information->to],0,strlen(servent_receive_message[information->from][information->to]));			
	memset(func_message,0,strlen(func_message));	
	memset(message,0,strlen(message));	

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
    
    int i,j,b,command_comparison_result=1,website_comparison_result=1,peer_comparison_result=0;
   
    
    char func_message[1024];
    char message[1024];
    
    
    while(client_and_servent_tunnel_work_over[information->client_id][information->servent_id] != 1){
    	
	
	if(servent_to_client_tunnel_ready_signal[information->client_id][information->servent_id] == 1 ){
			
	printf("client %ld ip:%s port:%d receive message from servent %ld ip:%s port:%d : %s\n", information->client_id , client[information->client_id].ip, client[information->client_id].port, 
	information->servent_id,  servent[information->servent_id].ip,servent[information->servent_id].port, client_receive_message[information->client_id][information->servent_id]);
	
	strcpy(func_message,client_receive_message[information->client_id][information->servent_id]);
	
	
	if(client_commmand_analysis(information->client_id,func_message) == 1){
	
	//puts(client_bot_command_buffer[information->client_id][client_bot_command_buffer_pointer[information->client_id]].command_code);
	
	if(strcmp(client_bot_command_buffer[information->client_id][client_bot_command_buffer_pointer[information->client_id]].command_code ,"f002" )==0 ){
	b=0;
		
	printf("command from servent %ld:\n",information->servent_id);
	
	if(strlen(servent_botmaster_command_buffer[information->servent_id][0].content)!= 0){ 
	for(i=0;i<client_botmaster_command_buffer_pointer[information->client_id];i++){	
	command_comparison_result=strcmp(client_botmaster_command_buffer[information->client_id][i].content,
	servent_botmaster_command_buffer[information->servent_id][servent_botmaster_command_buffer_pointer[information->servent_id]-1].content);
	if(command_comparison_result== 0){break;}
	}
				
	if(command_comparison_result== 0){
	printf("client %ld already have (%s) \n",information->client_id,servent_botmaster_command_buffer[information->servent_id][servent_botmaster_command_buffer_pointer[information->servent_id]-1].content);
	}
	if(command_comparison_result != 0){
	strcpy(client_botmaster_command_buffer[information->client_id][client_botmaster_command_buffer_pointer[information->client_id]].content,
	servent_botmaster_command_buffer[information->servent_id][servent_botmaster_command_buffer_pointer[information->servent_id]-1].content);
	
	puts(client_botmaster_command_buffer[information->client_id][client_botmaster_command_buffer_pointer[information->client_id]].content);
	//client_commmand_analysis(information->client_id,client_botmaster_command_buffer[information->client_id][client_botmaster_command_buffer_pointer[information->client_id]].content);
	client_botmaster_command_buffer_pointer[information->client_id]++;
	}
	}
	for(i=0;i<client_master_num[information->client_id];i++){	
	if(client_master[information->client_id][i].master_id ==  information->servent_id && client_master[information->client_id][i].reputation_value < reputation_value_max){
	client_master[information->client_id][i].reputation_value++;
	break;
	}
	}
	client_master_num[information->client_id]=0;
	for(i=0;i<NUM_SERVENT_BOTS;i++){
	if(client_master[information->client_id][i].master_id!= -1)
	client_master_num[information->client_id]++;
	} 
					
	for ( i = 0; i < client_master_num[information->client_id]; i++) {
	   for ( j = 0; j < i; j++) {
	       if (client_master[information->client_id][j].reputation_value > client_master[information->client_id][i].reputation_value) {
	       Master temp = client_master[information->client_id][j];
	       client_master[information->client_id][j] = client_master[information->client_id][i];
	       client_master[information->client_id][i] = temp;
	      	
	      }
	   }
	}
	   
	for ( i = 0; i < client_master_num[information->client_id]; i++) {
	   printf("id:%d value:%d\n",client_master[information->client_id][i].master_id, client_master[information->client_id][i].reputation_value);
	} 
	}
	
	
	if(strcmp(client_bot_command_buffer[information->client_id][client_bot_command_buffer_pointer[information->client_id]].command_code ,"f004" )==0 ){
	
	
	for ( i = 0; i < client_master_num[information->client_id]; i++) {	
	if(client_master[information->client_id][i].master_id == servent_peer_list[information->servent_id][servent_peer_num[information->servent_id]-1].peer_id ){
	printf("client %ld already have servent %d in master list !\n",information->client_id,servent_peer_list[information->servent_id][servent_peer_num[information->servent_id]-1].peer_id);
	if(client_master[information->client_id][i].reputation_value < reputation_value_base){
	client_master[information->client_id][i].reputation_value = reputation_value_base;
	}
	else{
	if( client_master[information->client_id][i].reputation_value < reputation_value_max){
	client_master[information->client_id][i].reputation_value ++; 
	}
	
	}
	peer_comparison_result=1;
	break;
	}
	
	
	}
	if(peer_comparison_result == 0){
	printf("client %ld remove servent %d in master list !\n",information->client_id,client_master[information->client_id][0].master_id);
	printf("client %ld add servent %d in master list !\n",information->client_id,servent_peer_list[information->servent_id][servent_peer_num[information->servent_id]-1].peer_id);
	client_master[information->client_id][0].master_id = servent_peer_list[information->servent_id][servent_peer_num[information->servent_id]-1].peer_id;
	client_master[information->client_id][0].reputation_value = reputation_value_base+1;
	
	}
	
	for(i=0;i<client_master_num[information->client_id];i++){	
	if(client_master[information->client_id][i].master_id ==  information->servent_id && client_master[information->client_id][i].reputation_value < reputation_value_max){
	client_master[information->client_id][i].reputation_value++;
	break;
	}
	}
	client_master_num[information->client_id]=0;
	for(i=0;i<NUM_SERVENT_BOTS;i++){
	if(client_master[information->client_id][i].master_id!= -1)
	client_master_num[information->client_id]++;
	} 
					
	for ( i = 0; i < client_master_num[information->client_id]; i++) {
	   for ( j = 0; j < i; j++) {
	       if (client_master[information->client_id][j].reputation_value > client_master[information->client_id][i].reputation_value) {
	       Master temp = client_master[information->client_id][j];
	       client_master[information->client_id][j] = client_master[information->client_id][i];
	       client_master[information->client_id][i] = temp;
	      	
	      }
	   }
	}
	   
	for ( i = 0; i < client_master_num[information->client_id]; i++) {
	   printf("id:%d value:%d\n",client_master[information->client_id][i].master_id, client_master[information->client_id][i].reputation_value);
	}   
	}
	
	}
	
	else{
	if(strcmp(func_message,"return website") == 0 ){
	b=0;
		
	printf("website from servent %ld:\n",information->servent_id);
	while(servent_bot_website_buffer[information->servent_id][b].url[0]!= '\0'){
				
	for(i=0;i<client_bot_website_buffer_pointer[information->client_id];i++){	
	website_comparison_result=strcmp(client_bot_website_buffer[information->client_id][i].url,servent_bot_website_buffer[information->servent_id][b].url);
	if(website_comparison_result== 0){break;}
	}
				
	if(website_comparison_result== 0){
	printf("client %ld already have (%s)\n",information->client_id,servent_bot_website_buffer[information->servent_id][b].url);
	}
	if(website_comparison_result != 0){
	strcpy(client_bot_website_buffer[information->client_id][client_bot_website_buffer_pointer[information->client_id]].url,servent_bot_website_buffer[information->servent_id][b].url);
	client_bot_website_buffer[information->client_id][client_bot_website_buffer_pointer[information->client_id]].reputation_value = servent_bot_website_buffer[information->servent_id][b].reputation_value;
	puts(client_bot_website_buffer[information->client_id][client_bot_website_buffer_pointer[information->client_id]].url);
	client_bot_website_buffer_pointer[information->client_id]++;
	}				
	b++;			
	}

	}
	for(i=0;i<client_master_num[information->client_id];i++){	
	if(client_master[information->client_id][i].master_id ==  information->servent_id && client_master[information->client_id][i].reputation_value < reputation_value_max){
	client_master[information->client_id][i].reputation_value++;
	break;
	}
	}
	client_master_num[information->client_id]=0;
	for(i=0;i<NUM_SERVENT_BOTS;i++){
	if(client_master[information->client_id][i].master_id!= -1)
	client_master_num[information->client_id]++;
	} 
					
	for ( i = 0; i < client_master_num[information->client_id]; i++) {
	   for ( j = 0; j < i; j++) {
	       if (client_master[information->client_id][j].reputation_value > client_master[information->client_id][i].reputation_value) {
	       Master temp = client_master[information->client_id][j];
	       client_master[information->client_id][j] = client_master[information->client_id][i];
	       client_master[information->client_id][i] = temp;
	      	
	      }
	   }
	}
	   
	for ( i = 0; i < client_master_num[information->client_id]; i++) {
	   printf("id:%d value:%d\n",client_master[information->client_id][i].master_id, client_master[information->client_id][i].reputation_value);
	} 
	}
	
	
	
	
	
	
					
	
	memset(client_receive_message[information->client_id][information->servent_id],0,strlen(client_receive_message[information->client_id][information->servent_id]));
	memset(func_message,0,strlen(func_message));
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
    
    
    int i,j,b,command_comparison_result=1,website_comparison_result=1,peer_comparison_result=0;
   
    int behavior_request=0;
    char func_message[1024];
    char message[1024];
    
    
    while(client_and_servent_tunnel_work_over[information->client_id][information->servent_id] != 1){
    	
	
	if(client_to_servent_tunnel_ready_signal[information->client_id][information->servent_id] == 1 ){
			
	printf("servent %ld ip:%s port:%d receive message from client %ld ip:%s port:%d : %s\n", information->servent_id , servent[information->servent_id].ip,servent[information->servent_id].port,
	information->client_id, client[information->client_id].ip, client[information->client_id].port, servent_receive_message_from_client[information->client_id][information->servent_id]);//
	
	strcpy(func_message,servent_receive_message_from_client[information->client_id][information->servent_id]);//
	
	if(servent_commmand_analysis(information->servent_id,func_message) == 1){
	
	if(strcmp(servent_bot_command_buffer[information->servent_id][servent_bot_command_buffer_pointer[information->servent_id]].command_code ,"f001" )==0 ){
	
	servent_make_command(message,servent[information->servent_id].id,client[information->client_id].id ,0,"f002" );
	
	
	memset(client_receive_message[information->client_id][information->servent_id],0,strlen(client_receive_message[information->client_id][information->servent_id]));	     			
	strcpy(client_receive_message[information->client_id][information->servent_id], message);
		     	
	servent_to_client_tunnel_ready_signal[information->client_id][information->servent_id] =1 ;
	
	}
	if(strcmp(servent_bot_command_buffer[information->servent_id][servent_bot_command_buffer_pointer[information->servent_id]].command_code ,"f003" )==0 ){
	
	servent_make_command(message,servent[information->servent_id].id,client[information->client_id].id ,0,"f004" );
	memset(client_receive_message[information->client_id][information->servent_id],0,strlen(client_receive_message[information->client_id][information->servent_id]));					     			
	strcpy(client_receive_message[information->client_id][information->servent_id], message);
	
	for ( i = 0; i < servent_peer_num[information->servent_id]; i++) {	
	if(servent_peer_list[information->servent_id][i].peer_id == client_master[information->client_id][client_master_num[information->client_id]-1].master_id){
	printf("servent %ld already have servent %d in peer list !\n",information->servent_id,client_master[information->client_id][client_master_num[information->client_id]-1].master_id);
	if(servent_peer_list[information->servent_id][i].reputation_value < reputation_value_base && servent_peer_list[information->servent_id][i].trust_signal == 0){ 
	servent_peer_list[information->servent_id][i].reputation_value = reputation_value_base;
	}
	else{
	if(servent_peer_list[information->servent_id][i].reputation_value < reputation_value_max){
	servent_peer_list[information->servent_id][i].reputation_value ++; 
	}
	
	}
	peer_comparison_result=1;
	break;
	}
	if(information->servent_id == client_master[information->client_id][client_master_num[information->client_id]-1].master_id){
	printf("servent %ld cant add itself in peer list !\n",information->servent_id,client_master[information->client_id][client_master_num[information->client_id]-1].master_id);
	
	peer_comparison_result=1;
	break;
	}
	
	}
	if(peer_comparison_result == 0){
	servent_peer_list[information->servent_id][0].peer_id = client_master[information->client_id][client_master_num[information->client_id]-1].master_id;
	servent_peer_list[information->servent_id][0].reputation_value = reputation_value_base+1;
	printf("servent %ld remove servent %d in peer list !\n",information->servent_id,servent_peer_list[information->servent_id][0].peer_id);
	printf("servent %ld add servent %d in peer list !\n",information->servent_id,client_master[information->client_id][client_master_num[information->client_id]-1].master_id);
	
	}
	
	
		     	
	servent_to_client_tunnel_ready_signal[information->client_id][information->servent_id] =1 ;
	
	}
	
	}
	else{
	
	if(func_message[0] == '*' ){
	
		behavior_request = func_message[1]-48;
		
	    	switch(behavior_request) {

				case 1:
					
					memset(message,0,strlen(message));
				 	strcpy(message,"return website");
				 	
				 	memset(client_receive_message[information->client_id][information->servent_id],0,strlen(client_receive_message[information->client_id][information->servent_id]));
		     			strcpy(client_receive_message[information->client_id][information->servent_id], message);
		     	
		     			servent_to_client_tunnel_ready_signal[information->client_id][information->servent_id] =1 ;
				 	
					break;	
					
				case 0:
					break;	
				
		}	 
    		
    	}
	
	}
	
	behavior_request=0;	
	memset(servent_receive_message_from_client[information->client_id][information->servent_id],0,strlen(servent_receive_message_from_client[information->client_id][information->servent_id]));				
	memset(func_message,0,strlen(func_message));
	memset(message,0,strlen(message));
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
		/*client_pattern[c_id] =  rand() % 3 +1  ;
		printf("client_pattern[%ld] = %d !\n", c_id,client_pattern[c_id]);*/
		switch(client_pattern[c_id]) {
			case 0:
	
				client_pattern[c_id]=99;
				break;
			case 1:
				
				client_master_num[c_id]=0;
				for(i=0;i<NUM_CLIENT_BOTS;i++){
				
				if(client_master[c_id][i].master_id!= -1){//###
				client_master_num[c_id]++;}
					
				}
				
				
				target_servent = client_master[c_id][rand() % (client_master_num[c_id])].master_id  ;
				
				for ( i = 0; i < client_master_num[c_id]; i++) {
				
				while(client_master[c_id][i].master_id == target_servent && client_master[c_id][target_servent].reputation_value < reputation_value_min){
				
				target_servent = client_master[c_id][rand() % (client_master_num[c_id])].master_id  ;
				i = 0;    
				}
				
				}
				while(servent_eliminate_signal[target_servent] == 1){
				
				target_servent = client_master[c_id][rand() % (client_master_num[c_id])].master_id  ;
				  
				}
				if(servent_latency_signal[target_servent]==1){
				printf("servent %ld already turn to latency bot  \n",target_servent);
				for ( i = 0; i < client_master_num[c_id]; i++) {
				
				if(client_master[c_id][i].master_id == target_servent ){
				client_master[c_id][i].reputation_value--;
				    
				}
				
				}
				
				client_pattern[c_id]=99;
				break;
				}
				
				while(client_and_servent_tunnel_work_over[c_id][target_servent]==0){
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
				
				memset(client_send_message[c_id],0,strlen(client_send_message[c_id]));
				client_make_command(client_send_message[c_id],client[c_id].id,servent[target_servent].id,0,"f001");
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
				
				if(client_master[c_id][i].master_id!= -1){
				client_master_num[c_id]++;}
					
				} 
				
				target_servent = client_master[c_id][rand() % (client_master_num[c_id])].master_id  ;
				for ( i = 0; i < client_master_num[c_id]; i++) {
				
				while(client_master[c_id][i].master_id == target_servent && client_master[c_id][target_servent].reputation_value < reputation_value_min){
				
				target_servent = client_master[c_id][rand() % (client_master_num[c_id])].master_id  ;
				i = 0;    
				}
				
				}
				while(servent_eliminate_signal[target_servent] == 1){
				
				target_servent = client_master[c_id][rand() % (client_master_num[c_id])].master_id  ;
				  
				}
				if(servent_latency_signal[target_servent]==1){
				printf("servent %ld already turn to latency bot  \n",target_servent);
				for ( i = 0; i < client_master_num[c_id]; i++) {
				
				if(client_master[c_id][i].master_id == target_servent ){
				client_master[c_id][i].reputation_value--;
				    
				}
				
				}
				
				client_pattern[c_id]=99;
				break;
				}
				
				while(client_and_servent_tunnel_work_over[c_id][target_servent]==0){
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
				
				memset(client_send_message[c_id],0,strlen(client_send_message[c_id]));
				client_make_command(client_send_message[c_id],client[c_id].id,servent[target_servent].id,0,"f003");
				
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
				
				if(client_master[c_id][i].master_id!= -1){
				client_master_num[c_id]++;}
					
				}
				
				target_servent = client_master[c_id][rand() % (client_master_num[c_id])].master_id  ;
				
				for ( i = 0; i < client_master_num[c_id]; i++) {
				
				while(client_master[c_id][i].master_id == target_servent && client_master[c_id][target_servent].reputation_value < reputation_value_min){
				
				target_servent = client_master[c_id][rand() % (client_master_num[c_id])].master_id  ;
				i = 0;    
				}
				
				}
				while(servent_eliminate_signal[target_servent] == 1){
				
				target_servent = client_master[c_id][rand() % (client_master_num[c_id])].master_id  ;
				  
				}
				
				
				if(servent_latency_signal[target_servent]==1){
				printf("servent %ld already turn to latency bot  \n",target_servent);
				for ( i = 0; i < client_master_num[c_id]; i++) {
				
				if(client_master[c_id][i].master_id == target_servent ){
				client_master[c_id][i].reputation_value--;
				    
				}
				
				}
				
				client_pattern[c_id]=99;
				break;
				}
				
				
				while(client_and_servent_tunnel_work_over[c_id][target_servent]==0){
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
				
				memset(client_send_message[c_id],0,strlen(client_send_message[c_id]));
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
				
				
			
			case (-1):
				printf(" client %ld terminated !\n", c_id);
				
				break;	
		}
		sleep(2);
		for ( i = 0; i < client_botmaster_command_buffer_pointer[c_id]; i++) {
		if(client_botmaster_command_buffer[c_id][i].effective_time != 0){
			   client_botmaster_command_buffer[c_id][i].effective_time--;
		}
			   
			   
		}
		for ( i = 0; i < client_botmaster_command_buffer_pointer[c_id]; i++) {
	        for ( j = 0; j < i; j++) {
		       if (client_botmaster_command_buffer[c_id][j].effective_time < client_botmaster_command_buffer[c_id][i].effective_time) {
		       Command temp = client_botmaster_command_buffer[c_id][j];
		       client_botmaster_command_buffer[c_id][j] = client_botmaster_command_buffer[c_id][i];
		       client_botmaster_command_buffer[c_id][i] = temp;
		      	
		       }
	        }
                }
	   
		for ( i = 0; i < client_botmaster_command_buffer_pointer[c_id]; i++) {
			   
			   if(client_botmaster_command_buffer[c_id][i].effective_time == 0){
			   memset(&client_botmaster_command_buffer[c_id][i],0,sizeof(Command));
			   client_botmaster_command_buffer_pointer[c_id]--;
			   }
			   
			  /* puts(client_botmaster_command_buffer[c_id][i].content);
			   printf("%d \n",client_botmaster_command_buffer[c_id][i].effective_time);*/
		}		
		
	

}
void servent_func(long s_id){


    int rc,a,b,i,j,send_target,command_comparison_result=1,result,file_num;
    char text[1024];
  
		if(servent_work_over[s_id] == 1){
			return;
		}
		for ( i = 0; i < NUM_SERVENT_BOTS; i++) {		
		    if(servent_resurrection_complete_signal[s_id] == 1 && servent_resurrection_complete_signal[i] == 0){
			printf("servent %ld wait for other servent execute resurrection\n", s_id);
			sleep(2);
			return;
		    }
		}
		/*servent_pattern[s_id] =  rand()  % 4+1;
		printf(" servent_pattern[%ld] = %d !\n", s_id,servent_pattern[s_id]);*/
		servent_peer_num[s_id]=0;
		for(i=0;i<NUM_SERVENT_BOTS;i++){
		   if(servent_peer_list[s_id][i].peer_id!= -1)
		   servent_peer_num[s_id]++;
		}
		
						
		for ( i = 0; i < servent_peer_num[s_id]; i++) {		   
			if (servent_peer_list[s_id][i].reputation_value < reputation_value_min) {
		        servent_pattern[s_id]=3;
		        }
		        if (servent_peer_list[s_id][i].reputation_value == reputation_value_max && servent_peer_list[s_id][i].trust_signal == 0) {
		               
				servent_peer_list[s_id][i].trust_signal=1;
				send_target = servent_peer_list[s_id][i].peer_id  ;
				
				
				while(servent_tunnel_work_over[s_id][send_target]==0){
					sleep(0.1);
				
				}
				/*while(servent_transmit_times[s_id][send_target]==1){
					sleep(0.1);
				
				}*/
				servent_tunnel_work_over[s_id][send_target]=0;        
				servent_transmit_data[s_id][send_target].from = s_id;
				servent_transmit_data[s_id][send_target].to = send_target;
				rc = pthread_create(&servent_transmit[s_id][send_target], NULL, servent_handle_transmit_func, &servent_transmit_data[s_id][send_target]);
				servent_transmit_tunnel_ready_signal[s_id][send_target]=0 ;
				
				
				servent_transmit_times[s_id][send_target]=1;
				
	
     	                        
				memset(servent_send_message[s_id],0,strlen(servent_send_message[s_id]));
				servent_make_command(servent_send_message[s_id],servent[s_id].id,servent[send_target].id ,0,"f005");
				printf("servent %ld , send_data: %s , target servent:%d", s_id,servent_send_message[s_id],send_target);
				
				
	     			
	     			
	     			
	     			puts("");
	     			
	     			
	     					
	     			while(servent_transmit_tunnel_ready_signal[s_id][send_target] ==1 ){
					pthread_cond_wait(&servent_receive_butter_empty[s_id][send_target],&mutex[s_id][send_target]);	
				}
						
	     					
	     			strcpy(servent_receive_message[s_id][send_target], servent_send_message[s_id]);//
	     					
	     			servent_transmit_tunnel_ready_signal[s_id][send_target] = 1 ;
	     			
	
	     			while(servent_transmit_times[s_id][send_target] != 0  ){
	     			
				sleep(0.1);
				}
				
				
				
				if(servent_transmit_times[s_id][send_target]== 0 ){
				
				
				servent_tunnel_work_over[s_id][send_target]=1;
				
						
				
				}
		        
		        }
		}
		//printf(" servent_pattern[%ld] !!!!\n", s_id); 
		if(servent_latency_signal[s_id] == 1){
		puts("I am latency bot I dont communicate with other bot");
		servent_pattern[s_id]=1;
		}
		
		if(servent_resurrection_request_peer_time[s_id] > 0){
		printf("I need to request peer (remain %d time )\n",servent_resurrection_request_peer_time[s_id]);
		servent_pattern[s_id]=3;
		}
		//printf(" servent_pattern[%ld] !!!!\n", s_id); 
		switch(servent_pattern[s_id]) {
			case 0:
				
				
				servent_pattern[s_id]=99;
				break;
			case 1://botmaster_command
				file_num=rand()  % 3;
				switch(file_num){
				case 0:
					a=0;
					b=0;
					
					printf("servent %ld receive command%d from relay station:\n",s_id,a+1);
					while(file_data[a][b][0]!= '\0'){
					
					for(i=0;i<servent_botmaster_command_buffer_pointer[s_id];i++){	
					command_comparison_result=strcmp(servent_botmaster_command_buffer[s_id][i].content,file_data[a][b]);
					if(command_comparison_result== 0){break;}
					}
					
					if(command_comparison_result== 0){
					printf("servent %ld already have (%s)\n",s_id,file_data[a][b]);
					
					}
					if(command_comparison_result != 0){
					strcpy(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].content,file_data[a][b]);
					puts(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].content);
					
					servent_botmaster_command_buffer_pointer[s_id]++;
					}
					
					
					b++;
					
					}
					servent_instruction_analysis(s_id,servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]-1].content);
					break;
				case 1:
					a=1;
					b=0;
					
					printf("servent %ld receive command%d from relay station:\n",s_id,a+1);
					while(file_data[a][b][0]!= '\0'){
					
					for(i=0;i<servent_botmaster_command_buffer_pointer[s_id];i++){	
					command_comparison_result=strcmp(servent_botmaster_command_buffer[s_id][i].content,file_data[a][b]);
					if(command_comparison_result== 0){break;}
					}
					
					if(command_comparison_result== 0){
					printf("servent %ld already have (%s)\n",s_id,file_data[a][b]);
					
					}
					if(command_comparison_result != 0){
					strcpy(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].content,file_data[a][b]);
					puts(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].content);
					
					servent_botmaster_command_buffer_pointer[s_id]++;
					}
					
					
					b++;
					
					}
					servent_instruction_analysis(s_id,servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]-1].content);
					break;
				case 2:
					a=2;
					b=0;
					//printf(" I am servent[%ld] \n", s_id);
					printf("servent %ld receive command%d from relay station:\n",s_id,a+1);
					while(file_data[a][b][0]!= '\0'){
					
					for(i=0;i<servent_botmaster_command_buffer_pointer[s_id];i++){	
					command_comparison_result=strcmp(servent_botmaster_command_buffer[s_id][i].content,file_data[a][b]);
					if(command_comparison_result== 0){break;}
					}
					
					if(command_comparison_result== 0){
					printf("servent %ld already have (%s)\n",s_id,file_data[a][b]);
					
					}
					if(command_comparison_result != 0){
					strcpy(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].content,file_data[a][b]);
					puts(servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]].content);
					
					servent_botmaster_command_buffer_pointer[s_id]++;
					}
					
					
					b++;
					
					}
					servent_instruction_analysis(s_id,servent_botmaster_command_buffer[s_id][servent_botmaster_command_buffer_pointer[s_id]-1].content);
					break;
				}
				
				
				servent_pattern[s_id]=99;
				
				break;
			case 2:
				
				servent_peer_num[s_id]=0;
				for(i=0;i<NUM_SERVENT_BOTS;i++){
				
				if(servent_peer_list[s_id][i].peer_id!= -1)
				servent_peer_num[s_id]++;
					
				} 
				
				send_target = servent_peer_list[s_id][rand() % (servent_peer_num[s_id])].peer_id  ;
				
				while(send_target==s_id){
    
				send_target = servent_peer_list[s_id][rand() % (servent_peer_num[s_id])].peer_id ;
				    
				}
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
				
				while(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value < reputation_value_min){
				
				send_target = servent_peer_list[s_id][rand() % (servent_peer_num[s_id])].peer_id ;
				i = 0;    
				}
				
				}
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
				
				while(servent_peer_list[s_id][i].peer_id == send_target && servent_eliminate_signal[send_target]==1){
				
				send_target = servent_peer_list[s_id][rand() % (servent_peer_num[s_id])].peer_id ;
				i = 0;    
				}
				
				}
				if(servent_latency_signal[send_target]==1){
				printf("servent %ld already turn to latency bot  \n",send_target);
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
					if(servent_peer_list[s_id][i].peer_id == send_target ){
						servent_peer_list[s_id][send_target].reputation_value--;
					}
				}
				
				servent_pattern[s_id]=99;
				break;
				}
				while(servent_tunnel_work_over[send_target][s_id]==0){
					sleep(0.1);
				
				}
				while(servent_tunnel_work_over[s_id][send_target]==0){
					sleep(0.1);
				
				}
				servent_tunnel_work_over[s_id][send_target]=0;        
				servent_transmit_data[s_id][send_target].from = s_id;
				servent_transmit_data[s_id][send_target].to = send_target;
				rc = pthread_create(&servent_transmit[s_id][send_target], NULL, servent_handle_transmit_func, &servent_transmit_data[s_id][send_target]);
				servent_transmit_tunnel_ready_signal[s_id][send_target]=0 ;
				
				
				servent_transmit_times[s_id][send_target]=1;
				
				
				
				servent_tunnel_work_over[send_target][s_id]=0;        
				servent_transmit_data[send_target][s_id].from = send_target;
				servent_transmit_data[send_target][s_id].to = s_id;
				rc = pthread_create(&servent_transmit[send_target][s_id], NULL, servent_handle_transmit_func, &servent_transmit_data[send_target][s_id]);
				servent_transmit_tunnel_ready_signal[send_target][s_id]=0 ;

				
				
				servent_transmit_times[send_target][s_id]=1;
				
     	                        
				memset(servent_send_message[s_id],0,strlen(servent_send_message[s_id]));
				servent_make_command(servent_send_message[s_id],servent[s_id].id,servent[send_target].id ,0,"f001");
				printf("servent %ld , send_data: %s , target servent:%d", s_id,servent_send_message[s_id],send_target);
				
				
	     			
	     			
	     			
	     			puts("");
	     			
	     			
	     					
	     			
						
	     					
	     			strcpy(servent_receive_message[s_id][send_target], servent_send_message[s_id]);//
	     					
	     			servent_transmit_tunnel_ready_signal[s_id][send_target] = 1 ;
	     			
	     					
	     			
	     			while(servent_transmit_times[s_id][send_target] != 0  || servent_transmit_times[send_target][s_id] != 0){
	     			
				sleep(0.1);
				}
				
				
				
				if(servent_transmit_times[s_id][send_target]== 0 && servent_transmit_times[send_target][s_id]==0){
				
				
				servent_tunnel_work_over[s_id][send_target]=1;
				servent_tunnel_work_over[send_target][s_id]=1;
						
				
				}
	     			servent_pattern[s_id]=99;
			 	
				break;
			case 3:
				if(servent_resurrection_request_peer_time[s_id] > 0){
				servent_resurrection_request_peer_time[s_id]--; 
				}
				servent_peer_num[s_id]=0;
				for(i=0;i<NUM_SERVENT_BOTS;i++){
				
				if(servent_peer_list[s_id][i].peer_id!= -1)
				servent_peer_num[s_id]++;
					
				} 
						
				
				send_target = servent_peer_list[s_id][rand() % (servent_peer_num[s_id])].peer_id  ;
				
				while(send_target==s_id){
    
				send_target = servent_peer_list[s_id][rand() % (servent_peer_num[s_id])].peer_id ;
				    
				}
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
				
				while(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value < reputation_value_min){
				
				send_target = servent_peer_list[s_id][rand() % (servent_peer_num[s_id])].peer_id ;
				i = 0;    
				}
				
				}
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
				
				while(servent_peer_list[s_id][i].peer_id == send_target && servent_eliminate_signal[send_target]==1){
				
				send_target = servent_peer_list[s_id][rand() % (servent_peer_num[s_id])].peer_id ;
				i = 0;    
				}
				
				}
				
				if(servent_latency_signal[send_target]==1){
				printf("servent %ld already turn to latency bot  \n",send_target);
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
					if(servent_peer_list[s_id][i].peer_id == send_target ){
						servent_peer_list[s_id][send_target].reputation_value--;
					}
				}
				
				servent_pattern[s_id]=99;
				break;
				}
				while(servent_tunnel_work_over[send_target][s_id]==0){
					sleep(0.1);
				
				}
				while(servent_tunnel_work_over[s_id][send_target]==0){
					sleep(0.1);
				
				}
				
				servent_tunnel_work_over[s_id][send_target]=0;        
				servent_transmit_data[s_id][send_target].from = s_id;
				servent_transmit_data[s_id][send_target].to = send_target;
				rc = pthread_create(&servent_transmit[s_id][send_target], NULL, servent_handle_transmit_func, &servent_transmit_data[s_id][send_target]); 
				servent_transmit_tunnel_ready_signal[s_id][send_target]=0 ;
				
				servent_transmit_times[s_id][send_target]=1;
				
				
				
				servent_tunnel_work_over[send_target][s_id]=0;        
				servent_transmit_data[send_target][s_id].from = send_target;
				servent_transmit_data[send_target][s_id].to = s_id;
				rc = pthread_create(&servent_transmit[send_target][s_id], NULL, servent_handle_transmit_func, &servent_transmit_data[send_target][s_id]);
				servent_transmit_tunnel_ready_signal[send_target][s_id]=0 ;

				
				
				servent_transmit_times[send_target][s_id]=1;
				memset(servent_send_message[s_id],0,strlen(servent_send_message[s_id]));
				servent_make_command(servent_send_message[s_id],servent[s_id].id,servent[send_target].id ,0,"f003");
				
				printf("servent %ld , send_data: %s , target servent:%d", s_id,servent_send_message[s_id],send_target);
				
				
	     			
	     			
	     			
	     			puts("");
	     			
	     			
	     					
	     			
						
	     					
	     			strcpy(servent_receive_message[s_id][send_target], servent_send_message[s_id]);//
	     					
	     			servent_transmit_tunnel_ready_signal[s_id][send_target] = 1 ;
	     			
	     					
	     			
	     			while(servent_transmit_times[s_id][send_target] != 0  || servent_transmit_times[send_target][s_id] != 0){
	     			
				sleep(0.1);
				}
				
				
				
				if(servent_transmit_times[s_id][send_target]== 0 && servent_transmit_times[send_target][s_id]==0){
				
				
				servent_tunnel_work_over[s_id][send_target]=1;
				servent_tunnel_work_over[send_target][s_id]=1;
						
				
				}
	     			servent_pattern[s_id]=99;
				break;	
				
			case 4:	
			        
			        
			        servent_peer_num[s_id]=0;
				for(i=0;i<NUM_SERVENT_BOTS;i++){
				
				if(servent_peer_list[s_id][i].peer_id!= -1)
				servent_peer_num[s_id]++;
					
				} 
				send_target = servent_peer_list[s_id][rand() % (servent_peer_num[s_id])].peer_id  ;
				
				while(send_target==s_id){
    
				send_target = servent_peer_list[s_id][rand() % (servent_peer_num[s_id])].peer_id ;
				    
				}
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
				
				while(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value < reputation_value_min){
				
				send_target = servent_peer_list[s_id][rand() % (servent_peer_num[s_id])].peer_id ;
				i = 0;    
				}
				
				}
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
				
				while(servent_peer_list[s_id][i].peer_id == send_target && servent_eliminate_signal[send_target]==1){
				
				send_target = servent_peer_list[s_id][rand() % (servent_peer_num[s_id])].peer_id ;
				i = 0;    
				}
				
				}
				if(servent_latency_signal[send_target]==1){
				printf("servent %ld already turn to latency bot  \n",send_target);
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
					if(servent_peer_list[s_id][i].peer_id == send_target ){
						servent_peer_list[s_id][send_target].reputation_value--;
					}
				}
				
				servent_pattern[s_id]=99;
				break;
				}//***
				while(servent_tunnel_work_over[send_target][s_id]==0){
					sleep(0.1);
				
				}
				while(servent_tunnel_work_over[s_id][send_target]==0){
					sleep(0.1);
				
				}
				
				servent_tunnel_work_over[s_id][send_target]=0;        
				servent_transmit_data[s_id][send_target].from = s_id;
				servent_transmit_data[s_id][send_target].to = send_target;
				rc = pthread_create(&servent_transmit[s_id][send_target], NULL, servent_handle_transmit_func, &servent_transmit_data[s_id][send_target]);
				servent_transmit_tunnel_ready_signal[s_id][send_target]=0 ;
				
				servent_transmit_times[s_id][send_target]=1;
				
				
				
				servent_tunnel_work_over[send_target][s_id]=0;        
				servent_transmit_data[send_target][s_id].from = send_target;
				servent_transmit_data[send_target][s_id].to = s_id;
				rc = pthread_create(&servent_transmit[send_target][s_id], NULL, servent_handle_transmit_func, &servent_transmit_data[send_target][s_id]);
				servent_transmit_tunnel_ready_signal[send_target][s_id]=0 ;

				
				
				servent_transmit_times[send_target][s_id]=1;
				memset(servent_send_message[s_id],0,strlen(servent_send_message[s_id]));
				strcpy(servent_send_message[s_id],"*1");//
				printf("servent %ld , send_data: %s , target servent:%d", s_id,servent_send_message[s_id],send_target);
				
				
	     			
	     			
	     			
	     			puts("");
	     			
	     			
	     					
	     			
						
	     					
	     			strcpy(servent_receive_message[s_id][send_target], servent_send_message[s_id]);//
	     					
	     			servent_transmit_tunnel_ready_signal[s_id][send_target] = 1 ;
	     			
	     					
	     			
	     			while(servent_transmit_times[s_id][send_target] != 0  || servent_transmit_times[send_target][s_id] != 0){
	     			
				sleep(0.1);
				}
				
				
				
				if(servent_transmit_times[s_id][send_target]== 0 && servent_transmit_times[send_target][s_id]==0){
				
				
				servent_tunnel_work_over[s_id][send_target]=1;
				servent_tunnel_work_over[send_target][s_id]=1;
						
				
				}
	     			servent_pattern[s_id]=99;
				break;	
			
			case (-1):
				printf(" servent %ld terminated !\n", s_id);
				
				break;	
		}
		sleep(1);
		for ( i = 0; i < servent_botmaster_command_buffer_pointer[s_id]; i++) {
		if(servent_botmaster_command_buffer[s_id][i].effective_time != 0){
			   servent_botmaster_command_buffer[s_id][i].effective_time--;
		}
		}
		
		for ( i = 0; i < servent_botmaster_command_buffer_pointer[s_id]; i++) {
	        for ( j = 0; j < i; j++) {
		       if (servent_botmaster_command_buffer[s_id][j].effective_time < servent_botmaster_command_buffer[s_id][i].effective_time) {
		       Command temp = servent_botmaster_command_buffer[s_id][j];
		       servent_botmaster_command_buffer[s_id][j] = servent_botmaster_command_buffer[s_id][i];
		       servent_botmaster_command_buffer[s_id][i] = temp;
		      	
		       }
	        }
                }
	   
		for ( i = 0; i < servent_botmaster_command_buffer_pointer[s_id]; i++) {
			   
			   if(servent_botmaster_command_buffer[s_id][i].effective_time == 0){
			   memset(&servent_botmaster_command_buffer[s_id][i],0,sizeof(Command));
			   servent_botmaster_command_buffer_pointer[s_id]--;
			   }
			   /*puts(servent_botmaster_command_buffer[s_id][i].content);
			   printf("%d \n",servent_botmaster_command_buffer[s_id][i].effective_time);*/
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
    
    
    client_master[i][j].reputation_value = reputation_value_base;//###
    //client_master[i][j].reputation_value = rand() % reputation_value_base ;
    client_master[i][j].master_id = rand() % NUM_SERVENT_BOTS  ;

    for (a = 0; a < j; a++) {
    
    while(client_master[i][a].master_id==client_master[i][j].master_id){
    client_master[i][j].master_id = rand() % NUM_SERVENT_BOTS;
    a = 0;
    }
    }
    }
    
    }
    
    for(i=0;i<NUM_CLIENT_BOTS;i++){
    puts("");
    printf("master of client %ld have:", i);
    for (j = 0; j < NUM_SERVENT_BOTS; j++) {
    
    
    if(client_master[i][j].master_id!= -1)
    	printf("%ld %ld  ", client_master[i][j].master_id, client_master[i][j].reputation_value);
    
    }	
    
    } 
    puts("");

}
void init_servent_peer_list(){
	
    int i,j,a;

    for (i = 0; i < NUM_SERVENT_BOTS; i++) {

    for (j = 0; j < 4; j++) {//*** max : NUM_SERVENT_BOTS-1

    servent_peer_list[i][j].reputation_value=reputation_value_base; 
    //servent_peer_list[i][j].reputation_value=rand() % reputation_value_base;
    
    servent_peer_list[i][j].peer_id = rand() % NUM_SERVENT_BOTS  ;
    
    
    while(servent_peer_list[i][j].peer_id==i){
    
    servent_peer_list[i][j].peer_id = rand() % NUM_SERVENT_BOTS ;
    
    }
    
    for (a = 0; a < j; a++) {

    while(servent_peer_list[i][j].peer_id == servent_peer_list[i][a].peer_id){
    servent_peer_list[i][j].peer_id = rand() % NUM_SERVENT_BOTS;
    while(servent_peer_list[i][j].peer_id == i){
    servent_peer_list[i][j].peer_id = rand() % NUM_SERVENT_BOTS;
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
    
    
    if(servent_peer_list[i][j].peer_id!= -1)
    	printf("%ld %ld  ", servent_peer_list[i][j].peer_id, servent_peer_list[i][j].reputation_value);

    
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
    strcpy(servent_bot_website_buffer[i][0].url, website[random]);
    servent_bot_website_buffer[i][0].reputation_value = reputation_value_base;
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
    for (i = 0; i < NUM_SERVENT_BOTS; i++){  	    	
    servent[i].id = i;
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
    client[i].id = i+NUM_SERVENT_BOTS;
    }
    		
    for (i = 0; i < NUM_SERVENT_BOTS; i++){
    printf("servent[%d]->id : %d\n",i,servent[i].id);
    printf("servent[%d]->ip\n",i);
    puts(servent[i].ip);
    printf("servent[%d]->port: %d\n",i,servent[i].port);
    }
    for (i = 0; i < NUM_CLIENT_BOTS; i++){
    printf("client[%d]->id : %d\n",i,client[i].id);
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
        servent_botmaster_command_buffer_pointer[i]=0;
        servent_bot_website_buffer_pointer[i]=0;
        servent_trust_threshold[i]=0;
        servent_latency_signal[i]=0;
        servent_resurrection_complete_signal[i]=0;
	servent_eliminate_signal[i]=0;
	servent_resurrection_request_peer_time[i]=0;
        for (j = 0; j < NUM_SERVENT_BOTS; j++){
		
		servent_peer_list[i][j].peer_id = -1;
		servent_tunnel_work_over[i][j]=1; 
		
	}

     }
     for (i = 0; i < NUM_CLIENT_BOTS; i++) {
    	client_pattern[i]=99;
        client_bot_command_buffer_pointer[i]=0;
        client_botmaster_command_buffer_pointer[i]=0;
        client_bot_website_buffer_pointer[i]=0;
        for (j = 0; j < NUM_SERVENT_BOTS; j++){
		
		client_master[i][j].master_id = -1;
		client_and_servent_tunnel_work_over[i][j]=1; 
		
	}

     }

    init_servent_peer_list();

    init_servent_website();

    init_client_master();

    init_servent_and_client_information();
    
     
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
   
    
    time(&current);
    info = localtime( &current );
    strftime(date,sizeof(date),"%Y.%m.%d",info);
    	
    


   
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
	
				/*for (i = 0; i < (NUM_SERVENT_BOTS); i++) {
				servent_pattern[i]=1;
				}*/
				servent_pattern[0]=1;
				//sleep(2);
				//client_pattern[0]=master_command;
				break;
				
			case 2:
				/*for (i = 0; i < (NUM_SERVENT_BOTS); i++) {
				servent_pattern[i]=1;
				}*/
				servent_pattern[0]=2;
				//sleep(2);
				//client_pattern[0]=master_command;
				
				break;				
			case 3:
				
					
				/*for (i = 0; i < (NUM_SERVENT_BOTS); i++) {
				servent_pattern[i]=1;
				}*/
				servent_pattern[0]=3;
				//sleep(2);
				//client_pattern[0]=master_command;		
				break;
			case 4:
				
				/*for (i = 0; i < (NUM_SERVENT_BOTS); i++) {
				servent_pattern[i]=1;
				}*/
				servent_pattern[0]=4;
				//sleep(2);
				//client_pattern[0]=master_command;	
				break;	
			case 5:
				servent_peer_list[0][1].reputation_value = reputation_value_max;
				break;	
			case 6:
				servent_peer_list[0][0].reputation_value = reputation_value_min-1;
				break;
			case 7:
				servent_trust_threshold[0]=4;
				
				break;	
			case 8:
				servent_trust_threshold[0]=4;
				servent_trust_threshold[1]=4;
				servent_trust_threshold[2]=4;
				servent_trust_threshold[3]=4;
				for (i = 0; i < (NUM_SERVENT_BOTS); i++) {
					for (j = 0; j < NUM_SERVENT_BOTS; j++){
						if(servent_peer_list[i][j].peer_id ==  4){
						servent_peer_list[i][j].reputation_value = reputation_value_base+3;
						}
						if(servent_peer_list[i][j].peer_id ==  5){
						servent_peer_list[i][j].reputation_value = reputation_value_base+3;
						}
						if(servent_peer_list[i][j].peer_id ==  6){
						servent_peer_list[i][j].reputation_value = reputation_value_base+3;
						}
						if(servent_peer_list[i][j].peer_id ==  7){
						servent_peer_list[i][j].reputation_value = reputation_value_base+3;
						}
					}
				}
				break;	//***
									
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
