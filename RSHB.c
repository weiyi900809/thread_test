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

// gcc -mcmodel=large thread_test_2.c  -lpthread -o test


#define NUM_SERVENT_PEER 10//NUM_SERVENT_BOTS-1

#define NUM_BOTS 5
#define reputation_value_base 0
#define reputation_value_max 5
#define reputation_value_min 0

#define NUM_SERVENT_BOTS 4800
#define NUM_FAKE_SERVENT_BOTS 100
#define NUM_SENSOR 100
#define NUM_CLIENT_BOTS 15000



#define NUM_OF_SELECT_PATTERN_TIMES 240

/*int NUM_SERVENT_BOTS =1000;
int NUM_FAKE_SERVENT_BOTS =10;
int NUM_CLIENT_BOTS =500;*/

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
int detect_signal;
int detect_and_reply_signal;
int request_signal;	
int fake_signal;
int sensor_signal;
}Bot; 

typedef struct  peer{

long peer_id;
long reputation_value;
int trust_signal;
int already_send_TM_signal;	

}Peer; 

typedef struct  master{

long master_id;
long reputation_value;
int trust_signal;
int already_send_TM_signal;
	
}Master; 

typedef struct  command{

char content[1024];
int effective_time;
int sender;//self
int receiver;//target or botmaster(9999)
char timestamp[1024];
char extra_information[1024];  
char command_code[2048]; 

}Command; 

typedef struct  website{

char url[1024];
int reputation_value;

	
}Website;

typedef struct  log{


int num_pattern;
long communication_target;
int communication_result;
long receive_peer;
long send_peer;
int now_sec;	
}Log;
typedef struct  pc{


int servent_or_client;
long bot_ID;

	
}PC;

typedef struct  period{


int start_time;
int end_time;
int off_peak_to_peak;
int peak_to_off_peak;
	
}Period;

typedef struct  latency{


int servent_latency_num;
int crawler_latency_num;
int sensor_latency_num;

}Latency;

int abc_signal=0;
PC device[100000];
int already_register_PC[100000];
int already_register_servent[5000];
int already_register_client[15000];
int already_register_PC_num=0;

Period change_period_time[200];
int change_period_time_pointer=0;

int servent_latency_num_temp=0;
int crawler_latency_num_temp=0;
int sensor_latency_num_temp=0;

Latency latency_num_record_array[200];
int latency_num_record_pointer=0;

int change_time_period_signal=0;
int offpeak_to_peak_time=0;
int all_servent_try_to_turn_latency_signal=0;
int program_terminate_signal=0;
int pattern_execute_time=0;
int boot_control_terminate_signal=0;
int program_start_time=0;
int inject_signal=0;
int enumeration_signal=0;
int write_botmaster_command_time=0;
int enumeration_start_time=0;
int request_hour=0;
long request_time[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR];
long request_time_largest=0;
long request_time_record_array[100];
int request_time_record_pointer=0;
int request_time_last_record_time=0;
int request_time_record_terminate_signal=0;
int write_botmaster_command_signal=0;
int servent_bot_num_now=50;
int servent_thread_num_now=1;
int servent_thread_num_last_time=0;
int client_thread_num_now=3;
int client_thread_num_last_time=0;
int client_bot_num_now=150;
int infect_terminate_signal=0;
char date[1024];
int hour=0,min=0,sec=0,vrc=0,vc=0;
int vs=0;
char s_hour[1024],s_min[1024],s_sec[1024];  
Bot servent[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR];
Bot client[NUM_CLIENT_BOTS];  



Log servent_log[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR][1000];
Log client_log[NUM_CLIENT_BOTS][1000];
int servent_execute_pattern_time[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR];
int client_execute_pattern_time[NUM_CLIENT_BOTS];

int set_of_vc[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR];
int servent_thread_work_over[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR/50];  // /1000
int fake_servent_thread_work_over[NUM_FAKE_SERVENT_BOTS/5]; // /1000
int sensor_thread_work_over[NUM_SENSOR/5];

int client_thread_work_over[NUM_CLIENT_BOTS/50];// /1000
char file_data[10][10][1024]; 
char already_exist_ip[NUM_CLIENT_BOTS+NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR][1024];
int already_exist_ip_num=0;
int already_exist_port[NUM_CLIENT_BOTS+NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR];
int already_exist_port_num=0;
int relay_station_terminate_signal=0;
int data_record_terminate_signal=0;
int limit=reputation_value_base+2;
int survive_servent_num=0;


//servent----------------------------------
//-----------------------------------------
long servent_pattern[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR]; 
int servent_select_pattern_signal[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR],servent_last_time_select_pattern[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR];
Command servent_command_buffer[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR][100];
int servent_command_buffer_pointer[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR];
Website servent_bot_website_buffer[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR][100]; 
int servent_bot_website_buffer_pointer[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR];
int servent_send_trust_message_to_me[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR][NUM_SERVENT_PEER];
int servent_send_trust_message_to_me_pointer[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR];
int select_again_signal[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR];
//-----------------------------------------

int servent_work_over[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR];
//-----------------------------------------
Peer servent_peer_list[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR][NUM_SERVENT_PEER];
int servent_peer_num[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR];
int servent_trust_threshold[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR];
int servent_latency_signal[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR];
int servent_resurrection_complete_signal[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR];
int servent_eliminate_signal[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR];
int servent_resurrection_request_peer_signal[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR]; 

int servent_already_turn_to_latency_signal[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR];
int servent_already_execute_resurrection_signal[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR];
int servent_boot_signal[NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR];
//-----------------------------------------
/*int servent_client_list[NUM_SERVENT_BOTS][NUM_SERVENT_BOTS];
int servent_client_num[NUM_SERVENT_BOTS];*/
//client-----------------------------------
//-----------------------------------------
long client_pattern[NUM_CLIENT_BOTS]; 
int client_select_pattern_signal[NUM_CLIENT_BOTS],client_last_time_select_pattern[NUM_CLIENT_BOTS];
Command client_command_buffer[NUM_CLIENT_BOTS][100]; 
int client_command_buffer_pointer[NUM_CLIENT_BOTS];
Website client_bot_website_buffer[NUM_CLIENT_BOTS][100]; 
int client_bot_website_buffer_pointer[NUM_CLIENT_BOTS];
//-----------------------------------------

//char client_send_message[1024];
int client_work_over[NUM_CLIENT_BOTS];
//-----------------------------------------
Master client_master[NUM_CLIENT_BOTS][NUM_SERVENT_PEER];
int client_master_num[NUM_CLIENT_BOTS];
int client_exchange_servent_target[NUM_CLIENT_BOTS];
int client_eliminate_signal[NUM_CLIENT_BOTS];
int client_boot_signal[NUM_CLIENT_BOTS];
struct tm *info;
time_t current = 0;


pthread_mutex_t mutex[NUM_BOTS][NUM_BOTS];

pthread_attr_t attr;            
pthread_t servent_threads[(NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR)/50]; // /1000
pthread_t client_threads[NUM_CLIENT_BOTS/50];// /1000
pthread_t fake_servent_threads[NUM_FAKE_SERVENT_BOTS/5];// /1000
pthread_t sensor_threads[NUM_SENSOR/5];// /1000
pthread_t relay_station;
pthread_t data_record;
pthread_t infect_and_inject_enumeration;
pthread_t  boot_control;

void servent_rearrange_peer(int s_id){
	
	int i=0,j=0;
	servent_peer_num[s_id]=0;
	for(i=0;i<NUM_SERVENT_PEER;i++){
	if(servent_peer_list[s_id][i].peer_id!= -1)
	servent_peer_num[s_id]++;
	} 
	
	if(servent_peer_num[s_id]!=0){
	for(i=0;i<NUM_SERVENT_PEER;i++){
			if(servent_peer_list[s_id][i].peer_id != -1 && i>=servent_peer_num[s_id]){
			for ( j = 0; j < servent_peer_num[s_id]; j++) {
			if(servent_peer_list[s_id][j].peer_id == -1){
			    	   Peer temp = servent_peer_list[s_id][j];
				   servent_peer_list[s_id][j] = servent_peer_list[s_id][i];
				   servent_peer_list[s_id][i] = temp;
				   break;
			}
			   
			}
			   
			}
			   
	}
			   
	}
	
}
void servent_sort_peer(int s_id){
	int i=0,j=0;
	servent_peer_num[s_id]=0;
	for(i=0;i<NUM_SERVENT_PEER;i++){
	if(servent_peer_list[s_id][i].peer_id!= -1)
	servent_peer_num[s_id]++;
	} 
	for ( i = 0; i < servent_peer_num[s_id]; i++) {
		for ( j = 0; j < i; j++) {
		    if (servent_peer_list[s_id][j].reputation_value > servent_peer_list[s_id][i].reputation_value 
		    && servent_peer_list[s_id][i].peer_id!=-1 && servent_peer_list[s_id][j].peer_id!=-1 ) {
		    Peer temp = servent_peer_list[s_id][j];
		    servent_peer_list[s_id][j] = servent_peer_list[s_id][i];
		    servent_peer_list[s_id][i] = temp;
		      	
		    }
		}
		}
}
void client_rearrange_peer(int c_id){
	int i=0,j=0;
	client_master_num[c_id]=0;
	for(i=0;i<NUM_SERVENT_PEER;i++){
	if(client_master[c_id][i].master_id!= -1)
	client_master_num[c_id]++;
	} 
	
	for(i=0;i<NUM_SERVENT_PEER;i++){
		if(client_master[c_id][i].master_id != -1 && i>=client_master_num[c_id]){
			for ( j = 0; j < client_master_num[c_id]; j++) {
				if(client_master[c_id][j].master_id == -1){
				Master temp = client_master[c_id][j];
				client_master[c_id][j] = client_master[c_id][i];
				client_master[c_id][i] = temp;
				break;
				}
						   
			}
					   
		}
				   
	}
}
void client_sort_peer(int c_id){
	int i=0,j=0;
	client_master_num[c_id]=0;
	for(i=0;i<NUM_SERVENT_PEER;i++){
	if(client_master[c_id][i].master_id!= -1)
	client_master_num[c_id]++;
	} 
	for ( i = 0; i < client_master_num[c_id]; i++) {
	   for ( j = 0; j < i; j++) {
	       if (client_master[c_id][j].reputation_value > client_master[c_id][i].reputation_value 
	       && client_master[c_id][i].master_id !=-1 && client_master[c_id][j].master_id !=-1) { 
	       Master temp = client_master[c_id][j];
	       client_master[c_id][j] = client_master[c_id][i];
	       client_master[c_id][i] = temp;
	      	
	      }
	   }
	}
}
void servent_make_command(char *func_result, int sender_id, int receive_id, int extra_information_target, char *send_command_code){
   int i,j,format=rand() % 3+1;
   char text[80];
   time(&current);
   info = localtime( &current );
   strftime(date,sizeof(date),"%Y.%m.%d",info);
   strftime(s_hour,sizeof(s_hour),"%H",info);
   hour = atoi(s_hour);	
   strftime(s_min,sizeof(s_min),"%M",info);
   min = atoi(s_min);	
   strftime(s_sec,sizeof(s_sec),"%S",info);
   sec = atoi(s_sec);	
   
   sec+=(60*min)+(60*60*hour);
   //printf("format = %d \n", format);
   
   if(strcmp(send_command_code,"f003") == 0 || strcmp(send_command_code,"f004") == 0){
   if(servent_already_execute_resurrection_signal[sender_id] == 1){
	for(i=0;i<NUM_SERVENT_PEER;i++){
	if(servent_peer_list[sender_id][i].peer_id != -1 && servent_eliminate_signal[servent_peer_list[sender_id][i].peer_id] == 1){
	servent_peer_list[sender_id][i].peer_id = -1;
	servent_peer_list[sender_id][i].reputation_value = 0;
	//servent_peer_list[sender_id][i].sensor_signal = 0;	   
   }
		   
   }
   }
   
   servent_peer_num[sender_id]=0;
   for(i=0;i<NUM_SERVENT_PEER;i++){
   if(servent_peer_list[sender_id][i].peer_id!= -1)
   servent_peer_num[sender_id]++;
   } 
   if(servent_peer_num[sender_id]!=0){
   for(i=0;i<NUM_SERVENT_PEER;i++){
		if(servent_peer_list[sender_id][i].peer_id != -1 && i>=servent_peer_num[sender_id]){
		for ( j = 0; j < servent_peer_num[sender_id]; j++) {
		if(servent_peer_list[sender_id][j].peer_id == -1){
		    	   Peer temp = servent_peer_list[sender_id][j];
			   servent_peer_list[sender_id][j] = servent_peer_list[sender_id][i];
			   servent_peer_list[sender_id][i] = temp;
			   break;
		}
		   
		}
		   
		}
		   
   }
		   
   }			
   for ( i = 0; i < servent_peer_num[sender_id]; i++) {
   for ( j = 0; j < i; j++) {
       if (servent_peer_list[sender_id][j].reputation_value > servent_peer_list[sender_id][i].reputation_value && servent_peer_list[sender_id][i].peer_id!=-1  && servent_peer_list[sender_id][j].peer_id!=-1 ) { 
       Peer temp = servent_peer_list[sender_id][j];
       servent_peer_list[sender_id][j] = servent_peer_list[sender_id][i];
       servent_peer_list[sender_id][i] = temp;
      	
       }
     }
   }
   int send_peer_from_sensor_or_crawler=0;
   send_peer_from_sensor_or_crawler=rand() % 100 + (NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS);
   switch(format) {
   	case 1:
   		
   		strncat(func_result,send_command_code ,strlen(send_command_code));//command_code
   		strcat(func_result,";");
   		sprintf(text, "%s:%d:%d", servent[servent_peer_list[sender_id][servent_peer_num[sender_id]-1].peer_id].ip,servent[servent_peer_list[sender_id][servent_peer_num[sender_id]-1].peer_id].port,servent_peer_list[sender_id][servent_peer_num[sender_id]-1].peer_id  );
   		//extra information
   		strncat(func_result,text ,strlen(text));//extra information
                strcat(func_result,";");
   		sprintf(text, "%d", sender_id);  //sender
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%d", receive_id);//receicer
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%s.%d", date,sec);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%d",NUM_OF_SELECT_PATTERN_TIMES*4);//effective time
   		strncat(func_result,text ,strlen(text));
   		
   		//puts(func_result);
   		
   		
   		break;
   	case 2:
   		strcat(func_result,"[");
   		sprintf(text, "%s:%d:%d", servent[servent_peer_list[sender_id][servent_peer_num[sender_id]-1].peer_id].ip,servent[servent_peer_list[sender_id][servent_peer_num[sender_id]-1].peer_id].port,servent_peer_list[sender_id][servent_peer_num[sender_id]-1].peer_id  );
   		//extra information
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
   		sprintf(text, "%s.%d", date,sec);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		sprintf(text, "%d",NUM_OF_SELECT_PATTERN_TIMES*4);//effective time
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		
   		//puts(func_result);
   		break;
   	case 3:
   		sprintf(text, "%s.%d", date,sec);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"||");
   		sprintf(text, "%d",NUM_OF_SELECT_PATTERN_TIMES*4);//effective time
   		strncat(func_result,text ,strlen(text));
                strcat(func_result,"||");
                sprintf(text, "%s:%d:%d", servent[servent_peer_list[sender_id][servent_peer_num[sender_id]-1].peer_id].ip,servent[servent_peer_list[sender_id][servent_peer_num[sender_id]-1].peer_id].port,servent_peer_list[sender_id][servent_peer_num[sender_id]-1].peer_id  );
   		//extra information
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
   		sprintf(text, "%s.%d", date,sec);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%d",NUM_OF_SELECT_PATTERN_TIMES*4);//effective time
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
   		sprintf(text, "%s.%d", date,sec);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		sprintf(text, "%d",NUM_OF_SELECT_PATTERN_TIMES*4);//effective time
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		
   		//puts(func_result);
   		break;
   	case 3:
   		sprintf(text, "%s.%d", date,sec);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"||");
   		sprintf(text, "%d",NUM_OF_SELECT_PATTERN_TIMES*4);//effective time
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
void client_make_command(char *func_result, int sender_id, int receive_id, int extra_information_target, char *send_command_code){
   int format=rand() % 3+1;
   char text[80];
   time(&current);
   info = localtime( &current );
   strftime(date,sizeof(date),"%Y.%m.%d",info);
   strftime(s_hour,sizeof(s_hour),"%H",info);
   hour = atoi(s_hour);	
   strftime(s_min,sizeof(s_min),"%M",info);
   min = atoi(s_min);	
   strftime(s_sec,sizeof(s_sec),"%S",info);
   sec = atoi(s_sec);	
    
   sec+=(60*min)+(60*60*hour);


   if(strcmp(send_command_code,"f003") == 0 || strcmp(send_command_code,"f004") == 0){
   int i=0,j=0,target_servent=0,c_id = sender_id-NUM_SERVENT_BOTS-NUM_FAKE_SERVENT_BOTS-NUM_SENSOR;
   
   for(i=0;i<NUM_SERVENT_PEER;i++){
   if(client_master[c_id][i].master_id != -1 && servent_eliminate_signal[client_master[c_id][i].master_id] == 1){

   client_master[c_id][i].master_id = -1;
   client_master[c_id][i].reputation_value = 0;

   }
		   
   }
   client_master_num[c_id]=0;
   for(i=0;i<NUM_SERVENT_PEER;i++){
				
   if(client_master[c_id][i].master_id != -1){
   client_master_num[c_id]++;
   }
							
   }
   if(client_master_num[c_id]!=0){
   for(i=0;i<NUM_SERVENT_PEER;i++){
		if(client_master[c_id][i].master_id != -1 && i>=client_master_num[c_id]){
		for ( j = 0; j < client_master_num[c_id]; j++) {
		if(client_master[c_id][j].master_id == -1){
		    	   Master temp = client_master[c_id][j];
			   client_master[c_id][j] = client_master[c_id][i];
			   client_master[c_id][i] = temp;
			   break;
		}
		   
		}
		   
		}
		   
   }
		   
   }	
   		
   for ( i = 0; i < client_master_num[c_id]; i++) {
   for ( j = 0; j < i; j++) {
       if (client_master[c_id][j].reputation_value > client_master[c_id][i].reputation_value && client_master[c_id][i].master_id!=-1  && client_master[c_id][j].master_id!=-1 ) {
       Master temp = client_master[c_id][j];
       client_master[c_id][j] = client_master[c_id][i];
       client_master[c_id][i] = temp;
      	
       }
     }
   }
   //s5s5
   
   
   target_servent = client_master[c_id][client_master_num[c_id]-1].master_id;
   
   switch(format) {
   	case 1:
   		
   		strncat(func_result,send_command_code ,strlen(send_command_code));//command_code
   		
   		strcat(func_result,";");
   		sprintf(text, "%s:%d:%d", servent[target_servent].ip, servent[target_servent].port,target_servent );//extra information
   		strncat(func_result,text ,strlen(text));
                strcat(func_result,";");
   		sprintf(text, "%d", sender_id);  //sender
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%d", receive_id);//receicer
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%s.%d", date,sec);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%d",NUM_OF_SELECT_PATTERN_TIMES*4);//effective time
   		strncat(func_result,text ,strlen(text));
   		
   		//puts(func_result);
   		
   		
   		break;
   	case 2:
   		strcat(func_result,"[");
   		sprintf(text, "%s:%d:%d", servent[target_servent].ip, servent[target_servent].port,target_servent );//extra information
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
   		sprintf(text, "%s.%d", date,sec);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		sprintf(text, "%d",NUM_OF_SELECT_PATTERN_TIMES*4);//effective time
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		
   		//puts(func_result);
   		break;
   	case 3:
   		sprintf(text, "%s.%d", date,sec);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"||");
   		sprintf(text, "%d",NUM_OF_SELECT_PATTERN_TIMES*4);//effective time
   		strncat(func_result,text ,strlen(text));
                strcat(func_result,"||");
   		sprintf(text, "%s:%d:%d", servent[target_servent].ip, servent[target_servent].port,target_servent );//extra information
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
   		sprintf(text, "%s.%d", date,sec);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,";");
   		sprintf(text, "%d",NUM_OF_SELECT_PATTERN_TIMES*4);//effective time
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
   		sprintf(text, "%s.%d", date,sec);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		strcat(func_result,"[");
   		sprintf(text, "%d",NUM_OF_SELECT_PATTERN_TIMES*4);//effective time
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"]");
   		
   		//puts(func_result);
   		break;
   	case 3:
   		sprintf(text, "%s.%d", date,sec);//timestamp
   		strncat(func_result,text ,strlen(text));
   		strcat(func_result,"||");
   		sprintf(text, "%d",NUM_OF_SELECT_PATTERN_TIMES*4);//effective time
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
void strchrn(char *dest,char *src, int begin, int end) { //456
   int range=end-begin-1;
   char data[1024];
   memset(data,0,strlen(data));
   strncpy(data, src+begin , range);
   data[strlen(data)] = '\0';   
   strcpy(dest ,data);
   
   
}
void timestamp_split(char src[], char func_year[], char func_month[], char func_day[], char func_sec[]) {//xxx
   int i=0,j=0,dot_pointer[10];
   char func_result[10][2048];
   
   //puts(src);
   
   for(i=0;i<4;i++){
   
   while(src[j] != '.'){
   if(src[j] == '\0')break;
   		
   j++;
   }
   	
   j++;
   dot_pointer[i]=j;
   if(i==0){
   strchrn(func_result[i],src,0,dot_pointer[i]);
   strcpy(func_year ,func_result[i]);
   //puts(func_year);
   }
   if(i==1){
   strchrn(func_result[i],src,dot_pointer[i-1],dot_pointer[i]);
   strcpy(func_month ,func_result[i]);
   //puts(func_month);
   }
   if(i==2){
   strchrn(func_result[i],src,dot_pointer[i-1],dot_pointer[i]);
   strcpy(func_day ,func_result[i]);
   //puts(func_day);
   
   }
   if(i==3){
   
   strchrn(func_result[i],src,dot_pointer[i-1],dot_pointer[i]);
   strcpy(func_sec ,func_result[i]);
   //puts(func_sec );
   }
   }
   //xxxx
    
}
void extra_information_split(char src[], char func_ip[], char func_port[], char func_id[]) {//xxx
   int i=0,j=0,dot_pointer[10];
   char func_result[10][2048];
   
   //puts(src);
   
   for(i=0;i<3;i++){
   
   while(src[j] != ':'){
   if(src[j] == '\0')break;
   		
   j++;
   }
   	
   j++;
   dot_pointer[i]=j;
   if(i==0){
   strchrn(func_result[i],src,0,dot_pointer[i]);
   strcpy(func_ip ,func_result[i]);
   //puts(func_ip);
   }
   if(i==1){
   strchrn(func_result[i],src,dot_pointer[i-1],dot_pointer[i]);
   strcpy(func_port ,func_result[i]);
   //puts(func_port);
   }
   if(i==2){
   strchrn(func_result[i],src,dot_pointer[i-1],dot_pointer[i]);
   strcpy(func_id ,func_result[i]);
   //puts(func_id);
   
   }
   
   }
   //xxxx
    
}

int servent_command_analysis(int s_id,char *servent_command){
   int format=0,segment_pointer[10];
   int i=0,j=0;
   char segment[1024];
   char result[10][1024];
   char string_now_year[1024],string_now_month[1024],string_now_day[1024];//xxx
   char string_now_hour[1024],string_now_min[1024],string_now_sec[1024];
   char string_command_year[4096],string_command_month[4096],string_command_day[4096],string_command_sec[4096];
   int now_year,now_month,now_day,now_hour,now_min,now_sec;
   int command_year,command_month,command_day,command_sec;
   int deadline_year,deadline_month,deadline_day,deadline_sec;
   int delete_command_signal=0;
   for(i=0;i<10;i++){
   segment_pointer[i]=0;
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
   		strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].command_code ,result[i]);
   		//puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].command_code);
   		}
   		if(i==1){
   		
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]);
   		strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].extra_information ,result[i]);
   		//puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].extra_information);
   		}
   		if(i==2){
   		
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]);
   		servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].sender = atoi(result[i]);
   		//printf("%d \n",servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].sender);
   		}
   		if(i==3){
   		
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]);
   		
   		servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].receiver = atoi(result[i]);
   		//printf("%d \n",servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].receiver);
   		}
   		if(i==4){
   		
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]);
   		strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].timestamp ,result[i]);
   		//puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].timestamp);
   		}
   		if(i==5){
   		
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]);
   		servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].effective_time = atoi(result[i]);
   		//printf("%d \n",servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].effective_time);
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
   		strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].extra_information ,result[i]);
   		//puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].extra_information);
   		}
   		if(i==1){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].command_code ,result[i]);
   		//puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].command_code);
   		}
   		if(i==2){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]-1);
   		servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].sender = atoi(result[i]);
   		//printf("%d \n",servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].sender);
   		}
   		if(i==3){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]);
   		
   		servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].receiver = atoi(result[i]);
   		//printf("%d \n",servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].receiver);
   		}
   		if(i==4){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].timestamp ,result[i]);
   		//puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].timestamp);
   		}
   		if(i==5){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]-1);
   		servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].effective_time = atoi(result[i]);
   		//printf("%d \n",servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].effective_time);
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
   		strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].timestamp ,result[i]);
   		//puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].timestamp);
   		}
   		if(i==1){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]-1);
   		servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].effective_time = atoi(result[i]) ;
   		//printf("%d \n",servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].effective_time);
   		
   		}
   		if(i==2){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].extra_information ,result[i]);
   		//puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].extra_information);
   		}
   		if(i==3){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]-1);
   		
   		strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].command_code ,result[i]);
   		//puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].command_code);
   		
   		}
   		if(i==4){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]-1);
   		servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].sender = atoi(result[i]);
   		//printf("%d \n",servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].sender);
   		
   		}
   		if(i==5){
   		strchrn(result[i],servent_command,segment_pointer[i-1],segment_pointer[i]);
   		servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].receiver = atoi(result[i]);
   		//printf("%d \n",servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].receiver);
   		}
   		
   		}
   		
   		for(i=0;i<6;i++){
   		memset(result[i],0,strlen(result[i]));
   		}
   		
   		break;
   	case 0:
   		break;	    
   }
  
  		time(&current);
		info = localtime( &current );
		
		strftime(string_now_year,sizeof(string_now_year),"%Y",info);
		now_year = atoi(string_now_year);
		strftime(string_now_month,sizeof(string_now_month),"%m",info);
		now_month = atoi(string_now_month);
		strftime(string_now_day,sizeof(string_now_day),"%d",info);
		now_day = atoi(string_now_day);
		
		strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
		now_hour = atoi(string_now_hour);	
		strftime(string_now_min,sizeof(string_now_min),"%M",info);
		now_min = atoi(string_now_min);	
		strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
		now_sec = atoi(string_now_sec);	
		now_sec+=(60*now_min)+(60*60*now_hour);
		
  		if(strlen(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content) != 0){
			
			//puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
			timestamp_split(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].timestamp ,
			string_command_year,string_command_month,string_command_day,string_command_sec);
			
			command_year = atoi(string_command_year);	
			command_month = atoi(string_command_month);	
			command_day = atoi(string_command_day);	
			command_sec = atoi(string_command_sec);	
			//printf("effective_time %d  !\n", servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].effective_time);
			deadline_sec = command_sec + servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].effective_time;
			deadline_day = command_day ;
			deadline_month = command_month ;
			deadline_year = command_year ;
			int leap_year=0;
			if((deadline_year % 4 == 0 && deadline_year % 100 != 0 ) || deadline_year % 400 == 0) {
			leap_year = 1;
			}
			//printf("leap_year %d  !\n", leap_year);
			
			if(deadline_sec > 86400 ){
			deadline_sec = deadline_sec-86400;
			deadline_day++;
			}
			if( (deadline_month == 4 || deadline_month == 6 || deadline_month == 9 || deadline_month == 11) && deadline_day > 30 ){
			deadline_day = 1;
			deadline_month++;
			}
			if( (deadline_month == 1 || deadline_month == 3 || deadline_month == 5 || deadline_month == 7 || deadline_month == 10 || deadline_month == 12 ) && deadline_day > 31 ){
			deadline_day = 1;
			deadline_month++;
			}
			if( leap_year == 1 && ( deadline_month == 2) && ( deadline_day > 29) ){
			deadline_day = 1;
			deadline_month++;
			}
			if( leap_year == 0 && ( deadline_month == 2) && ( deadline_day > 28) ){
			deadline_day = 1;
			deadline_month++;
			}
			if( deadline_month > 12 ){
			deadline_month = 1;
			deadline_year++;
			}
			
			
			if(now_year > deadline_year){
			delete_command_signal = 1;
			}
			if(now_year == deadline_year){
				if(now_month > deadline_month){
				delete_command_signal = 1;
				}
				if(now_month == deadline_month){
					if(now_day > deadline_day){
					delete_command_signal = 1;
					}
					if(now_day == deadline_day){
						if(now_sec >= deadline_sec){
						delete_command_signal = 1;
						}
					}
					
				}
				
			}
			
			
			if(delete_command_signal == 1){
			printf("this command is over deadline \n");
		        memset(&servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1],0,sizeof(Command));
			servent_command_buffer_pointer[s_id]--;
			
			return 0;
			}
			
		}
	   	
   int command_comparison_result=1,
   extra_information_comparison_result=1,
   receiver_comparison_result=1,
   sender_comparison_result=1,
   command_code_comparison_result=1;
   
   //printf("servent_command_buffer_pointer[s_id] %d  !\n",servent_command_buffer_pointer[s_id] );
   if((servent_command_buffer_pointer[s_id]-1) >= 0){
   
   for(i=0;i<servent_command_buffer_pointer[s_id];i++){	
	   if(i != servent_command_buffer_pointer[s_id]-1){
	   	
		extra_information_comparison_result=strcmp(servent_command_buffer[s_id][i].extra_information,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].extra_information);
		//printf("extra_information_comparison_result %d  !\n",extra_information_comparison_result );
		
		if(servent_command_buffer[s_id][i].receiver == servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].receiver){
		receiver_comparison_result=0;
		}
		//printf("receiver_comparison_result %d  !\n",receiver_comparison_result );
		
		if(servent_command_buffer[s_id][i].sender == servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].sender){
		sender_comparison_result=0;
		}
		//printf("sender_comparison_result %d  !\n",sender_comparison_result );
		
		command_code_comparison_result=strcmp(servent_command_buffer[s_id][i].command_code,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].command_code);
		//printf("command_code_comparison_result %d  !\n",command_code_comparison_result );
		
		if(extra_information_comparison_result == 0 && sender_comparison_result == 0 && receiver_comparison_result == 0 && command_code_comparison_result == 0){
		command_comparison_result=0;
		//printf("command_comparison_result %d  !\n",command_comparison_result );
		break;
		}
	   }
		
   }
   }
   
   
   							
   if(command_comparison_result == 0 && enumeration_signal == 1){
   printf("servent %ld already have (%s) \n",s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
   memset(&servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1],0,sizeof(Command));
   servent_command_buffer_pointer[s_id]--;
   return 0;				
   }
   
   	
   if(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].receiver == servent[s_id].id ||  servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].receiver == 9999){ 
   printf("servent %d this command is to me \n",s_id);
   
   
   if(strcmp(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].command_code,"f005") == 0){
   
   printf("servent %ld receive trust message !\n",s_id);
   if(servent_latency_signal[s_id]!=1){
   servent_trust_threshold[s_id]++;
   }
   
   servent_send_trust_message_to_me[s_id][servent_send_trust_message_to_me_pointer[s_id]] = servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].sender;
   servent_send_trust_message_to_me_pointer[s_id]++;
   }
   
   if(strcmp(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].command_code,"f006") == 0){
   	printf("servent %d cannt communicate servent %d anymore \n",s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].sender);
   	servent_peer_num[s_id]=0;
	for(i=0;i<NUM_SERVENT_PEER;i++){
	if(servent_peer_list[s_id][i].peer_id!= -1)
	servent_peer_num[s_id]++;
	}
		
	if(servent_peer_num[s_id]!=0){
		
		for(i=0;i<NUM_SERVENT_PEER;i++){
			if(servent_peer_list[s_id][i].peer_id != -1 && i>=servent_peer_num[s_id]){
				for ( j = 0; j < servent_peer_num[s_id]; j++) {
					if(servent_peer_list[s_id][j].peer_id == -1){
					    	   Peer temp = servent_peer_list[s_id][j];
						   servent_peer_list[s_id][j] = servent_peer_list[s_id][i];
						   servent_peer_list[s_id][i] = temp;
						   break;
					}
				   
				}
			   
			}
		   
		}
		   
	}
						
	for ( i = 0; i < servent_peer_num[s_id]; i++) {	
		if(servent_peer_list[s_id][i].peer_id == servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].sender && servent_peer_list[s_id][i].trust_signal == 0){
		servent_peer_list[s_id][i].trust_signal = 1;	
		}
	}
         
   }
   
   
   if(strcmp(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].command_code,"f007") == 0){//***-
   
   
   
   if(servent_already_execute_resurrection_signal[s_id]!=1){
   printf("servent %d execute command f007 \n",s_id);
   
   //printf("!!!%d \n",servent_latency_signal[s_id]);
   
   if(servent_latency_signal[s_id]==1){
   
   servent_peer_num[s_id]=0;
   for(i=0;i<NUM_SERVENT_PEER;i++){
   if(servent_peer_list[s_id][i].peer_id!= -1)
   servent_peer_num[s_id]++;
   }
   for(i=0;i<servent_peer_num[s_id];i++){
   if(servent_peer_list[s_id][i].reputation_value <= limit && servent_peer_list[s_id][i].peer_id != -1 ){
   
       printf("servent %d  remove servent %d\n" ,s_id,servent_peer_list[s_id][i].peer_id);
       servent_peer_list[s_id][i].peer_id = -1;
       servent_peer_list[s_id][i].reputation_value = 0;
       
       
   }
   /*if( servent_peer_list[s_id][i].peer_id !=-1 ){
   servent_peer_list[s_id][i].trust_signal = 0;
   //servent_peer_list[s_id][i].already_send_TM_signal = 0;
   }*/
   
   
   }

   
   servent_resurrection_request_peer_signal[s_id] = 1; 
   
   }
   
   if(servent_latency_signal[s_id]==0 ){
   
   int high_confidence_num = 0;
   servent_peer_num[s_id]=0;
   for(i=0;i<NUM_SERVENT_PEER;i++){
   if(servent_peer_list[s_id][i].peer_id!= -1)
   servent_peer_num[s_id]++;
   }
   for(i=0;i<servent_peer_num[s_id];i++){
   if(servent_peer_list[s_id][i].reputation_value > limit && servent_peer_list[s_id][i].peer_id !=-1 ){
       high_confidence_num++;
       servent_peer_list[s_id][i].trust_signal = 0;
       //servent_peer_list[s_id][i].already_send_TM_signal = 0;
   }
   else if(servent_peer_list[s_id][i].reputation_value <= limit && servent_peer_list[s_id][i].peer_id !=-1 ){
   
       printf("servent %d  remove servent %d\n" ,s_id,servent_peer_list[s_id][i].peer_id);
       servent_peer_list[s_id][i].peer_id = -1;
       servent_peer_list[s_id][i].reputation_value = 0;
       
   }

   }
   if(high_confidence_num < 6){
   printf("servent %d have not enough high confidence peers  \n" ,s_id);
   servent_eliminate_signal[s_id] = 1;
   
   }
   if(high_confidence_num >= 6){
   servent_resurrection_request_peer_signal[s_id] = 1;
   
   }
   
   
   
   

   				
   
   
   
   }
   servent_already_execute_resurrection_signal[s_id]=1;
   servent_latency_signal[s_id]=0;

   
   }
   
   
   
   }
   
   return 1;
   }
   
   return 0;
   
   
}
int client_command_analysis(int c_id,char *client_command){
   int format=0,segment_pointer[10];
   int i=0,j=0;
   char segment[1024];
   char result[10][1024];
   
   for(i=0;i<10;i++){
   segment_pointer[i]=0;
   }
   //puts(client_command);
  
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
   		strcpy(client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].command_code ,result[i]);
   		//puts(client_command_buffer[c_id][client_command_buffer_pointer[c_id]].command_code);
   		}
   		if(i==1){
   		
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]);
   		strcpy(client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].extra_information ,result[i]);
   		//puts(client_command_buffer[c_id][client_command_buffer_pointer[c_id]].extra_information);
   		
   		}
   		if(i==2){
   		
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]);
   		client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].sender = atoi(result[i]);
   		//printf("%d \n",client_command_buffer[c_id][client_command_buffer_pointer[c_id]].sender);
   		}
   		if(i==3){
   		
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]);
   		client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].receiver = atoi(result[i]);
   		//printf("%d \n",client_command_buffer[c_id][client_command_buffer_pointer[c_id]].receiver);
   		}
   		if(i==4){
   		
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]);
   		strcpy(client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].timestamp ,result[i]);
   		//puts(client_command_buffer[c_id][client_command_buffer_pointer[c_id]].timestamp);
   		}
   		if(i==5){
   		
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]);
   		client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].effective_time = atoi(result[i]);
   		//printf("%d \n",client_command_buffer[c_id][client_command_buffer_pointer[c_id]].effective_time);
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
   		strcpy(client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].extra_information ,result[i]);
   		//puts(client_command_buffer[c_id][client_command_buffer_pointer[c_id]].extra_information);
   		}
   		if(i==1){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].command_code ,result[i]);
   		//puts(client_command_buffer[c_id][client_command_buffer_pointer[c_id]].command_code);
   		}
   		if(i==2){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].sender = atoi(result[i]);
   		//printf("%d \n",client_command_buffer[c_id][client_command_buffer_pointer[c_id]].sender);
   		}
   		if(i==3){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].receiver = atoi(result[i]);
   		//printf("%d \n",client_command_buffer[c_id][client_command_buffer_pointer[c_id]].receiver);
   		}
   		if(i==4){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].timestamp ,result[i]);
   		//puts(client_command_buffer[c_id][client_command_buffer_pointer[c_id]].timestamp);
   		}
   		if(i==5){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].effective_time = atoi(result[i]);
   		//printf("%d \n",client_command_buffer[c_id][client_command_buffer_pointer[c_id]].effective_time);
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
   		strcpy(client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].timestamp ,result[i]);
   		//puts(client_command_buffer[c_id][client_command_buffer_pointer[c_id]].timestamp);
   		
   		}
   		if(i==1){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].effective_time = atoi(result[i]);
   		//printf("%d \n",client_command_buffer[c_id][client_command_buffer_pointer[c_id]].effective_time);
   		
   		}
   		if(i==2){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].extra_information ,result[i]);
   		//puts(client_command_buffer[c_id][client_command_buffer_pointer[c_id]].extra_information);
   		}
   		if(i==3){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		strcpy(client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].command_code ,result[i]);
   		//puts(client_command_buffer[c_id][client_command_buffer_pointer[c_id]].command_code);
   		}
   		if(i==4){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].sender = atoi(result[i]);
   		//printf("%d \n",client_command_buffer[c_id][client_command_buffer_pointer[c_id]].sender);
   		
   		}
   		if(i==5){
   		strchrn(result[i],client_command,segment_pointer[i-1],segment_pointer[i]-1);
   		client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].receiver = atoi(result[i]);
   		//printf("%d \n",client_command_buffer[c_id][client_command_buffer_pointer[c_id]].receiver);
   		
   		
   		}
   		
   		}
   		
   		for(i=0;i<6;i++){
   		memset(result[i],0,strlen(result[i]));
   		}
   		break;   
   		
   	case 0:
   		break; 
   }
   
   if(client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].receiver == client[c_id].id ){ //####
   printf("client %d this command is to me \n",c_id);
   return 1;
   }
   
   return 0;
   
}


   
   	




void client_func(long c_id){


    int rc,a,b,i,j,target_servent=0;
    int command_comparison_result=1,website_comparison_result=1,peer_comparison_result=0;
    char string_now_year[1024],string_now_month[1024],string_now_day[1024];
    char string_now_hour[1024],string_now_min[1024],string_now_sec[1024];
    char string_command_year[4096],string_command_month[4096],string_command_day[4096],string_command_sec[4096];
    int now_year,now_month,now_day,now_hour,now_min,now_sec;
    int command_year,command_month,command_day,command_sec;
    int deadline_year,deadline_month,deadline_day,deadline_sec;
    char send_message[1024];
    int invalid_peer_num=0;
    int peer_num=0;
		if(client_work_over[c_id] == 1){
			return;
		}
		if(client_eliminate_signal[c_id]==1){
		//printf("client %ld eliminate!!!\n", c_id);
		client_pattern[c_id]=99;
		sleep(2);
		return;	
		}
		
		time(&current);
		info = localtime( &current );
		
		strftime(string_now_year,sizeof(string_now_year),"%Y",info);
		now_year = atoi(string_now_year);
		strftime(string_now_month,sizeof(string_now_month),"%m",info);
		now_month = atoi(string_now_month);
		strftime(string_now_day,sizeof(string_now_day),"%d",info);
		now_day = atoi(string_now_day);
		
		strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
		now_hour = atoi(string_now_hour);	
		strftime(string_now_min,sizeof(string_now_min),"%M",info);
		now_min = atoi(string_now_min);	
		strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
		now_sec = atoi(string_now_sec);	
		    
		now_sec+=(60*now_min)+(60*60*now_hour);
		
		
		
		
		if(now_sec > client_last_time_select_pattern[c_id] ){
		
		if((now_sec - client_last_time_select_pattern[c_id] ) >= NUM_OF_SELECT_PATTERN_TIMES){
		
		client_select_pattern_signal[c_id]  = 1;
		
		}
		if((now_sec - client_last_time_select_pattern[c_id] ) < NUM_OF_SELECT_PATTERN_TIMES){
		client_select_pattern_signal[c_id]  = 0; 
		
		}
		
		}
		else if(now_sec < client_last_time_select_pattern[c_id] ){
		
		if((now_sec+(86400-client_last_time_select_pattern[c_id]) ) >= NUM_OF_SELECT_PATTERN_TIMES){
		client_select_pattern_signal[c_id]  = 1;
		
		}
		if((now_sec+(86400-client_last_time_select_pattern[c_id]) ) < NUM_OF_SELECT_PATTERN_TIMES){
		client_select_pattern_signal[c_id]  = 0;
		
		}
		
		
		}
		if(client_boot_signal[c_id]==0){
		client_pattern[c_id]=99;
		return;	
		}
		if(client_select_pattern_signal[c_id] == 1){
		client_pattern[c_id] =  rand() % 3 +1 ;//zxcc
		}
		if(client_select_pattern_signal[c_id] == 0){
		client_pattern[c_id] = 99;
		}
		
		
		for(i=0;i<NUM_SERVENT_PEER;i++){
		if(client_master[c_id][i].master_id != -1 && servent_eliminate_signal[client_master[c_id][i].master_id] == 1){
		client_master[c_id][i].master_id = -1;

		}
		   
		}
		client_master_num[c_id]=0;//***-
		for(i=0;i<NUM_SERVENT_PEER;i++){
				
			if(client_master[c_id][i].master_id != -1 ){
			client_master_num[c_id]++;
			}
					
		}
		
		if(client_master_num[c_id]!=0){
		   for(i=0;i<NUM_SERVENT_PEER;i++){
				if(client_master[c_id][i].master_id != -1 && i>=client_master_num[c_id]){
				for ( j = 0; j < client_master_num[c_id]; j++) {
				if(client_master[c_id][j].master_id == -1){
				    	   Master temp = client_master[c_id][j];
					   client_master[c_id][j] = client_master[c_id][i];
					   client_master[c_id][i] = temp;
					   break;
				}
				   
				}
				   
				}
				   
		   }
				   
		}
		
		
		for ( i = 0; i < client_master_num[c_id]; i++) {		   
			if (client_master[c_id][i].reputation_value < 0) {
		        client_pattern[c_id]=2;
		         
		        }
		        /*if (client_master[c_id][i].reputation_value == reputation_value_max && client_master[c_id][i].already_send_TM_signal == 0 
		        && servent_latency_signal[client_master[c_id][i].master_id] == 0 ) {   

		                client_master[c_id][i].already_send_TM_signal = 1;
		                client_master[c_id][i].trust_signal = 1;
				target_servent = client_master[c_id][i].master_id;
				
				
				memset(send_message,0,1024);
				
				client_make_command(send_message,client[c_id].id,servent[target_servent].id,0,"f005");

	     			puts("");
	     			if(servent_command_buffer_pointer[target_servent] > 99){
	     			servent_command_buffer_pointer[target_servent] = 0;
	     			}
	     			strcpy(servent_command_buffer[target_servent][servent_command_buffer_pointer[target_servent]].content, send_message);
	     			servent_command_buffer_pointer[target_servent]++;
	     				
	     			puts(servent_command_buffer[target_servent][servent_command_buffer_pointer[target_servent]-1].content);
	     			
	     			
	     			servent_command_analysis(target_servent,servent_command_buffer[target_servent][servent_command_buffer_pointer[target_servent]-1].content); 
				
		        
		        }*/
		        
		}
                if(client_pattern[c_id]!=99){
		printf("client_pattern[%ld] = %d !\n", c_id,client_pattern[c_id]); 
		client_log[c_id][client_execute_pattern_time[c_id]].num_pattern = client_pattern[c_id];
		client_log[c_id][client_execute_pattern_time[c_id]].now_sec = now_sec;
		}
		switch(client_pattern[c_id]) {
			case 0:
	
				client_pattern[c_id]=99;
				break;
			case 1:
				target_servent = 0;
				invalid_peer_num = 0;
				
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(client_master[c_id][i].master_id != -1 && servent_eliminate_signal[client_master[c_id][i].master_id] == 1){
				client_master[c_id][i].master_id = -1;
				client_master[c_id][i].reputation_value = 0;
				   
				}
				   
				}
				client_master_num[c_id]=0;
				for(i=0;i<NUM_SERVENT_PEER;i++){
				
					if(client_master[c_id][i].master_id != -1 ){
					client_master_num[c_id]++;
					}
							
				}
				if(client_master_num[c_id]==0){
				printf("client %ld doesnt has peer !!! \n",c_id);
				client_pattern[c_id]=99;
				break;
				}
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(client_master[c_id][i].master_id != -1 ){
				if( client_master[c_id][j].trust_signal == 1){//client_master[c_id][i].reputation_value == -1 ||
				invalid_peer_num++;
				}  
				}  
				if(invalid_peer_num >= NUM_SERVENT_PEER-1){
				break;
				}
				
				}
				if(invalid_peer_num >= NUM_SERVENT_PEER-1){
				printf("client %ld doesnt has peer can communicate !!! \n",c_id);
				client_pattern[c_id]=99;
				break;
				}
				client_rearrange_peer(c_id);
				client_sort_peer(c_id);	
				
				/*for ( i = client_master_num[c_id]-1; i >= 0 ; i--) {
					if(client_master[c_id][i].reputation_value != 5 && client_master[c_id][i].reputation_value != -1){
						target_servent = client_master[c_id][i].master_id;
						
						break;
					}
					
				}*/
				
				
				if(client_master_num[c_id] != 0){
				peer_num = client_master_num[c_id];
				target_servent = client_master[c_id][rand() % (peer_num)].master_id  ;
				
				
				for ( i = 0; i < client_master_num[c_id]; i++) {
				
				if(client_master[c_id][i].master_id == target_servent){
				
				j=i;
				while(  client_master[c_id][j].trust_signal == 1){//client_master[c_id][j].reputation_value == -1 ||
				
					
				target_servent = client_master[c_id][rand() % (peer_num)].master_id  ;
				for ( j = 0; j < client_master_num[c_id]; j++) {
				if(client_master[c_id][j].master_id == target_servent){
				break;
				}
				}
				
				i = 0;    
				}
				
				}
				
				}
				
				}
				
				
				client_log[c_id][client_execute_pattern_time[c_id]].communication_target = target_servent;
				
				if(servent_boot_signal[target_servent]==0){
				printf("servent %ld shut down  \n",target_servent);
				printf("servent %ld reputation_value--!!! \n",target_servent);
				for ( i = 0; i < client_master_num[c_id]; i++) {
					if(client_master[c_id][i].master_id == target_servent && client_master[c_id][i].reputation_value > -1 ){//
						client_master[c_id][i].reputation_value--;
					}
					
				}
				client_log[c_id][client_execute_pattern_time[c_id]].communication_result = 0;
				client_execute_pattern_time[c_id]++;
				client_pattern[c_id]=99;
				break;
				}
				
				
				if(servent_eliminate_signal[target_servent]==1){
				printf("servent %ld eliminate \n",target_servent);
				
				client_log[c_id][client_execute_pattern_time[c_id]].communication_result = 0;
				client_execute_pattern_time[c_id]++;
				client_pattern[c_id]=99;
				break;
				}
				
				if(servent[target_servent].sensor_signal == 1 ){
						request_time[target_servent]++;
				}
				if(servent[target_servent].sensor_signal == 1 && client[c_id].request_signal == 0 && enumeration_signal == 1){
						client[c_id].request_signal = 1;
						vs++;
				}
				
				
				
				if(servent_latency_signal[target_servent]==1){
				printf("servent %ld already turn to latency bot  \n",target_servent);
				printf("servent %ld reputation_value--!!! \n",target_servent);
				for ( i = 0; i < client_master_num[c_id]; i++) {
					if(client_master[c_id][i].master_id == target_servent && client_master[c_id][i].reputation_value > -1 ){//
						client_master[c_id][i].reputation_value--;
					}
					
				}
				client_log[c_id][client_execute_pattern_time[c_id]].communication_result = 0;
				client_execute_pattern_time[c_id]++;
				client_pattern[c_id]=99;
				break;
				}
				
				//--------------------
				
				
					

				//asdf
				
				char newest_command[1024];
				
				memset(newest_command,0,strlen(newest_command));
				if(servent_command_buffer_pointer[target_servent] != 0 && strlen(servent_command_buffer[target_servent][servent_command_buffer_pointer[target_servent]-1].content)!= 0){
				strcpy(newest_command, servent_command_buffer[target_servent][servent_command_buffer_pointer[target_servent]-1].content);
				}
				
				memset(send_message,0,1024);
				client_make_command(send_message,client[c_id].id,servent[target_servent].id,0,"f001");
				printf("client %ld , send_data: %s , target_servent:%d", c_id,send_message,target_servent);

	     			puts("");
	     			
	     			if(servent_command_buffer_pointer[target_servent] > 99){
				servent_command_buffer_pointer[target_servent] = 0;
				}
				
				
	     			strcpy(servent_command_buffer[target_servent][servent_command_buffer_pointer[target_servent]].content, send_message);
	     			servent_command_buffer_pointer[target_servent]++;
	     			puts(servent_command_buffer[target_servent][servent_command_buffer_pointer[target_servent]-1].content);
	     			if(servent_command_analysis(target_servent,servent_command_buffer[target_servent][servent_command_buffer_pointer[target_servent]-1].content) == 0 
	     			&& enumeration_signal == 1
	     			){// zxxx //&& servent[target_servent].sensor_signal != 1 && servent[target_servent].fake_signal != 1

				printf("servent %ld reputation_value--!!! \n",target_servent);
				for ( i = 0; i < client_master_num[c_id]; i++) {
					if(client_master[c_id][i].master_id == target_servent && client_master[c_id][i].reputation_value > -1 ){//
						client_master[c_id][i].reputation_value--;
					}
					
				}
				client_log[c_id][client_execute_pattern_time[c_id]].communication_result = 0;
				client_execute_pattern_time[c_id]++;
				client_pattern[c_id]=99;
				break;
				
	     			}
	     			
	     			if(servent_command_analysis(target_servent,servent_command_buffer[target_servent][servent_command_buffer_pointer[target_servent]-1].content) == 1){
	
				if(strcmp(servent_command_buffer[target_servent][servent_command_buffer_pointer[target_servent]-1].command_code ,"f001" )==0 ){
				
				
				
				memset(send_message,0,1024);
				servent_make_command(send_message,servent[target_servent].id,client[c_id].id ,0,"f002" );
				
				if(client_command_buffer_pointer[c_id] > 99){
				client_command_buffer_pointer[c_id] = 0;
				}
				strcpy(client_command_buffer[c_id][client_command_buffer_pointer[c_id]].content, send_message);
	     			client_command_buffer_pointer[c_id]++;					
				puts(client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].content); 	     			
				
					     	
				
					     			
				}
				
	     			}
	     			
	     			
	     			
	     			if(client_command_analysis(c_id,client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].content) == 1){
	
				if(strcmp(client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].command_code ,"f002" )==0 ){
							
				
				
					
				printf("client %ld receive command from servent %ld:\n",c_id,target_servent);

				if(strlen(newest_command)!= 0){
				for(i=0;i<client_command_buffer_pointer[c_id];i++){	
				command_comparison_result=strcmp(client_command_buffer[c_id][i].content,newest_command);
				if(command_comparison_result== 0){break;}
				}
							
				if(command_comparison_result== 0){
				printf("client %ld already have (%s) \n",c_id,newest_command);
				}
				if(command_comparison_result != 0){
				if(client_command_buffer_pointer[c_id] > 99){
				client_command_buffer_pointer[c_id] = 0;
				}
				strcpy(client_command_buffer[c_id][client_command_buffer_pointer[c_id]].content,newest_command);
				puts(client_command_buffer[c_id][client_command_buffer_pointer[c_id]].content);
				client_command_buffer_pointer[c_id]++;
				client_command_analysis(c_id,client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].content);
				}
				
				}
				
				
				
				
				client_rearrange_peer(c_id);   
				
				for(i=0;i<client_master_num[c_id];i++){	
				if(client_master[c_id][i].master_id ==  target_servent && client_master[c_id][i].reputation_value < reputation_value_max ){
				client_master[c_id][i].reputation_value++;
				break;
				}
				}
				
				
					
				client_sort_peer(c_id);			
				
				for ( i = 0; i < client_master_num[c_id]; i++) {
				if(client_master[c_id][i].master_id!= -1 ){
				printf("id:%d value:%d\n",client_master[c_id][i].master_id, client_master[c_id][i].reputation_value);
				}
				
				}	     			
				
					     	
				
					     			
				}
				
	     			}
	     			
	     			client_log[c_id][client_execute_pattern_time[c_id]].communication_result = 1;
				client_execute_pattern_time[c_id]++;
	     			client_pattern[c_id]=99;
				
				break;
			case 2:

				target_servent = 0;
				invalid_peer_num = 0;
				
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(client_master[c_id][i].master_id != -1 && servent_eliminate_signal[client_master[c_id][i].master_id] == 1){
				client_master[c_id][i].master_id = -1;
				client_master[c_id][i].reputation_value = 0;
				   
				}
				   
				}
				client_master_num[c_id]=0;
				for(i=0;i<NUM_SERVENT_PEER;i++){
				
					if(client_master[c_id][i].master_id != -1 ){
					client_master_num[c_id]++;
					}
							
				}
				if(client_master_num[c_id]==0){
				printf("client %ld doesnt has peer !!! \n",c_id);
				client_pattern[c_id]=99;
				break;
				}
				if(client_master_num[c_id]==1){
				printf("client %ld doesnt has peer can communicate !!! \n",c_id);
				client_pattern[c_id]=99;
				break;
				}
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(client_master[c_id][i].master_id != -1 ){
				if(client_master[c_id][j].trust_signal == 1){//client_master[c_id][i].reputation_value == -1 || 
				invalid_peer_num++;
				}  
				}  
				if(invalid_peer_num >= NUM_SERVENT_PEER-1){
				break;
				}
				
				}
				if(invalid_peer_num >= NUM_SERVENT_PEER-1){
				printf("client %ld doesnt has peer can communicate !!! \n",c_id);
				client_pattern[c_id]=99;
				break;
				}
				client_rearrange_peer(c_id);
				client_sort_peer(c_id);	
				
				/*for ( i = client_master_num[c_id]-1; i >= 0 ; i--) {
					if(client_master[c_id][i].reputation_value != 5 && client_master[c_id][i].reputation_value != -1){
						target_servent = client_master[c_id][i].master_id;
						
						break;
					}
					
				}*/
				
				if(client_master_num[c_id] != 0){
				
				peer_num = client_master_num[c_id];
				target_servent = client_master[c_id][rand() % (peer_num)].master_id  ;
				for ( i = 0; i < client_master_num[c_id]; i++) {
				
				if(client_master[c_id][i].master_id == target_servent){
				
				j=i;
				while(client_master[c_id][j].trust_signal == 1 ){// client_master[c_id][j].reputation_value == -1 || 
				
					

				target_servent = client_master[c_id][rand() % (peer_num)].master_id  ;
				for ( j = 0; j < client_master_num[c_id]; j++) {
				if(client_master[c_id][j].master_id == target_servent){
				break;
				}
				}
				
				i = 0;    
				}
				
				}
				
				}
				
				}
				
				client_log[c_id][client_execute_pattern_time[c_id]].communication_target = target_servent;
				servent_peer_num[target_servent]=0;
				for(i=0;i<NUM_SERVENT_PEER;i++){
				
				if(servent_peer_list[target_servent][i].peer_id!= -1){
				servent_peer_num[target_servent]++;
				}	
				} 
				
				
				if(servent_boot_signal[target_servent]==0){
				printf("servent %ld shut down  \n",target_servent);
				printf("servent %ld reputation_value--!!! \n",target_servent);
				for ( i = 0; i < client_master_num[c_id]; i++) {
					if(client_master[c_id][i].master_id == target_servent && client_master[c_id][i].reputation_value > -1 ){//
						client_master[c_id][i].reputation_value--;
					}
					
				}
				client_log[c_id][client_execute_pattern_time[c_id]].communication_result = 0;
				client_execute_pattern_time[c_id]++;   
				client_pattern[c_id]=99;
				break;
				}
				
				if(servent_eliminate_signal[target_servent]==1){
				printf("servent %ld eliminate \n",target_servent);
				
				client_log[c_id][client_execute_pattern_time[c_id]].communication_result = 0;
				client_execute_pattern_time[c_id]++;
				client_pattern[c_id]=99;
				break;
				}
				if(servent[target_servent].sensor_signal == 1 ){
						request_time[target_servent]++;
				}
				if(servent[target_servent].sensor_signal == 1 && client[c_id].request_signal == 0 && enumeration_signal == 1){
						client[c_id].request_signal = 1;
						vs++;
				}
				if(servent_latency_signal[target_servent]==1){
				printf("servent %ld already turn to latency bot  \n",target_servent);
				printf("servent %ld reputation_value--!!! \n",target_servent);
				for ( i = 0; i < client_master_num[c_id]; i++) {
					if(client_master[c_id][i].master_id == target_servent && client_master[c_id][i].reputation_value > -1 ){//
						client_master[c_id][i].reputation_value--;
					}
					
				}
				client_log[c_id][client_execute_pattern_time[c_id]].communication_result = 0;
				client_execute_pattern_time[c_id]++; 
				client_pattern[c_id]=99;
				break;
				}
				if(servent_peer_num[target_servent]==0){
				printf("target_servent:servent %ld doesnt has peer !!! \n",target_servent);
				
				
				client_log[c_id][client_execute_pattern_time[c_id]].communication_result = 0;
				client_execute_pattern_time[c_id]++;
				client_pattern[c_id]=99;
				break;
				}
				
				
				
				
				
				memset(send_message,0,1024);
				client_make_command(send_message,client[c_id].id,servent[target_servent].id,0,"f003");
				printf("client %ld , send_data: %s , target_servent:%d", c_id,send_message,target_servent);

	     			puts("");
	     			if(servent_command_buffer_pointer[target_servent] > 99){
				servent_command_buffer_pointer[target_servent] = 0;
				}
				
				
	     			strcpy(servent_command_buffer[target_servent][servent_command_buffer_pointer[target_servent]].content, send_message);
	     			servent_command_buffer_pointer[target_servent]++;
	     			puts(servent_command_buffer[target_servent][servent_command_buffer_pointer[target_servent]-1].content);
	     			if(servent_command_analysis(target_servent,servent_command_buffer[target_servent][servent_command_buffer_pointer[target_servent]-1].content) == 0
	     			&& enumeration_signal == 1
	     			){// zxxx //&& servent[target_servent].sensor_signal != 1 && servent[target_servent].fake_signal != 1

				printf("servent %ld reputation_value--!!! \n",target_servent);
				for ( i = 0; i < client_master_num[c_id]; i++) {
					if(client_master[c_id][i].master_id == target_servent && client_master[c_id][i].reputation_value > -1 ){//
						client_master[c_id][i].reputation_value--;
					}
					
				}
				client_log[c_id][client_execute_pattern_time[c_id]].communication_result = 0;
				client_execute_pattern_time[c_id]++;
				client_pattern[c_id]=99;
				break;
				
	     			}
	     			
	     			peer_comparison_result=0;
	     			if(servent_command_analysis(target_servent,servent_command_buffer[target_servent][servent_command_buffer_pointer[target_servent]-1].content) == 1){
	
				if(strcmp(servent_command_buffer[target_servent][servent_command_buffer_pointer[target_servent]-1].command_code ,"f003" )==0 ){
				
				
				
				
				memset(send_message,0,1024);
				servent_make_command(send_message,servent[target_servent].id,client[c_id].id ,0,"f004" );
				
				
				
				
				//s5s5
				for ( i = 0; i < servent_peer_num[target_servent]; i++) {
				if(servent_peer_list[target_servent][i].peer_id == client_master[c_id][client_master_num[c_id]-1].master_id){
				
				
				
				printf("servent %ld already have servent %d in peer list !\n",target_servent,client_master[c_id][client_master_num[c_id]-1].master_id);
				peer_comparison_result=1;
				break;
				}
				
				
				}
				if(target_servent == client_master[c_id][client_master_num[c_id]-1].master_id){
				printf("servent %ld cant add itself in peer list !\n",target_servent);
				
				peer_comparison_result=1;
				
				}
				if(client_master[c_id][client_master_num[c_id]-1].master_id == -1){
				printf("servent %ld cant add invalid peer in peer list !\n",target_servent);
				
				peer_comparison_result=1;
				
				}
				if(peer_comparison_result == 0 && servent_peer_num[target_servent]==NUM_SERVENT_PEER){//tttt
				printf("servent %ld remove servent %d in peer list !\n",target_servent,servent_peer_list[target_servent][0].peer_id);
				printf("servent %ld add servent %d in peer list !\n",target_servent,client_master[c_id][client_master_num[c_id]-1].master_id);
				servent_peer_list[target_servent][0].peer_id = client_master[c_id][client_master_num[c_id]-1].master_id;
				servent_peer_list[target_servent][0].reputation_value = reputation_value_base;
				
				
				
				
				}
				if(peer_comparison_result == 0 && servent_peer_num[target_servent] < NUM_SERVENT_PEER  ){
				//printf("servent %ld remove servent %d in peer list !\n",target_servent,servent_peer_list[target_servent][0].peer_id);
				printf("servent %ld add servent %d in peer list !\n",target_servent,client_master[c_id][client_master_num[c_id]-1].master_id);
				servent_peer_list[target_servent][servent_peer_num[target_servent]].peer_id = client_master[c_id][client_master_num[c_id]-1].master_id;
				servent_peer_list[target_servent][servent_peer_num[target_servent]].reputation_value = reputation_value_base;
				servent_peer_num[target_servent]++;
				
				
					
				
				}
				
				if(servent[target_servent].fake_signal == 1 && peer_comparison_result==0){
				
				if(servent[client_master[c_id][client_master_num[c_id]-1].master_id].detect_signal == 0 && enumeration_signal == 1 && servent[target_servent].fake_signal == 1){
				
				servent[client_master[c_id][client_master_num[c_id]-1].master_id].detect_signal = 1;
				vc++;
				} 
				
				if(servent[client_master[c_id][client_master_num[c_id]-1].master_id].request_signal == 0 && enumeration_signal == 1 && servent[target_servent].fake_signal == 1){
						servent[client_master[c_id][client_master_num[c_id]-1].master_id].request_signal = 1;
						vs++;
				}
				
				
						
				}
				
				if(client_log[c_id][client_execute_pattern_time[c_id]].num_pattern == 2 && peer_comparison_result == 0){
				client_log[c_id][client_execute_pattern_time[c_id]].send_peer=client_master[c_id][client_master_num[c_id]-1].master_id;
				
				}
				
				if(client_command_buffer_pointer[c_id] > 99){
				client_command_buffer_pointer[c_id] = 0;
				}
				strcpy(client_command_buffer[c_id][client_command_buffer_pointer[c_id]].content, send_message);
	     			client_command_buffer_pointer[c_id]++;					
				puts(client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].content); 	
				
					     	
				
					     			
				}
				
	     			}
	     			
	     			peer_comparison_result=0;
	     			if(client_command_analysis(c_id,client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].content) == 1){
	
				if(strcmp(client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].command_code ,"f004" )==0 ){
							
							
				if(servent[target_servent].fake_signal != 1 && servent[target_servent].sensor_signal != 1){
				
				for ( i = 0; i < client_master_num[c_id]; i++) {	
				if(client_master[c_id][i].master_id == servent_peer_list[target_servent][servent_peer_num[target_servent]-1].peer_id ){
				
				
				
				printf("client %ld already have servent %d in master list !\n",c_id,servent_peer_list[target_servent][servent_peer_num[target_servent]-1].peer_id);
				peer_comparison_result=1;
				break;
				}
				
				
				}
				if(servent_peer_list[target_servent][servent_peer_num[target_servent]-1].peer_id == -1){
				printf("client %ld cant add invalid peer in peer list !\n",c_id);
				
				peer_comparison_result=1;
				
				}
				if(peer_comparison_result == 0){
				printf("client %ld remove servent %d in master list !\n",c_id,client_master[c_id][0].master_id);
				printf("client %ld add servent %d in master list !\n",c_id,servent_peer_list[target_servent][servent_peer_num[target_servent]-1].peer_id);
				client_master[c_id][0].master_id = servent_peer_list[target_servent][servent_peer_num[target_servent]-1].peer_id;
				client_master[c_id][0].reputation_value = reputation_value_base; 
				
				
				
				
				}
				if(client_log[c_id][client_execute_pattern_time[c_id]].num_pattern == 2 && peer_comparison_result == 0 
				&& (servent[target_servent].fake_signal != 1 && servent[target_servent].sensor_signal != 1)){
				client_log[c_id][client_execute_pattern_time[c_id]].receive_peer=servent_peer_list[target_servent][servent_peer_num[target_servent]-1].peer_id;
				
				}
				
				}			
				else if(servent[target_servent].fake_signal == 1 || servent[target_servent].sensor_signal == 1){
				
				char string_ip[100],string_port[100],string_id[100];
				extra_information_split(client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1].extra_information,string_ip,string_port,string_id);
				int send_peer = atoi(string_id);
				for ( i = 0; i < client_master_num[c_id]; i++) {	
				if(client_master[c_id][i].master_id == send_peer ){
				
				
				
				printf("client %ld already have servent %d in master list !\n",c_id,send_peer);
				peer_comparison_result=1;
				break;
				}
				
				
				}
				if(send_peer == -1){
				printf("client %ld cant add invalid peer in peer list !\n",c_id);
				
				peer_comparison_result=1;
				
				}
				if(peer_comparison_result == 0){
				printf("client %ld remove servent %d in master list !\n",c_id,client_master[c_id][0].master_id);
				printf("client %ld add servent %d in master list !\n",c_id,send_peer);
				client_master[c_id][0].master_id = send_peer;
				client_master[c_id][0].reputation_value = reputation_value_base; 
				
				
				
				
				}
				if(client_log[c_id][client_execute_pattern_time[c_id]].num_pattern == 2 && peer_comparison_result == 0 
				&& (servent[target_servent].fake_signal == 1 || servent[target_servent].sensor_signal == 1)){
				client_log[c_id][client_execute_pattern_time[c_id]].receive_peer=send_peer;
				
				}
				
				}
				
				client_rearrange_peer(c_id);   
				
				for(i=0;i<client_master_num[c_id];i++){	
				if(client_master[c_id][i].master_id ==  target_servent && client_master[c_id][i].reputation_value < reputation_value_max ){
				client_master[c_id][i].reputation_value++;
				break;
				}
				}
				
				
					
				client_sort_peer(c_id);			
				
				for ( i = 0; i < client_master_num[c_id]; i++) {
				if(client_master[c_id][i].master_id!= -1 ){
				printf("id:%d value:%d\n",client_master[c_id][i].master_id, client_master[c_id][i].reputation_value);
				}
				
				}	     			
				
					     	
				
					     			
				}
				
	     			}
				
				client_log[c_id][client_execute_pattern_time[c_id]].communication_result = 1;
				client_execute_pattern_time[c_id]++;
	     			client_pattern[c_id]=99;
			 	
				break;
			case 3:
				target_servent = 0;
				invalid_peer_num = 0;
				
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(client_master[c_id][i].master_id != -1 && servent_eliminate_signal[client_master[c_id][i].master_id] == 1){
				client_master[c_id][i].master_id = -1;
				client_master[c_id][i].reputation_value = 0;
				   
				}
				   
				}
				client_master_num[c_id]=0;
				for(i=0;i<NUM_SERVENT_PEER;i++){
				
					if(client_master[c_id][i].master_id != -1 ){
					client_master_num[c_id]++;
					}
							
				}
				if(client_master_num[c_id]==0){
				printf("client %ld doesnt has peer !!! \n",c_id);
				client_pattern[c_id]=99;
				break;
				}
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(client_master[c_id][i].master_id != -1 ){
				if( client_master[c_id][j].trust_signal == 1){//client_master[c_id][i].reputation_value == -1 ||
				invalid_peer_num++;
				}  
				}  
				if(invalid_peer_num >= NUM_SERVENT_PEER-1){
				break;
				}
				
				}
				if(invalid_peer_num >= NUM_SERVENT_PEER-1){
				printf("client %ld doesnt has peer can communicate !!! \n",c_id);
				client_pattern[c_id]=99;
				break;
				}
				client_rearrange_peer(c_id);
				client_sort_peer(c_id);	
				
				/*for ( i = client_master_num[c_id]-1; i >= 0 ; i--) {
					if(client_master[c_id][i].reputation_value != 5 && client_master[c_id][i].reputation_value != -1){
						target_servent = client_master[c_id][i].master_id;
						
						break;
					}
					
				}*/
				if(client_master_num[c_id] != 0){
				
				peer_num = client_master_num[c_id];
				target_servent = client_master[c_id][rand() % (peer_num)].master_id  ;
				for ( i = 0; i < client_master_num[c_id]; i++) {
				
				if(client_master[c_id][i].master_id == target_servent){
				
				j=i;
				while(  client_master[c_id][j].trust_signal == 1 ){//client_master[c_id][j].reputation_value == -1 ||
				
					
				target_servent = client_master[c_id][rand() % (peer_num)].master_id  ;
				for ( j = 0; j < client_master_num[c_id]; j++) {
				if(client_master[c_id][j].master_id == target_servent){
				break;
				}
				}
				
				i = 0;    
				}
				
				}
				
				}
				
				}
				
				/*if(servent[target_servent].sensor_signal == 1 ){
						request_time++;
				}
				if(servent[target_servent].sensor_signal == 1 && client[c_id].request_signal == 0 && enumeration_signal == 1){
						client[c_id].request_signal = 1;
						vs++;
				}*/
				client_log[c_id][client_execute_pattern_time[c_id]].communication_target = target_servent;
				
				if(servent_latency_signal[target_servent]==1){
				printf("servent %ld already turn to latency bot  \n",target_servent);
				
				client_log[c_id][client_execute_pattern_time[c_id]].communication_result = 0;
				client_execute_pattern_time[c_id]++;
				client_pattern[c_id]=99;
				break;
				}
				if(servent_boot_signal[target_servent]==0){
				printf("servent %ld shut down  \n",target_servent);
				
				client_log[c_id][client_execute_pattern_time[c_id]].communication_result = 0;
				client_execute_pattern_time[c_id]++;
				client_pattern[c_id]=99;
				break;
				}
				if(servent_eliminate_signal[target_servent]==1){
				printf("servent %ld eliminate \n",target_servent);
				
				client_log[c_id][client_execute_pattern_time[c_id]].communication_result = 0;
				client_execute_pattern_time[c_id]++;
				client_pattern[c_id]=99;
				break;
				}
				
				
				
				
				
				
				b=0;
		
				printf("website from servent %ld:\n",target_servent);
				while(servent_bot_website_buffer[target_servent][b].url[0]!= '\0'){
							
				for(i=0;i<client_bot_website_buffer_pointer[c_id];i++){	
				website_comparison_result=strcmp(client_bot_website_buffer[c_id][i].url,servent_bot_website_buffer[target_servent][b].url);
				if(website_comparison_result== 0){break;}
				}
							
				if(website_comparison_result== 0){
				printf("client %ld already have (%s)\n",c_id,servent_bot_website_buffer[target_servent][b].url);
				}
				if(website_comparison_result != 0){
				strcpy(client_bot_website_buffer[c_id][client_bot_website_buffer_pointer[c_id]].url,servent_bot_website_buffer[target_servent][b].url);
				client_bot_website_buffer[c_id][client_bot_website_buffer_pointer[c_id]].reputation_value = servent_bot_website_buffer[target_servent][b].reputation_value;
				puts(client_bot_website_buffer[c_id][client_bot_website_buffer_pointer[c_id]].url);
				client_bot_website_buffer_pointer[c_id]++;
				}				
				b++;			
				}
				client_log[c_id][client_execute_pattern_time[c_id]].communication_result = 1;
				client_execute_pattern_time[c_id]++;
	     			client_pattern[c_id]=99;
				break;	
				
				
			
			case (-1):
				printf(" client %ld terminated !\n", c_id);
				
				break;	
		}
		time(&current);
		info = localtime( &current );
		
		strftime(string_now_year,sizeof(string_now_year),"%Y",info);
		now_year = atoi(string_now_year);
		strftime(string_now_month,sizeof(string_now_month),"%m",info);
		now_month = atoi(string_now_month);
		strftime(string_now_day,sizeof(string_now_day),"%d",info);
		now_day = atoi(string_now_day);
		
		strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
		now_hour = atoi(string_now_hour);	
		strftime(string_now_min,sizeof(string_now_min),"%M",info);
		now_min = atoi(string_now_min);	
		strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
		now_sec = atoi(string_now_sec);	
		    
		now_sec+=(60*now_min)+(60*60*now_hour);
		if(client_select_pattern_signal[c_id] ==1){
		client_last_time_select_pattern[c_id]  = now_sec;
		
		}
		
		
			
			int delete_command_signal=0;
		//printf("before:client_command_buffer_pointer[c_id] %d  !\n", client_command_buffer_pointer[c_id]);
			
		for ( i = 0; i < client_command_buffer_pointer[c_id]; i++) {
		
		if(strlen(client_command_buffer[c_id][i].content) != 0){
			
			//puts(client_command_buffer[c_id][i].content);
			timestamp_split(client_command_buffer[c_id][i].timestamp ,string_command_year,string_command_month,string_command_day,string_command_sec);
			
			command_year = atoi(string_command_year);	
			command_month = atoi(string_command_month);	
			command_day = atoi(string_command_day);	
			command_sec = atoi(string_command_sec);	
			//printf("effective_time %d  !\n", client_command_buffer[c_id][i].effective_time);
			deadline_sec = command_sec + client_command_buffer[c_id][i].effective_time;
			deadline_day = command_day ;
			deadline_month = command_month ;
			deadline_year = command_year ;
			int leap_year=0;
			if((deadline_year % 4 == 0 && deadline_year % 100 != 0 ) || deadline_year % 400 == 0) {
			leap_year = 1;
			}
			//printf("leap_year %d  !\n", leap_year);
			
			if(deadline_sec > 86400 ){
			deadline_sec = deadline_sec-86400;
			deadline_day++;
			}
			if( (deadline_month == 4 || deadline_month == 6 || deadline_month == 9 || deadline_month == 11) && deadline_day > 30 ){
			deadline_day = 1;
			deadline_month++;
			}
			if( (deadline_month == 1 || deadline_month == 3 || deadline_month == 5 || deadline_month == 7 || deadline_month == 10 || deadline_month == 12 ) && deadline_day > 31 ){
			deadline_day = 1;
			deadline_month++;
			}
			if( leap_year == 1 && ( deadline_month == 2) && ( deadline_day > 29) ){
			deadline_day = 1;
			deadline_month++;
			}
			if( leap_year == 0 && ( deadline_month == 2) && ( deadline_day > 28) ){
			deadline_day = 1;
			deadline_month++;
			}
			if( deadline_month > 12 ){
			deadline_month = 1;
			deadline_year++;
			}
			
			/*printf("deadline_year %d  !\n", deadline_year);
			printf("deadline_month %d  !\n", deadline_month);
			printf("deadline_day %d  !\n", deadline_day);
			printf("deadline_sec %d  !\n", deadline_sec);
			printf("now_year %d  !\n", now_year);
			printf("now_month %d  !\n", now_month);
			printf("now_day %d  !\n", now_day);
			printf("now_sec %d  !\n", now_sec);*/
			if(now_year > deadline_year){
			delete_command_signal = 1;
			}
			if(now_year == deadline_year){
				if(now_month > deadline_month){
				delete_command_signal = 1;
				}
				if(now_month == deadline_month){
					if(now_day > deadline_day){
					delete_command_signal = 1;
					}
					if(now_day == deadline_day){
						if(now_sec >= deadline_sec){
						delete_command_signal = 1;
						}
					}
					
				}
				
			}
			
			
			if(delete_command_signal == 1){
			printf("delete_command_signal[%ld] %d  !\n", c_id,delete_command_signal);
			Command temp = client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1];
			client_command_buffer[c_id][client_command_buffer_pointer[c_id]-1] = client_command_buffer[c_id][i];
		        client_command_buffer[c_id][i] = temp;
		        
		        memset(&client_command_buffer[c_id][i],0,sizeof(Command));

			client_command_buffer_pointer[c_id]--;
			}
			
		}
		
		}		
		
		sleep(0.2);

}
void servent_func(long s_id){


    int rc,a=0,b=0,i=0,j=0,k=0,send_target=0,result,file_num,invalid_peer_num=0;
    int command_comparison_result=1,website_comparison_result=1,peer_comparison_result=0;
    Peer temp;
    char text[1024];
    char string_now_year[1024],string_now_month[1024],string_now_day[1024];//xxx
    char string_now_hour[1024],string_now_min[1024],string_now_sec[1024];
    char string_command_year[4096],string_command_month[4096],string_command_day[4096],string_command_sec[4096];
    int now_year,now_month,now_day,now_hour,now_min,now_sec;
    int command_year,command_month,command_day,command_sec;
    int deadline_year,deadline_month,deadline_day,deadline_sec;
    char servent_send_message[1024];
    int peer_num=0;     
                  
  
		if(servent_work_over[s_id] == 1){
			return;
		}
		if(servent_eliminate_signal[s_id]==1){
		//printf("servent %ld eliminate!!!\n", s_id);
		servent_pattern[s_id] = 99;
		return;	
		}
		if(servent_boot_signal[s_id]==0){
		servent_pattern[s_id] = 99;
		return;	
		}
		
		time(&current);
		info = localtime( &current );
		
		strftime(string_now_year,sizeof(string_now_year),"%Y",info);
		now_year = atoi(string_now_year);
		strftime(string_now_month,sizeof(string_now_month),"%m",info);
		now_month = atoi(string_now_month);
		strftime(string_now_day,sizeof(string_now_day),"%d",info);
		now_day = atoi(string_now_day);
		
		strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
		now_hour = atoi(string_now_hour);	
		strftime(string_now_min,sizeof(string_now_min),"%M",info);
		now_min = atoi(string_now_min);	
		strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
		now_sec = atoi(string_now_sec);	
		    
		now_sec+=(60*now_min)+(60*60*now_hour);
		
		if(now_sec > servent_last_time_select_pattern[s_id] ){
		
		if((now_sec - servent_last_time_select_pattern[s_id] ) >= NUM_OF_SELECT_PATTERN_TIMES){
		//printf("now_sec %d  !\n", now_sec);
		//printf("last_time_select_pattern[s_id]  %d  !\n", last_time_select_pattern[s_id] );
		servent_select_pattern_signal[s_id]  = 1;
		
		}
		if((now_sec - servent_last_time_select_pattern[s_id] ) < NUM_OF_SELECT_PATTERN_TIMES){
		servent_select_pattern_signal[s_id]  = 0;
		
		}
		
		}
		else if(now_sec < servent_last_time_select_pattern[s_id] ){
		
		if((now_sec+(86400-servent_last_time_select_pattern[s_id]) ) >= NUM_OF_SELECT_PATTERN_TIMES){
		servent_select_pattern_signal[s_id]  = 1;
		
		}
		if((now_sec+(86400-servent_last_time_select_pattern[s_id]) ) < NUM_OF_SELECT_PATTERN_TIMES){
		servent_select_pattern_signal[s_id]  = 0;
		
		}
		
		
		}
		
		if(servent_already_execute_resurrection_signal[s_id] == 1){
		for(i=0;i<NUM_SERVENT_PEER;i++){
		if(servent_peer_list[s_id][i].peer_id != -1 && servent_eliminate_signal[servent_peer_list[s_id][i].peer_id] == 1){
		servent_peer_list[s_id][i].peer_id = -1;
		servent_peer_list[s_id][i].reputation_value = 0;
		//servent_peer_list[s_id][i].sensor_signal = 0;
		}
		   
		}
		}
		
		servent_peer_num[s_id]=0;
		for(i=0;i<NUM_SERVENT_PEER;i++){
		   if(servent_peer_list[s_id][i].peer_id!= -1)
		   servent_peer_num[s_id]++;
		}
		
		if(servent_peer_num[s_id]!=0){
		
		for(i=0;i<NUM_SERVENT_PEER;i++){
		if(servent_peer_list[s_id][i].peer_id != -1 && i>=servent_peer_num[s_id]){
		for ( j = 0; j < servent_peer_num[s_id]; j++) {
		if(servent_peer_list[s_id][j].peer_id == -1){
		    	   temp = servent_peer_list[s_id][j];
			   servent_peer_list[s_id][j] = servent_peer_list[s_id][i];
			   servent_peer_list[s_id][i] = temp;
			   break;
		}
		   
		}
		   
		}
		   
		}
		   
		}
		
		
		
		
		if(servent_select_pattern_signal[s_id] == 1){
		servent_pattern[s_id] =  rand() % 4+1;
		//zxcc
		}
		if(servent_select_pattern_signal[s_id] == 0){
		servent_pattern[s_id] = 99;
		}
		//***-
		
						
		for ( i = 0; i < servent_peer_num[s_id]; i++) {		   
			if (servent_peer_list[s_id][i].reputation_value < 0 && servent_latency_signal[s_id] == 0) {
		        printf("servent %d has to request peer !!!!! \n",s_id);
		        servent_pattern[s_id]=3;
		        break;
		        }
		        if (servent_peer_list[s_id][i].reputation_value == reputation_value_max && servent_peer_list[s_id][i].already_send_TM_signal == 0 
		        && servent_latency_signal[servent_peer_list[s_id][i].peer_id] == 0 && servent_latency_signal[s_id] == 0) {   

		                servent_peer_list[s_id][i].already_send_TM_signal = 1;
		                servent_peer_list[s_id][i].trust_signal = 1;
				send_target = servent_peer_list[s_id][i].peer_id;
				
				
				memset(servent_send_message,0,1024);
				servent_make_command(servent_send_message,servent[s_id].id,servent[send_target].id ,0,"f005");
				

	     			puts("");
	     			if(servent_command_buffer_pointer[send_target] > 99){
	     			servent_command_buffer_pointer[send_target] = 0;
	     			}
	     			strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     			servent_command_buffer_pointer[send_target]++;
	     				
	     			puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			
	     			
	     			servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content); 
				
		        
		        }
		}
		

	   	if(servent_trust_threshold[s_id] >= 4 && servent_latency_signal[s_id]==0 && servent_already_turn_to_latency_signal[s_id] == 0){
	   	printf("servent %d trust_threshold %d !!!!! \n",s_id,servent_trust_threshold[s_id]);
	   	printf("servent %d turn to latency !!!!! \n",s_id);
	   	servent_latency_signal[s_id]=1;
	   	servent_trust_threshold[s_id]=0;
	   	servent_already_turn_to_latency_signal[s_id]=1;
	   	
	   	servent_peer_num[s_id]=0;
		for(i=0;i<NUM_SERVENT_PEER;i++){
		   if(servent_peer_list[s_id][i].peer_id!= -1)
		   servent_peer_num[s_id]++;
		}
		
		if(servent_peer_num[s_id]!=0){
		
		for(i=0;i<NUM_SERVENT_PEER;i++){
		if(servent_peer_list[s_id][i].peer_id != -1 && i>=servent_peer_num[s_id]){
		for ( j = 0; j < servent_peer_num[s_id]; j++) {
		if(servent_peer_list[s_id][j].peer_id == -1){
		    	   temp = servent_peer_list[s_id][j];
			   servent_peer_list[s_id][j] = servent_peer_list[s_id][i];
			   servent_peer_list[s_id][i] = temp;
			   break;
		}
		   
		}
		   
		}
		   
		}
		   
		}
	   	
	   	for ( i = 0; i < servent_peer_num[s_id]; i++) {		   
			
				send_target = servent_peer_list[s_id][i].peer_id;
				
				
				memset(servent_send_message,0,1024);
				servent_make_command(servent_send_message,servent[s_id].id,servent[send_target].id ,0,"f006");
				

	     			puts("");
	     			if(servent_command_buffer_pointer[send_target] > 99){
	     			servent_command_buffer_pointer[send_target] = 0;
	     			}
	     			strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     			servent_command_buffer_pointer[send_target]++;
	     				
	     			puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			
	     			
	     			servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content); 
				
		        
		        
		}
		puts("");

	        /*for ( i = 0; i < servent_send_trust_message_to_me_pointer[s_id]; i++) {	
	        		  
	        		send_target = servent_send_trust_message_to_me[s_id][i];
				printf("servent %ld send f006 to servent %d",s_id,send_target);
				
				memset(servent_send_message,0,1024);
				servent_make_command(servent_send_message,servent[s_id].id,servent[send_target].id ,0,"f006");
				

	     			puts("");
	     			if(servent_command_buffer_pointer[send_target] > 99){
	     			servent_command_buffer_pointer[send_target] = 0;
	     			}
	     			
	     			strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     			servent_command_buffer_pointer[send_target]++;
	     				
	     			puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			
	     			
	     			servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content); 
				
		        	puts("");
		        
	        }*/
	        
	   	}
	   	
	   
	   	
		
		
		if(servent_latency_signal[s_id] == 1 && servent_select_pattern_signal[s_id] == 1){
		puts("I am latency bot. I dont communicate with other bot");
		servent_pattern[s_id]=1;
		}
		
		if(servent_resurrection_request_peer_signal[s_id] == 1){
		
		printf("servent %ld:I need to request 2 peer !!!!!!!\n",s_id);
		
		if(servent_already_execute_resurrection_signal[s_id] == 1){
		for(i=0;i<NUM_SERVENT_PEER;i++){
		if(servent_peer_list[s_id][i].peer_id != -1 && servent_eliminate_signal[servent_peer_list[s_id][i].peer_id] == 1){
		servent_peer_list[s_id][i].peer_id = -1;
		servent_peer_list[s_id][i].reputation_value = 0;
		//servent_peer_list[s_id][i].sensor_signal = 0;
		   
		}
		   
		}
		}
		servent_peer_num[s_id]=0;
		for(i=0;i<NUM_SERVENT_PEER;i++){
		   if(servent_peer_list[s_id][i].peer_id!= -1 )
		   servent_peer_num[s_id]++;
		}
		if(servent_peer_num[s_id]==0){
		printf("servent %ld doesnt has peer !!! \n",s_id);
		
		servent_pattern[s_id]=99;
		
		}
		if(servent_peer_num[s_id]!=0){
		
		for(i=0;i<NUM_SERVENT_PEER;i++){
		if(servent_peer_list[s_id][i].peer_id != -1 && i>=servent_peer_num[s_id]){
		for ( j = 0; j < servent_peer_num[s_id]; j++) {
		if(servent_peer_list[s_id][j].peer_id == -1){
		    	   temp = servent_peer_list[s_id][j];
			   servent_peer_list[s_id][j] = servent_peer_list[s_id][i];
			   servent_peer_list[s_id][i] = temp;
			   break;
		}
		   
		}
		   
		}
		   
		}
		   
		}
		for ( i = 0; i < servent_peer_num[s_id] ; i++) {
		   
		printf("id:%d value:%d\n",servent_peer_list[s_id][i].peer_id, servent_peer_list[s_id][i].reputation_value);
		}
		
		if(servent_peer_num[s_id]!=0){
		int before_send_target = -1;
		int shut_down_signal=0;
		
		for ( k = 0; k < 2; k++){
		
		//printf("k %d \n",k);
		if(shut_down_signal == 1){
		break;
		}
		
		send_target=0;
		invalid_peer_num = 0;
		for(i=0;i<NUM_SERVENT_PEER;i++){
		if(servent_peer_list[s_id][i].peer_id != -1 ){
		if(servent_peer_list[s_id][i].reputation_value == -1 || servent_peer_list[s_id][i].trust_signal == 1 ){//|| servent_boot_signal[servent_peer_list[s_id][i].peer_id] == 0
		invalid_peer_num++;
		}  
		}  
		if(invalid_peer_num >= NUM_SERVENT_PEER-1){
		break;
		}
				
		}
		if(invalid_peer_num >= NUM_SERVENT_PEER-1){
		printf("servent %ld doesnt has peer can communicate !!! \n",s_id);
		servent_pattern[s_id]=99;
		shut_down_signal=1;
		break;
		}
		servent_sort_peer(s_id);
				
		/*for ( i = servent_peer_num[s_id]-1; i >= 0 ; i--) {
			if(servent_peer_list[s_id][i].reputation_value != 5 && servent_peer_list[s_id][i].reputation_value != -1 
			&& servent_peer_list[s_id][i].peer_id != before_send_target){
				send_target = servent_peer_list[s_id][i].peer_id;
						
				break;
			}
					
		}*/
		
		if(servent_peer_num[s_id] != 0){
		peer_num = servent_peer_num[s_id];
		send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
				
				
		for ( i = 0; i < servent_peer_num[s_id]; i++) {
				
		if(servent_peer_list[s_id][i].peer_id == send_target){
		j=i;
		while(servent_peer_list[s_id][j].reputation_value == -1 || servent_peer_list[s_id][j].trust_signal == 1 || servent_peer_list[s_id][j].peer_id == before_send_target 
		){//
			
		send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
		
		for ( j = 0; j < servent_peer_num[s_id]; j++) {
		if(servent_peer_list[s_id][j].peer_id == send_target){
		break;
		}
		}
				
		i = 0;    
		
		}
				
		}
				
		}
				
		}
		
		
		before_send_target = send_target;
		servent_peer_num[send_target]=0;
		for(i=0;i<NUM_SERVENT_PEER;i++){
			if(servent_peer_list[send_target][i].peer_id!= -1 )
			servent_peer_num[send_target]++;
		}
		
				
		if(servent_boot_signal[send_target]==0){
		printf("servent %ld shut down !!! \n",send_target);
		printf("servent %ld reputation_value--!!! \n",send_target);
		for ( i = 0; i < servent_peer_num[s_id]; i++) {
			if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
				servent_peer_list[s_id][i].reputation_value--;
			}
		}
				
		servent_pattern[s_id]=99;
		break;
		}
		if(servent_eliminate_signal[send_target]==1){
		printf("servent %ld eliminate \n",send_target);
		
		servent_pattern[s_id]=99;
		break;
		}
		
		
		if(servent[send_target].sensor_signal == 1 ){
			request_time[send_target]++;
		}
		if(servent[send_target].sensor_signal == 1 && servent[s_id].request_signal == 0 && enumeration_signal == 1){
			servent[s_id].request_signal = 1;
			vs++;
		}
		if(servent_peer_num[send_target]==0){
			printf("send_target:servent %ld doesnt has peer !!! \n",send_target);
			
			
		if(servent_peer_num[s_id] == 1){	
		shut_down_signal=1;
		printf("shut_down_signal %d !!! \n",shut_down_signal);
		}
		servent_pattern[s_id]=99;
		break;
		}
		if(servent_latency_signal[send_target]==1){
		printf("servent %ld already turn to latency bot !!! \n",send_target);
		printf("servent %ld reputation_value--!!! \n",send_target);
		for ( i = 0; i < servent_peer_num[s_id]; i++) {
			if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
				servent_peer_list[s_id][i].reputation_value--;
			}
		}
				
		servent_pattern[s_id]=99;
		break;
		}
		
		
		memset(servent_send_message,0,1024);
		servent_make_command(servent_send_message,servent[s_id].id,servent[send_target].id ,0,"f003");
		printf("servent %ld , send_data: %s , target servent:%d", s_id,servent_send_message,send_target);

	     	puts("");
	     			
	     	if(servent_command_buffer_pointer[send_target] <= 99){
	     	strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     	servent_command_buffer_pointer[send_target]++;
	     	puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     	}
	     	else if(servent_command_buffer_pointer[send_target] > 99){
	     	servent_command_buffer_pointer[send_target] = 0;
	     	strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     	servent_command_buffer_pointer[send_target]++;
	     	puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			
	     	}
	     	
	     	if(servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content) == 0 
	     	//&& servent[send_target].sensor_signal != 1 && servent[send_target].fake_signal != 1
	     	&& enumeration_signal == 1
	     	){//&& enumeration_signal == 1 zxxx
	     	printf("servent %ld reputation_value--!!! \n",send_target);
		for ( i = 0; i < servent_peer_num[s_id]; i++) {
			if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
				servent_peer_list[s_id][i].reputation_value--;
			}
		}
				
		servent_pattern[s_id]=99;
		break;
	     	}
	     					
	     	peer_comparison_result=0;		
	     			
	     	if(servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content) == 1){
	
		if(strcmp(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].command_code ,"f003" )==0 ){
				
		memset(servent_send_message,0,1024);
		servent_make_command(servent_send_message,servent[send_target].id,servent[s_id].id ,0,"f004" );
				
		
		//ccccc 
		
			
		for ( i = 0; i < servent_peer_num[send_target]; i++) {	
		if(servent_peer_list[send_target][i].peer_id == servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id){
		
		
		printf("servent %ld already have servent %d in peer list !\n",send_target,servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id);
		peer_comparison_result=1;
		break;
		}
				
				
		}
		if(send_target == servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id){
		printf("servent %ld cant add itself in peer list !\n",send_target);
		peer_comparison_result=1;
				
		}
		if(servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id == -1 ){
		printf("servent %ld cant add invalid peer in peer list !\n",send_target);
		peer_comparison_result=1;
				
		}
				
		if(peer_comparison_result == 0 && servent_peer_num[send_target]==NUM_SERVENT_PEER){
		printf("servent %ld remove servent %d in peer list !\n",send_target,servent_peer_list[send_target][0].peer_id);
		printf("servent %ld add servent %d in peer list !\n",send_target,servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id);
		servent_peer_list[send_target][0].peer_id = servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id;
		servent_peer_list[send_target][0].reputation_value = reputation_value_base;
				
		
				
		
				
		}
		if(peer_comparison_result == 0 && servent_peer_num[send_target] < NUM_SERVENT_PEER  ){
		//printf("servent %ld remove servent %d in peer list !\n",send_target,servent_peer_list[send_target][0].peer_id);
		printf("servent %ld add servent %d in peer list !\n",send_target,servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id);
		servent_peer_list[send_target][servent_peer_num[send_target]].peer_id = servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id;
		servent_peer_list[send_target][servent_peer_num[send_target]].reputation_value = reputation_value_base;
		servent_peer_num[send_target]++;
		
		}
		//s5s5
		if(servent[send_target].fake_signal == 1 && peer_comparison_result == 0){
				
		if(servent[servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id].detect_signal == 0 && enumeration_signal == 1 && servent[send_target].fake_signal == 1){
				
		servent[servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id].detect_signal = 1;
		vc++;
		} 
				
		if(servent[servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id].request_signal == 0 && enumeration_signal == 1 && servent[send_target].fake_signal == 1){
		
		servent[servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id].request_signal = 1;
		vs++;
		}
				
				
				
		}		
				
				
				
		if(servent_command_buffer_pointer[s_id] <= 99){
		strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content, servent_send_message);
	     	servent_command_buffer_pointer[s_id]++;
		puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
		}
		else if(servent_command_buffer_pointer[s_id] > 99){
		servent_command_buffer_pointer[s_id]=0;
		strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content, servent_send_message);
	     	servent_command_buffer_pointer[s_id]++;
		puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
		}			     	
				
					     			
		}
				
	     	}
		peer_comparison_result=0;		
		if(servent_command_analysis(s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content) == 1){
	
		if(strcmp(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].command_code ,"f004" )==0 ){
							
		if(servent[send_target].fake_signal != 1 && servent[send_target].sensor_signal != 1){
		for ( i = 0; i < servent_peer_num[s_id]; i++) {	

		if(servent_peer_list[s_id][i].peer_id == servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id){
		
		
		
		printf("servent %ld already have servent %d in peer list !\n",s_id,servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id);
		peer_comparison_result=1;
		break;
		}
				
				
		}
		if(s_id == servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id){
		printf("servent %ld cant add itself in peer list !\n",s_id);
		peer_comparison_result=1;
				
		}
		
		if(servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id == -1){
		printf("servent %ld cant add invalid peer in peer list !\n",s_id);
		peer_comparison_result=1;
				
		}
		if(peer_comparison_result == 0 && servent_peer_num[s_id]==NUM_SERVENT_PEER){
		printf("servent %ld remove servent %d in peer list !\n",s_id,servent_peer_list[s_id][0].peer_id);
		printf("servent %ld add servent %d in peer list !\n",s_id,servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id);
		servent_peer_list[s_id][0].peer_id = servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id;
		servent_peer_list[s_id][0].reputation_value = reputation_value_base;
					
		
				
		}
		if(peer_comparison_result == 0 && servent_peer_num[s_id] < NUM_SERVENT_PEER  ){
		//printf("servent %ld remove servent %d in peer list !\n",s_id,servent_peer_list[s_id][0].peer_id);
		printf("servent %ld add servent %d in peer list !\n",s_id,servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id);
		servent_peer_list[s_id][servent_peer_num[s_id]].peer_id = servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id;
		servent_peer_list[s_id][servent_peer_num[s_id]].reputation_value = reputation_value_base;
		
		servent_peer_num[s_id]++;
				
		
		}
		}		
		else if(servent[send_target].fake_signal == 1 || servent[send_target].sensor_signal == 1){
		char string_ip[100],string_port[100],string_id[100];
		extra_information_split(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].extra_information,string_ip,string_port,string_id);
		int send_peer = atoi(string_id);
		
		for ( i = 0; i < servent_peer_num[s_id]; i++) {	
		if(servent_peer_list[s_id][i].peer_id == send_peer){
				
				
				
				
		printf("servent %ld already have servent %d in peer list !\n",s_id,send_peer);
		peer_comparison_result=1;
		break;
		}
				
				
		}
		if(s_id == send_peer){
		printf("servent %ld cant add itself in peer list !\n",s_id);
		peer_comparison_result=1;
				
		}
		if(send_peer == -1){
		printf("servent %ld cant add invalid peer in peer list !\n",s_id);
		peer_comparison_result=1;
				
		}
		if(peer_comparison_result == 0 && servent_peer_num[s_id]==NUM_SERVENT_PEER){
		printf("servent %ld remove servent %d in peer list !\n",s_id,servent_peer_list[s_id][0].peer_id);
		printf("servent %ld add servent %d in peer list !\n",s_id,send_peer);
		servent_peer_list[s_id][0].peer_id = send_peer;
		servent_peer_list[s_id][0].reputation_value = reputation_value_base;

				
				
				
		}
		if(peer_comparison_result == 0 && servent_peer_num[s_id] < NUM_SERVENT_PEER  ){
		//printf("servent %ld remove servent %d in peer list !\n",s_id,servent_peer_list[s_id][0].peer_id);
		printf("servent %ld add servent %d in peer list !\n",s_id,send_peer);
		servent_peer_list[s_id][servent_peer_num[s_id]].peer_id = send_peer;
		servent_peer_list[s_id][servent_peer_num[s_id]].reputation_value = reputation_value_base;

		servent_peer_num[s_id]++;
				
				
								
				
		}
		
		}
				
					
				  			
				
		servent_rearrange_peer(s_id);   
				
		for(i=0;i<servent_peer_num[s_id];i++){	
		if(servent_peer_list[s_id][i].peer_id ==  send_target && servent_peer_list[s_id][i].reputation_value < reputation_value_max ){
		servent_peer_list[s_id][i].reputation_value++;
		break;
		}
		}
				
				
					
		servent_sort_peer(s_id);			
				
		for ( i = 0; i < servent_peer_num[s_id]; i++) {
		if(servent_peer_list[s_id][i].peer_id!= -1 ){
		printf("id:%d value:%d\n",servent_peer_list[s_id][i].peer_id, servent_peer_list[s_id][i].reputation_value);
		}
				
		}	     	
				
					     			
		}
				
	     	}
		
		
			
		}
			if(servent_already_execute_resurrection_signal[s_id] == 1){
			for(i=0;i<NUM_SERVENT_PEER;i++){
			if(servent_peer_list[s_id][i].peer_id != -1 && servent_eliminate_signal[servent_peer_list[s_id][i].peer_id] == 1){
			servent_peer_list[s_id][i].peer_id = -1;
			servent_peer_list[s_id][i].reputation_value = 0;
			//servent_peer_list[s_id][i].sensor_signal = 0;
			   
			}
			   
			}
			}
			servent_rearrange_peer(s_id); 
			
			servent_peer_num[s_id]=0;
			for(i=0;i<NUM_SERVENT_PEER;i++){
			   if(servent_peer_list[s_id][i].peer_id!= -1 )
			   servent_peer_num[s_id]++;
			}
			
			for(i=0;i<servent_peer_num[s_id];i++){
			if(servent_peer_list[s_id][i].reputation_value > 2 && servent_peer_list[s_id][i].peer_id !=-1 ){

			       
			       servent_peer_list[s_id][i].reputation_value = 2;
			       
			}
			else if( servent_latency_signal[servent_peer_list[s_id][i].peer_id] == 1 && servent_peer_list[s_id][i].peer_id !=-1 ){

			       
			       servent_peer_list[s_id][i].reputation_value = 2;
			       
			}
			else if(servent_peer_list[s_id][i].reputation_value <= 2 && servent_peer_list[s_id][i].peer_id !=-1 ){

			       
			       servent_peer_list[s_id][i].reputation_value = 0;
			       
			}
			   
			   
			}
		
		}
		
		servent_resurrection_request_peer_signal[s_id] = 0;
		
		
		}
		
		if(servent_pattern[s_id]!=99){
		servent_log[s_id][servent_execute_pattern_time[s_id]].num_pattern = servent_pattern[s_id];
		servent_log[s_id][servent_execute_pattern_time[s_id]].now_sec = now_sec;
		printf(" servent[%ld]_pattern = %d now_sec = %d !\n", s_id,servent_pattern[s_id],now_sec);
		}
		
		//zxcc
		switch(servent_pattern[s_id]) {
			case 0:
				
				
				servent_pattern[s_id]=99;
				break;
			case 1://botmaster_command
				//file_num=rand()  % 3;
				file_num=0;//***-
				switch(file_num){
				case 0:
					a=0;
					b=0;
					
					printf("servent %ld receive command%d from relay station:\n",s_id,a+1);
					while(file_data[a][b][0]!= '\0'){
					
					for(i=0;i<servent_command_buffer_pointer[s_id];i++){	
					
						command_comparison_result=strcmp(servent_command_buffer[s_id][i].content,file_data[a][b]);
						if(command_comparison_result== 0){
						puts(servent_command_buffer[s_id][i].content);
						break;
						}
					}
					
					if(command_comparison_result== 0){
					printf("servent %ld already have (%s)\n",s_id,file_data[a][b]);
					
					}
					if(command_comparison_result != 0){
					if(servent_command_buffer_pointer[s_id] > 99){
					servent_command_buffer_pointer[s_id]=0;
					}
					strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content,file_data[a][b]);
					puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content);
					
					servent_command_buffer_pointer[s_id]++;
					servent_command_analysis(s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
					}
					
					
					b++;
					
					}
					
					
					
					break;
				case 1:
					a=1;
					b=0;
					
					printf("servent %ld receive command%d from relay station:\n",s_id,a+1);
					while(file_data[a][b][0]!= '\0'){
					
					for(i=0;i<servent_command_buffer_pointer[s_id];i++){	
					command_comparison_result=strcmp(servent_command_buffer[s_id][i].content,file_data[a][b]);
					if(command_comparison_result== 0){break;}
					}
					
					if(command_comparison_result== 0){
					printf("servent %ld already have (%s)\n",s_id,file_data[a][b]);
					
					}
					if(command_comparison_result != 0){
					if(servent_command_buffer_pointer[s_id] > 99){
					servent_command_buffer_pointer[s_id]=0;
					}
					strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content,file_data[a][b]);
					puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content);
					
					servent_command_buffer_pointer[s_id]++;
					servent_command_analysis(s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
					}
					
					
					b++;
					
					}
					
					
					
					break;
				case 2:
					a=2;
					b=0;
					printf("servent %ld receive command%d from relay station:\n",s_id,a+1);
					while(file_data[a][b][0]!= '\0'){
					
					for(i=0;i<servent_command_buffer_pointer[s_id];i++){	
					command_comparison_result=strcmp(servent_command_buffer[s_id][i].content,file_data[a][b]);
					if(command_comparison_result== 0){break;}
					}
					
					if(command_comparison_result== 0){
					printf("servent %ld already have (%s)\n",s_id,file_data[a][b]);
					
					}
					if(command_comparison_result != 0){
					if(servent_command_buffer_pointer[s_id] > 99){
					servent_command_buffer_pointer[s_id]=0;
					}
					strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content,file_data[a][b]);
					puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content);
					servent_command_buffer_pointer[s_id]++;
					servent_command_analysis(s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
					}
					
					
					b++;
					
					}
					
					
					break;
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 1;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				
				break;
			case 2:
				
				send_target=0;
				invalid_peer_num = 0;
				if(servent_already_execute_resurrection_signal[s_id] == 1){
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(servent_peer_list[s_id][i].peer_id != -1 && servent_eliminate_signal[servent_peer_list[s_id][i].peer_id] == 1){
				servent_peer_list[s_id][i].peer_id = -1;
				servent_peer_list[s_id][i].reputation_value = 0;
				//servent_peer_list[s_id][i].sensor_signal = 0;
				}
				   
				}
				}
				servent_peer_num[s_id]=0;
				for(i=0;i<NUM_SERVENT_PEER;i++){
				
				if(servent_peer_list[s_id][i].peer_id!= -1)
				servent_peer_num[s_id]++;
					
				} 
				
				if(servent_peer_num[s_id]==0){
				printf("servent %ld doesnt has peer !!! \n",s_id);
				servent_pattern[s_id]=99;
				break;
				}
				
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(servent_peer_list[s_id][i].peer_id != -1 ){
				if(servent_peer_list[s_id][i].reputation_value == -1 || servent_peer_list[s_id][i].trust_signal == 1 ){//|| servent_boot_signal[servent_peer_list[s_id][i].peer_id] == 0
				invalid_peer_num++;
				}  
				}  
				if(invalid_peer_num >= NUM_SERVENT_PEER){
				break;
				}
				
				}
				if(invalid_peer_num >= NUM_SERVENT_PEER){
				printf("servent %ld doesnt has peer can communicate !!! \n",s_id);
				servent_pattern[s_id]=99;
				break;
				}
				servent_rearrange_peer(s_id);
				servent_sort_peer(s_id);
				
				/*for ( i = servent_peer_num[s_id]-1; i >= 0 ; i--) {
					if(servent_peer_list[s_id][i].reputation_value != 5 && servent_peer_list[s_id][i].reputation_value != -1 ){
						send_target = servent_peer_list[s_id][i].peer_id;
								
						break;
					}
							
				}*/
				
				if(servent_peer_num[s_id] != 0){
				peer_num = servent_peer_num[s_id];
				send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
		
				
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
				
				if(servent_peer_list[s_id][i].peer_id == send_target){
				j=i;
				while(servent_peer_list[s_id][j].reputation_value == -1 || servent_peer_list[s_id][j].trust_signal == 1 ){//
				
					
				send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
				
				for ( j = 0; j < servent_peer_num[s_id]; j++) {
				if(servent_peer_list[s_id][j].peer_id == send_target){
				break;
				}
				}
				
				i = 0;    
				
				}
				
				}
				
				}
				
				}
				
				
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_target = send_target;
				
				
				
				if(servent_boot_signal[send_target]==0){
				printf("servent %ld shut down !!! \n",send_target);
				printf("servent %ld reputation_value--!!! \n",send_target);
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
					if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
						servent_peer_list[s_id][i].reputation_value--;
					}
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				break;
				}
				
				
				if(servent[send_target].sensor_signal == 1 ){
						request_time[send_target]++;
				}
				if(servent[send_target].sensor_signal == 1 && servent[s_id].request_signal == 0 && enumeration_signal == 1){
						servent[s_id].request_signal = 1;
						vs++;
				}
				
				if(servent_latency_signal[send_target]==1){
				printf("servent %ld already turn to latency bot !!! \n",send_target);
				printf("servent %ld reputation_value--!!! \n",send_target);
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
					if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
						servent_peer_list[s_id][i].reputation_value--;
					}
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				
				break;
				}
				
				//asdf
				
				char newest_command[1024];
				memset(newest_command,0,strlen(newest_command));
				if(servent_command_buffer_pointer[send_target] != 0 && strlen(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content)!= 0){
				strcpy(newest_command, servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
				}
				
				memset(servent_send_message,0,1024);
				servent_make_command(servent_send_message,servent[s_id].id,servent[send_target].id ,0,"f001");
				

	     			puts("");
	     			
	     			
	     			
	     			if(servent_command_buffer_pointer[send_target] <= 99){
	     			strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     			servent_command_buffer_pointer[send_target]++;
	     			puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			}
	     			else if(servent_command_buffer_pointer[send_target] > 99){
	     			servent_command_buffer_pointer[send_target] = 0;
	     			strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     			servent_command_buffer_pointer[send_target]++;
	     			puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			
	     			}
	     			
	     			if(servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content) == 0 
	     			//&& servent[send_target].sensor_signal != 1 && servent[send_target].fake_signal != 1
	     			&& enumeration_signal == 1
	     			){
	     			//&& enumeration_signal == 1 zxxx
	     			printf("servent %ld reputation_value--!!! \n",send_target);
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
					if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
						servent_peer_list[s_id][i].reputation_value--;
					}
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				
				break;
	     			}
	     			
	     			
	     			if(servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content) == 1){
				
				if(strcmp(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].command_code ,"f001" )==0 ){
				
				
				
				
				memset(servent_send_message,0,1024);
				servent_make_command(servent_send_message,servent[send_target].id,servent[s_id].id ,0,"f002" );
				
				
				
				if(servent_command_buffer_pointer[s_id] <= 99){
				strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content, servent_send_message);
	     			servent_command_buffer_pointer[s_id]++;
				puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
				}
				else if(servent_command_buffer_pointer[s_id] > 99){
				servent_command_buffer_pointer[s_id]=0;
				strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content, servent_send_message);
	     			servent_command_buffer_pointer[s_id]++;
				puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
				}
				
				
					     			
				}
				
	     			}
	     			
	     			if(servent_command_analysis(s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content) == 1){
	
				if(strcmp(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].command_code ,"f002" )==0 ){
							
				
				printf("servent %ld receive command from servent %ld:\n",s_id,send_target);

				if(strlen(newest_command)!= 0){
				
				for(i=0;i<servent_command_buffer_pointer[s_id];i++){	
				command_comparison_result=strcmp(servent_command_buffer[s_id][i].content,newest_command);
				if(command_comparison_result== 0){break;}
				}
							
				if(command_comparison_result== 0){
				printf("servent %ld already have (%s) \n",s_id,newest_command);
				}
				if(command_comparison_result != 0){
				if(servent_command_buffer_pointer[s_id] > 99){
				servent_command_buffer_pointer[s_id]=0;
				}
				
				strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content,newest_command);
				puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content);
				servent_command_buffer_pointer[s_id]++;
				servent_command_analysis(s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
				}
				
				}
					
				
				
				
				
				
				servent_rearrange_peer(s_id);   
				
				for(i=0;i<servent_peer_num[s_id];i++){	
				if(servent_peer_list[s_id][i].peer_id ==  send_target && servent_peer_list[s_id][i].reputation_value < reputation_value_max ){
				servent_peer_list[s_id][i].reputation_value++;
				break;
				}
				}
				
				
					
				servent_sort_peer(s_id);			
				
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
				if(servent_peer_list[s_id][i].peer_id!= -1 ){
				printf("id:%d value:%d\n",servent_peer_list[s_id][i].peer_id, servent_peer_list[s_id][i].reputation_value);
				}
				
				}	     			
				
					     	
				
					     			
				}
				
	     			}
	     			
	     			
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 1;
				servent_execute_pattern_time[s_id]++;
	     			servent_pattern[s_id]=99;
			 	
				break;
			case 3:
				
				send_target=0;
				invalid_peer_num = 0;
				if(servent_already_execute_resurrection_signal[s_id] == 1){
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(servent_peer_list[s_id][i].peer_id != -1 && servent_eliminate_signal[servent_peer_list[s_id][i].peer_id] == 1){
				servent_peer_list[s_id][i].peer_id = -1;
				servent_peer_list[s_id][i].reputation_value = 0;
				//servent_peer_list[s_id][i].sensor_signal = 0;
				}
				   
				}
				}
				servent_peer_num[s_id]=0;
				for(i=0;i<NUM_SERVENT_PEER;i++){
				
				if(servent_peer_list[s_id][i].peer_id!= -1)
				servent_peer_num[s_id]++;
					
				} 
				
				if(servent_peer_num[s_id]==0){
				printf("servent %ld doesnt has peer !!! \n",s_id);
				servent_pattern[s_id]=99;
				break;
				}
				
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(servent_peer_list[s_id][i].peer_id != -1 ){
				if(servent_peer_list[s_id][i].reputation_value == -1 || servent_peer_list[s_id][i].trust_signal == 1 ){//|| servent_boot_signal[servent_peer_list[s_id][i].peer_id] == 0
				invalid_peer_num++;
				}  
				}  
				if(invalid_peer_num >= NUM_SERVENT_PEER-1){
				break;
				}
				
				}
				if(invalid_peer_num >= NUM_SERVENT_PEER-1){
				printf("servent %ld doesnt has peer can communicate !!! \n",s_id);
				servent_pattern[s_id]=99;
				break;
				}
				servent_rearrange_peer(s_id);
				servent_sort_peer(s_id);
				
				/*for ( i = servent_peer_num[s_id]-1; i >= 0 ; i--) {
					if(servent_peer_list[s_id][i].reputation_value != 5 && servent_peer_list[s_id][i].reputation_value != -1 ){
						send_target = servent_peer_list[s_id][i].peer_id;
								
						break;
					}
							
				}*/
				if(servent_peer_num[s_id] != 0){
				peer_num = servent_peer_num[s_id];
				send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
				
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
				
				if(servent_peer_list[s_id][i].peer_id == send_target){
				j=i;
				while(servent_peer_list[s_id][j].reputation_value == -1 || servent_peer_list[s_id][j].trust_signal == 1  ){
				
					
				send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
				
				for ( j = 0; j < servent_peer_num[s_id]; j++) {
				if(servent_peer_list[s_id][j].peer_id == send_target){
				break;
				}
				}
				
				i = 0;    
				
				}
				
				}
				
				}
				
				}
				
				
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_target = send_target;
				servent_peer_num[send_target]=0;
				for(i=0;i<NUM_SERVENT_PEER;i++){
				
				if(servent_peer_list[send_target][i].peer_id!= -1){
				servent_peer_num[send_target]++;
				}	
				}
				
				if(servent_boot_signal[send_target]==0){
				printf("servent %ld shut down !!! \n",send_target);
				printf("servent %ld reputation_value--!!! \n",send_target);
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
					if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
						servent_peer_list[s_id][i].reputation_value--;
					}
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				break;
				}
				if(servent[send_target].sensor_signal == 1 ){
						request_time[send_target]++;
				}
				if(servent[send_target].sensor_signal == 1 && servent[s_id].request_signal == 0 && enumeration_signal == 1){
						servent[s_id].request_signal = 1;
						vs++;
				}
				 
				if(servent_peer_num[send_target]==0){
				printf("send_target:servent %ld doesnt has peer !!! \n",send_target);
				
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				break;
				}
				
				if(servent_latency_signal[send_target]==1){
				printf("servent %ld already turn to latency bot !!! \n",send_target);
				printf("servent %ld reputation_value--!!! \n",send_target);
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
					if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
						servent_peer_list[s_id][i].reputation_value--;
					}
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id] = 99;
				
				break;
				}
				
				
				
				
				
				memset(servent_send_message,0,1024);
				servent_make_command(servent_send_message,servent[s_id].id,servent[send_target].id ,0,"f003");
				printf("servent %ld , send_data: %s , target servent:%d", s_id,servent_send_message,send_target);

	     			puts("");
	     			
	     			if(servent_command_buffer_pointer[send_target] <= 99){
	     			strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     			servent_command_buffer_pointer[send_target]++;
	     			puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			}
	     			else if(servent_command_buffer_pointer[send_target] > 99){
	     			servent_command_buffer_pointer[send_target] = 0;
	     			strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     			servent_command_buffer_pointer[send_target]++;
	     			puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			
	     			}
	     			
	     			if(servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content) == 0 
	     			//&& servent[send_target].sensor_signal != 1 && servent[send_target].fake_signal != 1
	     			&& enumeration_signal == 1
	     			){
	     			//&& enumeration_signal == 1 zxxx
	     			printf("servent %ld reputation_value--!!! \n",send_target);
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
					if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
						servent_peer_list[s_id][i].reputation_value--;
					}
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				
				break;
	     			}
	     			
	     			
	     			peer_comparison_result=0;
	     			if(servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content) == 1){
	
				if(strcmp(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].command_code ,"f003" )==0 ){
				
				memset(servent_send_message,0,1024);
				servent_make_command(servent_send_message,servent[send_target].id,servent[s_id].id ,0,"f004" );
				
				
				
				//------
				
				
				for ( i = 0; i < servent_peer_num[send_target]; i++) {	
				if(servent_peer_list[send_target][i].peer_id == servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id){
				
				
				printf("servent %ld already have servent %d in peer list !\n",send_target,servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id);
				peer_comparison_result=1;
				break;
				}
				
				
				}
				if(send_target == servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id){
				printf("servent %ld cant add itself in peer list !\n",send_target);
				peer_comparison_result=1;
				
				}
				if(servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id == -1 ){
				printf("servent %ld cant add invalid peer in peer list !\n",send_target);
				peer_comparison_result=1;
				
				}
				
				if(peer_comparison_result == 0 && servent_peer_num[send_target]==NUM_SERVENT_PEER){
				printf("servent %ld remove servent %d in peer list !\n",send_target,servent_peer_list[send_target][0].peer_id);
				printf("servent %ld add servent %d in peer list !\n",send_target,servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id);
				servent_peer_list[send_target][0].peer_id = servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id;
				servent_peer_list[send_target][0].reputation_value = reputation_value_base;
				
				
					
				}
				if(peer_comparison_result == 0 && servent_peer_num[send_target] < NUM_SERVENT_PEER  ){
				//printf("servent %ld remove servent %d in peer list !\n",send_target,servent_peer_list[send_target][0].peer_id);
				printf("servent %ld add servent %d in peer list !\n",send_target,servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id);
				servent_peer_list[send_target][servent_peer_num[send_target]].peer_id = servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id;
				servent_peer_list[send_target][servent_peer_num[send_target]].reputation_value = reputation_value_base;
				servent_peer_num[send_target]++;
				

				}
				//s5s5
				if(servent[send_target].fake_signal == 1 && peer_comparison_result == 0){
				
				if(servent[servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id].detect_signal == 0 && enumeration_signal == 1 && servent[send_target].fake_signal == 1){
				
				servent[servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id].detect_signal = 1;
				vc++;
				} 
				
				if(servent[servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id].request_signal == 0 && enumeration_signal == 1 && servent[send_target].fake_signal == 1){
		
				servent[servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id].request_signal = 1;
				vs++;
				}
				
				
				
				}
				if(servent_log[s_id][servent_execute_pattern_time[s_id]].num_pattern == 3 && peer_comparison_result == 0){
				servent_log[s_id][servent_execute_pattern_time[s_id]].send_peer=servent_peer_list[s_id][servent_peer_num[s_id]-1].peer_id;
				
				}
				//------
				if(servent_command_buffer_pointer[s_id] <= 99){
				strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content, servent_send_message);
	     			servent_command_buffer_pointer[s_id]++;
				puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
				}
				else if(servent_command_buffer_pointer[s_id] > 99){
				servent_command_buffer_pointer[s_id]=0;
				strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content, servent_send_message);
	     			servent_command_buffer_pointer[s_id]++;
				puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
				}	     	
				
					     			
				}
				
	     			}
				peer_comparison_result=0;
				if(servent_command_analysis(s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content) == 1){
	
				if(strcmp(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].command_code ,"f004" )==0 ){
							
				//ccccc
				if(servent[send_target].fake_signal != 1 && servent[send_target].sensor_signal != 1){
				for ( i = 0; i < servent_peer_num[s_id]; i++) {	
				if(servent_peer_list[s_id][i].peer_id == servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id){
				
				
				
				
				printf("servent %ld already have servent %d in peer list !\n",s_id,servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id);
				peer_comparison_result=1;
				break;
				}
				
				
				}
				if(s_id == servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id){
				printf("servent %ld cant add itself in peer list !\n",s_id);
				peer_comparison_result=1;
				
				}
				if(servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id == -1){
				printf("servent %ld cant add invalid peer in peer list !\n",s_id);
				peer_comparison_result=1;
				
				}
				if(peer_comparison_result == 0 && servent_peer_num[s_id]==NUM_SERVENT_PEER){
				printf("servent %ld remove servent %d in peer list !\n",s_id,servent_peer_list[s_id][0].peer_id);
				printf("servent %ld add servent %d in peer list !\n",s_id,servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id);
				servent_peer_list[s_id][0].peer_id = servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id;
				servent_peer_list[s_id][0].reputation_value = reputation_value_base;

				
				
				
				}
				if(peer_comparison_result == 0 && servent_peer_num[s_id] < NUM_SERVENT_PEER  ){
				//printf("servent %ld remove servent %d in peer list !\n",s_id,servent_peer_list[s_id][0].peer_id);
				printf("servent %ld add servent %d in peer list !\n",s_id,servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id);
				servent_peer_list[s_id][servent_peer_num[s_id]].peer_id = servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id;
				servent_peer_list[s_id][servent_peer_num[s_id]].reputation_value = reputation_value_base;

				servent_peer_num[s_id]++;
				
				
								
				
				}
				if(servent_log[s_id][servent_execute_pattern_time[s_id]].num_pattern == 3 && peer_comparison_result == 0 
				&& (servent[send_target].fake_signal != 1 && servent[send_target].sensor_signal != 1)){
				servent_log[s_id][servent_execute_pattern_time[s_id]].receive_peer=servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id;
				
				}
				
				}
				
				else if(servent[send_target].fake_signal == 1 || servent[send_target].sensor_signal == 1){
				char string_ip[100],string_port[100],string_id[100];
				extra_information_split(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].extra_information,string_ip,string_port,string_id);
				int send_peer = atoi(string_id);
				
				
				for ( i = 0; i < servent_peer_num[s_id]; i++) {	
				if(servent_peer_list[s_id][i].peer_id == send_peer){
				
				
				
				
				printf("servent %ld already have servent %d in peer list !\n",s_id,send_peer);
				peer_comparison_result=1;
				break;
				}
				
				
				}
				if(s_id == send_peer){
				printf("servent %ld cant add itself in peer list !\n",s_id);
				peer_comparison_result=1;
				
				}
				if(send_peer == -1){
				printf("servent %ld cant add invalid peer in peer list !\n",s_id);
				peer_comparison_result=1;
				
				}
				if(peer_comparison_result == 0 && servent_peer_num[s_id]==NUM_SERVENT_PEER){
				printf("servent %ld remove servent %d in peer list !\n",s_id,servent_peer_list[s_id][0].peer_id);
				printf("servent %ld add servent %d in peer list !\n",s_id,send_peer);
				servent_peer_list[s_id][0].peer_id = send_peer;
				servent_peer_list[s_id][0].reputation_value = reputation_value_base;

				
				
				
				}
				if(peer_comparison_result == 0 && servent_peer_num[s_id] < NUM_SERVENT_PEER  ){
				//printf("servent %ld remove servent %d in peer list !\n",s_id,servent_peer_list[s_id][0].peer_id);
				printf("servent %ld add servent %d in peer list !\n",s_id,send_peer);
				servent_peer_list[s_id][servent_peer_num[s_id]].peer_id = send_peer;
				servent_peer_list[s_id][servent_peer_num[s_id]].reputation_value = reputation_value_base;

				servent_peer_num[s_id]++;
				
				
								
				
				}
				if(servent_log[s_id][servent_execute_pattern_time[s_id]].num_pattern == 3 && peer_comparison_result == 0 
				&& (servent[send_target].fake_signal == 1 || servent[send_target].sensor_signal == 1) ){
				
				servent_log[s_id][servent_execute_pattern_time[s_id]].receive_peer=send_peer;
				/*printf("servent %ld servent_log[s_id][servent_execute_pattern_time[s_id]].receive_peer %d !\n",s_id,servent_log[s_id][servent_execute_pattern_time[s_id]].receive_peer);*/
				}
				}
					
				  			
				
				servent_rearrange_peer(s_id);   
				
				for(i=0;i<servent_peer_num[s_id];i++){	
				if(servent_peer_list[s_id][i].peer_id ==  send_target && servent_peer_list[s_id][i].reputation_value < reputation_value_max ){
				servent_peer_list[s_id][i].reputation_value++;
				break;
				}
				}
				
				
					
				servent_sort_peer(s_id);			
				
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
				if(servent_peer_list[s_id][i].peer_id!= -1 ){
				printf("id:%d value:%d\n",servent_peer_list[s_id][i].peer_id, servent_peer_list[s_id][i].reputation_value);
				}
				
				}	     	
				
					     			
				}
				
	     			}
				//------
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 1;
				servent_execute_pattern_time[s_id]++;
	     			servent_pattern[s_id]=99;
				break;	
				
			case 4:	
			        send_target=0;
				invalid_peer_num = 0;
				if(servent_already_execute_resurrection_signal[s_id] == 1){
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(servent_peer_list[s_id][i].peer_id != -1 && servent_eliminate_signal[servent_peer_list[s_id][i].peer_id] == 1){
				servent_peer_list[s_id][i].peer_id = -1;
				servent_peer_list[s_id][i].reputation_value = 0;
				//servent_peer_list[s_id][i].sensor_signal = 0;
				
				}
				   
				}
				}
				servent_peer_num[s_id]=0;
				for(i=0;i<NUM_SERVENT_PEER;i++){
				
				if(servent_peer_list[s_id][i].peer_id!= -1)
				servent_peer_num[s_id]++;
					
				} 
				
				if(servent_peer_num[s_id]==0){
				printf("servent %ld doesnt has peer !!! \n",s_id);
				servent_pattern[s_id]=99;
				break;
				}
				
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(servent_peer_list[s_id][i].peer_id != -1 ){
				if(servent_peer_list[s_id][i].reputation_value == -1 || servent_peer_list[s_id][i].trust_signal == 1 ){//|| servent_boot_signal[servent_peer_list[s_id][i].peer_id] == 0
				invalid_peer_num++;
				}  
				}  
				if(invalid_peer_num >= NUM_SERVENT_PEER){
				break;
				}
				
				}
				if(invalid_peer_num >= NUM_SERVENT_PEER){
				printf("servent %ld doesnt has peer can communicate !!! \n",s_id);
				servent_pattern[s_id]=99;
				break;
				}
				servent_rearrange_peer(s_id);
				servent_sort_peer(s_id);
				
				/*for ( i = servent_peer_num[s_id]-1; i >= 0 ; i--) {
					if(servent_peer_list[s_id][i].reputation_value != 5 && servent_peer_list[s_id][i].reputation_value != -1 ){
						send_target = servent_peer_list[s_id][i].peer_id;
								
						break;
					}
							
				}*/
				if(servent_peer_num[s_id] != 0){
				peer_num = servent_peer_num[s_id];
				send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
				
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
				
				if(servent_peer_list[s_id][i].peer_id == send_target){
				j=i;
				while(servent_peer_list[s_id][j].reputation_value == -1 || servent_peer_list[s_id][j].trust_signal == 1 ){//|| servent_boot_signal[servent_peer_list[s_id][j].peer_id] == 0
				
					
				send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
				
				for ( j = 0; j < servent_peer_num[s_id]; j++) {
				if(servent_peer_list[s_id][j].peer_id == send_target){
				break;
				}
				}
				
				i = 0;    
				
				}
				
				}
				
				}
				
				}
				/*
				if(servent[send_target].sensor_signal == 1 ){
						request_time++;
				}
				if(servent[send_target].sensor_signal == 1 && servent[s_id].request_signal == 0 && enumeration_signal == 1){
						servent[s_id].request_signal = 1;
						vs++;
				}*/
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_target = send_target;
				if(servent_latency_signal[send_target]==1){
				printf("servent %ld already turn to latency bot !!! \n",send_target);
				
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				break;
				}
				
				if(servent_boot_signal[send_target]==0){
				printf("servent %ld shut down !!! \n",send_target);
				
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				break;
				}
				/*if(servent[send_target].sensor_signal == 1){
				printf("servent %ld is sensor bot !!! \n",send_target);
				
				
				servent_pattern[s_id]=99;
				break;
				}
				if(servent[send_target].fake_signal == 1){
				printf("servent %ld is crawler bot !!! \n",send_target);
				
				servent_pattern[s_id]=99;
				break;
				}*/
				
				
				b=0;
				
				printf("website from servent %ld:\n",send_target);
				while(servent_bot_website_buffer[send_target][b].url[0]!= '\0'){
							
				for(i=0;i<servent_bot_website_buffer_pointer[s_id];i++){	
				website_comparison_result=strcmp(servent_bot_website_buffer[s_id][i].url,servent_bot_website_buffer[send_target][b].url);
				if(website_comparison_result== 0){break;}
				}
							
				if(website_comparison_result== 0){
				printf("servent %ld already have (%s)\n",s_id,servent_bot_website_buffer[send_target][b].url);
				}
				if(website_comparison_result != 0){
				strcpy(servent_bot_website_buffer[s_id][servent_bot_website_buffer_pointer[s_id]].url,servent_bot_website_buffer[send_target][b].url);
				servent_bot_website_buffer[s_id][servent_bot_website_buffer_pointer[s_id]].reputation_value = servent_bot_website_buffer[send_target][b].reputation_value ;
				
				puts(servent_bot_website_buffer[s_id][servent_bot_website_buffer_pointer[s_id]].url);
				servent_bot_website_buffer_pointer[s_id]++;
				}				
				b++;			
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 1;
				servent_execute_pattern_time[s_id]++;
	     			servent_pattern[s_id]=99;
				break;	
			
			case (-1):
				printf(" servent %ld terminated !\n", s_id);
				break;	
		}
		//***-
		
		time(&current);
		info = localtime( &current );
		
		strftime(string_now_year,sizeof(string_now_year),"%Y",info);
		now_year = atoi(string_now_year);
		strftime(string_now_month,sizeof(string_now_month),"%m",info);
		now_month = atoi(string_now_month);
		strftime(string_now_day,sizeof(string_now_day),"%d",info);
		now_day = atoi(string_now_day);
		
		strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
		now_hour = atoi(string_now_hour);	
		strftime(string_now_min,sizeof(string_now_min),"%M",info);
		now_min = atoi(string_now_min);	
		strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
		now_sec = atoi(string_now_sec);	
		    
		now_sec+=(60*now_min)+(60*60*now_hour);
		
		
		if(servent_select_pattern_signal[s_id] == 1){
		servent_last_time_select_pattern[s_id]  = now_sec;

		
		}
			
			int delete_command_signal=0;
		//printf("before:servent_command_buffer_pointer[s_id] %d  !\n", servent_command_buffer_pointer[s_id]);
			
		for ( i = 0; i < servent_command_buffer_pointer[s_id]; i++) {
		
		if(strlen(servent_command_buffer[s_id][i].content) != 0){
			
			//puts(servent_command_buffer[s_id][i].content);
			timestamp_split(servent_command_buffer[s_id][i].timestamp ,string_command_year,string_command_month,string_command_day,string_command_sec);
			
			command_year = atoi(string_command_year);	
			command_month = atoi(string_command_month);	
			command_day = atoi(string_command_day);	
			command_sec = atoi(string_command_sec);	
			//printf("effective_time %d  !\n", servent_command_buffer[s_id][i].effective_time);
			deadline_sec = command_sec + servent_command_buffer[s_id][i].effective_time;
			deadline_day = command_day ;
			deadline_month = command_month ;
			deadline_year = command_year ;
			int leap_year=0;
			if((deadline_year % 4 == 0 && deadline_year % 100 != 0 ) || deadline_year % 400 == 0) {
			leap_year = 1;
			}
			//printf("leap_year %d  !\n", leap_year);
			
			if(deadline_sec > 86400 ){
			deadline_sec = deadline_sec-86400;
			deadline_day++;
			}
			if( (deadline_month == 4 || deadline_month == 6 || deadline_month == 9 || deadline_month == 11) && deadline_day > 30 ){
			deadline_day = 1;
			deadline_month++;
			}
			if( (deadline_month == 1 || deadline_month == 3 || deadline_month == 5 || deadline_month == 7 || deadline_month == 10 || deadline_month == 12 ) && deadline_day > 31 ){
			deadline_day = 1;
			deadline_month++;
			}
			if( leap_year == 1 && ( deadline_month == 2) && ( deadline_day > 29) ){
			deadline_day = 1;
			deadline_month++;
			}
			if( leap_year == 0 && ( deadline_month == 2) && ( deadline_day > 28) ){
			deadline_day = 1;
			deadline_month++;
			}
			if( deadline_month > 12 ){
			deadline_month = 1;
			deadline_year++;
			}
			
			/*printf("deadline_year %d  !\n", deadline_year);
			printf("deadline_month %d  !\n", deadline_month);
			printf("deadline_day %d  !\n", deadline_day);
			printf("deadline_sec %d  !\n", deadline_sec);
			printf("now_year %d  !\n", now_year);
			printf("now_month %d  !\n", now_month);
			printf("now_day %d  !\n", now_day);
			printf("now_sec %d  !\n", now_sec);*/
			if(now_year > deadline_year){
			delete_command_signal = 1;
			}
			if(now_year == deadline_year){
				if(now_month > deadline_month){
				delete_command_signal = 1;
				}
				if(now_month == deadline_month){
					if(now_day > deadline_day){
					delete_command_signal = 1;
					}
					if(now_day == deadline_day){
						if(now_sec >= deadline_sec){
						delete_command_signal = 1;
						}
					}
					
				}
				
			}
			
			
			if(delete_command_signal == 1){
			printf("delete_command_signal[%ld] %d  !\n", s_id,delete_command_signal);
			Command temp = servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1];
			servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1] = servent_command_buffer[s_id][i];
		        servent_command_buffer[s_id][i] = temp;
		        
		        memset(&servent_command_buffer[s_id][i],0,sizeof(Command));
			servent_command_buffer_pointer[s_id]--;
			}
			
		}
		
		}
		
		sleep(0.2);
	        
		
		
	   
		
	

}
void fake_servent_func(long s_id){


    int rc,a=0,b=0,i=0,j=0,k=0,send_target=0,result,file_num,invalid_peer_num=0;
    int command_comparison_result=1,website_comparison_result=1,peer_comparison_result=0;
    Peer temp;
    char text[1024];
    char string_now_year[1024],string_now_month[1024],string_now_day[1024];//xxx
    char string_now_hour[1024],string_now_min[1024],string_now_sec[1024];
    char string_command_year[4096],string_command_month[4096],string_command_day[4096],string_command_sec[4096];
    int now_year,now_month,now_day,now_hour,now_min,now_sec;
    int command_year,command_month,command_day,command_sec;
    int deadline_year,deadline_month,deadline_day,deadline_sec;
    char servent_send_message[1024];
    int send_peer=0; 
        int peer_num=0;   
                  
  		if(servent_work_over[s_id] == 1){
			return;
		}
		if(servent_eliminate_signal[s_id]==1){
		//printf("servent %ld eliminate!!!\n", s_id);
		servent_pattern[s_id] = 99;
		return;	
		}
		if(servent_boot_signal[s_id]==0){
		servent_pattern[s_id] = 99;
		return;	
		}
		time(&current);
		info = localtime( &current );
		
		strftime(string_now_year,sizeof(string_now_year),"%Y",info);
		now_year = atoi(string_now_year);
		strftime(string_now_month,sizeof(string_now_month),"%m",info);
		now_month = atoi(string_now_month);
		strftime(string_now_day,sizeof(string_now_day),"%d",info);
		now_day = atoi(string_now_day);
		
		strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
		now_hour = atoi(string_now_hour);	
		strftime(string_now_min,sizeof(string_now_min),"%M",info);
		now_min = atoi(string_now_min);	
		strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
		now_sec = atoi(string_now_sec);	
		    
		now_sec+=(60*now_min)+(60*60*now_hour);
		
		if(now_sec > servent_last_time_select_pattern[s_id] ){
		
		if((now_sec - servent_last_time_select_pattern[s_id] ) >= NUM_OF_SELECT_PATTERN_TIMES){
		
		servent_select_pattern_signal[s_id]  = 1;
		
		}
		if((now_sec - servent_last_time_select_pattern[s_id] ) < NUM_OF_SELECT_PATTERN_TIMES){
		servent_select_pattern_signal[s_id]  = 0;
		
		}
		
		}
		else if(now_sec < servent_last_time_select_pattern[s_id] ){
		
		if((now_sec+(86400-servent_last_time_select_pattern[s_id]) ) >= NUM_OF_SELECT_PATTERN_TIMES){
		servent_select_pattern_signal[s_id]  = 1;
		
		}
		if((now_sec+(86400-servent_last_time_select_pattern[s_id]) ) < NUM_OF_SELECT_PATTERN_TIMES){
		servent_select_pattern_signal[s_id]  = 0;
		
		}
		
		
		}
		
		if(servent_already_execute_resurrection_signal[s_id] == 1){
		for(i=0;i<NUM_SERVENT_PEER;i++){
		if(servent_peer_list[s_id][i].peer_id != -1 && servent_eliminate_signal[servent_peer_list[s_id][i].peer_id] == 1){
		servent_peer_list[s_id][i].peer_id = -1;
		servent_peer_list[s_id][i].reputation_value = 0;
		//servent_peer_list[s_id][i].sensor_signal = 0;
		}
		   
		}
		}
		
		servent_peer_num[s_id]=0;
		for(i=0;i<NUM_SERVENT_PEER;i++){
		   if(servent_peer_list[s_id][i].peer_id!= -1)
		   servent_peer_num[s_id]++;
		}
		
		if(servent_peer_num[s_id]!=0){
		
		for(i=0;i<NUM_SERVENT_PEER;i++){
		if(servent_peer_list[s_id][i].peer_id != -1 && i>=servent_peer_num[s_id]){
		for ( j = 0; j < servent_peer_num[s_id]; j++) {
		if(servent_peer_list[s_id][j].peer_id == -1){
		    	   temp = servent_peer_list[s_id][j];
			   servent_peer_list[s_id][j] = servent_peer_list[s_id][i];
			   servent_peer_list[s_id][i] = temp;
			   break;
		}
		   
		}
		   
		}
		   
		}
		   
		}
		
		
		
		
		if(servent_select_pattern_signal[s_id] == 1){
		servent_pattern[s_id] = 2; 
		
		//zxcc
		}
		if(servent_select_pattern_signal[s_id] == 0 ){
		servent_pattern[s_id] = 99;
		}
		
		for ( i = 0; i < servent_peer_num[s_id]; i++) {		   
			if (servent_peer_list[s_id][i].reputation_value < 0 && servent_latency_signal[s_id] == 0) {
		        printf("servent %d has to request peer !!!!! \n",s_id);
		        servent_pattern[s_id]=2;
		        break;
		        }
		        if (servent_peer_list[s_id][i].reputation_value == reputation_value_max && servent_peer_list[s_id][i].already_send_TM_signal == 0 
		        && servent_latency_signal[servent_peer_list[s_id][i].peer_id] == 0 && servent_latency_signal[s_id] == 0) {   

		                servent_peer_list[s_id][i].already_send_TM_signal = 1;
		                servent_peer_list[s_id][i].trust_signal = 1;
				send_target = servent_peer_list[s_id][i].peer_id;
				
				
				memset(servent_send_message,0,1024);
				servent_make_command(servent_send_message,servent[s_id].id,servent[send_target].id ,0,"f005");
				

	     			puts("");
	     			if(servent_command_buffer_pointer[send_target] > 99){
	     			servent_command_buffer_pointer[send_target] = 0;
	     			}
	     			strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     			servent_command_buffer_pointer[send_target]++;
	     				
	     			puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			
	     			
	     			servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content); 
				
		        
		        }
		}
		

	   	if(servent_trust_threshold[s_id] >= 4 && servent_latency_signal[s_id]==0 && servent_already_turn_to_latency_signal[s_id] == 0){

	   	printf("servent %d trust_threshold %d !!!!! \n",s_id,servent_trust_threshold[s_id]);
	   	printf("servent %d turn to latency !!!!! \n",s_id);
	   	servent_latency_signal[s_id]=1;
	   	servent_trust_threshold[s_id]=0;
	   	servent_already_turn_to_latency_signal[s_id]=1;
	   	
	   	servent_peer_num[s_id]=0;
		for(i=0;i<NUM_SERVENT_PEER;i++){
		   if(servent_peer_list[s_id][i].peer_id!= -1)
		   servent_peer_num[s_id]++;
		}
		
		if(servent_peer_num[s_id]!=0){
		
		for(i=0;i<NUM_SERVENT_PEER;i++){
		if(servent_peer_list[s_id][i].peer_id != -1 && i>=servent_peer_num[s_id]){
		for ( j = 0; j < servent_peer_num[s_id]; j++) {
		if(servent_peer_list[s_id][j].peer_id == -1){
		    	   temp = servent_peer_list[s_id][j];
			   servent_peer_list[s_id][j] = servent_peer_list[s_id][i];
			   servent_peer_list[s_id][i] = temp;
			   break;
		}
		   
		}
		   
		}
		   
		}
		   
		}
	   	
	   	for ( i = 0; i < servent_peer_num[s_id]; i++) {		   
			
				send_target = servent_peer_list[s_id][i].peer_id;
				
				
				memset(servent_send_message,0,1024);
				servent_make_command(servent_send_message,servent[s_id].id,servent[send_target].id ,0,"f006");
				

	     			puts("");
	     			if(servent_command_buffer_pointer[send_target] > 99){
	     			servent_command_buffer_pointer[send_target] = 0;
	     			}
	     			strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     			servent_command_buffer_pointer[send_target]++;
	     				
	     			puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			
	     			
	     			servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content); 
				
		        
		        
		}
		puts("");

	        /*for ( i = 0; i < servent_send_trust_message_to_me_pointer[s_id]; i++) {	
	        		  
	        		send_target = servent_send_trust_message_to_me[s_id][i];
				printf("servent %ld send f006 to servent %d",s_id,send_target);
				
				memset(servent_send_message,0,1024);
				servent_make_command(servent_send_message,servent[s_id].id,servent[send_target].id ,0,"f006");
				

	     			puts("");
	     			if(servent_command_buffer_pointer[send_target] > 99){
	     			servent_command_buffer_pointer[send_target] = 0;
	     			}
	     			
	     			strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     			servent_command_buffer_pointer[send_target]++;
	     				
	     			puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			
	     			
	     			servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content); 
				
		        	puts("");
		        
	        }*/
	        
	   	}
	   	
	   
	   	
		
		
		if(servent_latency_signal[s_id] == 1 && servent_select_pattern_signal[s_id] == 1){
		puts("I am latency bot. I dont communicate with other bot");
		//servent_pattern[s_id]=1;
		}
		
		if(servent_resurrection_request_peer_signal[s_id] == 1){
		
		printf("servent %ld:I need to request 2 peer !!!!!!!\n",s_id);
		
		if(servent_already_execute_resurrection_signal[s_id] == 1){
		for(i=0;i<NUM_SERVENT_PEER;i++){
		if(servent_peer_list[s_id][i].peer_id != -1 && servent_eliminate_signal[servent_peer_list[s_id][i].peer_id] == 1){
		servent_peer_list[s_id][i].peer_id = -1;
		servent_peer_list[s_id][i].reputation_value = 0;
		//servent_peer_list[s_id][i].sensor_signal = 0;
		   
		}
		   
		}
		}
		servent_peer_num[s_id]=0;
		for(i=0;i<NUM_SERVENT_PEER;i++){
		   if(servent_peer_list[s_id][i].peer_id!= -1 )
		   servent_peer_num[s_id]++;
		}
		if(servent_peer_num[s_id]==0){
		printf("servent %ld doesnt has peer !!! \n",s_id);
		
		servent_pattern[s_id]=99;
		
		}
		if(servent_peer_num[s_id]!=0){
		
		for(i=0;i<NUM_SERVENT_PEER;i++){
		if(servent_peer_list[s_id][i].peer_id != -1 && i>=servent_peer_num[s_id]){
		for ( j = 0; j < servent_peer_num[s_id]; j++) {
		if(servent_peer_list[s_id][j].peer_id == -1){
		    	   temp = servent_peer_list[s_id][j];
			   servent_peer_list[s_id][j] = servent_peer_list[s_id][i];
			   servent_peer_list[s_id][i] = temp;
			   break;
		}
		   
		}
		   
		}
		   
		}
		   
		}
		for ( i = 0; i < servent_peer_num[s_id] ; i++) {
		   
		printf("id:%d value:%d\n",servent_peer_list[s_id][i].peer_id, servent_peer_list[s_id][i].reputation_value);
		}
		
		if(servent_peer_num[s_id]!=0){
		int before_send_target = -1;
		int shut_down_signal=0;
		
		for ( k = 0; k < 2; k++){
		
		//printf("k %d \n",k);
		if(shut_down_signal == 1){
		break;
		}
		
		send_target=0;
		invalid_peer_num = 0;
		for(i=0;i<NUM_SERVENT_PEER;i++){
		if(servent_peer_list[s_id][i].peer_id != -1 ){
		if(servent_peer_list[s_id][i].reputation_value == -1 || servent_peer_list[s_id][i].trust_signal == 1 ){//|| servent_boot_signal[servent_peer_list[s_id][i].peer_id] == 0
		invalid_peer_num++;
		}  
		}  
		if(invalid_peer_num >= NUM_SERVENT_PEER-1){
		break;
		}
				
		}
		if(invalid_peer_num >= NUM_SERVENT_PEER-1){
		printf("servent %ld doesnt has peer can communicate !!! \n",s_id);
		servent_pattern[s_id]=99;
		shut_down_signal=1;
		break;
		}
		servent_sort_peer(s_id);
				
		/*for ( i = servent_peer_num[s_id]-1; i >= 0 ; i--) {
			if(servent_peer_list[s_id][i].reputation_value != 5 && servent_peer_list[s_id][i].reputation_value != -1 
			&& servent_peer_list[s_id][i].peer_id != before_send_target){
				send_target = servent_peer_list[s_id][i].peer_id;
						
				break;
			}
					
		}
		*/
		if(servent_peer_num[s_id] != 0){
		peer_num = servent_peer_num[s_id];
		send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
				
				
		for ( i = 0; i < servent_peer_num[s_id]; i++) {
				
		if(servent_peer_list[s_id][i].peer_id == send_target){
		j=i;
		while(servent_peer_list[s_id][j].reputation_value == -1 || servent_peer_list[s_id][j].trust_signal == 1 || servent_peer_list[s_id][j].peer_id == before_send_target 
		){//|| servent_boot_signal[servent_peer_list[s_id][j].peer_id] == 0
			
		send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
		
		for ( j = 0; j < servent_peer_num[s_id]; j++) {
		if(servent_peer_list[s_id][j].peer_id == send_target){
		break;
		}
		}
				
		i = 0;    
		
		}
				
		}
				
		}
				
		}
		
		
		before_send_target = send_target;
		servent_peer_num[send_target]=0;
		for(i=0;i<NUM_SERVENT_PEER;i++){
			if(servent_peer_list[send_target][i].peer_id!= -1 )
			servent_peer_num[send_target]++;
		}
		
				
		if(servent_boot_signal[send_target]==0){
		printf("servent %ld shut down !!! \n",send_target);
		printf("servent %ld reputation_value--!!! \n",send_target);
		for ( i = 0; i < servent_peer_num[s_id]; i++) {
			if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
				servent_peer_list[s_id][i].reputation_value--;
			}
		}
				
		servent_pattern[s_id]=99;
		break;
		}
		
		if(servent[send_target].sensor_signal == 1 ){
			request_time[send_target]++;
		}
		if(servent[send_target].sensor_signal == 1 && servent[s_id].request_signal == 0 && enumeration_signal == 1){
			servent[s_id].request_signal = 1;
			vs++;
		}
		if(servent_peer_num[send_target]==0){
			printf("send_target:servent %ld doesnt has peer !!! \n",send_target);
			
			
		if(servent_peer_num[s_id] == 1){	
		shut_down_signal=1;
		printf("shut_down_signal %d !!! \n",shut_down_signal);
		}
		servent_pattern[s_id]=99;
		break;
		}
		if(servent_latency_signal[send_target]==1){
		printf("servent %ld already turn to latency bot !!! \n",send_target);
		printf("servent %ld reputation_value--!!! \n",send_target);
		for ( i = 0; i < servent_peer_num[s_id]; i++) {
			if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
				servent_peer_list[s_id][i].reputation_value--;
			}
		}
				
		servent_pattern[s_id]=99;
		break;
		}
		
		
		
		
		memset(servent_send_message,0,1024);
		servent_make_command(servent_send_message,servent[s_id].id,servent[send_target].id ,0,"f003");
		printf("servent %ld , send_data: %s , target servent:%d", s_id,servent_send_message,send_target);

	     	puts("");
	     			
	     	if(servent_command_buffer_pointer[send_target] <= 99){
	     	strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     	servent_command_buffer_pointer[send_target]++;
	     	puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     	}
	     	else if(servent_command_buffer_pointer[send_target] > 99){
	     	servent_command_buffer_pointer[send_target] = 0;
	     	strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     	servent_command_buffer_pointer[send_target]++;
	     	puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			
	     	}
	     	
	     	if(servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content) == 0 
	     	//&& servent[send_target].sensor_signal != 1 && servent[send_target].fake_signal != 1
	     	&& enumeration_signal == 1
	     	){//&& enumeration_signal == 1 zxxx
	     	printf("servent %ld reputation_value--!!! \n",send_target);
		for ( i = 0; i < servent_peer_num[s_id]; i++) {
			if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
				servent_peer_list[s_id][i].reputation_value--;
			}
		}
				
		servent_pattern[s_id]=99;
		break;
	     	}
	     					
	     	peer_comparison_result=0;		
	     			
	     	if(servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content) == 1){
	
		if(strcmp(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].command_code ,"f003" )==0 ){
				
		char string_ip[100],string_port[100],string_id[100];
		extra_information_split(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].extra_information,string_ip,string_port,string_id);
		send_peer = atoi(string_id);
				
		memset(servent_send_message,0,1024);
		servent_make_command(servent_send_message,servent[send_target].id,servent[s_id].id ,0,"f004" );
				
				
				
		for ( i = 0; i < servent_peer_num[send_target]; i++) {	
		if(servent_peer_list[send_target][i].peer_id == send_peer){
				
				
		printf("servent %ld already have servent %d in peer list !\n",send_target,send_peer);
		peer_comparison_result=1;
		break;
		}
				
				
		}
		if(send_target == send_peer){
		printf("servent %ld cant add itself in peer list !\n",send_target);
		peer_comparison_result=1;
				
		}
		if(send_peer == -1 ){
		printf("servent %ld cant add invalid peer in peer list !\n",send_target);
		peer_comparison_result=1;
				
		}
				
		if(peer_comparison_result == 0 && servent_peer_num[send_target]==NUM_SERVENT_PEER){
		printf("servent %ld remove servent %d in peer list !\n",send_target,servent_peer_list[send_target][0].peer_id);
		printf("servent %ld add servent %d in peer list !\n",send_target,send_peer);
		servent_peer_list[send_target][0].peer_id = send_peer;
		servent_peer_list[send_target][0].reputation_value = reputation_value_base;
				
				
				
				
				
		}
		if(peer_comparison_result == 0 && servent_peer_num[send_target] < NUM_SERVENT_PEER  ){
		//printf("servent %ld remove servent %d in peer list !\n",send_target,servent_peer_list[send_target][0].peer_id);
		printf("servent %ld add servent %d in peer list !\n",send_target,send_peer);
		servent_peer_list[send_target][servent_peer_num[send_target]].peer_id = send_peer;
		servent_peer_list[send_target][servent_peer_num[send_target]].reputation_value = reputation_value_base;
				
		servent_peer_num[send_target]++;
						
				
				
		}
				
					
		if(servent_command_buffer_pointer[s_id] <= 99){
		strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content, servent_send_message);
	     	servent_command_buffer_pointer[s_id]++;
		puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
		}
		else if(servent_command_buffer_pointer[s_id] > 99){
		servent_command_buffer_pointer[s_id]=0;
		strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content, servent_send_message);
	     	servent_command_buffer_pointer[s_id]++;
		puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
		}			     	
				
					     			
		}
				
	     	}
		peer_comparison_result=0;		
		if(servent_command_analysis(s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content) == 1){
	
		if(strcmp(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].command_code ,"f004" )==0 ){

							
		
		for ( i = 0; i < servent_peer_num[s_id]; i++) {	

		if(servent_peer_list[s_id][i].peer_id == servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id){
		
		
		
		printf("servent %ld already have servent %d in peer list !\n",s_id,servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id);
		peer_comparison_result=1;
		break;
		}
				
				
		}
		if(s_id == servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id){
		printf("servent %ld cant add itself in peer list !\n",s_id);
		peer_comparison_result=1;
				
		}
		
		if(servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id == -1){
		printf("servent %ld cant add invalid peer in peer list !\n",s_id);
		peer_comparison_result=1;
				
		}
		if(peer_comparison_result == 0 && servent_peer_num[s_id]==NUM_SERVENT_PEER){
		printf("servent %ld remove servent %d in peer list !\n",s_id,servent_peer_list[s_id][0].peer_id);
		printf("servent %ld add servent %d in peer list !\n",s_id,servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id);
		servent_peer_list[s_id][0].peer_id = servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id;
		servent_peer_list[s_id][0].reputation_value = reputation_value_base;
					
		
				
		}
		if(peer_comparison_result == 0 && servent_peer_num[s_id] < NUM_SERVENT_PEER  ){
		//printf("servent %ld remove servent %d in peer list !\n",s_id,servent_peer_list[s_id][0].peer_id);
		printf("servent %ld add servent %d in peer list !\n",s_id,servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id);
		servent_peer_list[s_id][servent_peer_num[s_id]].peer_id = servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id;
		servent_peer_list[s_id][servent_peer_num[s_id]].reputation_value = reputation_value_base;
		
		servent_peer_num[s_id]++;
				
		
		}
				
		
				
					
				  			
				
		servent_rearrange_peer(s_id);   
				
		for(i=0;i<servent_peer_num[s_id];i++){	
		if(servent_peer_list[s_id][i].peer_id ==  send_target && servent_peer_list[s_id][i].reputation_value < reputation_value_max ){
		servent_peer_list[s_id][i].reputation_value++;
		break;
		}
		}
				
				
					
		servent_sort_peer(s_id);			
				
		for ( i = 0; i < servent_peer_num[s_id]; i++) {
		if(servent_peer_list[s_id][i].peer_id!= -1 ){
		printf("id:%d value:%d\n",servent_peer_list[s_id][i].peer_id, servent_peer_list[s_id][i].reputation_value);
		}
				
		}	     	
				
					     			
		}
				
	     	}
		
		
			
		}
			if(servent_already_execute_resurrection_signal[s_id] == 1){
			for(i=0;i<NUM_SERVENT_PEER;i++){
			if(servent_peer_list[s_id][i].peer_id != -1 && servent_eliminate_signal[servent_peer_list[s_id][i].peer_id] == 1){
			servent_peer_list[s_id][i].peer_id = -1;
			servent_peer_list[s_id][i].reputation_value = 0;
			//servent_peer_list[s_id][i].sensor_signal = 0;
			   
			}
			   
			}
			}
			servent_rearrange_peer(s_id); 
			
			servent_peer_num[s_id]=0;
			for(i=0;i<NUM_SERVENT_PEER;i++){
			   if(servent_peer_list[s_id][i].peer_id!= -1 )
			   servent_peer_num[s_id]++;
			}
			
			for(i=0;i<servent_peer_num[s_id];i++){
			if(servent_peer_list[s_id][i].reputation_value > 2 && servent_peer_list[s_id][i].peer_id !=-1 ){

			       
			       servent_peer_list[s_id][i].reputation_value = 2;
			       
			}
			else if( servent_latency_signal[servent_peer_list[s_id][i].peer_id] == 1 && servent_peer_list[s_id][i].peer_id !=-1 ){

			       
			       servent_peer_list[s_id][i].reputation_value = 2;
			       
			}
			else if(servent_peer_list[s_id][i].reputation_value <= 2 && servent_peer_list[s_id][i].peer_id !=-1 ){

			       
			       servent_peer_list[s_id][i].reputation_value = 0;
			       
			}
			   
			   
			}
		
		}
		
		servent_resurrection_request_peer_signal[s_id] = 0;
		
		
		}
		
		if(servent_pattern[s_id]!=99){
		servent_log[s_id][servent_execute_pattern_time[s_id]].num_pattern = servent_pattern[s_id];
		servent_log[s_id][servent_execute_pattern_time[s_id]].now_sec = now_sec;
		printf(" servent[%ld]_pattern = %d now_sec = %d !\n", s_id,servent_pattern[s_id],now_sec);
		}
		
		//zxcc
		switch(servent_pattern[s_id]) {
			case 0:
				
				
				servent_pattern[s_id]=99;
				break;
			
			case 1://botmaster_command
				//file_num=rand()  % 3;
				file_num=0;//***-
				switch(file_num){
				case 0:
					a=0;
					b=0;
					
					printf("servent %ld receive command%d from relay station:\n",s_id,a+1);
					while(file_data[a][b][0]!= '\0'){
					
					for(i=0;i<servent_command_buffer_pointer[s_id];i++){	
					
						command_comparison_result=strcmp(servent_command_buffer[s_id][i].content,file_data[a][b]);
						if(command_comparison_result== 0){
						puts(servent_command_buffer[s_id][i].content);
						break;
						}
					}
					
					if(command_comparison_result== 0){
					printf("servent %ld already have (%s)\n",s_id,file_data[a][b]);
					
					}
					if(command_comparison_result != 0){
					if(servent_command_buffer_pointer[s_id] > 99){
					servent_command_buffer_pointer[s_id]=0;
					}
					strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content,file_data[a][b]);
					puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content);
					
					servent_command_buffer_pointer[s_id]++;
					servent_command_analysis(s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
					}
					
					
					b++;
					
					}
					
					
					
					break;
				case 1:
					a=1;
					b=0;
					
					printf("servent %ld receive command%d from relay station:\n",s_id,a+1);
					while(file_data[a][b][0]!= '\0'){
					
					for(i=0;i<servent_command_buffer_pointer[s_id];i++){	
					command_comparison_result=strcmp(servent_command_buffer[s_id][i].content,file_data[a][b]);
					if(command_comparison_result== 0){break;}
					}
					
					if(command_comparison_result== 0){
					printf("servent %ld already have (%s)\n",s_id,file_data[a][b]);
					
					}
					if(command_comparison_result != 0){
					if(servent_command_buffer_pointer[s_id] > 99){
					servent_command_buffer_pointer[s_id]=0;
					}
					strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content,file_data[a][b]);
					puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content);
					
					servent_command_buffer_pointer[s_id]++;
					servent_command_analysis(s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
					}
					
					
					b++;
					
					}
					
					
					
					break;
				case 2:
					a=2;
					b=0;
					printf("servent %ld receive command%d from relay station:\n",s_id,a+1);
					while(file_data[a][b][0]!= '\0'){
					
					for(i=0;i<servent_command_buffer_pointer[s_id];i++){	
					command_comparison_result=strcmp(servent_command_buffer[s_id][i].content,file_data[a][b]);
					if(command_comparison_result== 0){break;}
					}
					
					if(command_comparison_result== 0){
					printf("servent %ld already have (%s)\n",s_id,file_data[a][b]);
					
					}
					if(command_comparison_result != 0){
					if(servent_command_buffer_pointer[s_id] > 99){
					servent_command_buffer_pointer[s_id]=0;
					}
					strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content,file_data[a][b]);
					puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content);
					servent_command_buffer_pointer[s_id]++;
					servent_command_analysis(s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
					}
					
					
					b++;
					
					}
					
					
					break;
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 1;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				
				break;
			case 2:
				
				send_target=0;
				invalid_peer_num = 0;
				send_peer=0;
				if(servent_already_execute_resurrection_signal[s_id] == 1){
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(servent_peer_list[s_id][i].peer_id != -1 && servent_eliminate_signal[servent_peer_list[s_id][i].peer_id] == 1){
				servent_peer_list[s_id][i].peer_id = -1;
				servent_peer_list[s_id][i].reputation_value = 0;
				//servent_peer_list[s_id][i].sensor_signal = 0;
				}
				   
				}
				}
				servent_peer_num[s_id]=0;
				for(i=0;i<NUM_SERVENT_PEER;i++){
				
				if(servent_peer_list[s_id][i].peer_id!= -1)
				servent_peer_num[s_id]++;
					
				} 
				
				if(servent_peer_num[s_id]==0){
				printf("servent %ld doesnt has peer !!! \n",s_id);
				servent_pattern[s_id]=99;
				break;
				}
				
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(servent_peer_list[s_id][i].peer_id != -1 ){
				if(servent_peer_list[s_id][i].reputation_value == -1 || servent_peer_list[s_id][i].trust_signal == 1 ){//|| servent_boot_signal[servent_peer_list[s_id][i].peer_id] == 0
				invalid_peer_num++;
				}  
				}  
				if(invalid_peer_num >= NUM_SERVENT_PEER-1){
				break;
				}
				
				}
				if(invalid_peer_num >= NUM_SERVENT_PEER-1){
				printf("servent %ld doesnt has peer can communicate !!! \n",s_id);
				servent_pattern[s_id]=99;
				break;
				}
				servent_rearrange_peer(s_id);
				servent_sort_peer(s_id);
				
				/*for ( i = servent_peer_num[s_id]-1; i >= 0 ; i--) {
					if(servent_peer_list[s_id][i].reputation_value != 5 && servent_peer_list[s_id][i].reputation_value != -1 ){
						send_target = servent_peer_list[s_id][i].peer_id;
								
						break;
					}
							
				}*/
				if(servent_peer_num[s_id] != 0){
				peer_num = servent_peer_num[s_id];
				send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
				
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
				
				if(servent_peer_list[s_id][i].peer_id == send_target){
				j=i;
				while(servent_peer_list[s_id][j].reputation_value == -1 || servent_peer_list[s_id][j].trust_signal == 1 ){//|| servent_boot_signal[servent_peer_list[s_id][j].peer_id] == 0
					
			
				
				send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
				
				for ( j = 0; j < servent_peer_num[s_id]; j++) {
				if(servent_peer_list[s_id][j].peer_id == send_target){
				break;
				}
				}
				
				i = 0;    
				
				}
				
				}
				
				}
				
				}
				
				
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_target = send_target;
				servent_peer_num[send_target]=0;
				for(i=0;i<NUM_SERVENT_PEER;i++){
				
				if(servent_peer_list[send_target][i].peer_id!= -1){
				servent_peer_num[send_target]++;
				}	
				} 
				
				
				if(servent_boot_signal[send_target]==0){
				printf("servent %ld shut down !!! \n",send_target);
				printf("servent %ld reputation_value--!!! \n",send_target);
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
					if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
						servent_peer_list[s_id][i].reputation_value--;
					}
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				break;
				}
				
				
				
				if(servent[send_target].sensor_signal == 1 ){
						request_time[send_target]++;
				}
				if(servent[send_target].sensor_signal == 1 && servent[s_id].request_signal == 0 && enumeration_signal == 1){
						servent[s_id].request_signal = 1;
						vs++;
				}
				
				if(servent_peer_num[send_target]==0){
				printf("send_target:servent %ld doesnt has peer !!! \n",send_target);
				
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				break;
				}
				
				if(servent_latency_signal[send_target]==1){
				printf("servent %ld already turn to latency bot !!! \n",send_target);
				printf("servent %ld reputation_value--!!! \n",send_target);
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
					if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
						servent_peer_list[s_id][i].reputation_value--;
					}
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				break;
				}
				
				
				
				memset(servent_send_message,0,1024);
				servent_make_command(servent_send_message,servent[s_id].id,servent[send_target].id ,0,"f003");
				printf("servent %ld , send_data: %s , target servent:%d", s_id,servent_send_message,send_target);

	     			puts("");
	     			if(servent_command_buffer_pointer[send_target] <= 99){
	     			strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     			servent_command_buffer_pointer[send_target]++;
	     			puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			}
	     			else if(servent_command_buffer_pointer[send_target] > 99){
	     			servent_command_buffer_pointer[send_target] = 0;
	     			strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     			servent_command_buffer_pointer[send_target]++;
	     			puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			
	     			}
	     			
	     			if(servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content) == 0 
	     			//&& servent[send_target].sensor_signal != 1 && servent[send_target].fake_signal != 1
	     			&& enumeration_signal == 1
	     			){
	     			//&& enumeration_signal == 1 zxxx
	     			printf("servent %ld reputation_value--!!! \n",send_target);
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
					if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
						servent_peer_list[s_id][i].reputation_value--;
					}
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				
				break;
	     			}
				
	     			peer_comparison_result=0;
	     			if(servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content) == 1){
	
				if(strcmp(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].command_code ,"f003" )==0 ){
				
				char string_ip[100],string_port[100],string_id[100];
				extra_information_split(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].extra_information,string_ip,string_port,string_id);
				send_peer = atoi(string_id);
				
				memset(servent_send_message,0,1024);
				servent_make_command(servent_send_message,servent[send_target].id,servent[s_id].id ,0,"f004" );
				
				
				
				for ( i = 0; i < servent_peer_num[send_target]; i++) {	
				if(servent_peer_list[send_target][i].peer_id == send_peer){
				
				
				printf("servent %ld already have servent %d in peer list !\n",send_target,send_peer);
				peer_comparison_result=1;
				break;
				}
				
				
				}
				if(send_target == send_peer){
				printf("servent %ld cant add itself in peer list !\n",send_target);
				peer_comparison_result=1;
				
				}
				if(send_peer == -1 ){
				printf("servent %ld cant add invalid peer in peer list !\n",send_target);
				peer_comparison_result=1;
				
				}
				
				if(peer_comparison_result == 0 && servent_peer_num[send_target]==NUM_SERVENT_PEER){
				printf("servent %ld remove servent %d in peer list !\n",send_target,servent_peer_list[send_target][0].peer_id);
				printf("servent %ld add servent %d in peer list !\n",send_target,send_peer);
				servent_peer_list[send_target][0].peer_id = send_peer;
				servent_peer_list[send_target][0].reputation_value = reputation_value_base;
				
				
				
				
				
				}
				if(peer_comparison_result == 0 && servent_peer_num[send_target] < NUM_SERVENT_PEER  ){
				//printf("servent %ld remove servent %d in peer list !\n",send_target,servent_peer_list[send_target][0].peer_id);
				printf("servent %ld add servent %d in peer list !\n",send_target,send_peer);
				servent_peer_list[send_target][servent_peer_num[send_target]].peer_id = send_peer;
				servent_peer_list[send_target][servent_peer_num[send_target]].reputation_value = reputation_value_base;
				
				servent_peer_num[send_target]++;
						
				
				
				}
				
				if(servent_log[s_id][servent_execute_pattern_time[s_id]].num_pattern == 1 && peer_comparison_result == 0 ){
				servent_log[s_id][servent_execute_pattern_time[s_id]].send_peer=send_peer;
				}
				
				if(servent_command_buffer_pointer[s_id] <= 99){
				strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content, servent_send_message);
	     			servent_command_buffer_pointer[s_id]++;
				puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
				}
				else if(servent_command_buffer_pointer[s_id] > 99){
				servent_command_buffer_pointer[s_id]=0;
				strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content, servent_send_message);
	     			servent_command_buffer_pointer[s_id]++;
				puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
				}	     	
				
					     			
				}
				
	     			}
				peer_comparison_result=0;
				if(servent_command_analysis(s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content) == 1){
	
				if(strcmp(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].command_code ,"f004" )==0 ){
							
				if(servent[send_target].detect_and_reply_signal == 0 && servent[send_target].detect_signal == 1 && servent[s_id].fake_signal == 1 && enumeration_signal == 1){
				servent[send_target].detect_and_reply_signal = 1;
				vrc++;
				}
				

				if(servent[servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id].detect_signal == 0 && servent[s_id].fake_signal == 1 && enumeration_signal == 1){
				servent[servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id].detect_signal = 1;
				vc++;
				}
				
				if(servent[servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id].request_signal == 0 && enumeration_signal == 1 && servent[s_id].fake_signal == 1){
				servent[servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id].request_signal = 1;
				vs++;
				}
				
				for ( i = 0; i < servent_peer_num[s_id]; i++) {	
				if(servent_peer_list[s_id][i].peer_id == servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id){
				
				
				
				
				printf("servent %ld already have servent %d in peer list !\n",s_id,servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id);
				peer_comparison_result=1;
				break;
				}
				
				
				}
				if(s_id == servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id){
				printf("servent %ld cant add itself in peer list !\n",s_id);
				peer_comparison_result=1;
				
				}
				if(servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id == -1){
				printf("servent %ld cant add invalid peer in peer list !\n",s_id);
				peer_comparison_result=1;
				
				}
				if(peer_comparison_result == 0 && servent_peer_num[s_id]==NUM_SERVENT_PEER){
				printf("servent %ld remove servent %d in peer list !\n",s_id,servent_peer_list[s_id][0].peer_id);
				printf("servent %ld add servent %d in peer list !\n",s_id,servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id);
				servent_peer_list[s_id][0].peer_id = servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id;
				servent_peer_list[s_id][0].reputation_value = reputation_value_base;

				
				
				
				}
				if(peer_comparison_result == 0 && servent_peer_num[s_id] < NUM_SERVENT_PEER  ){
				//printf("servent %ld remove servent %d in peer list !\n",s_id,servent_peer_list[s_id][0].peer_id);
				printf("servent %ld add servent %d in peer list !\n",s_id,servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id);
				servent_peer_list[s_id][servent_peer_num[s_id]].peer_id = servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id;
				servent_peer_list[s_id][servent_peer_num[s_id]].reputation_value = reputation_value_base;

				servent_peer_num[s_id]++;
				
				
								
				
				}
				
				if(servent_log[s_id][servent_execute_pattern_time[s_id]].num_pattern == 1 && peer_comparison_result == 0 ){
				servent_log[s_id][servent_execute_pattern_time[s_id]].receive_peer=servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id;
				}
					
				  			
				
				servent_rearrange_peer(s_id);   
				
				for(i=0;i<servent_peer_num[s_id];i++){	
				if(servent_peer_list[s_id][i].peer_id ==  send_target && servent_peer_list[s_id][i].reputation_value < reputation_value_max ){
				servent_peer_list[s_id][i].reputation_value++;
				break;
				}
				}
				
				
					
				servent_sort_peer(s_id);			
				
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
				if(servent_peer_list[s_id][i].peer_id!= -1 ){
				printf("id:%d value:%d\n",servent_peer_list[s_id][i].peer_id, servent_peer_list[s_id][i].reputation_value);
				}
				
				}	     	
				
					     	
					     		     			
				}
				
	     			}
	     			servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 1;
				servent_execute_pattern_time[s_id]++;
	     			servent_pattern[s_id]=99;
	     			select_again_signal[s_id]=0;
				break;	
				
			
			
			case (-1):
				printf(" servent %ld terminated !\n", s_id);
				break;	
		}
		//***-
		
		
		time(&current);
		info = localtime( &current );
		
		strftime(string_now_year,sizeof(string_now_year),"%Y",info);
		now_year = atoi(string_now_year);
		strftime(string_now_month,sizeof(string_now_month),"%m",info);
		now_month = atoi(string_now_month);
		strftime(string_now_day,sizeof(string_now_day),"%d",info);
		now_day = atoi(string_now_day);
		
		strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
		now_hour = atoi(string_now_hour);	
		strftime(string_now_min,sizeof(string_now_min),"%M",info);
		now_min = atoi(string_now_min);	
		strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
		now_sec = atoi(string_now_sec);	
		    
		now_sec+=(60*now_min)+(60*60*now_hour);
		if(servent_select_pattern_signal[s_id] ==1){
		servent_last_time_select_pattern[s_id]  = now_sec;
		
		}
			
			int delete_command_signal=0;
		//printf("before:servent_command_buffer_pointer[s_id] %d  !\n", servent_command_buffer_pointer[s_id]);
			
		for ( i = 0; i < servent_command_buffer_pointer[s_id]; i++) {
		
		if(strlen(servent_command_buffer[s_id][i].content) != 0){
			
			//puts(servent_command_buffer[s_id][i].content);
			timestamp_split(servent_command_buffer[s_id][i].timestamp ,string_command_year,string_command_month,string_command_day,string_command_sec);
			
			command_year = atoi(string_command_year);	
			command_month = atoi(string_command_month);	
			command_day = atoi(string_command_day);	
			command_sec = atoi(string_command_sec);	
			//printf("effective_time %d  !\n", servent_command_buffer[s_id][i].effective_time);
			deadline_sec = command_sec + servent_command_buffer[s_id][i].effective_time;
			deadline_day = command_day ;
			deadline_month = command_month ;
			deadline_year = command_year ;
			int leap_year=0;
			if((deadline_year % 4 == 0 && deadline_year % 100 != 0 ) || deadline_year % 400 == 0) {
			leap_year = 1;
			}
			//printf("leap_year %d  !\n", leap_year);
			
			if(deadline_sec > 86400 ){
			deadline_sec = deadline_sec-86400;
			deadline_day++;
			}
			if( (deadline_month == 4 || deadline_month == 6 || deadline_month == 9 || deadline_month == 11) && deadline_day > 30 ){
			deadline_day = 1;
			deadline_month++;
			}
			if( (deadline_month == 1 || deadline_month == 3 || deadline_month == 5 || deadline_month == 7 || deadline_month == 10 || deadline_month == 12 ) && deadline_day > 31 ){
			deadline_day = 1;
			deadline_month++;
			}
			if( leap_year == 1 && ( deadline_month == 2) && ( deadline_day > 29) ){
			deadline_day = 1;
			deadline_month++;
			}
			if( leap_year == 0 && ( deadline_month == 2) && ( deadline_day > 28) ){
			deadline_day = 1;
			deadline_month++;
			}
			if( deadline_month > 12 ){
			deadline_month = 1;
			deadline_year++;
			}
			
			/*printf("deadline_year %d  !\n", deadline_year);
			printf("deadline_month %d  !\n", deadline_month);
			printf("deadline_day %d  !\n", deadline_day);
			printf("deadline_sec %d  !\n", deadline_sec);
			printf("now_year %d  !\n", now_year);
			printf("now_month %d  !\n", now_month);
			printf("now_day %d  !\n", now_day);
			printf("now_sec %d  !\n", now_sec);*/
			if(now_year > deadline_year){
			delete_command_signal = 1;
			}
			if(now_year == deadline_year){
				if(now_month > deadline_month){
				delete_command_signal = 1;
				}
				if(now_month == deadline_month){
					if(now_day > deadline_day){
					delete_command_signal = 1;
					}
					if(now_day == deadline_day){
						if(now_sec >= deadline_sec){
						delete_command_signal = 1;
						}
					}
					
				}
				
			}
			
			
			if(delete_command_signal == 1){
			//printf("delete_command_signal[%ld] %d  !\n", s_id,delete_command_signal);
			Command temp = servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1];
			servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1] = servent_command_buffer[s_id][i];
		        servent_command_buffer[s_id][i] = temp;
		        
		        memset(&servent_command_buffer[s_id][i],0,sizeof(Command));

			servent_command_buffer_pointer[s_id]--;
			}
			
		}
		
		}
		
		sleep(0.2);
	        
		
		
	   
		
	

}
void sensor_func(long s_id){


    int rc,a=0,b=0,i=0,j=0,k=0,send_target=0,result,file_num,invalid_peer_num=0;
    int command_comparison_result=1,website_comparison_result=1,peer_comparison_result=0;
    Peer temp;
    char text[1024];
    char string_now_year[1024],string_now_month[1024],string_now_day[1024];//xxx
    char string_now_hour[1024],string_now_min[1024],string_now_sec[1024];
    char string_command_year[4096],string_command_month[4096],string_command_day[4096],string_command_sec[4096];
    int now_year,now_month,now_day,now_hour,now_min,now_sec;
    int command_year,command_month,command_day,command_sec;
    int deadline_year,deadline_month,deadline_day,deadline_sec;
    char servent_send_message[1024];
    int send_peer=0;     
        int peer_num=0;              
  
		
		if(servent_work_over[s_id] == 1){
			return;
		}
		if(servent_eliminate_signal[s_id]==1){
		//printf("servent %ld eliminate!!!\n", s_id);
		servent_pattern[s_id] = 99;
		return;	
		}
		if(servent_boot_signal[s_id]==0){
		servent_pattern[s_id] = 99;
		return;	
		}
		
		time(&current);
		info = localtime( &current );
		
		strftime(string_now_year,sizeof(string_now_year),"%Y",info);
		now_year = atoi(string_now_year);
		strftime(string_now_month,sizeof(string_now_month),"%m",info);
		now_month = atoi(string_now_month);
		strftime(string_now_day,sizeof(string_now_day),"%d",info);
		now_day = atoi(string_now_day);
		
		strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
		now_hour = atoi(string_now_hour);	
		strftime(string_now_min,sizeof(string_now_min),"%M",info);
		now_min = atoi(string_now_min);	
		strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
		now_sec = atoi(string_now_sec);	
		    
		now_sec+=(60*now_min)+(60*60*now_hour);
		
		if(now_sec > servent_last_time_select_pattern[s_id] ){
		
		if((now_sec - servent_last_time_select_pattern[s_id] ) >= NUM_OF_SELECT_PATTERN_TIMES){
		//printf("now_sec %d  !\n", now_sec);
		//printf("last_time_select_pattern[s_id]  %d  !\n", last_time_select_pattern[s_id] );
		servent_select_pattern_signal[s_id]  = 1;
		
		}
		if((now_sec - servent_last_time_select_pattern[s_id] ) < NUM_OF_SELECT_PATTERN_TIMES){
		servent_select_pattern_signal[s_id]  = 0;
		
		}
		
		}
		else if(now_sec < servent_last_time_select_pattern[s_id] ){
		
		if((now_sec+(86400-servent_last_time_select_pattern[s_id]) ) >= NUM_OF_SELECT_PATTERN_TIMES){
		servent_select_pattern_signal[s_id]  = 1;
		
		}
		if((now_sec+(86400-servent_last_time_select_pattern[s_id]) ) < NUM_OF_SELECT_PATTERN_TIMES){
		servent_select_pattern_signal[s_id]  = 0;
		
		}
		
		
		}
		
		if(servent_already_execute_resurrection_signal[s_id] == 1){
		for(i=0;i<NUM_SERVENT_PEER;i++){
		if(servent_peer_list[s_id][i].peer_id != -1 && servent_eliminate_signal[servent_peer_list[s_id][i].peer_id] == 1){
		servent_peer_list[s_id][i].peer_id = -1;
		servent_peer_list[s_id][i].reputation_value = 0;
		//servent_peer_list[s_id][i].sensor_signal = 0;
		}
		   
		}
		}
		
		servent_peer_num[s_id]=0;
		for(i=0;i<NUM_SERVENT_PEER;i++){
		   if(servent_peer_list[s_id][i].peer_id!= -1)
		   servent_peer_num[s_id]++;
		}
		
		if(servent_peer_num[s_id]!=0){
		
		for(i=0;i<NUM_SERVENT_PEER;i++){
		if(servent_peer_list[s_id][i].peer_id != -1 && i>=servent_peer_num[s_id]){
		for ( j = 0; j < servent_peer_num[s_id]; j++) {
		if(servent_peer_list[s_id][j].peer_id == -1){
		    	   temp = servent_peer_list[s_id][j];
			   servent_peer_list[s_id][j] = servent_peer_list[s_id][i];
			   servent_peer_list[s_id][i] = temp;
			   break;
		}
		   
		}
		   
		}
		   
		}
		   
		}
		
		
		
		
		if(servent_select_pattern_signal[s_id] == 1){
		servent_pattern[s_id] =  rand() % 4+1;
		//zxcc
		}
		if(servent_select_pattern_signal[s_id] == 0){
		servent_pattern[s_id] = 99;
		}
		//***-
		
						
		for ( i = 0; i < servent_peer_num[s_id]; i++) {		   
			if (servent_peer_list[s_id][i].reputation_value < 0 && servent_latency_signal[s_id] == 0) {
		        servent_pattern[s_id]=3;
		        printf("servent %d has to request peer !!!!! \n",s_id);
		        break;
		        }
		        if (servent_peer_list[s_id][i].reputation_value == reputation_value_max && servent_peer_list[s_id][i].already_send_TM_signal == 0 
		        && servent_latency_signal[servent_peer_list[s_id][i].peer_id] == 0 && servent_latency_signal[s_id] == 0) {   

		                servent_peer_list[s_id][i].already_send_TM_signal = 1;
		                servent_peer_list[s_id][i].trust_signal = 1;
				send_target = servent_peer_list[s_id][i].peer_id;
				
				
				memset(servent_send_message,0,1024);
				servent_make_command(servent_send_message,servent[s_id].id,servent[send_target].id ,0,"f005");
				

	     			puts("");
	     			if(servent_command_buffer_pointer[send_target] > 99){
	     			servent_command_buffer_pointer[send_target] = 0;
	     			}
	     			strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     			servent_command_buffer_pointer[send_target]++;
	     				
	     			puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			
	     			
	     			servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content); 
				
		        
		        }
		}
		

	   	if(servent_trust_threshold[s_id] >= 4 && servent_latency_signal[s_id]==0 && servent_already_turn_to_latency_signal[s_id] == 0){

	   	printf("servent %d trust_threshold %d !!!!! \n",s_id,servent_trust_threshold[s_id]);
	   	printf("servent %d turn to latency !!!!! \n",s_id);
	   	servent_latency_signal[s_id]=1;
	   	servent_trust_threshold[s_id]=0;
	   	servent_already_turn_to_latency_signal[s_id]=1;
	   	
	   	servent_peer_num[s_id]=0;
		for(i=0;i<NUM_SERVENT_PEER;i++){
		   if(servent_peer_list[s_id][i].peer_id!= -1)
		   servent_peer_num[s_id]++;
		}
		
		if(servent_peer_num[s_id]!=0){
		
		for(i=0;i<NUM_SERVENT_PEER;i++){
		if(servent_peer_list[s_id][i].peer_id != -1 && i>=servent_peer_num[s_id]){
		for ( j = 0; j < servent_peer_num[s_id]; j++) {
		if(servent_peer_list[s_id][j].peer_id == -1){
		    	   temp = servent_peer_list[s_id][j];
			   servent_peer_list[s_id][j] = servent_peer_list[s_id][i];
			   servent_peer_list[s_id][i] = temp;
			   break;
		}
		   
		}
		   
		}
		   
		}
		   
		}
	   	
	   	for ( i = 0; i < servent_peer_num[s_id]; i++) {		   
			
				send_target = servent_peer_list[s_id][i].peer_id;
				
				
				memset(servent_send_message,0,1024);
				servent_make_command(servent_send_message,servent[s_id].id,servent[send_target].id ,0,"f006");
				

	     			puts("");
	     			if(servent_command_buffer_pointer[send_target] > 99){
	     			servent_command_buffer_pointer[send_target] = 0;
	     			}
	     			strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     			servent_command_buffer_pointer[send_target]++;
	     				
	     			puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			
	     			
	     			servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content); 
				
		        
		        
		}
		puts("");

	        /*for ( i = 0; i < servent_send_trust_message_to_me_pointer[s_id]; i++) {	
	        		  
	        		send_target = servent_send_trust_message_to_me[s_id][i];
				printf("servent %ld send f006 to servent %d",s_id,send_target);
				
				memset(servent_send_message,0,1024);
				servent_make_command(servent_send_message,servent[s_id].id,servent[send_target].id ,0,"f006");
				

	     			puts("");
	     			if(servent_command_buffer_pointer[send_target] > 99){
	     			servent_command_buffer_pointer[send_target] = 0;
	     			}
	     			
	     			strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     			servent_command_buffer_pointer[send_target]++;
	     				
	     			puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			
	     			
	     			servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content); 
				
		        	puts("");
		        
	        }*/
	        
	   	}
	   	
	   
	   	
		
		
		if(servent_latency_signal[s_id] == 1 && servent_select_pattern_signal[s_id] == 1){
		puts("I am latency bot. I dont communicate with other bot");
		servent_pattern[s_id]=1;
		}
		
		if(servent_resurrection_request_peer_signal[s_id] == 1){
		
		printf("servent %ld:I need to request 2 peer !!!!!!!\n",s_id);
		
		if(servent_already_execute_resurrection_signal[s_id] == 1){
		for(i=0;i<NUM_SERVENT_PEER;i++){
		if(servent_peer_list[s_id][i].peer_id != -1 && servent_eliminate_signal[servent_peer_list[s_id][i].peer_id] == 1){
		servent_peer_list[s_id][i].peer_id = -1;
		servent_peer_list[s_id][i].reputation_value = 0;
		//servent_peer_list[s_id][i].sensor_signal = 0;
		   
		}
		   
		}
		}
		servent_peer_num[s_id]=0;
		for(i=0;i<NUM_SERVENT_PEER;i++){
		   if(servent_peer_list[s_id][i].peer_id!= -1 )
		   servent_peer_num[s_id]++;
		}
		if(servent_peer_num[s_id]==0){
		printf("servent %ld doesnt has peer !!! \n",s_id);
		
		servent_pattern[s_id]=99;
		
		}
		if(servent_peer_num[s_id]!=0){
		
		for(i=0;i<NUM_SERVENT_PEER;i++){
		if(servent_peer_list[s_id][i].peer_id != -1 && i>=servent_peer_num[s_id]){
		for ( j = 0; j < servent_peer_num[s_id]; j++) {
		if(servent_peer_list[s_id][j].peer_id == -1){
		    	   temp = servent_peer_list[s_id][j];
			   servent_peer_list[s_id][j] = servent_peer_list[s_id][i];
			   servent_peer_list[s_id][i] = temp;
			   break;
		}
		   
		}
		   
		}
		   
		}
		   
		}
		for ( i = 0; i < servent_peer_num[s_id] ; i++) {
		   
		printf("id:%d value:%d\n",servent_peer_list[s_id][i].peer_id, servent_peer_list[s_id][i].reputation_value);
		}
		
		if(servent_peer_num[s_id]!=0){
		int before_send_target = -1;
		int shut_down_signal=0;
		
		for ( k = 0; k < 2; k++){
		
		//printf("k %d \n",k);
		if(shut_down_signal == 1){
		break;
		}
		
		send_target=0;
		invalid_peer_num = 0;
		for(i=0;i<NUM_SERVENT_PEER;i++){
		if(servent_peer_list[s_id][i].peer_id != -1 ){
		if(servent_peer_list[s_id][i].reputation_value == -1 || servent_peer_list[s_id][i].trust_signal == 1 ){//|| servent_boot_signal[servent_peer_list[s_id][i].peer_id] == 0
		invalid_peer_num++;
		}  
		}  
		if(invalid_peer_num >= NUM_SERVENT_PEER-1){
		break;
		}
				
		}
		if(invalid_peer_num >= NUM_SERVENT_PEER-1){
		printf("servent %ld doesnt has peer can communicate !!! \n",s_id);
		servent_pattern[s_id]=99;
		shut_down_signal=1;
		break;
		}
		servent_sort_peer(s_id);
				
		/*for ( i = servent_peer_num[s_id]-1; i >= 0 ; i--) {
			if(servent_peer_list[s_id][i].reputation_value != 5 && servent_peer_list[s_id][i].reputation_value != -1 
			&& servent_peer_list[s_id][i].peer_id != before_send_target){
				send_target = servent_peer_list[s_id][i].peer_id;
						
				break;
			}
					
		}*/
		if(servent_peer_num[s_id] != 0){
		peer_num = servent_peer_num[s_id];
		send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
				
				
		for ( i = 0; i < servent_peer_num[s_id]; i++) {
				
		if(servent_peer_list[s_id][i].peer_id == send_target){
		j=i;
		while(servent_peer_list[s_id][j].reputation_value == -1 || servent_peer_list[s_id][j].trust_signal == 1 || servent_peer_list[s_id][j].peer_id == before_send_target 
		){//|| servent_boot_signal[servent_peer_list[s_id][j].peer_id] == 0
			
		send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
		
		for ( j = 0; j < servent_peer_num[s_id]; j++) {
		if(servent_peer_list[s_id][j].peer_id == send_target){
		break;
		}
		}
				
		i = 0;    
		
		}
				
		}
				
		}
				
		}
		
		
		before_send_target = send_target;
		servent_peer_num[send_target]=0;
		for(i=0;i<NUM_SERVENT_PEER;i++){
			if(servent_peer_list[send_target][i].peer_id!= -1 )
			servent_peer_num[send_target]++;
		}
		
				
		if(servent_boot_signal[send_target]==0){
		printf("servent %ld shut down !!! \n",send_target);
		printf("servent %ld reputation_value--!!! \n",send_target);
		for ( i = 0; i < servent_peer_num[s_id]; i++) {
			if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
				servent_peer_list[s_id][i].reputation_value--;
			}
		}
				
		servent_pattern[s_id]=99;
		break;
		}
		
		
		if(servent[send_target].sensor_signal == 1 ){
			request_time[send_target]++;
		}
		if(servent[send_target].sensor_signal == 1 && servent[s_id].request_signal == 0 && enumeration_signal == 1){
			servent[s_id].request_signal = 1;
			vs++;
		}
		
		if(servent_peer_num[send_target]==0){
			printf("send_target:servent %ld doesnt has peer !!! \n",send_target);
			
		if(servent_peer_num[s_id] == 1){	
		shut_down_signal=1;
		printf("shut_down_signal %d !!! \n",shut_down_signal);
		}
		servent_pattern[s_id]=99;
		break;
		}
		if(servent_latency_signal[send_target]==1){
		printf("servent %ld already turn to latency bot !!! \n",send_target);
		printf("servent %ld reputation_value--!!! \n",send_target);
		for ( i = 0; i < servent_peer_num[s_id]; i++) {
			if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
				servent_peer_list[s_id][i].reputation_value--;
			}
		}
				
		servent_pattern[s_id]=99;
		break;
		}
		
		
		
		memset(servent_send_message,0,1024);
		servent_make_command(servent_send_message,servent[s_id].id,servent[send_target].id ,0,"f003");
		printf("servent %ld , send_data: %s , target servent:%d", s_id,servent_send_message,send_target);

	     	puts("");
	     			
	     	if(servent_command_buffer_pointer[send_target] <= 99){
	     	strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     	servent_command_buffer_pointer[send_target]++;
	     	puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     	}
	     	else if(servent_command_buffer_pointer[send_target] > 99){
	     	servent_command_buffer_pointer[send_target] = 0;
	     	strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     	servent_command_buffer_pointer[send_target]++;
	     	puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			
	     	}
	     	
	     	if(servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content) == 0 
	     	//&& servent[send_target].sensor_signal != 1 && servent[send_target].fake_signal != 1
	     	&& enumeration_signal == 1
	     	){//&& enumeration_signal == 1 zxxx
	     	printf("servent %ld reputation_value--!!! \n",send_target);
		for ( i = 0; i < servent_peer_num[s_id]; i++) {
			if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
				servent_peer_list[s_id][i].reputation_value--;
			}
		}
				
		servent_pattern[s_id]=99;
		break;
	     	}
	     					
	     	peer_comparison_result=0;		
	     			
	     	if(servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content) == 1){
	
		if(strcmp(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].command_code ,"f003" )==0 ){
				
		char string_ip[100],string_port[100],string_id[100];
		extra_information_split(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].extra_information,string_ip,string_port,string_id);
		send_peer = atoi(string_id);
				
		memset(servent_send_message,0,1024);
		servent_make_command(servent_send_message,servent[send_target].id,servent[s_id].id ,0,"f004" );
				
				
				
		for ( i = 0; i < servent_peer_num[send_target]; i++) {	
		if(servent_peer_list[send_target][i].peer_id == send_peer){
				
				
		printf("servent %ld already have servent %d in peer list !\n",send_target,send_peer);
		peer_comparison_result=1;
		break;
		}
				
				
		}
		if(send_target == send_peer){
		printf("servent %ld cant add itself in peer list !\n",send_target);
		peer_comparison_result=1;
				
		}
		if(send_peer == -1 ){
		printf("servent %ld cant add invalid peer in peer list !\n",send_target);
		peer_comparison_result=1;
				
		}
				
		if(peer_comparison_result == 0 && servent_peer_num[send_target]==NUM_SERVENT_PEER){
		printf("servent %ld remove servent %d in peer list !\n",send_target,servent_peer_list[send_target][0].peer_id);
		printf("servent %ld add servent %d in peer list !\n",send_target,send_peer);
		servent_peer_list[send_target][0].peer_id = send_peer;
		servent_peer_list[send_target][0].reputation_value = reputation_value_base;
				
				
				
				
				
		}
		if(peer_comparison_result == 0 && servent_peer_num[send_target] < NUM_SERVENT_PEER  ){
		//printf("servent %ld remove servent %d in peer list !\n",send_target,servent_peer_list[send_target][0].peer_id);
		printf("servent %ld add servent %d in peer list !\n",send_target,send_peer);
		servent_peer_list[send_target][servent_peer_num[send_target]].peer_id = send_peer;
		servent_peer_list[send_target][servent_peer_num[send_target]].reputation_value = reputation_value_base;
				
		servent_peer_num[send_target]++;
						
				
				
		}
				
			
				
				
		if(servent_command_buffer_pointer[s_id] <= 99){
		strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content, servent_send_message);
	     	servent_command_buffer_pointer[s_id]++;
		puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
		}
		else if(servent_command_buffer_pointer[s_id] > 99){
		servent_command_buffer_pointer[s_id]=0;
		strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content, servent_send_message);
	     	servent_command_buffer_pointer[s_id]++;
		puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
		}			     	
				
					     			
		}
				
	     	}
		peer_comparison_result=0;		
		if(servent_command_analysis(s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content) == 1){
	
		if(strcmp(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].command_code ,"f004" )==0 ){

							
		
		for ( i = 0; i < servent_peer_num[s_id]; i++) {	

		if(servent_peer_list[s_id][i].peer_id == servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id){
		
		
		
		printf("servent %ld already have servent %d in peer list !\n",s_id,servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id);
		peer_comparison_result=1;
		break;
		}
				
				
		}
		if(s_id == servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id){
		printf("servent %ld cant add itself in peer list !\n",s_id);
		peer_comparison_result=1;
				
		}
		
		if(servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id == -1){
		printf("servent %ld cant add invalid peer in peer list !\n",s_id);
		peer_comparison_result=1;
				
		}
		if(peer_comparison_result == 0 && servent_peer_num[s_id]==NUM_SERVENT_PEER){
		printf("servent %ld remove servent %d in peer list !\n",s_id,servent_peer_list[s_id][0].peer_id);
		printf("servent %ld add servent %d in peer list !\n",s_id,servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id);
		servent_peer_list[s_id][0].peer_id = servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id;
		servent_peer_list[s_id][0].reputation_value = reputation_value_base;
					
		
				
		}
		if(peer_comparison_result == 0 && servent_peer_num[s_id] < NUM_SERVENT_PEER  ){
		//printf("servent %ld remove servent %d in peer list !\n",s_id,servent_peer_list[s_id][0].peer_id);
		printf("servent %ld add servent %d in peer list !\n",s_id,servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id);
		servent_peer_list[s_id][servent_peer_num[s_id]].peer_id = servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id;
		servent_peer_list[s_id][servent_peer_num[s_id]].reputation_value = reputation_value_base;
		
		servent_peer_num[s_id]++;
				
		
		}
				
		  			
				
		servent_rearrange_peer(s_id);   
				
		for(i=0;i<servent_peer_num[s_id];i++){	
		if(servent_peer_list[s_id][i].peer_id ==  send_target && servent_peer_list[s_id][i].reputation_value < reputation_value_max ){
		servent_peer_list[s_id][i].reputation_value++;
		break;
		}
		}
				
				
					
		servent_sort_peer(s_id);			
				
		for ( i = 0; i < servent_peer_num[s_id]; i++) {
		if(servent_peer_list[s_id][i].peer_id!= -1 ){
		printf("id:%d value:%d\n",servent_peer_list[s_id][i].peer_id, servent_peer_list[s_id][i].reputation_value);
		}
				
		}	     	
				
					     			
		}
				
	     	}
		
		
			
		}
			if(servent_already_execute_resurrection_signal[s_id] == 1){
			for(i=0;i<NUM_SERVENT_PEER;i++){
			if(servent_peer_list[s_id][i].peer_id != -1 && servent_eliminate_signal[servent_peer_list[s_id][i].peer_id] == 1){
			servent_peer_list[s_id][i].peer_id = -1;
			servent_peer_list[s_id][i].reputation_value = 0;
			//servent_peer_list[s_id][i].sensor_signal = 0;
			   
			}
			   
			}
			}
			servent_rearrange_peer(s_id); 
			
			servent_peer_num[s_id]=0;
			for(i=0;i<NUM_SERVENT_PEER;i++){
			   if(servent_peer_list[s_id][i].peer_id!= -1 )
			   servent_peer_num[s_id]++;
			}
			
			for(i=0;i<servent_peer_num[s_id];i++){
			if(servent_peer_list[s_id][i].reputation_value > 2 && servent_peer_list[s_id][i].peer_id !=-1 ){

			       
			       servent_peer_list[s_id][i].reputation_value = 2;
			       
			}
			else if( servent_latency_signal[servent_peer_list[s_id][i].peer_id] == 1 && servent_peer_list[s_id][i].peer_id !=-1 ){

			       
			       servent_peer_list[s_id][i].reputation_value = 2;
			       
			}
			else if(servent_peer_list[s_id][i].reputation_value <= 2 && servent_peer_list[s_id][i].peer_id !=-1 ){

			       
			       servent_peer_list[s_id][i].reputation_value = 0;
			       
			}
			   
			   
			}
		
		}
		
		servent_resurrection_request_peer_signal[s_id] = 0;
		
		
		}
		
		if(servent_pattern[s_id]!=99){
		servent_log[s_id][servent_execute_pattern_time[s_id]].num_pattern = servent_pattern[s_id];
		servent_log[s_id][servent_execute_pattern_time[s_id]].now_sec = now_sec;
		printf(" servent[%ld]_pattern = %d now_sec = %d !\n", s_id,servent_pattern[s_id],now_sec);
		}
		
		//zxcc
		switch(servent_pattern[s_id]) {
			case 0:
				
				
				servent_pattern[s_id]=99;
				break;
			case 1://botmaster_command
				//file_num=rand()  % 3;
				file_num=0;//***-
				switch(file_num){
				case 0:
					a=0;
					b=0;
					
					printf("servent %ld receive command%d from relay station:\n",s_id,a+1);
					while(file_data[a][b][0]!= '\0'){
					
					for(i=0;i<servent_command_buffer_pointer[s_id];i++){	
					
						command_comparison_result=strcmp(servent_command_buffer[s_id][i].content,file_data[a][b]);
						if(command_comparison_result== 0){
						puts(servent_command_buffer[s_id][i].content);
						break;
						}
					}
					
					if(command_comparison_result== 0){
					printf("servent %ld already have (%s)\n",s_id,file_data[a][b]);
					
					}
					if(command_comparison_result != 0){
					if(servent_command_buffer_pointer[s_id] > 99){
					servent_command_buffer_pointer[s_id]=0;
					}
					strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content,file_data[a][b]);
					puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content);
					
					servent_command_buffer_pointer[s_id]++;
					servent_command_analysis(s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
					}
					
					
					b++;
					
					}
					
					
					
					break;
				case 1:
					a=1;
					b=0;
					
					printf("servent %ld receive command%d from relay station:\n",s_id,a+1);
					while(file_data[a][b][0]!= '\0'){
					
					for(i=0;i<servent_command_buffer_pointer[s_id];i++){	
					command_comparison_result=strcmp(servent_command_buffer[s_id][i].content,file_data[a][b]);
					if(command_comparison_result== 0){break;}
					}
					
					if(command_comparison_result== 0){
					printf("servent %ld already have (%s)\n",s_id,file_data[a][b]);
					
					}
					if(command_comparison_result != 0){
					if(servent_command_buffer_pointer[s_id] > 99){
					servent_command_buffer_pointer[s_id]=0;
					}
					strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content,file_data[a][b]);
					puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content);
					
					servent_command_buffer_pointer[s_id]++;
					servent_command_analysis(s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
					}
					
					
					b++;
					
					}
					
					
					
					break;
				case 2:
					a=2;
					b=0;
					printf("servent %ld receive command%d from relay station:\n",s_id,a+1);
					while(file_data[a][b][0]!= '\0'){
					
					for(i=0;i<servent_command_buffer_pointer[s_id];i++){	
					command_comparison_result=strcmp(servent_command_buffer[s_id][i].content,file_data[a][b]);
					if(command_comparison_result== 0){break;}
					}
					
					if(command_comparison_result== 0){
					printf("servent %ld already have (%s)\n",s_id,file_data[a][b]);
					
					}
					if(command_comparison_result != 0){
					if(servent_command_buffer_pointer[s_id] > 99){
					servent_command_buffer_pointer[s_id]=0;
					}
					strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content,file_data[a][b]);
					puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content);
					servent_command_buffer_pointer[s_id]++;
					servent_command_analysis(s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
					}
					
					
					b++;
					
					}
					
					
					break;
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 1;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				
				break;
			case 2:
				
				send_target=0;
				invalid_peer_num = 0;
				if(servent_already_execute_resurrection_signal[s_id] == 1){
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(servent_peer_list[s_id][i].peer_id != -1 && servent_eliminate_signal[servent_peer_list[s_id][i].peer_id] == 1){
				servent_peer_list[s_id][i].peer_id = -1;
				servent_peer_list[s_id][i].reputation_value = 0;
				//servent_peer_list[s_id][i].sensor_signal = 0;
				}
				   
				}
				}
				servent_peer_num[s_id]=0;
				for(i=0;i<NUM_SERVENT_PEER;i++){
				
				if(servent_peer_list[s_id][i].peer_id!= -1)
				servent_peer_num[s_id]++;
					
				} 
				
				if(servent_peer_num[s_id]==0){
				printf("servent %ld doesnt has peer !!! \n",s_id);
				servent_pattern[s_id]=99;
				break;
				}
				
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(servent_peer_list[s_id][i].peer_id != -1 ){
				if(servent_peer_list[s_id][i].reputation_value == -1 || servent_peer_list[s_id][i].trust_signal == 1 ){//|| servent_boot_signal[servent_peer_list[s_id][i].peer_id] == 0
				invalid_peer_num++;
				}  
				}  
				if(invalid_peer_num >= NUM_SERVENT_PEER){
				break;
				}
				
				}
				if(invalid_peer_num >= NUM_SERVENT_PEER){
				printf("servent %ld doesnt has peer can communicate !!! \n",s_id);
				servent_pattern[s_id]=99;
				break;
				}
				servent_rearrange_peer(s_id);
				servent_sort_peer(s_id);
				
				/*for ( i = servent_peer_num[s_id]-1; i >= 0 ; i--) {
					if(servent_peer_list[s_id][i].reputation_value != 5 && servent_peer_list[s_id][i].reputation_value != -1 ){
						send_target = servent_peer_list[s_id][i].peer_id;
								
						break;
					}
							
				}*/
				
				if(servent_peer_num[s_id] != 0){
				peer_num = servent_peer_num[s_id];
				send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
				
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
				
				if(servent_peer_list[s_id][i].peer_id == send_target){
				j=i;
				while(servent_peer_list[s_id][j].reputation_value == -1 || servent_peer_list[s_id][j].trust_signal == 1 ){//|| servent_boot_signal[servent_peer_list[s_id][j].peer_id] == 0
					
			
				send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
				
				for ( j = 0; j < servent_peer_num[s_id]; j++) {
				if(servent_peer_list[s_id][j].peer_id == send_target){
				break;
				}
				}
				
				i = 0;    
				
				}
				
				}
				
				}
				
				}
				
				
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_target = send_target;
				
				
				
				if(servent_boot_signal[send_target]==0){
				printf("servent %ld shut down !!! \n",send_target);
				printf("servent %ld reputation_value--!!! \n",send_target);
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
					if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
						servent_peer_list[s_id][i].reputation_value--;
					}
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				break;
				}
				
				
				if(servent[send_target].sensor_signal == 1 ){
						request_time[send_target]++;
				}
				if(servent[send_target].sensor_signal == 1 && servent[s_id].request_signal == 0 && enumeration_signal == 1){
						servent[s_id].request_signal = 1;
						vs++;
				}
				
				if(servent_latency_signal[send_target]==1){
				printf("servent %ld already turn to latency bot !!! \n",send_target);
				printf("servent %ld reputation_value--!!! \n",send_target);
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
					if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
						servent_peer_list[s_id][i].reputation_value--;
					}
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				break;
				}
				
				//asdf
				
				char newest_command[1024];
				memset(newest_command,0,strlen(newest_command));
				if(servent_command_buffer_pointer[send_target] != 0 && strlen(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content)!= 0){
				strcpy(newest_command, servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
				}
				
				memset(servent_send_message,0,1024);
				servent_make_command(servent_send_message,servent[s_id].id,servent[send_target].id ,0,"f001");
				

	     			puts("");
	     			
	     			
	     			
	     			if(servent_command_buffer_pointer[send_target] <= 99){
	     			strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     			servent_command_buffer_pointer[send_target]++;
	     			puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			}
	     			else if(servent_command_buffer_pointer[send_target] > 99){
	     			servent_command_buffer_pointer[send_target] = 0;
	     			strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     			servent_command_buffer_pointer[send_target]++;
	     			puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			
	     			}
	     			
	     			if(servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content) == 0 
	     			//&& servent[send_target].sensor_signal != 1 && servent[send_target].fake_signal != 1
	     			&& enumeration_signal == 1
	     			){
	     			// && enumeration_signal == 1 zxxx
	     			printf("servent %ld reputation_value--!!! \n",send_target);
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
					if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
						servent_peer_list[s_id][i].reputation_value--;
					}
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				
				break;
	     			}
	     			
	     			if(servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content) == 1){
				
				if(strcmp(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].command_code ,"f001" )==0 ){
				
				
				
				
				memset(servent_send_message,0,1024);
				servent_make_command(servent_send_message,servent[send_target].id,servent[s_id].id ,0,"f002" );
				
				
				
				if(servent_command_buffer_pointer[s_id] <= 99){
				strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content, servent_send_message);
	     			servent_command_buffer_pointer[s_id]++;
				puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
				}
				else if(servent_command_buffer_pointer[s_id] > 99){
				servent_command_buffer_pointer[s_id]=0;
				strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content, servent_send_message);
	     			servent_command_buffer_pointer[s_id]++;
				puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
				}
				
				
					     			
				}
				
	     			}
	     			
	     			if(servent_command_analysis(s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content) == 1){
	
				if(strcmp(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].command_code ,"f002" )==0 ){
							
				
				printf("servent %ld receive command from servent %ld:\n",s_id,send_target);

				if(strlen(newest_command)!= 0){
				
				for(i=0;i<servent_command_buffer_pointer[s_id];i++){	
				command_comparison_result=strcmp(servent_command_buffer[s_id][i].content,newest_command);
				if(command_comparison_result== 0){break;}
				}
							
				if(command_comparison_result== 0){
				printf("servent %ld already have (%s) \n",s_id,newest_command);
				}
				if(command_comparison_result != 0){
				if(servent_command_buffer_pointer[s_id] > 99){
				servent_command_buffer_pointer[s_id]=0;
				}
				
				strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content,newest_command);
				puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content);
				servent_command_buffer_pointer[s_id]++;
				servent_command_analysis(s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
				}
				
				}
					
				
				
				
				
				
				servent_rearrange_peer(s_id);   
				
				for(i=0;i<servent_peer_num[s_id];i++){	
				if(servent_peer_list[s_id][i].peer_id ==  send_target && servent_peer_list[s_id][i].reputation_value < reputation_value_max ){
				servent_peer_list[s_id][i].reputation_value++;
				break;
				}
				}
				
				
					
				servent_sort_peer(s_id);			
				
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
				if(servent_peer_list[s_id][i].peer_id!= -1 ){
				printf("id:%d value:%d\n",servent_peer_list[s_id][i].peer_id, servent_peer_list[s_id][i].reputation_value);
				}
				
				}	     			
				
					     	
				
					     			
				}
				
	     			}
	     			
	     			
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 1;
				servent_execute_pattern_time[s_id]++;
	     			servent_pattern[s_id]=99;
			 	
				break;
			case 3:
				
				send_target=0;
				invalid_peer_num = 0;
				send_peer=0;
				if(servent_already_execute_resurrection_signal[s_id] == 1){
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(servent_peer_list[s_id][i].peer_id != -1 && servent_eliminate_signal[servent_peer_list[s_id][i].peer_id] == 1){
				servent_peer_list[s_id][i].peer_id = -1;
				servent_peer_list[s_id][i].reputation_value = 0;
				//servent_peer_list[s_id][i].sensor_signal = 0;
				}
				   
				}
				}
				servent_peer_num[s_id]=0;
				for(i=0;i<NUM_SERVENT_PEER;i++){
				
				if(servent_peer_list[s_id][i].peer_id!= -1)
				servent_peer_num[s_id]++;
					
				} 
				
				if(servent_peer_num[s_id]==0){
				printf("servent %ld doesnt has peer !!! \n",s_id);
				servent_pattern[s_id]=99;
				break;
				}
				
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(servent_peer_list[s_id][i].peer_id != -1 ){
				if(servent_peer_list[s_id][i].reputation_value == -1 || servent_peer_list[s_id][i].trust_signal == 1 ){//|| servent_boot_signal[servent_peer_list[s_id][i].peer_id] == 0
				invalid_peer_num++;
				}  
				}  
				if(invalid_peer_num >= NUM_SERVENT_PEER-1){
				break;
				}
				
				}
				if(invalid_peer_num >= NUM_SERVENT_PEER-1){
				printf("servent %ld doesnt has peer can communicate !!! \n",s_id);
				servent_pattern[s_id]=99;
				break;
				}
				servent_rearrange_peer(s_id);
				servent_sort_peer(s_id);
				
				/*for ( i = servent_peer_num[s_id]-1; i >= 0 ; i--) {
					if(servent_peer_list[s_id][i].reputation_value != 5 && servent_peer_list[s_id][i].reputation_value != -1 ){
						send_target = servent_peer_list[s_id][i].peer_id;
								
						break;
					}
							
				}*/
				if(servent_peer_num[s_id] != 0){
				peer_num = servent_peer_num[s_id];
				send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
				
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
				
				if(servent_peer_list[s_id][i].peer_id == send_target){
				j=i;
				while(servent_peer_list[s_id][j].reputation_value == -1 || servent_peer_list[s_id][j].trust_signal == 1 ){//|| servent_boot_signal[servent_peer_list[s_id][j].peer_id] == 0
					
			
		
				send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
				
				for ( j = 0; j < servent_peer_num[s_id]; j++) {
				if(servent_peer_list[s_id][j].peer_id == send_target){
				break;
				}
				}
				
				i = 0;    
				
				}
				
				}
				
				}
				
				}
				
				
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_target = send_target;
				servent_peer_num[send_target]=0;
				for(i=0;i<NUM_SERVENT_PEER;i++){
				
				if(servent_peer_list[send_target][i].peer_id!= -1){
				servent_peer_num[send_target]++;
				}	
				} 
				
				
				if(servent_boot_signal[send_target]==0){
				printf("servent %ld shut down !!! \n",send_target);
				printf("servent %ld reputation_value--!!! \n",send_target);
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
					if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
						servent_peer_list[s_id][i].reputation_value--;
					}
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				break;
				}
				
				
				
				if(servent[send_target].sensor_signal == 1 ){
						request_time[send_target]++;
				}
				if(servent[send_target].sensor_signal == 1 && servent[s_id].request_signal == 0 && enumeration_signal == 1){
						servent[s_id].request_signal = 1;
						vs++;
				}
				
				if(servent_peer_num[send_target]==0){
				printf("send_target:servent %ld doesnt has peer !!! \n",send_target);
				
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				break;
				}
				
				if(servent_latency_signal[send_target]==1){
				printf("servent %ld already turn to latency bot !!! \n",send_target);
				printf("servent %ld reputation_value--!!! \n",send_target);
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
					if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
						servent_peer_list[s_id][i].reputation_value--;
					}
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				break;
				}
				
				memset(servent_send_message,0,1024);
				servent_make_command(servent_send_message,servent[s_id].id,servent[send_target].id ,0,"f003");
				printf("servent %ld , send_data: %s , target servent:%d", s_id,servent_send_message,send_target);

	     			puts("");
	     			
	     			if(servent_command_buffer_pointer[send_target] <= 99){
	     			strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     			servent_command_buffer_pointer[send_target]++;
	     			puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			}
	     			else if(servent_command_buffer_pointer[send_target] > 99){
	     			servent_command_buffer_pointer[send_target] = 0;
	     			strcpy(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]].content, servent_send_message);
	     			servent_command_buffer_pointer[send_target]++;
	     			puts(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content);
	     			
	     			}
	     			
	     			if(servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content) == 0 
	     			//&& servent[send_target].sensor_signal != 1 && servent[send_target].fake_signal != 1
	     			&& enumeration_signal == 1
	     			){
	     			//&& enumeration_signal == 1 zxxx
	     			printf("servent %ld reputation_value--!!! \n",send_target);
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
					if(servent_peer_list[s_id][i].peer_id == send_target && servent_peer_list[s_id][i].reputation_value > -1 ){
						servent_peer_list[s_id][i].reputation_value--;
					}
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				
				break;
	     			}
	     			
	     			
	     			peer_comparison_result=0;
	     			if(servent_command_analysis(send_target,servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].content) == 1){
	
				if(strcmp(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].command_code ,"f003" )==0 ){
				
				char string_ip[100],string_port[100],string_id[100];
				extra_information_split(servent_command_buffer[send_target][servent_command_buffer_pointer[send_target]-1].extra_information,string_ip,string_port,string_id);
				send_peer = atoi(string_id);
				
				memset(servent_send_message,0,1024);
				servent_make_command(servent_send_message,servent[send_target].id,servent[s_id].id ,0,"f004" );
				
				
				
				for ( i = 0; i < servent_peer_num[send_target]; i++) {	
				if(servent_peer_list[send_target][i].peer_id == send_peer){
				
				
				printf("servent %ld already have servent %d in peer list !\n",send_target,send_peer);
				peer_comparison_result=1;
				break;
				}
				
				
				}
				if(send_target == send_peer){
				printf("servent %ld cant add itself in peer list !\n",send_target);
				peer_comparison_result=1;
				
				}
				if(send_peer == -1 ){
				printf("servent %ld cant add invalid peer in peer list !\n",send_target);
				peer_comparison_result=1;
				
				}
				
				if(peer_comparison_result == 0 && servent_peer_num[send_target]==NUM_SERVENT_PEER){
				printf("servent %ld remove servent %d in peer list !\n",send_target,servent_peer_list[send_target][0].peer_id);
				printf("servent %ld add servent %d in peer list !\n",send_target,send_peer);
				servent_peer_list[send_target][0].peer_id = send_peer;
				servent_peer_list[send_target][0].reputation_value = reputation_value_base;
				
				
				
				
				
				}
				if(peer_comparison_result == 0 && servent_peer_num[send_target] < NUM_SERVENT_PEER  ){
				//printf("servent %ld remove servent %d in peer list !\n",send_target,servent_peer_list[send_target][0].peer_id);
				printf("servent %ld add servent %d in peer list !\n",send_target,send_peer);
				servent_peer_list[send_target][servent_peer_num[send_target]].peer_id = send_peer;
				servent_peer_list[send_target][servent_peer_num[send_target]].reputation_value = reputation_value_base;
				
				servent_peer_num[send_target]++;
						
				
				
				}
				
				if(servent_log[s_id][servent_execute_pattern_time[s_id]].num_pattern == 3 && peer_comparison_result == 0 ){
				servent_log[s_id][servent_execute_pattern_time[s_id]].send_peer=send_peer;
				}
				//s5s5
				
				
				//------
				if(servent_command_buffer_pointer[s_id] <= 99){
				strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content, servent_send_message);
	     			servent_command_buffer_pointer[s_id]++;
				puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
				}
				else if(servent_command_buffer_pointer[s_id] > 99){
				servent_command_buffer_pointer[s_id]=0;
				strcpy(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]].content, servent_send_message);
	     			servent_command_buffer_pointer[s_id]++;
				puts(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content);
				}	     	
				
					     			
				}
				
	     			}
				peer_comparison_result=0;
				if(servent_command_analysis(s_id,servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].content) == 1){
	
				if(strcmp(servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1].command_code ,"f004" )==0 ){
							
				//ccccc
				
				for ( i = 0; i < servent_peer_num[s_id]; i++) {	
				if(servent_peer_list[s_id][i].peer_id == servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id){
				
				
				
				
				printf("servent %ld already have servent %d in peer list !\n",s_id,servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id);
				peer_comparison_result=1;
				break;
				}
				
				
				}
				if(s_id == servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id){
				printf("servent %ld cant add itself in peer list !\n",s_id);
				peer_comparison_result=1;
				
				}
				if(servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id == -1){
				printf("servent %ld cant add invalid peer in peer list !\n",s_id);
				peer_comparison_result=1;
				
				}
				if(peer_comparison_result == 0 && servent_peer_num[s_id]==NUM_SERVENT_PEER){
				printf("servent %ld remove servent %d in peer list !\n",s_id,servent_peer_list[s_id][0].peer_id);
				printf("servent %ld add servent %d in peer list !\n",s_id,servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id);
				servent_peer_list[s_id][0].peer_id = servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id;
				servent_peer_list[s_id][0].reputation_value = reputation_value_base;

				
				
				
				}
				if(peer_comparison_result == 0 && servent_peer_num[s_id] < NUM_SERVENT_PEER  ){
				//printf("servent %ld remove servent %d in peer list !\n",s_id,servent_peer_list[s_id][0].peer_id);
				printf("servent %ld add servent %d in peer list !\n",s_id,servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id);
				servent_peer_list[s_id][servent_peer_num[s_id]].peer_id = servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id;
				servent_peer_list[s_id][servent_peer_num[s_id]].reputation_value = reputation_value_base;

				servent_peer_num[s_id]++;
				
				
								
				
				}
				if(servent_log[s_id][servent_execute_pattern_time[s_id]].num_pattern == 3 && peer_comparison_result == 0 
				&& (servent[send_target].fake_signal != 1 && servent[send_target].sensor_signal != 1)){
				servent_log[s_id][servent_execute_pattern_time[s_id]].receive_peer=servent_peer_list[send_target][servent_peer_num[send_target]-1].peer_id;
				
				}
				
				
				
				
					
				  			
				
				servent_rearrange_peer(s_id);   
				
				for(i=0;i<servent_peer_num[s_id];i++){	
				if(servent_peer_list[s_id][i].peer_id ==  send_target && servent_peer_list[s_id][i].reputation_value < reputation_value_max ){
				servent_peer_list[s_id][i].reputation_value++;
				break;
				}
				}
				
				
					
				servent_sort_peer(s_id);			
				
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
				if(servent_peer_list[s_id][i].peer_id!= -1 ){
				printf("id:%d value:%d\n",servent_peer_list[s_id][i].peer_id, servent_peer_list[s_id][i].reputation_value);
				}
				
				}	     	
				
					     			
				}
				
	     			}
				//------
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 1;
				servent_execute_pattern_time[s_id]++;
	     			servent_pattern[s_id]=99;
				break;	
				
			case 4:	
			        send_target=0;
				invalid_peer_num = 0;
				if(servent_already_execute_resurrection_signal[s_id] == 1){
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(servent_peer_list[s_id][i].peer_id != -1 && servent_eliminate_signal[servent_peer_list[s_id][i].peer_id] == 1){
				servent_peer_list[s_id][i].peer_id = -1;
				servent_peer_list[s_id][i].reputation_value = 0;
				//servent_peer_list[s_id][i].sensor_signal = 0;
				
				}
				   
				}
				}
				servent_peer_num[s_id]=0;
				for(i=0;i<NUM_SERVENT_PEER;i++){
				
				if(servent_peer_list[s_id][i].peer_id!= -1)
				servent_peer_num[s_id]++;
					
				} 
				
				if(servent_peer_num[s_id]==0){
				printf("servent %ld doesnt has peer !!! \n",s_id);
				servent_pattern[s_id]=99;
				break;
				}
				
				for(i=0;i<NUM_SERVENT_PEER;i++){
				if(servent_peer_list[s_id][i].peer_id != -1 ){
				if(servent_peer_list[s_id][i].reputation_value == -1 || servent_peer_list[s_id][i].trust_signal == 1 ){//|| servent_boot_signal[servent_peer_list[s_id][i].peer_id] == 0
				invalid_peer_num++;
				}  
				}  
				if(invalid_peer_num >= NUM_SERVENT_PEER){
				break;
				}
				
				}
				if(invalid_peer_num >= NUM_SERVENT_PEER){
				printf("servent %ld doesnt has peer can communicate !!! \n",s_id);
				servent_pattern[s_id]=99;
				break;
				}
				servent_rearrange_peer(s_id);
				servent_sort_peer(s_id);
				
				/*for ( i = servent_peer_num[s_id]-1; i >= 0 ; i--) {
					if(servent_peer_list[s_id][i].reputation_value != 5 && servent_peer_list[s_id][i].reputation_value != -1 ){
						send_target = servent_peer_list[s_id][i].peer_id;
								
						break;
					}
							
				}*/
				if(servent_peer_num[s_id] != 0){
				peer_num = servent_peer_num[s_id];
				send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
				
				for ( i = 0; i < servent_peer_num[s_id]; i++) {
				
				if(servent_peer_list[s_id][i].peer_id == send_target){
				j=i;
				while(servent_peer_list[s_id][j].reputation_value == -1 || servent_peer_list[s_id][j].trust_signal == 1 ){//|| servent_boot_signal[servent_peer_list[s_id][j].peer_id] == 0
				
					
		
				send_target = servent_peer_list[s_id][rand() % (peer_num)].peer_id;
				
				for ( j = 0; j < servent_peer_num[s_id]; j++) {
				if(servent_peer_list[s_id][j].peer_id == send_target){
				break;
				}
				}
				
				i = 0;    
				
				}
				
				}
				
				}
				
				}
				/*
				if(servent[send_target].sensor_signal == 1 ){
						request_time++;
				}

				if(servent[send_target].sensor_signal == 1 && servent[s_id].request_signal == 0 && enumeration_signal == 1){
						servent[s_id].request_signal = 1;
						vs++;
				}*/
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_target = send_target;
				if(servent_latency_signal[send_target]==1){
				printf("servent %ld already turn to latency bot !!! \n",send_target);
				
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				break;
				}
				
				if(servent_boot_signal[send_target]==0){
				printf("servent %ld shut down !!! \n",send_target);
				
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 0;
				servent_execute_pattern_time[s_id]++;
				servent_pattern[s_id]=99;
				break;
				}
				
				
				
				b=0;
				
				printf("website from servent %ld:\n",send_target);
				while(servent_bot_website_buffer[send_target][b].url[0]!= '\0'){
							
				for(i=0;i<servent_bot_website_buffer_pointer[s_id];i++){	
				website_comparison_result=strcmp(servent_bot_website_buffer[s_id][i].url,servent_bot_website_buffer[send_target][b].url);
				if(website_comparison_result== 0){break;}
				}
							
				if(website_comparison_result== 0){
				printf("servent %ld already have (%s)\n",s_id,servent_bot_website_buffer[send_target][b].url);
				}
				if(website_comparison_result != 0){
				strcpy(servent_bot_website_buffer[s_id][servent_bot_website_buffer_pointer[s_id]].url,servent_bot_website_buffer[send_target][b].url);
				servent_bot_website_buffer[s_id][servent_bot_website_buffer_pointer[s_id]].reputation_value = servent_bot_website_buffer[send_target][b].reputation_value ;
				
				puts(servent_bot_website_buffer[s_id][servent_bot_website_buffer_pointer[s_id]].url);
				servent_bot_website_buffer_pointer[s_id]++;
				}				
				b++;			
				}
				servent_log[s_id][servent_execute_pattern_time[s_id]].communication_result = 1;
				servent_execute_pattern_time[s_id]++;
	     			servent_pattern[s_id]=99;
				break;	
			
			case (-1):
				printf(" servent %ld terminated !\n", s_id);
				break;	
		}
		//***-
		
		
		
		time(&current);
		info = localtime( &current );
		
		strftime(string_now_year,sizeof(string_now_year),"%Y",info);
		now_year = atoi(string_now_year);
		strftime(string_now_month,sizeof(string_now_month),"%m",info);
		now_month = atoi(string_now_month);
		strftime(string_now_day,sizeof(string_now_day),"%d",info);
		now_day = atoi(string_now_day);
		
		strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
		now_hour = atoi(string_now_hour);	
		strftime(string_now_min,sizeof(string_now_min),"%M",info);
		now_min = atoi(string_now_min);	
		strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
		now_sec = atoi(string_now_sec);	
		    
		now_sec+=(60*now_min)+(60*60*now_hour);
		if(servent_select_pattern_signal[s_id] ==1){
		servent_last_time_select_pattern[s_id]  = now_sec;

		
		}
			
			int delete_command_signal=0;
		//printf("before:servent_command_buffer_pointer[s_id] %d  !\n", servent_command_buffer_pointer[s_id]);
			
		for ( i = 0; i < servent_command_buffer_pointer[s_id]; i++) {
		
		if(strlen(servent_command_buffer[s_id][i].content) != 0){
			
			//puts(servent_command_buffer[s_id][i].content);
			timestamp_split(servent_command_buffer[s_id][i].timestamp ,string_command_year,string_command_month,string_command_day,string_command_sec);
			
			command_year = atoi(string_command_year);	
			command_month = atoi(string_command_month);	
			command_day = atoi(string_command_day);	
			command_sec = atoi(string_command_sec);	
			//printf("effective_time %d  !\n", servent_command_buffer[s_id][i].effective_time);
			deadline_sec = command_sec + servent_command_buffer[s_id][i].effective_time;
			deadline_day = command_day ;
			deadline_month = command_month ;
			deadline_year = command_year ;
			int leap_year=0;
			if((deadline_year % 4 == 0 && deadline_year % 100 != 0 ) || deadline_year % 400 == 0) {
			leap_year = 1;
			}
			//printf("leap_year %d  !\n", leap_year);
			
			if(deadline_sec > 86400 ){
			deadline_sec = deadline_sec-86400;
			deadline_day++;
			}
			if( (deadline_month == 4 || deadline_month == 6 || deadline_month == 9 || deadline_month == 11) && deadline_day > 30 ){
			deadline_day = 1;
			deadline_month++;
			}
			if( (deadline_month == 1 || deadline_month == 3 || deadline_month == 5 || deadline_month == 7 || deadline_month == 10 || deadline_month == 12 ) && deadline_day > 31 ){
			deadline_day = 1;
			deadline_month++;
			}
			if( leap_year == 1 && ( deadline_month == 2) && ( deadline_day > 29) ){
			deadline_day = 1;
			deadline_month++;
			}
			if( leap_year == 0 && ( deadline_month == 2) && ( deadline_day > 28) ){

			deadline_day = 1;
			deadline_month++;
			}
			if( deadline_month > 12 ){
			deadline_month = 1;
			deadline_year++;
			}
			
			/*printf("deadline_year %d  !\n", deadline_year);
			printf("deadline_month %d  !\n", deadline_month);
			printf("deadline_day %d  !\n", deadline_day);
			printf("deadline_sec %d  !\n", deadline_sec);

			printf("now_year %d  !\n", now_year);
			printf("now_month %d  !\n", now_month);
			printf("now_day %d  !\n", now_day);
			printf("now_sec %d  !\n", now_sec);*/
			if(now_year > deadline_year){
			delete_command_signal = 1;
			}
			if(now_year == deadline_year){
				if(now_month > deadline_month){
				delete_command_signal = 1;
				}
				if(now_month == deadline_month){
					if(now_day > deadline_day){
					delete_command_signal = 1;
					}
					if(now_day == deadline_day){
						if(now_sec >= deadline_sec){
						delete_command_signal = 1;
						}
					}
					
				}
				
			}
			
			
			if(delete_command_signal == 1){
			//printf("delete_command_signal[%ld] %d  !\n", s_id,delete_command_signal);
			Command temp = servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1];
			servent_command_buffer[s_id][servent_command_buffer_pointer[s_id]-1] = servent_command_buffer[s_id][i];
		        servent_command_buffer[s_id][i] = temp;
		        
		        memset(&servent_command_buffer[s_id][i],0,sizeof(Command));
			servent_command_buffer_pointer[s_id]--;
			}
			
		}
		
		}
		
		sleep(0.2);
	        
		
		
	   
		
	

}
void *sensor_thread_func(void *threadid) {// 1 thread = 5 bots
    long tid;
    int i,j;
    int rc;
    
    tid = (long)threadid;
    
    
   
    while(sensor_thread_work_over[tid/5] != 1){
		
		sleep(2.2);
		for(i=0;i<5;i++){	
		sensor_func((NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS)+tid+i);
		sleep(0.7);
		//printf("%d..*.*.*.\n", (NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS)+tid+i );
		}		
		
		
	
	}
	
    printf(" sensor_thread %ld terminated !\n", tid);
    pthread_exit(NULL);
}
void *fake_servent_thread_func(void *threadid) {// 1 thread = 5 bots
    long tid;
    int i,j;
    int rc;
    
    tid = (long)threadid;
    
    
   
    while(fake_servent_thread_work_over[tid/5] != 1){
		
		sleep(2.2);
		for(i=0;i<5;i++){	
		fake_servent_func((NUM_SERVENT_BOTS)+tid+i);
		sleep(0.7);
		//printf("%d..*.*.*.\n", (NUM_SERVENT_BOTS)+tid+i);
		}		
		
		
	
	}
	
    printf(" fake_servent_thread %ld terminated !\n", tid);
    pthread_exit(NULL);
}
void *servent_thread_func(void *threadid) {// 1 thread = 5 bots
    long tid;
    int i,j;
    int rc;
    
    tid = (long)threadid;
    
    
    //printf("servent_func_id %d..*.*.*.\n", 50*tid+i);
    
    while(servent_thread_work_over[tid] != 1){
		
		sleep(2.2);
		for(i=0;i<50;i++){
		servent_func(50*tid+i);
		//
		sleep(0.7);
		}
		
		
	
	}
	
    printf(" servent_thread %ld terminated !\n", tid);
    pthread_exit(NULL);
}
void *client_thread_func(void *threadid) {// 1 thread = 5 bots
    long tid;
    int i,j;
    int rc;
    
    tid = (long)threadid;
    
    while(client_thread_work_over[tid] != 1){
		
		sleep(2.2);
		for(i=0;i<50;i++){
		client_func(50*tid+i);
		sleep(0.7);
		
		}
		
	
	
	}
	
    printf(" client_thread %ld terminated !\n", tid);
    pthread_exit(NULL);
}
void program_over(int signal){
    int i=0;
    int j=0;
    if(signal==1){
    
  	    for (i = 0; i < NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR; i++) {
    	    servent_pattern[i]=99;
    	    }for (i = 0; i < NUM_CLIENT_BOTS; i++) {
    	    client_pattern[i]=99;
    	    }
    	    //sleep(120);
  	    for (i = 0; i < (NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR/50); i++) { // /1000z
					
		servent_thread_work_over[i]=1;
	    	
	    }		
	    for (i = 0; i < (NUM_CLIENT_BOTS/50); i++) { // /1000
					
		client_thread_work_over[i]=1;
	    	
	    }
	    for (i = 0; i < (NUM_FAKE_SERVENT_BOTS/5); i++) { // /1000
		
		fake_servent_thread_work_over[i]=1;
		
	    	
	    }  
	    for (i = 0; i < (NUM_SENSOR/5); i++) { // /1000
		
		sensor_thread_work_over[i]=1;
		
	    	
	    }  
	   /* for (i = 0; i < NUM_SERVENT_BOTS; i++) {
					
		servent_work_over[i]=1;
		for (j = 0; j < NUM_SERVENT_BOTS; j++){
			servent_tunnel_work_over[i][j]=1;
		}		
	    }*/
	    
	   /* for (i = 0; i < NUM_CLIENT_BOTS; i++) {
					
		client_work_over[i]=1;
		for (j = 0; j < NUM_SERVENT_BOTS; j++){
			client_and_servent_tunnel_work_over[i][j]=1;
		}		
	    }*/
	    
	   
	    data_record_terminate_signal=1;
	    relay_station_terminate_signal=1;	
	    infect_terminate_signal=1;
	    boot_control_terminate_signal=1;
	    request_time_record_terminate_signal=1;
	    program_terminate_signal=1;
	
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
	int port = rand() % 30000;
	return port;
}
void init_client_master(int c_id){
	
    int i=0,j=0,a=0,limit=0;
    i=c_id;
    /*if(servent_bot_num_now >= NUM_SERVENT_BOTS){
    limit = NUM_SERVENT_BOTS;
    }
    if(servent_bot_num_now < NUM_SERVENT_BOTS){
    limit = servent_thread_num_now*50;
    }*/
    limit = 5000;
    
    for (j = 0; j < NUM_SERVENT_PEER; j++) {//***- NUM_SERVENT_BOTS
    
    
    client_master[i][j].reputation_value = reputation_value_base;//###
    
    client_master[i][j].master_id = rand() % limit  ;

    for (a = 0; a < j; a++) {
    
    while(client_master[i][a].master_id==client_master[i][j].master_id){
    client_master[i][j].master_id = rand() % limit;
    a = 0;
    }
    }
    }

    int probability=0;
    /*probability = rand() % 10;
    if( probability>=2 && probability<=2 ){
	client_master[i][8].master_id = rand() % 100+4900;
    }
    if( probability>=3 && probability<=3 ){
	client_master[i][7].master_id = rand() % 100+4900;
    }*/
    
    /*for (j = 0; j < NUM_SERVENT_PEER; j++) {
    
    
	    if(client_master[i][j].master_id != -1 && client_master[i][j].master_id >= 50){
	    	client_master[i][j].reputation_value=1; 
	    }
    	    if((j>=1 && j<=3 ) && client_master[i][j].master_id != -1){
	    probability = rand() % 2;
	    if( probability==0  ){
	    client_master[i][j].reputation_value=0;
	    }
	    else if( probability==1  ){
	    client_master[i][j].reputation_value=2;
	    }
    
    	    }
    }*/
    
   
    puts("");
    printf("master of client %ld have:", i);
    for (j = 0; j < NUM_SERVENT_PEER; j++) {
    
    
    if(client_master[i][j].master_id!= -1)
    	printf("%ld %ld  ", client_master[i][j].master_id, client_master[i][j].reputation_value);
    
    }	
    
    
    puts("");

}
void init_fake_servent_and_sensor_peer_list(){
	
    int i=0,j=0,a=0,limit=0;

    /*if(servent_bot_num_now >= 5000){
    limit = NUM_SERVENT_BOTS;
    }
    if(servent_bot_num_now < 5000){
    limit = servent_thread_num_now*50;
    }*/
    limit = 5000;
    for (i = NUM_SERVENT_BOTS; i < NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS; i++){
    servent[i].detect_signal = 1;
    servent[i].detect_and_reply_signal = 1;
    servent[i].request_signal = 1; 
    servent[i].fake_signal=1;
    servent[i].sensor_signal=0;
				
				
    }
    for (i = NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS; i < NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR; i++){
    servent[i].detect_signal = 1;
    servent[i].detect_and_reply_signal = 1;
    servent[i].request_signal = 1; 
    servent[i].fake_signal=0;
    servent[i].sensor_signal=1;
				
				
    }
    
    
    
    for (i = NUM_SERVENT_BOTS; i < NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR; i++) {

    for (j = 0; j < NUM_SERVENT_PEER ; j++) {//***- max : NUM_SERVENT_BOTS-1 NUM_SERVENT_PEER=10

    servent_peer_list[i][j].reputation_value=reputation_value_base; 
    
    
    servent_peer_list[i][j].peer_id = rand() % limit  ;
    
    
    while(servent_peer_list[i][j].peer_id==i){
    
    servent_peer_list[i][j].peer_id = rand() % limit ;
    
    }
    
    for (a = 0; a < j; a++) {

    while(servent_peer_list[i][j].peer_id == servent_peer_list[i][a].peer_id){
    servent_peer_list[i][j].peer_id = rand() % limit;
    while(servent_peer_list[i][j].peer_id == i){
    servent_peer_list[i][j].peer_id = rand() % limit;
    }
    a = 0;
    }
    

    }
    
 
    }
    
    
    
    }
    int probability=0;	
    /*for(i=NUM_SERVENT_BOTS;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
    for (j = 0; j < NUM_SERVENT_PEER; j++) {
    
    
	    if(servent_peer_list[i][j].peer_id != -1 && servent_peer_list[i][j].peer_id >= 50){
	    	servent_peer_list[i][j].reputation_value=1; 
	    }	
	    if((j>=1 && j<=3 ) && servent_peer_list[i][j].peer_id!= -1){
	    probability = rand() % 2;
	    if( probability==0  ){
	    servent_peer_list[i][j].reputation_value=0;
	    }
	    else if( probability==1  ){
	    servent_peer_list[i][j].reputation_value=2;
	    }
    
    	    }
    }
    }*/
    int detect_num=0;
    for(i=NUM_SERVENT_BOTS;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){// ccccc
    puts("");
	    if(servent[i].fake_signal==1){
	    printf("peer list of fake_servent %ld have:", i);
	    for (j = 0; j < NUM_SERVENT_PEER; j++) {
	    
	    
	    if(servent_peer_list[i][j].peer_id!= -1 && inject_signal == 1){
	    
	    printf("%ld %ld  ", servent_peer_list[i][j].peer_id, servent_peer_list[i][j].reputation_value);
	    
	    if(servent[servent_peer_list[i][j].peer_id].detect_signal==0){
	    servent[servent_peer_list[i][j].peer_id].detect_signal = 1;
	    //detect_num++;
	    }
	    
	    
	    
	    }
	    
	    	

	    
	    }	
	    }
	    else if(servent[i].sensor_signal==1){
	    
	    printf("peer list of sensor %ld have:", i);
	    for (j = 0; j < NUM_SERVENT_PEER; j++) {
	    
	    
	    if(servent_peer_list[i][j].peer_id!= -1 && inject_signal == 1){
	    
	    printf("%ld %ld  ", servent_peer_list[i][j].peer_id, servent_peer_list[i][j].reputation_value);
	    
	    if(servent[servent_peer_list[i][j].peer_id].detect_signal==0){
	    servent[servent_peer_list[i][j].peer_id].detect_signal = 1;
	    //detect_num++;
	    }
	    
	    
	    
	    }
	    
	    	

	    
	    }
	    }
    }
    /*for(i=0;i<NUM_SERVENT_BOTS;i++){
    if(servent[i].detect_signal==1){
    
    detect_num++;
    }
    }
    printf("detect_num:%d\n",detect_num);*/
    
    puts("");

}
void init_servent_peer_list(int s_id){
	
    int i=0,j=0,a=0,limit=0;

    i=s_id;
    /*if(servent_bot_num_now >= NUM_SERVENT_BOTS){
    limit = NUM_SERVENT_BOTS;
    }
    if(servent_bot_num_now < NUM_SERVENT_BOTS){
    limit = servent_thread_num_now*50;
    }*/
    if(i<=50){
    limit=50;
    }
    else if(i>50){
    limit=5000;
    }
    

    for (j = 0; j < NUM_SERVENT_PEER ; j++) {//***- max : NUM_SERVENT_BOTS-1 NUM_SERVENT_PEER=10

    servent_peer_list[i][j].reputation_value=reputation_value_base; 
    
    
    servent_peer_list[i][j].peer_id = rand() % limit  ;
    
    
    while(servent_peer_list[i][j].peer_id==i){
    
    servent_peer_list[i][j].peer_id = rand() % limit ;
    
    }
    
    for (a = 0; a < j; a++) {

    while(servent_peer_list[i][j].peer_id == servent_peer_list[i][a].peer_id){
    servent_peer_list[i][j].peer_id = rand() % limit;
    while(servent_peer_list[i][j].peer_id == i){
    servent_peer_list[i][j].peer_id = rand() % limit;
    }
    a = 0;
    }
    

    }
    
 
    }
    int probability=0;	   
    /*for (j = 0; j < NUM_SERVENT_PEER; j++) {
    
    
    if(servent_peer_list[i][j].peer_id!= -1 && servent_peer_list[i][j].peer_id >= 50){
    	servent_peer_list[i][j].reputation_value=1; 
    	
    }
    if((j>=1 && j<=3 ) && servent_peer_list[i][j].peer_id!= -1){
    probability = rand() % 2;
    if( probability==0  ){
    servent_peer_list[i][j].reputation_value=0;
    }
    else if( probability==1  ){
    servent_peer_list[i][j].reputation_value=2;
    }
    
    }
    
    }*/
    
    
    
   
    
    
    puts("");
    printf("peer list of servent %ld have:", i);
    for (j = 0; j < NUM_SERVENT_PEER; j++) {
    
    
    if(servent_peer_list[i][j].peer_id!= -1)
    	printf("%ld %ld  ", servent_peer_list[i][j].peer_id, servent_peer_list[i][j].reputation_value);

    
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
    for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
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
    
    for (i = 0; i < NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR; i++){
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
    
    for (i = 0; i < NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR; i++){
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
    for (i = 0; i < NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR; i++){  	    	
    servent[i].id = i;
    }
    
    //----
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
    for (i = 0; i < NUM_CLIENT_BOTS; i++){  	    	
    client[i].id =i+NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;
    }
    
    //----
    
    
    
    //--
    for (i = 0; i < NUM_CLIENT_BOTS; i++){
    printf("client[%d]->id : %d\n",i,client[i].id);
    printf("client[%d]->ip\n",i);
    puts(client[i].ip);
    printf("client[%d]->port: %d\n",i,client[i].port);
    }
    for (i = 0; i < NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR; i++){
    printf("servent[%d]->id : %d\n",i,servent[i].id);
    printf("servent[%d]->ip\n",i);
    puts(servent[i].ip);
    printf("servent[%d]->port: %d\n",i,servent[i].port);
    }
    
    		
    
    
    
    
    
   
 
}
void *relay_station_func(){
    long time_counter =5;
    char path[1024];
    char write_data[1024];

    int i=0,j=0,f=1;
    int now_hour,now_min,now_sec;
    char string_now_hour[1024],string_now_min[1024],string_now_sec[1024];
    
    	
    FILE* file;
    printf("Hello There! I am relay station read command/%d sec\n",time_counter);
    while(relay_station_terminate_signal != 1){//1655
    	    now_sec = 0;
	    time(&current);
	    info = localtime( &current );
		
	    strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
	    now_hour = atoi(string_now_hour);	
	    strftime(string_now_min,sizeof(string_now_min),"%M",info);
	    now_min = atoi(string_now_min);	
	    strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
	    now_sec = atoi(string_now_sec);	
	    now_sec+=(60*now_min)+(60*60*now_hour);//zzzz
	    
	    if(enumeration_start_time != 0){
	    printf(" now_time %s:%s:%s !\n",string_now_hour,string_now_min,string_now_sec);
	    printf("now_sec %d enumeration_start_time %d\n",now_sec,enumeration_start_time);
	    printf("execute time %d \n",((NUM_OF_SELECT_PATTERN_TIMES*50)+offpeak_to_peak_time) );
	    }
	    if(now_sec > enumeration_start_time ){
            
	    if((now_sec-enumeration_start_time) >= ((NUM_OF_SELECT_PATTERN_TIMES*50)+offpeak_to_peak_time) && enumeration_start_time != 0 && change_time_period_signal != 1){//(NUM_OF_SELECT_PATTERN_TIMES*50)
	    
	    program_over(1);
	    program_terminate_signal =1;
	    }
	    }
	    else if(now_sec < enumeration_start_time ){
	    
	    if((now_sec+(86400-enumeration_start_time) ) >= ((NUM_OF_SELECT_PATTERN_TIMES*50)+offpeak_to_peak_time) && enumeration_start_time != 0 && change_time_period_signal != 1){
	    
	    program_over(1);
	    program_terminate_signal =1;
	    }
	    }
	    
	    
	    if(now_sec > enumeration_start_time ){
            
	    if((now_sec-enumeration_start_time) >= (NUM_OF_SELECT_PATTERN_TIMES*6) && enumeration_start_time != 0){
	    write_botmaster_command_signal = 1;
	    
	    }
	    }
	    else if(now_sec < enumeration_start_time ){
	    
	    if((now_sec+(86400-enumeration_start_time) ) >= (NUM_OF_SELECT_PATTERN_TIMES*6) && enumeration_start_time != 0){
	    write_botmaster_command_signal = 1;
	    
	    }
	    }
	    if(write_botmaster_command_signal == 1 && write_botmaster_command_time == 0){
	    write_botmaster_command_time=1;
	    
	    for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
	    //printf("servent %ld\n",i);
	    strcpy(servent_command_buffer[i][servent_command_buffer_pointer[i]].content,"f007;;9999;9999;2022.10.29.86400;7200");
	    puts(servent_command_buffer[i][servent_command_buffer_pointer[i]].content);
	    servent_command_buffer_pointer[i]++;
	    servent_command_analysis(i,servent_command_buffer[i][servent_command_buffer_pointer[i]-1].content);
	    }
	    
	    for(i=0,f=1;i<3,f<=3;i++,f++){
	    
		    
		    if(f==1){
		    sprintf(path, "command%d.txt", f); 
		    file = fopen(path, "w");
		    if(!f){
			printf("data not exist");
			system("PAUSE");
			pthread_exit(NULL);
		    }
		    strcpy(write_data,"f007;;9999;9999;2022.10.29.86400;7200");
		    fwrite( write_data, 1,strlen(write_data), file );
    		    //puts(write_data);
    		    fclose(file);
		    }
		    if(f==2){
		    sprintf(path, "command%d.txt", f); 
		    file = fopen(path, "w");
		    if(!f){
			printf("data not exist");
			system("PAUSE");
			pthread_exit(NULL);
		    }
		     
		    strcpy(write_data,"[][f007][9999][9999][2022.10.29.86400][7200]");
		    fwrite( write_data, 1,strlen(write_data), file );
    		    //puts(write_data);
    		    fclose(file);
		    }
		    if(f==3){
		    sprintf(path, "command%d.txt", f); 
		    file = fopen(path, "w");
		    if(!f){
			printf("data not exist");
			system("PAUSE");
			pthread_exit(NULL);
		    }
		    strcpy(write_data,"2022.10.29.86400||7200||||f007||9999||9999");
		    fwrite( write_data, 1,strlen(write_data), file );
    		    //puts(write_data);
    		    fclose(file);
		    }
		    
		    
	    }    
	    }
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
		    file_data[i][j][strlen(file_data[i][j])] = '\0';
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
void *data_record_func(){
    int record_times=1,last_time_record=0;
    char record_data[1024];
    int now_hour,now_min,now_sec1;
    char string_now_hour[1024],string_now_min[1024],string_now_sec[1024];
    char strings_start_hour[1024],strings_start_min[1024],strings_start_sec[1024];
    int start_hour,start_min,start_sec;  
    long i=0;
    
  /*  time(&current);
   info = localtime( &current );
   strftime(strings_start_hour,sizeof(strings_start_hour),"%H",info);
   start_hour = atoi(strings_start_hour);	
   strftime(strings_start_min,sizeof(strings_start_min),"%M",info);
   start_min = atoi(strings_start_min);	
   strftime(strings_start_sec,sizeof(strings_start_sec),"%S",info);
   start_sec = atoi(strings_start_sec);
   start_sec+=(60*start_min)+(60*60*start_hour);*/
    
    FILE* f;
    f = fopen("data_record.txt" , "w");
    if(!f){
    printf("data not exist");
    system("PAUSE");
    pthread_exit(NULL);
    }
    
    while(data_record_terminate_signal != 1){
    
    time(&current);
    info = localtime( &current );
   
	
    strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
    now_hour = atoi(string_now_hour);
    strftime(string_now_min,sizeof(string_now_min),"%M",info);
    now_min = atoi(string_now_min);	
    strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
    now_sec1 = atoi(string_now_sec);	
    
    
    now_sec1+=(60*now_min)+(60*60*now_hour);
    servent_latency_num_temp=0;
    crawler_latency_num_temp=0;
    sensor_latency_num_temp=0;
    	for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
				    
		if(servent_eliminate_signal[i] != 1 && servent_latency_signal[i] == 1 && servent[i].fake_signal != 1 && servent[i].sensor_signal != 1){  
			servent_latency_num_temp++;		
		}
		else if(servent_eliminate_signal[i] != 1 && servent_latency_signal[i] == 1 && servent[i].fake_signal == 1 && servent[i].sensor_signal != 1){  
			crawler_latency_num_temp++;		
		}
		else if(servent_eliminate_signal[i] != 1 && servent_latency_signal[i] == 1 && servent[i].fake_signal != 1 && servent[i].sensor_signal == 1){  
			sensor_latency_num_temp++;		
		}
	}
	
    if(change_time_period_signal != 1 ){
	    if(now_sec1 > last_time_record ){
	    if((now_sec1 - last_time_record ) >= (NUM_OF_SELECT_PATTERN_TIMES*2) ){//zzzz NUM_OF_SELECT_PATTERN_TIMES*2
	    printf(" now_sec > last_time_record !\n");
	    printf(" now_sec %d last_time_record %d !\n",now_sec1,last_time_record);
	    //printf(" now_time %s:%s:%s !\n",string_now_hour,string_now_min,string_now_sec);
	    //printf(" timesss %s!\n",timesss);
	    
	    last_time_record  = now_sec1;
	    sprintf(record_data, "%d:vc.%d:vrc.%d:vs.%d:now_sec.%d \n",record_times ,vc ,vrc ,vs ,now_sec1  );
	    record_times++;
	    fwrite( record_data, 1,strlen(record_data), f );
	    puts(record_data);
	    latency_num_record_array[latency_num_record_pointer].servent_latency_num=servent_latency_num_temp;
	    latency_num_record_array[latency_num_record_pointer].crawler_latency_num=crawler_latency_num_temp;
	    latency_num_record_array[latency_num_record_pointer].sensor_latency_num=sensor_latency_num_temp;
	    latency_num_record_pointer++;
	    }
	    }
	    
	    else if(now_sec1 < last_time_record ){
	    if((now_sec1+(86400-last_time_record) ) >= (NUM_OF_SELECT_PATTERN_TIMES*2) ){
	    //printf(" now_time %s:%s:%s !\n",string_now_hour,string_now_min,string_now_sec);
	    //printf(" timesss %s!\n",timesss);
	    printf(" now_sec < last_time_record !\n");
	    printf(" now_sec %d last_time_record %d !\n",now_sec1,last_time_record);
	    //printf(" now_sec %d last_time_record %d !\n",now_sec,last_time_record);
	    
	    last_time_record  = now_sec1;
	    sprintf(record_data, "%d:vc.%d:vrc.%d:vs.%d:now_sec.%d \n",record_times ,vc ,vrc ,vs ,now_sec1  );
	    record_times++;
	    fwrite( record_data, 1,strlen(record_data), f );
	    puts(record_data);
	    latency_num_record_array[latency_num_record_pointer].servent_latency_num=servent_latency_num_temp;
	    latency_num_record_array[latency_num_record_pointer].crawler_latency_num=crawler_latency_num_temp;
	    latency_num_record_array[latency_num_record_pointer].sensor_latency_num=sensor_latency_num_temp;
	    latency_num_record_pointer++;	
	    }
	    }
    }
    else if(change_time_period_signal == 1 ){
    	    printf("stop record !\n");
    	    last_time_record  = now_sec1;
    	    
    }
    sleep(1);
    }
    sprintf(record_data, "%d:vc.%d:vrc.%d:vs.%d:now_sec.%d \n",record_times ,vc ,vrc ,vs ,now_sec1  );
    fwrite( record_data, 1,strlen(record_data), f );
    puts(record_data);
    fclose(f);
    
    printf(" data_record_thread terminated !\n");
    pthread_exit(NULL);





}
void *infect_and_inject_enumeration_thread_func(){
	long i=0,j=0,k=0,rc=0;
	int infect_probability=0;
	int last_time_infect=0;
	int num_store_pool=0,add_num=0;
	int now_hour,now_min,now_sec;
    	char string_now_hour[1024],string_now_min[1024],string_now_sec[1024];
    	char record_data[100];
    	int crawler_in_servent_peerlist_reputation_value_negative_one=0,
    	crawler_in_servent_peerlist_reputation_value_zero=0,
    	crawler_in_servent_peerlist_reputation_value_one=0,
    	crawler_in_servent_peerlist_reputation_value_two=0,
    	crawler_in_servent_peerlist_reputation_value_three=0,
    	crawler_in_servent_peerlist_reputation_value_four=0,
    	crawler_in_servent_peerlist_reputation_value_five=0,
    	
    	sensor_in_servent_peerlist_reputation_value_negative_one=0,
    	sensor_in_servent_peerlist_reputation_value_zero=0,
    	sensor_in_servent_peerlist_reputation_value_one=0,
    	sensor_in_servent_peerlist_reputation_value_two=0,
    	sensor_in_servent_peerlist_reputation_value_three=0,
    	sensor_in_servent_peerlist_reputation_value_four=0,
    	sensor_in_servent_peerlist_reputation_value_five=0,
    	
    	crawler_in_client_peerlist_reputation_value_negative_one=0,
    	crawler_in_client_peerlist_reputation_value_zero=0,
    	crawler_in_client_peerlist_reputation_value_one=0,
    	crawler_in_client_peerlist_reputation_value_two=0,
    	crawler_in_client_peerlist_reputation_value_three=0,
    	crawler_in_client_peerlist_reputation_value_four=0,
    	crawler_in_client_peerlist_reputation_value_five=0,
    	
    	sensor_in_client_peerlist_reputation_value_negative_one=0,
    	sensor_in_client_peerlist_reputation_value_zero=0,    	
    	sensor_in_client_peerlist_reputation_value_one=0,
    	sensor_in_client_peerlist_reputation_value_two=0,
    	sensor_in_client_peerlist_reputation_value_three=0,
    	sensor_in_client_peerlist_reputation_value_four=0,
    	sensor_in_client_peerlist_reputation_value_five=0,
    	
    	servent_in_servent_peerlist_reputation_value_negative_one=0,
    	servent_in_servent_peerlist_reputation_value_zero=0,
    	servent_in_servent_peerlist_reputation_value_one=0,
    	servent_in_servent_peerlist_reputation_value_two=0,
    	servent_in_servent_peerlist_reputation_value_three=0,
    	servent_in_servent_peerlist_reputation_value_four=0,
    	servent_in_servent_peerlist_reputation_value_five=0,
    	
    	servent_in_client_peerlist_reputation_value_negative_one=0,
    	servent_in_client_peerlist_reputation_value_zero=0,
    	servent_in_client_peerlist_reputation_value_one=0,
    	servent_in_client_peerlist_reputation_value_two=0,
    	servent_in_client_peerlist_reputation_value_three=0,
    	servent_in_client_peerlist_reputation_value_four=0,
    	servent_in_client_peerlist_reputation_value_five=0;
    	
    	int crawler_in_servent_peerlist_num=0,sensor_in_servent_peerlist_num=0,sensor_in_client_peerlist_num=0,crawler_in_client_peerlist_num=0;
    	FILE* f;
	/*f = fopen("request_time_record.txt" , "w");
	if(!f){
	printf("data not exist");
	system("PAUSE");
	pthread_exit(NULL);
	}*/
	
    	/*while(infect_terminate_signal != 1 && servent_bot_num_now < 5000){
    	now_sec = 0 ;
    	
    	time(&current);
	info = localtime( &current );
		
	strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
	now_hour = atoi(string_now_hour);	
	strftime(string_now_min,sizeof(string_now_min),"%M",info);
	now_min = atoi(string_now_min);	
	strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
	now_sec = atoi(string_now_sec);	
	now_sec+=(60*now_min)+(60*60*now_hour);
	if(now_sec > last_time_infect ){
	
	if((now_sec - last_time_infect ) >= 5  ){ 
	printf(" now_time %s:%s:%s !\n",string_now_hour,string_now_min,string_now_sec); //  NUM_OF_SELECT_PATTERN_TIMES/2 
	printf("now_sec > last_time_infect!!!!!!!!\n");
		last_time_infect  = now_sec;
		printf(" servent_bot_num_now = %d !\n",servent_bot_num_now);
		for (i = 0; i < servent_bot_num_now; i++) {
		infect_probability=rand() % 2;
		if(infect_probability == 1){
		add_num++;
		}
		
		}
		
	
	}
	}
	
	else if(now_sec < last_time_infect ){
	
	if((now_sec+(86400-last_time_infect) ) >= 5  ){
	printf(" now_time %s:%s:%s !\n",string_now_hour,string_now_min,string_now_sec);
	printf("now_sec < last_time_infect!!!!!!!!\n");
		last_time_infect  = now_sec;
		infect_probability=rand() % 2;
		printf(" servent_bot_num_now = %d !\n",servent_bot_num_now);
		for (i = 0; i < servent_bot_num_now; i++) {
		infect_probability=rand() % 2;
		if(infect_probability == 1){
		add_num++;
		}
		
		}
		
	
	}
	}
	
	if(add_num != 0){
	servent_bot_num_now = servent_bot_num_now+add_num+num_store_pool;
	printf(" servent_bot_num_now = %d !\n",servent_bot_num_now);
	servent_thread_num_last_time = servent_thread_num_now;
	client_thread_num_last_time = client_thread_num_now;
	//printf(" servent_thread_num_last_time = %d !\n",servent_thread_num_last_time);
	
	servent_thread_num_now = servent_bot_num_now/50;
	client_thread_num_now = 3*servent_thread_num_now;
	if(servent_thread_num_now > 5000/50){
	servent_thread_num_now = 5000/50;
	}
	if(client_thread_num_now > 15000/50){
	client_thread_num_now = 15000/50;
	}
	num_store_pool = servent_bot_num_now%50;
	
	for (j = servent_thread_num_last_time*50; j < servent_thread_num_now*50; j++){
	if(j<5000){
	init_servent_peer_list(j);
	}
	}
	for (i = servent_thread_num_last_time; i < servent_thread_num_now; i++) {
	if(i<5000/50){
		
	printf(" create servent_thread [%d] !\n",i);
	rc = pthread_create(&servent_threads[i], &attr, servent_thread_func, (void *)i);
        
	if (rc) {
	printf("ERORR; return code from pthread_create() is %s\n", strerror(rc));
	exit(EXIT_FAILURE);
	}
		
		
		
	}
		
		
	}
	for (j = client_thread_num_last_time*50; j < client_thread_num_now*50; j++){
	if(j<15000){
	init_client_master(j);
	}
	}
	for (k = client_thread_num_last_time; k < client_thread_num_now; k++) {
	if(k<15000/50){
		
	printf(" create client_thread [%d] !\n",k);
	rc = pthread_create(&client_threads[k], &attr, client_thread_func, (void *)k);

	if (rc) {
	printf("ERORR; return code from pthread_create() is %s\n", strerror(rc));
	exit(EXIT_FAILURE);
	}
		
		
		
	}
		
		
	}
	
	
	
	add_num = 0;
	}	
		
    	
		
		
		
	sleep(1);
	}*/
	
	printf("inject start !\n");
	inject_signal=1;
	init_fake_servent_and_sensor_peer_list();
	int probability=0;
	for(k=0;k<NUM_SERVENT_BOTS;k++){
		probability = rand() % 10;
		for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
				for (j = 0; j < NUM_SERVENT_PEER; j++){
					if(servent[servent_peer_list[i][j].peer_id].sensor_signal != 1 && servent[servent_peer_list[i][j].peer_id].fake_signal != 1 ){
						if( servent_peer_list[i][j].peer_id == k && probability>=0 && probability<=1){
					   	servent_peer_list[i][j].reputation_value = 5;
					   	}
					   	if( servent_peer_list[i][j].peer_id == k && probability>=2 && probability<=3){
					   	servent_peer_list[i][j].reputation_value = 4;
					   	}
					   	if( servent_peer_list[i][j].peer_id == k && probability>=4 && probability<=5){
					   	servent_peer_list[i][j].reputation_value = 3;
					   	}
					   	if( servent_peer_list[i][j].peer_id == k && probability>=6 && probability<=7){
					   	servent_peer_list[i][j].reputation_value = 2;
					   	}
					   	if( servent_peer_list[i][j].peer_id == k && probability>=8 && probability<=8){
					   	servent_peer_list[i][j].reputation_value = 1;
					   	}
					   	if( servent_peer_list[i][j].peer_id == k && probability>=9 && probability<=9){
					   	servent_peer_list[i][j].reputation_value = 0;
					   	}
					}
					
					
					
				}
		}
		for(i=0;i<NUM_CLIENT_BOTS;i++){
				for (j = 0; j < NUM_SERVENT_PEER; j++){
					if(servent[client_master[i][j].master_id].sensor_signal != 1 && servent[client_master[i][j].master_id].fake_signal != 1){
						if( client_master[i][j].master_id == k && probability>=0 && probability<=1){
					   	client_master[i][j].reputation_value = 5;
					   	}
					   	if( client_master[i][j].master_id == k && probability>=2 && probability<=3){
					   	client_master[i][j].reputation_value = 4;
					   	}
					   	if( client_master[i][j].master_id == k && probability>=4 && probability<=5){
					   	client_master[i][j].reputation_value = 3;
					   	}
					   	if( client_master[i][j].master_id == k && probability>=6 && probability<=7){
					   	client_master[i][j].reputation_value = 2;
					   	}
					   	if( client_master[i][j].master_id == k && probability>=8 && probability<=8){
					   	client_master[i][j].reputation_value = 1;
					   	}
					   	if( client_master[i][j].master_id == k && probability>=9 && probability<=9){
					   	client_master[i][j].reputation_value = 0;
					   	}
					}
					
				
				
				
				}
		}
	}
	k=0;
	for(k=NUM_SERVENT_BOTS;k<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;k++){
		probability = rand() % 10;
		for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
				for (j = 0; j < NUM_SERVENT_PEER; j++){
					
					if( servent_peer_list[i][j].peer_id == k && probability>=0 && probability<=0){
				   	servent_peer_list[i][j].reputation_value = 5;
				   	}
				   	if( servent_peer_list[i][j].peer_id == k && probability>=1 && probability<=1){
				   	servent_peer_list[i][j].reputation_value = 4;
				   	}
				   	if( servent_peer_list[i][j].peer_id == k && probability>=2 && probability<=2){
				   	servent_peer_list[i][j].reputation_value = 3;
				   	}
				   	if( servent_peer_list[i][j].peer_id == k && probability>=3 && probability<=4){
				   	servent_peer_list[i][j].reputation_value = 2;
				   	}
				   	if( servent_peer_list[i][j].peer_id == k && probability>=5 && probability<=7){
				   	servent_peer_list[i][j].reputation_value = 1;
				   	}
				   	if( servent_peer_list[i][j].peer_id == k && probability>=8 && probability<=8){
				   	servent_peer_list[i][j].reputation_value = 0;
				   	}
				   	if( servent_peer_list[i][j].peer_id == k && probability>=9 && probability<=9){
				   	servent_peer_list[i][j].reputation_value = -1;
				   	}
					
					
				}
		}
		for(i=0;i<NUM_CLIENT_BOTS;i++){
				for (j = 0; j < NUM_SERVENT_PEER; j++){
					if( client_master[i][j].master_id == k && probability>=0 && probability<=0){
				   	client_master[i][j].reputation_value = 5;
				   	}
				   	if( client_master[i][j].master_id == k && probability>=1 && probability<=1){
				   	client_master[i][j].reputation_value = 4;
				   	}
				   	if( client_master[i][j].master_id == k && probability>=2 && probability<=2){
				   	client_master[i][j].reputation_value = 3;
				   	}
				   	if( client_master[i][j].master_id == k && probability>=3 && probability<=4){
				   	client_master[i][j].reputation_value = 2;
				   	}
				   	if( client_master[i][j].master_id == k && probability>=5 && probability<=7){
				   	client_master[i][j].reputation_value = 1;
				   	}
				   	if( client_master[i][j].master_id == k && probability>=8 && probability<=8){
				   	client_master[i][j].reputation_value = 0;
				   	}
				   	if( client_master[i][j].master_id == k && probability>=9 && probability<=9){
				   	client_master[i][j].reputation_value = -1;
				   	}
				
				
				
				}
		}
	}
	for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
				    
		
				    
				    
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				    	if(servent_peer_list[i][j].peer_id != -1 && servent[servent_peer_list[i][j].peer_id].fake_signal == 1){
						crawler_in_servent_peerlist_num++;
						
						break;
					
				    	}
				    	
				    	
				    }	
				    
				   
		
	}
	for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
				    
		
				    
				    
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				    	
				    	if(servent_peer_list[i][j].peer_id != -1 && servent[servent_peer_list[i][j].peer_id].sensor_signal == 1){
						sensor_in_servent_peerlist_num++;
						
						break;
				    	}
				    	
				    }	
				    
				   
		
	}
	for(i=0;i<NUM_CLIENT_BOTS;i++){
				    
				    
				    
				    
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				        if(client_master[i][j].master_id != -1 && servent[client_master[i][j].master_id].fake_signal == 1){
						crawler_in_client_peerlist_num++;
						break;
					}
					
				    }	
				    
				    
	}
	for(i=0;i<NUM_CLIENT_BOTS;i++){
				    
				    
				    
				    
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				        
					if(client_master[i][j].master_id != -1 && servent[client_master[i][j].master_id].sensor_signal == 1){
						sensor_in_client_peerlist_num++;
						break;
					}
				    }	
				    
				    
	}
	
	f = fopen("before_request_servent_peerlist_record.txt" , "w");
	if(!f){
	printf("data not exist");
	system("PAUSE");
	}
	for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
				    
				    if(servent_eliminate_signal[i]!= 1){
				    
				    //printf("peer list of servent %ld have:", i);
				    sprintf(record_data, "peer list of servent %ld have:",i  );
				    fwrite( record_data, 1,strlen(record_data), f );
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				    	if(servent_peer_list[i][j].peer_id != -1){
					    	if(servent[servent_peer_list[i][j].peer_id].fake_signal == 1 && servent_peer_list[i][j].reputation_value == -1){
					    	crawler_in_servent_peerlist_reputation_value_negative_one++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].fake_signal == 1 && servent_peer_list[i][j].reputation_value == 0){
					    	crawler_in_servent_peerlist_reputation_value_zero++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].fake_signal == 1 && servent_peer_list[i][j].reputation_value == 1){
					    	crawler_in_servent_peerlist_reputation_value_one++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].fake_signal == 1 && servent_peer_list[i][j].reputation_value == 2){
					    	crawler_in_servent_peerlist_reputation_value_two++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].fake_signal == 1 && servent_peer_list[i][j].reputation_value == 3){
					    	crawler_in_servent_peerlist_reputation_value_three++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].fake_signal == 1 && servent_peer_list[i][j].reputation_value == 4){
					    	crawler_in_servent_peerlist_reputation_value_four++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].fake_signal == 1 && servent_peer_list[i][j].reputation_value == 5){
					    	crawler_in_servent_peerlist_reputation_value_five++;
					    	}
					    	//-----
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal == 1 && servent_peer_list[i][j].reputation_value == -1){
					    	sensor_in_servent_peerlist_reputation_value_negative_one++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal == 1 && servent_peer_list[i][j].reputation_value == 0){
					    	sensor_in_servent_peerlist_reputation_value_zero++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal == 1 && servent_peer_list[i][j].reputation_value == 1){
					    	sensor_in_servent_peerlist_reputation_value_one++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal == 1 && servent_peer_list[i][j].reputation_value == 2){
					    	sensor_in_servent_peerlist_reputation_value_two++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal == 1 && servent_peer_list[i][j].reputation_value == 3){
					    	sensor_in_servent_peerlist_reputation_value_three++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal == 1 && servent_peer_list[i][j].reputation_value == 4){
					    	sensor_in_servent_peerlist_reputation_value_four++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal == 1 && servent_peer_list[i][j].reputation_value == 5){
					    	sensor_in_servent_peerlist_reputation_value_five++;
					    	}
					    	//------
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal != 1 && servent[servent_peer_list[i][j].peer_id].fake_signal != 1 
					    	&& servent_peer_list[i][j].reputation_value == -1){
					    	servent_in_servent_peerlist_reputation_value_negative_one++;
					    	
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal != 1 && servent[servent_peer_list[i][j].peer_id].fake_signal != 1 
					    	&& servent_peer_list[i][j].reputation_value == 0){
					    	servent_in_servent_peerlist_reputation_value_zero++;
					    	
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal != 1 && servent[servent_peer_list[i][j].peer_id].fake_signal != 1 
					    	&& servent_peer_list[i][j].reputation_value == 1){
					    	servent_in_servent_peerlist_reputation_value_one++;
					    	
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal != 1 && servent[servent_peer_list[i][j].peer_id].fake_signal != 1 
					    	&& servent_peer_list[i][j].reputation_value == 2){
					    	servent_in_servent_peerlist_reputation_value_two++;
					    	
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal != 1 && servent[servent_peer_list[i][j].peer_id].fake_signal != 1 
					    	&& servent_peer_list[i][j].reputation_value == 3){
					    	servent_in_servent_peerlist_reputation_value_three++;
					    	
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal != 1 && servent[servent_peer_list[i][j].peer_id].fake_signal != 1 
					    	&& servent_peer_list[i][j].reputation_value == 4){
					    	servent_in_servent_peerlist_reputation_value_four++;
					    	
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal != 1 && servent[servent_peer_list[i][j].peer_id].fake_signal != 1 
					    	&& servent_peer_list[i][j].reputation_value == 5){
					    	servent_in_servent_peerlist_reputation_value_five++;
					    	
					    	}
					    	
						//printf("%ld %ld  ", servent_peer_list[i][j].peer_id, servent_peer_list[i][j].reputation_value);
						sprintf(record_data, "%ld %ld  ", servent_peer_list[i][j].peer_id, servent_peer_list[i][j].reputation_value);
						fwrite( record_data, 1,strlen(record_data), f );
					}
				    }	
				    //puts("");
				    fwrite( "\n", 1,1, f );
				    }
	}	
	sprintf(record_data, "crawler_in_servent_peerlist_reputation_value_negative_one:%d  \n",crawler_in_servent_peerlist_reputation_value_negative_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_servent_peerlist_reputation_value_zero:%d  \n",crawler_in_servent_peerlist_reputation_value_zero );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_servent_peerlist_reputation_value_one:%d  \n",crawler_in_servent_peerlist_reputation_value_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_servent_peerlist_reputation_value_two:%d  \n",crawler_in_servent_peerlist_reputation_value_two );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_servent_peerlist_reputation_value_three:%d  \n",crawler_in_servent_peerlist_reputation_value_three );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_servent_peerlist_reputation_value_four:%d  \n",crawler_in_servent_peerlist_reputation_value_four );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_servent_peerlist_reputation_value_five:%d  \n",crawler_in_servent_peerlist_reputation_value_five );
	fwrite( record_data, 1,strlen(record_data), f );
	//-----
	sprintf(record_data, "sensor_in_servent_peerlist_reputation_value_negative_one:%d  \n",sensor_in_servent_peerlist_reputation_value_negative_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_servent_peerlist_reputation_value_zero:%d  \n",sensor_in_servent_peerlist_reputation_value_zero );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_servent_peerlist_reputation_value_one:%d  \n",sensor_in_servent_peerlist_reputation_value_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_servent_peerlist_reputation_value_two:%d  \n",sensor_in_servent_peerlist_reputation_value_two );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_servent_peerlist_reputation_value_three:%d  \n",sensor_in_servent_peerlist_reputation_value_three );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_servent_peerlist_reputation_value_four:%d  \n",sensor_in_servent_peerlist_reputation_value_four );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_servent_peerlist_reputation_value_five:%d  \n",sensor_in_servent_peerlist_reputation_value_five );
	fwrite( record_data, 1,strlen(record_data), f );
	//--------
	sprintf(record_data, "servent_in_servent_peerlist_reputation_value_negative_one:%d  \n",servent_in_servent_peerlist_reputation_value_negative_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_servent_peerlist_reputation_value_zero:%d  \n",servent_in_servent_peerlist_reputation_value_zero );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_servent_peerlist_reputation_value_one:%d  \n",servent_in_servent_peerlist_reputation_value_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_servent_peerlist_reputation_value_two:%d  \n",servent_in_servent_peerlist_reputation_value_two );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_servent_peerlist_reputation_value_three:%d  \n",servent_in_servent_peerlist_reputation_value_three );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_servent_peerlist_reputation_value_four:%d  \n",servent_in_servent_peerlist_reputation_value_four );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_servent_peerlist_reputation_value_five:%d  \n",servent_in_servent_peerlist_reputation_value_five );
	fwrite( record_data, 1,strlen(record_data), f );
	
	sprintf(record_data,"crawler_in_servent_peerlist_num:%d !\n",crawler_in_servent_peerlist_num);
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data,"sensor_in_servent_peerlist_num:%d !\n",sensor_in_servent_peerlist_num);
	fwrite( record_data, 1,strlen(record_data), f );
	fclose(f);
	
	f = fopen("before_request_client_peerlist_record.txt" , "w");
	if(!f){
	printf("data not exist");
	system("PAUSE");
	}
	for(i=0;i<NUM_CLIENT_BOTS;i++){
				    
				    
				    
				    sprintf(record_data, "peer list of client %ld have:",i  );
				    fwrite( record_data, 1,strlen(record_data), f );
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				    	if(client_master[i][j].master_id != -1){
						if(servent[client_master[i][j].master_id].fake_signal == 1 && client_master[i][j].reputation_value == -1){
					    	crawler_in_client_peerlist_reputation_value_negative_one++;
					    	}
					    	else if(servent[client_master[i][j].master_id].fake_signal == 1 && client_master[i][j].reputation_value == 0){
					    	crawler_in_client_peerlist_reputation_value_zero++;
					    	}
					    	else if(servent[client_master[i][j].master_id].fake_signal == 1 && client_master[i][j].reputation_value == 1){
					    	crawler_in_client_peerlist_reputation_value_one++;
					    	}
					    	else if(servent[client_master[i][j].master_id].fake_signal == 1 && client_master[i][j].reputation_value == 2){
					    	crawler_in_client_peerlist_reputation_value_two++;
					    	}
					    	else if(servent[client_master[i][j].master_id].fake_signal == 1 && client_master[i][j].reputation_value == 3){
					    	crawler_in_client_peerlist_reputation_value_three++;
					    	}
					    	else if(servent[client_master[i][j].master_id].fake_signal == 1 && client_master[i][j].reputation_value == 4){
					    	crawler_in_client_peerlist_reputation_value_four++;
					    	}
					    	else if(servent[client_master[i][j].master_id].fake_signal == 1 && client_master[i][j].reputation_value == 5){
					    	crawler_in_client_peerlist_reputation_value_five++;
					    	}
					    	//-----
					    	else if(servent[client_master[i][j].master_id].sensor_signal == 1 && client_master[i][j].reputation_value == -1){
					    	sensor_in_client_peerlist_reputation_value_negative_one++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal == 1 && client_master[i][j].reputation_value == 0){
					    	sensor_in_client_peerlist_reputation_value_zero++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal == 1 && client_master[i][j].reputation_value == 1){
					    	sensor_in_client_peerlist_reputation_value_one++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal == 1 && client_master[i][j].reputation_value == 2){
					    	sensor_in_client_peerlist_reputation_value_two++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal == 1 && client_master[i][j].reputation_value == 3){
					    	sensor_in_client_peerlist_reputation_value_three++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal == 1 && client_master[i][j].reputation_value == 4){
					    	sensor_in_client_peerlist_reputation_value_four++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal == 1 && client_master[i][j].reputation_value == 5){
					    	sensor_in_client_peerlist_reputation_value_five++;
					    	}
						//-------
						else if(servent[client_master[i][j].master_id].sensor_signal != 1 && servent[client_master[i][j].master_id].fake_signal != 1 
						&& client_master[i][j].reputation_value == -1){
					    	servent_in_client_peerlist_reputation_value_negative_one++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal != 1 && servent[client_master[i][j].master_id].fake_signal != 1 
					    	&& client_master[i][j].reputation_value == 0){
					    	servent_in_client_peerlist_reputation_value_zero++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal != 1 && servent[client_master[i][j].master_id].fake_signal != 1 
					    	&& client_master[i][j].reputation_value == 1){
					    	servent_in_client_peerlist_reputation_value_one++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal != 1 && servent[client_master[i][j].master_id].fake_signal != 1 
					    	&& client_master[i][j].reputation_value == 2){
					    	servent_in_client_peerlist_reputation_value_two++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal != 1 && servent[client_master[i][j].master_id].fake_signal != 1 
					    	&& client_master[i][j].reputation_value == 3){
					    	servent_in_client_peerlist_reputation_value_three++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal != 1 && servent[client_master[i][j].master_id].fake_signal != 1 
					    	&& client_master[i][j].reputation_value == 4){
					    	servent_in_client_peerlist_reputation_value_four++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal != 1 && servent[client_master[i][j].master_id].fake_signal != 1 
					    	&& client_master[i][j].reputation_value == 5){
					    	servent_in_client_peerlist_reputation_value_five++;
					    	}
					    	
						sprintf(record_data, "%ld %ld  ", client_master[i][j].master_id, client_master[i][j].reputation_value);
						fwrite( record_data, 1,strlen(record_data), f );
					}
				    }	
				    //puts("");
				    fwrite( "\n", 1,1, f );
				    
	}	
	
	sprintf(record_data, "crawler_in_client_peerlist_reputation_value_negative_one:%d  \n",crawler_in_client_peerlist_reputation_value_negative_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_client_peerlist_reputation_value_zero:%d  \n",crawler_in_client_peerlist_reputation_value_zero );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_client_peerlist_reputation_value_one:%d  \n",crawler_in_client_peerlist_reputation_value_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_client_peerlist_reputation_value_two:%d  \n",crawler_in_client_peerlist_reputation_value_two );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_client_peerlist_reputation_value_three:%d  \n",crawler_in_client_peerlist_reputation_value_three );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_client_peerlist_reputation_value_four:%d  \n",crawler_in_client_peerlist_reputation_value_four );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_client_peerlist_reputation_value_five:%d  \n",crawler_in_client_peerlist_reputation_value_five );
	fwrite( record_data, 1,strlen(record_data), f );
	//-----
	sprintf(record_data, "sensor_in_client_peerlist_reputation_value_negative_one:%d  \n",sensor_in_client_peerlist_reputation_value_negative_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_client_peerlist_reputation_value_zero:%d  \n",sensor_in_client_peerlist_reputation_value_zero );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_client_peerlist_reputation_value_one:%d  \n",sensor_in_client_peerlist_reputation_value_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_client_peerlist_reputation_value_two:%d  \n",sensor_in_client_peerlist_reputation_value_two );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_client_peerlist_reputation_value_three:%d  \n",sensor_in_client_peerlist_reputation_value_three );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_client_peerlist_reputation_value_four:%d  \n",sensor_in_client_peerlist_reputation_value_four );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_client_peerlist_reputation_value_five:%d  \n",sensor_in_client_peerlist_reputation_value_five );
	fwrite( record_data, 1,strlen(record_data), f );
	//-----
	sprintf(record_data, "servent_in_client_peerlist_reputation_value_negative_one:%d  \n",servent_in_client_peerlist_reputation_value_negative_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_client_peerlist_reputation_value_zero:%d  \n",servent_in_client_peerlist_reputation_value_zero );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_client_peerlist_reputation_value_one:%d  \n",servent_in_client_peerlist_reputation_value_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_client_peerlist_reputation_value_two:%d  \n",servent_in_client_peerlist_reputation_value_two );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_client_peerlist_reputation_value_three:%d  \n",servent_in_client_peerlist_reputation_value_three );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_client_peerlist_reputation_value_four:%d  \n",servent_in_client_peerlist_reputation_value_four );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_client_peerlist_reputation_value_five:%d  \n",servent_in_client_peerlist_reputation_value_five );
	fwrite( record_data, 1,strlen(record_data), f );
	
	sprintf(record_data,"crawler_in_client_peerlist_num:%d !\n",crawler_in_client_peerlist_num);
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data,"sensor_in_client_peerlist_num:%d !\n",sensor_in_client_peerlist_num);
	fwrite( record_data, 1,strlen(record_data), f );
	fclose(f);
	
	abc_signal=1;
	
	for (i = 0; i < (NUM_FAKE_SERVENT_BOTS/5); i++) { // 
				
	pthread_attr_init(&attr);       
    	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 
	rc = pthread_create(&fake_servent_threads[i], &attr, fake_servent_thread_func, (void *)(i*5));// 
	if (rc) {
	printf("ERORR; return code from pthread_create() is %s\n", strerror(rc));
	exit(EXIT_FAILURE);
	}
				
	}
	
	for (i = 0; i < (NUM_SENSOR/5); i++) { // 
				
	pthread_attr_init(&attr);       
    	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 
	rc = pthread_create(&sensor_threads[i], &attr, sensor_thread_func, (void *)(i*5));// 
	if (rc) {
	printf("ERORR; return code from pthread_create() is %s\n", strerror(rc));
	exit(EXIT_FAILURE);
	}
				
	}
	
	/*
	servent_thread_num_last_time = servent_thread_num_now;
	client_thread_num_last_time = client_thread_num_now;
	
	for (j = (servent_thread_num_last_time*50)+100; j < (servent_thread_num_now*50)+350; j++){
	
	init_servent_peer_list(j);
	
	}
	for (i = servent_thread_num_last_time+(100/50); i < servent_thread_num_now+(350/50); i++) {//100 is fake_servent_number
	
		
	printf(" create servent_thread [%d] !\n",i);
	rc = pthread_create(&servent_threads[i], &attr, servent_thread_func, (void *)i);
        
	if (rc) {
	printf("ERORR; return code from pthread_create() is %s\n", strerror(rc));
	exit(EXIT_FAILURE);
	}
		
		
		
	
		
		
	}
	
	for (j = client_thread_num_last_time*50; j < (client_thread_num_now*50)+750; j++){
	
	init_client_master(j);
	
	}
	for (k = client_thread_num_last_time; k < client_thread_num_now+(750/50); k++) {
	
		
	printf(" create client_thread [%d] !\n",k);
	rc = pthread_create(&client_threads[k], &attr, client_thread_func, (void *)k);

	if (rc) {
	printf("ERORR; return code from pthread_create() is %s\n", strerror(rc));
	exit(EXIT_FAILURE);
	}
		
	
	}
	
	*/
	
	for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
				request_time[i]=-25;
		}
	request_time_largest = -25;   	
	
	time(&current);
	info = localtime( &current );
		
	strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
	now_hour = atoi(string_now_hour);	
	strftime(string_now_min,sizeof(string_now_min),"%M",info);
	now_min = atoi(string_now_min);	
	strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
	now_sec = atoi(string_now_sec);	
	now_sec+=(60*now_min)+(60*60*now_hour);
	
	request_time_last_record_time = now_sec;
	printf(" request_time_last_record_time %s:%s:%s !\n",string_now_hour,string_now_min,string_now_sec);
	enumeration_signal = 0;
	all_servent_try_to_turn_latency_signal = 1;
	while(request_time_record_terminate_signal != 1 && enumeration_signal != 1){
	//zzzz
	time(&current);
	info = localtime( &current );
		
	strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
	now_hour = atoi(string_now_hour);	
	strftime(string_now_min,sizeof(string_now_min),"%M",info);
	now_min = atoi(string_now_min);	
	strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
	now_sec = atoi(string_now_sec);	
	now_sec+=(60*now_min)+(60*60*now_hour);
	servent_latency_num_temp=0;
        crawler_latency_num_temp=0;
        sensor_latency_num_temp=0;
    	for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
				    
		if(servent_eliminate_signal[i] != 1 && servent_latency_signal[i] == 1 && servent[i].fake_signal != 1 && servent[i].sensor_signal != 1){  
			servent_latency_num_temp++;		
		}
		else if(servent_eliminate_signal[i] != 1 && servent_latency_signal[i] == 1 && servent[i].fake_signal == 1 && servent[i].sensor_signal != 1){  
			crawler_latency_num_temp++;		
		}
		else if(servent_eliminate_signal[i] != 1 && servent_latency_signal[i] == 1 && servent[i].fake_signal != 1 && servent[i].sensor_signal == 1){  
			sensor_latency_num_temp++;		
		}
	}
	for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
		if(request_time[i] > request_time_largest){
			request_time_largest = request_time[i];
		}
	}
	
	//printf(" now_time %s:%s:%s now_sec %d!\n",string_now_hour,string_now_min,string_now_sec,now_sec); 
	if(now_sec > request_time_last_record_time ){
	
	if((now_sec - request_time_last_record_time ) >= (NUM_OF_SELECT_PATTERN_TIMES*2) ){//
	request_hour++;
	
	printf("request_time_largest:%d !\n",request_time_largest);
	request_time_record_array[request_time_record_pointer] = request_time_largest;
	request_time_record_pointer++;
	/*sprintf(record_data, "request_time.%d:now_sec.%d !\n",request_time ,now_sec  );
	puts(record_data);
	fwrite( record_data, 1,strlen(record_data), f );*/
	
	if(request_time_largest >= 1000 && all_servent_try_to_turn_latency_signal == 1){//
	enumeration_signal = 1;
	printf("enumeration_start !!!!!!!!!!!!!!!\n");
	}
	if(request_time_largest >= 1000 && all_servent_try_to_turn_latency_signal == 0){
		printf("request_time_last_record_time %s:%s:%s !\n",string_now_hour,string_now_min,string_now_sec);
		request_time_last_record_time = now_sec;
		request_time_largest=-25;
		for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
				request_time[i]=-25;
		}
		/*for (i = 0; i < NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR; i++) {
			if(servent_latency_signal[i] == 0){
				probability = rand() % 10;
				if(probability>=1 && probability<=2){
					servent_latency_signal[i] = 1;
					servent_trust_threshold[i]=0;
					servent_already_turn_to_latency_signal[i]=1;
				}
			}
		
		}
		for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
			for (j = 0; j < NUM_SERVENT_PEER; j++){
				
				if( servent_peer_list[i][j].peer_id != -1 && servent_latency_signal[servent_peer_list[i][j].peer_id] == 1){//
			   	servent_peer_list[i][j].reputation_value = 5;

			   	}
				
				
			}
	   	}*/
	   	
	   	
	   	
   		all_servent_try_to_turn_latency_signal = 1;
	}
	else if(request_time_largest < 1000){
	printf("request_time_last_record_time %s:%s:%s !\n",string_now_hour,string_now_min,string_now_sec);
	request_time_last_record_time = now_sec;	
	request_time_largest=-25;
		for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
				request_time[i]=-25;
		}
	
	
	}
	latency_num_record_array[latency_num_record_pointer].servent_latency_num=servent_latency_num_temp;
	latency_num_record_array[latency_num_record_pointer].crawler_latency_num=crawler_latency_num_temp;
	latency_num_record_array[latency_num_record_pointer].sensor_latency_num=sensor_latency_num_temp;
	latency_num_record_pointer++;
	
	}
	
	}
	
	else if(now_sec < request_time_last_record_time ){
	
	if((now_sec+(86400-request_time_last_record_time) ) >= (NUM_OF_SELECT_PATTERN_TIMES*2)  ){
	request_hour++;
	printf("request_time_largest:%d !\n",request_time_largest);
	request_time_record_array[request_time_record_pointer] = request_time_largest;
	request_time_record_pointer++;
	/*sprintf(record_data, "request_time.%d:now_sec.%d !\n",request_time ,now_sec  );
	puts(record_data);
	fwrite( record_data, 1,strlen(record_data), f );*/
	if(request_time_largest >= 1000 && all_servent_try_to_turn_latency_signal == 1 ){//
	enumeration_signal = 1;
	printf("enumeration_start !!!!!!!!!!!!!!!\n");
	}
	if(request_time_largest >= 1000 && all_servent_try_to_turn_latency_signal == 0){
		
		printf("request_time_last_record_time %s:%s:%s !\n",string_now_hour,string_now_min,string_now_sec);
		request_time_last_record_time = now_sec;
		request_time_largest=-25;
		for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
				request_time[i]=-25;
		}
		/*for (i = 0; i < NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR; i++) {
			if(servent_latency_signal[i] == 0){
				probability = rand() % 10;
				if(probability>=1 && probability<=2){
					servent_latency_signal[i] = 1;
					servent_trust_threshold[i]=0;
					servent_already_turn_to_latency_signal[i]=1;
				}
			}
		
		}
		
		for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
			for (j = 0; j < NUM_SERVENT_PEER; j++){
				
				if( servent_peer_list[i][j].peer_id != -1 && servent_latency_signal[servent_peer_list[i][j].peer_id] == 1){//
			   	servent_peer_list[i][j].reputation_value = 5;

			   	}
				
				
			}
	   	}*/
	   	
   		all_servent_try_to_turn_latency_signal = 1;
	}
	else if(request_time_largest < 1000){
	printf("request_time_last_record_time %s:%s:%s !\n",string_now_hour,string_now_min,string_now_sec);
	request_time_last_record_time = now_sec;
	request_time_largest=-25;
		for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
				request_time[i]=-25;
		}
	}
	latency_num_record_array[latency_num_record_pointer].servent_latency_num=servent_latency_num_temp;
	latency_num_record_array[latency_num_record_pointer].crawler_latency_num=crawler_latency_num_temp;
	latency_num_record_array[latency_num_record_pointer].sensor_latency_num=sensor_latency_num_temp;
	latency_num_record_pointer++;
	
	}
	
	}
	
	
	sleep(1);
	}
	
	
	
	for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
				    
		
				    
				    
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				    	if(servent_peer_list[i][j].peer_id != -1 && servent[servent_peer_list[i][j].peer_id].fake_signal == 1){
						crawler_in_servent_peerlist_num++;
						
						break;
					
				    	}
				    	
				    	
				    }	
				    
				   
		
	}
	for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
				    
		
				    
				    
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				    	
				    	if(servent_peer_list[i][j].peer_id != -1 && servent[servent_peer_list[i][j].peer_id].sensor_signal == 1){
						sensor_in_servent_peerlist_num++;
						
						break;
				    	}
				    	
				    }	
				    
				   
		
	}
	for(i=0;i<NUM_CLIENT_BOTS;i++){
				    
				    
				    
				    
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				        if(client_master[i][j].master_id != -1 && servent[client_master[i][j].master_id].fake_signal == 1){
						crawler_in_client_peerlist_num++;
						break;
					}
					
				    }	
				    
				    
	}
	for(i=0;i<NUM_CLIENT_BOTS;i++){
				    
				    
				    
				    
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				        
					if(client_master[i][j].master_id != -1 && servent[client_master[i][j].master_id].sensor_signal == 1){
						sensor_in_client_peerlist_num++;
						break;
					}
				    }	
				    
				    
	}
	
	f = fopen("after_request_servent_peerlist_record.txt" , "w");
	if(!f){
	printf("data not exist");
	system("PAUSE");
	}
	for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
				    
				    if(servent_eliminate_signal[i]!= 1){
				    
				    //printf("peer list of servent %ld have:", i);
				    sprintf(record_data, "peer list of servent %ld have:",i  );
				    fwrite( record_data, 1,strlen(record_data), f );
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				    	if(servent_peer_list[i][j].peer_id != -1){
					    	if(servent[servent_peer_list[i][j].peer_id].fake_signal == 1 && servent_peer_list[i][j].reputation_value == -1){
					    	crawler_in_servent_peerlist_reputation_value_negative_one++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].fake_signal == 1 && servent_peer_list[i][j].reputation_value == 0){
					    	crawler_in_servent_peerlist_reputation_value_zero++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].fake_signal == 1 && servent_peer_list[i][j].reputation_value == 1){
					    	crawler_in_servent_peerlist_reputation_value_one++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].fake_signal == 1 && servent_peer_list[i][j].reputation_value == 2){
					    	crawler_in_servent_peerlist_reputation_value_two++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].fake_signal == 1 && servent_peer_list[i][j].reputation_value == 3){
					    	crawler_in_servent_peerlist_reputation_value_three++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].fake_signal == 1 && servent_peer_list[i][j].reputation_value == 4){
					    	crawler_in_servent_peerlist_reputation_value_four++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].fake_signal == 1 && servent_peer_list[i][j].reputation_value == 5){
					    	crawler_in_servent_peerlist_reputation_value_five++;
					    	}
					    	//-----
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal == 1 && servent_peer_list[i][j].reputation_value == -1){
					    	sensor_in_servent_peerlist_reputation_value_negative_one++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal == 1 && servent_peer_list[i][j].reputation_value == 0){
					    	sensor_in_servent_peerlist_reputation_value_zero++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal == 1 && servent_peer_list[i][j].reputation_value == 1){
					    	sensor_in_servent_peerlist_reputation_value_one++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal == 1 && servent_peer_list[i][j].reputation_value == 2){
					    	sensor_in_servent_peerlist_reputation_value_two++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal == 1 && servent_peer_list[i][j].reputation_value == 3){
					    	sensor_in_servent_peerlist_reputation_value_three++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal == 1 && servent_peer_list[i][j].reputation_value == 4){
					    	sensor_in_servent_peerlist_reputation_value_four++;
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal == 1 && servent_peer_list[i][j].reputation_value == 5){
					    	sensor_in_servent_peerlist_reputation_value_five++;
					    	}
					    	//------
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal != 1 && servent[servent_peer_list[i][j].peer_id].fake_signal != 1 
					    	&& servent_peer_list[i][j].reputation_value == -1){
					    	servent_in_servent_peerlist_reputation_value_negative_one++;
					    	
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal != 1 && servent[servent_peer_list[i][j].peer_id].fake_signal != 1 
					    	&& servent_peer_list[i][j].reputation_value == 0){
					    	servent_in_servent_peerlist_reputation_value_zero++;
					    	
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal != 1 && servent[servent_peer_list[i][j].peer_id].fake_signal != 1 
					    	&& servent_peer_list[i][j].reputation_value == 1){
					    	servent_in_servent_peerlist_reputation_value_one++;
					    	
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal != 1 && servent[servent_peer_list[i][j].peer_id].fake_signal != 1 
					    	&& servent_peer_list[i][j].reputation_value == 2){
					    	servent_in_servent_peerlist_reputation_value_two++;
					    	
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal != 1 && servent[servent_peer_list[i][j].peer_id].fake_signal != 1 
					    	&& servent_peer_list[i][j].reputation_value == 3){
					    	servent_in_servent_peerlist_reputation_value_three++;
					    	
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal != 1 && servent[servent_peer_list[i][j].peer_id].fake_signal != 1 
					    	&& servent_peer_list[i][j].reputation_value == 4){
					    	servent_in_servent_peerlist_reputation_value_four++;
					    	
					    	}
					    	else if(servent[servent_peer_list[i][j].peer_id].sensor_signal != 1 && servent[servent_peer_list[i][j].peer_id].fake_signal != 1 
					    	&& servent_peer_list[i][j].reputation_value == 5){
					    	servent_in_servent_peerlist_reputation_value_five++;
					    	
					    	}
					    	
						//printf("%ld %ld  ", servent_peer_list[i][j].peer_id, servent_peer_list[i][j].reputation_value);
						sprintf(record_data, "%ld %ld  ", servent_peer_list[i][j].peer_id, servent_peer_list[i][j].reputation_value);
						fwrite( record_data, 1,strlen(record_data), f );
					}
				    }	
				    //puts("");
				    fwrite( "\n", 1,1, f );
				    }
	}	
	sprintf(record_data, "crawler_in_servent_peerlist_reputation_value_negative_one:%d  \n",crawler_in_servent_peerlist_reputation_value_negative_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_servent_peerlist_reputation_value_zero:%d  \n",crawler_in_servent_peerlist_reputation_value_zero );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_servent_peerlist_reputation_value_one:%d  \n",crawler_in_servent_peerlist_reputation_value_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_servent_peerlist_reputation_value_two:%d  \n",crawler_in_servent_peerlist_reputation_value_two );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_servent_peerlist_reputation_value_three:%d  \n",crawler_in_servent_peerlist_reputation_value_three );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_servent_peerlist_reputation_value_four:%d  \n",crawler_in_servent_peerlist_reputation_value_four );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_servent_peerlist_reputation_value_five:%d  \n",crawler_in_servent_peerlist_reputation_value_five );
	fwrite( record_data, 1,strlen(record_data), f );
	//-----
	sprintf(record_data, "sensor_in_servent_peerlist_reputation_value_negative_one:%d  \n",sensor_in_servent_peerlist_reputation_value_negative_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_servent_peerlist_reputation_value_zero:%d  \n",sensor_in_servent_peerlist_reputation_value_zero );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_servent_peerlist_reputation_value_one:%d  \n",sensor_in_servent_peerlist_reputation_value_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_servent_peerlist_reputation_value_two:%d  \n",sensor_in_servent_peerlist_reputation_value_two );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_servent_peerlist_reputation_value_three:%d  \n",sensor_in_servent_peerlist_reputation_value_three );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_servent_peerlist_reputation_value_four:%d  \n",sensor_in_servent_peerlist_reputation_value_four );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_servent_peerlist_reputation_value_five:%d  \n",sensor_in_servent_peerlist_reputation_value_five );
	fwrite( record_data, 1,strlen(record_data), f );
	//--------
	sprintf(record_data, "servent_in_servent_peerlist_reputation_value_negative_one:%d  \n",servent_in_servent_peerlist_reputation_value_negative_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_servent_peerlist_reputation_value_zero:%d  \n",servent_in_servent_peerlist_reputation_value_zero );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_servent_peerlist_reputation_value_one:%d  \n",servent_in_servent_peerlist_reputation_value_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_servent_peerlist_reputation_value_two:%d  \n",servent_in_servent_peerlist_reputation_value_two );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_servent_peerlist_reputation_value_three:%d  \n",servent_in_servent_peerlist_reputation_value_three );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_servent_peerlist_reputation_value_four:%d  \n",servent_in_servent_peerlist_reputation_value_four );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_servent_peerlist_reputation_value_five:%d  \n",servent_in_servent_peerlist_reputation_value_five );
	fwrite( record_data, 1,strlen(record_data), f );
	
	sprintf(record_data,"crawler_in_servent_peerlist_num:%d !\n",crawler_in_servent_peerlist_num);
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data,"sensor_in_servent_peerlist_num:%d !\n",sensor_in_servent_peerlist_num);
	fwrite( record_data, 1,strlen(record_data), f );
	fclose(f);
	
	f = fopen("after_request_client_peerlist_record.txt" , "w");
	if(!f){
	printf("data not exist");
	system("PAUSE");
	}
	for(i=0;i<NUM_CLIENT_BOTS;i++){
				    
				    
				    
				    sprintf(record_data, "peer list of client %ld have:",i  );
				    fwrite( record_data, 1,strlen(record_data), f );
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				    	if(client_master[i][j].master_id != -1){
						if(servent[client_master[i][j].master_id].fake_signal == 1 && client_master[i][j].reputation_value == -1){
					    	crawler_in_client_peerlist_reputation_value_negative_one++;
					    	}
					    	else if(servent[client_master[i][j].master_id].fake_signal == 1 && client_master[i][j].reputation_value == 0){
					    	crawler_in_client_peerlist_reputation_value_zero++;
					    	}
					    	else if(servent[client_master[i][j].master_id].fake_signal == 1 && client_master[i][j].reputation_value == 1){
					    	crawler_in_client_peerlist_reputation_value_one++;
					    	}
					    	else if(servent[client_master[i][j].master_id].fake_signal == 1 && client_master[i][j].reputation_value == 2){
					    	crawler_in_client_peerlist_reputation_value_two++;
					    	}
					    	else if(servent[client_master[i][j].master_id].fake_signal == 1 && client_master[i][j].reputation_value == 3){
					    	crawler_in_client_peerlist_reputation_value_three++;
					    	}
					    	else if(servent[client_master[i][j].master_id].fake_signal == 1 && client_master[i][j].reputation_value == 4){
					    	crawler_in_client_peerlist_reputation_value_four++;
					    	}
					    	else if(servent[client_master[i][j].master_id].fake_signal == 1 && client_master[i][j].reputation_value == 5){
					    	crawler_in_client_peerlist_reputation_value_five++;
					    	}
					    	//-----
					    	else if(servent[client_master[i][j].master_id].sensor_signal == 1 && client_master[i][j].reputation_value == -1){
					    	sensor_in_client_peerlist_reputation_value_negative_one++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal == 1 && client_master[i][j].reputation_value == 0){
					    	sensor_in_client_peerlist_reputation_value_zero++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal == 1 && client_master[i][j].reputation_value == 1){
					    	sensor_in_client_peerlist_reputation_value_one++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal == 1 && client_master[i][j].reputation_value == 2){
					    	sensor_in_client_peerlist_reputation_value_two++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal == 1 && client_master[i][j].reputation_value == 3){
					    	sensor_in_client_peerlist_reputation_value_three++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal == 1 && client_master[i][j].reputation_value == 4){
					    	sensor_in_client_peerlist_reputation_value_four++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal == 1 && client_master[i][j].reputation_value == 5){
					    	sensor_in_client_peerlist_reputation_value_five++;
					    	}
						//-------
						else if(servent[client_master[i][j].master_id].sensor_signal != 1 && servent[client_master[i][j].master_id].fake_signal != 1 
						&& client_master[i][j].reputation_value == -1){
					    	servent_in_client_peerlist_reputation_value_negative_one++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal != 1 && servent[client_master[i][j].master_id].fake_signal != 1 
					    	&& client_master[i][j].reputation_value == 0){
					    	servent_in_client_peerlist_reputation_value_zero++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal != 1 && servent[client_master[i][j].master_id].fake_signal != 1 
					    	&& client_master[i][j].reputation_value == 1){
					    	servent_in_client_peerlist_reputation_value_one++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal != 1 && servent[client_master[i][j].master_id].fake_signal != 1 
					    	&& client_master[i][j].reputation_value == 2){
					    	servent_in_client_peerlist_reputation_value_two++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal != 1 && servent[client_master[i][j].master_id].fake_signal != 1 
					    	&& client_master[i][j].reputation_value == 3){
					    	servent_in_client_peerlist_reputation_value_three++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal != 1 && servent[client_master[i][j].master_id].fake_signal != 1 
					    	&& client_master[i][j].reputation_value == 4){
					    	servent_in_client_peerlist_reputation_value_four++;
					    	}
					    	else if(servent[client_master[i][j].master_id].sensor_signal != 1 && servent[client_master[i][j].master_id].fake_signal != 1 
					    	&& client_master[i][j].reputation_value == 5){
					    	servent_in_client_peerlist_reputation_value_five++;
					    	}
					    	
						sprintf(record_data, "%ld %ld  ", client_master[i][j].master_id, client_master[i][j].reputation_value);
						fwrite( record_data, 1,strlen(record_data), f );
					}
				    }	
				    //puts("");
				    fwrite( "\n", 1,1, f );
				    
	}	
	
	sprintf(record_data, "crawler_in_client_peerlist_reputation_value_negative_one:%d  \n",crawler_in_client_peerlist_reputation_value_negative_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_client_peerlist_reputation_value_zero:%d  \n",crawler_in_client_peerlist_reputation_value_zero );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_client_peerlist_reputation_value_one:%d  \n",crawler_in_client_peerlist_reputation_value_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_client_peerlist_reputation_value_two:%d  \n",crawler_in_client_peerlist_reputation_value_two );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_client_peerlist_reputation_value_three:%d  \n",crawler_in_client_peerlist_reputation_value_three );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_client_peerlist_reputation_value_four:%d  \n",crawler_in_client_peerlist_reputation_value_four );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "crawler_in_client_peerlist_reputation_value_five:%d  \n",crawler_in_client_peerlist_reputation_value_five );
	fwrite( record_data, 1,strlen(record_data), f );
	//-----
	sprintf(record_data, "sensor_in_client_peerlist_reputation_value_negative_one:%d  \n",sensor_in_client_peerlist_reputation_value_negative_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_client_peerlist_reputation_value_zero:%d  \n",sensor_in_client_peerlist_reputation_value_zero );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_client_peerlist_reputation_value_one:%d  \n",sensor_in_client_peerlist_reputation_value_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_client_peerlist_reputation_value_two:%d  \n",sensor_in_client_peerlist_reputation_value_two );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_client_peerlist_reputation_value_three:%d  \n",sensor_in_client_peerlist_reputation_value_three );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_client_peerlist_reputation_value_four:%d  \n",sensor_in_client_peerlist_reputation_value_four );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "sensor_in_client_peerlist_reputation_value_five:%d  \n",sensor_in_client_peerlist_reputation_value_five );
	fwrite( record_data, 1,strlen(record_data), f );
	//-----
	sprintf(record_data, "servent_in_client_peerlist_reputation_value_negative_one:%d  \n",servent_in_client_peerlist_reputation_value_negative_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_client_peerlist_reputation_value_zero:%d  \n",servent_in_client_peerlist_reputation_value_zero );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_client_peerlist_reputation_value_one:%d  \n",servent_in_client_peerlist_reputation_value_one );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_client_peerlist_reputation_value_two:%d  \n",servent_in_client_peerlist_reputation_value_two );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_client_peerlist_reputation_value_three:%d  \n",servent_in_client_peerlist_reputation_value_three );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_client_peerlist_reputation_value_four:%d  \n",servent_in_client_peerlist_reputation_value_four );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data, "servent_in_client_peerlist_reputation_value_five:%d  \n",servent_in_client_peerlist_reputation_value_five );
	fwrite( record_data, 1,strlen(record_data), f );
	
	sprintf(record_data,"crawler_in_client_peerlist_num:%d !\n",crawler_in_client_peerlist_num);
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data,"sensor_in_client_peerlist_num:%d !\n",sensor_in_client_peerlist_num);
	fwrite( record_data, 1,strlen(record_data), f );
	fclose(f);
	
	
	time(&current);
	info = localtime( &current );
		
	strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
	now_hour = atoi(string_now_hour);	
	strftime(string_now_min,sizeof(string_now_min),"%M",info);
	now_min = atoi(string_now_min);	
	strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
	now_sec = atoi(string_now_sec);	
	now_sec+=(60*now_min)+(60*60*now_hour);
	enumeration_start_time=now_sec;//1655
	
	printf("enumeration_start_time %d !!!!!!!!!!!!!!!\n",enumeration_start_time);
	
	pthread_attr_init(&attr);       
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 
	rc = pthread_create(&data_record, &attr, data_record_func, NULL);  
	if (rc) {
	printf("ERORR; return code from pthread_create() is %s\n", strerror(rc));
	exit(EXIT_FAILURE);
	}
	
	
	printf(" servent_bot_num_now = %d !\n",servent_bot_num_now);
	printf(" infect_and_inject_func_thread terminated !\n");
    	pthread_exit(NULL);

}
void *boot_control_func(){
    	FILE* f;
    	char record_data[100];
    	char path[100];
    	long i=0,j=0,k=0,l=0,rc=0;
	int now_hour,now_min,now_sec;
    	char string_now_hour[1024],string_now_min[1024],string_now_sec[1024];
    	int num=0;
    	int dynamic_servent_boot_id[4][NUM_FAKE_SERVENT_BOTS+NUM_SERVENT_BOTS+NUM_SENSOR];
    	int dynamic_client_boot_id[4][NUM_CLIENT_BOTS];
    	int static_servent_boot_id[NUM_FAKE_SERVENT_BOTS+NUM_SERVENT_BOTS+NUM_SENSOR];
    	int static_client_boot_id[NUM_CLIENT_BOTS];
    	int PC_block=(100000/5000);//block = 20
    	int static_PC_block_num[20];
    	int dynamic_PC_block_num[4][20];
    	int dynamic_PC_boot_id[4][100000];
    	int static_PC_boot_id[100000];
    	int static_servent_boot_num=0;
    	int static_client_boot_num=0;
    	int dynamic_servent_boot_num[4];
    	int dynamic_client_boot_num[4];
    	int last_time_record=0;
    	int start_now_sec=0;
    	int end_now_sec=0;
    	int dynamic_pointer=0;
    	int random_PC_ID=0;
    	//zzzz

	for (i = 0 ; i < NUM_FAKE_SERVENT_BOTS+NUM_SERVENT_BOTS+NUM_SENSOR ; i++) {
		servent_boot_signal[i]=0;	
	}
	for (i = 0 ; i < NUM_CLIENT_BOTS ; i++) {
		client_boot_signal[i]=0;	

	}
	
    	for (i = 0 ; i < 4 ; i++) {
    	dynamic_servent_boot_num[i]=0;
    	dynamic_client_boot_num[i]=0;
    	} 
	for (i = 0 ; i < 100000 ; i++) {
	device[i].bot_ID = -1;
	}
	
    	for (i = 0 ; i < NUM_FAKE_SERVENT_BOTS+NUM_SERVENT_BOTS+NUM_SENSOR ; i++) {
    	random_PC_ID = rand() % 100000;
    		for(j=0;j<already_register_PC_num;j++){
		    	
			while(random_PC_ID == already_register_PC[j]){
			random_PC_ID = rand() % 100000;
			j=0;
			}
		    
		}
    	device[random_PC_ID].servent_or_client = 1;// servent = 1 client = 0
    	device[random_PC_ID].bot_ID =  rand() % 5000;
    		for(j=0;j<i;j++){
		    	
			while(device[random_PC_ID].bot_ID == already_register_servent[j]){
			device[random_PC_ID].bot_ID = rand() % 5000;
			j=0;
			}
		    
		}
		
    	already_register_servent[i] = device[random_PC_ID].bot_ID;
    	already_register_PC[already_register_PC_num] = random_PC_ID;
    	already_register_PC_num++;
    	}
    	
    	for (i = 0 ; i < NUM_CLIENT_BOTS ; i++) {
    	random_PC_ID = rand() % 100000;
    		for(j=0;j<already_register_PC_num;j++){
		    	
			while(random_PC_ID == already_register_PC[j]){
			random_PC_ID = rand() % 100000;
			j=0;
			}
		    
		}
    	device[random_PC_ID].servent_or_client = 0;// servent = 1 client = 0
    	device[random_PC_ID].bot_ID =  rand() % 15000;
    		for(j=0;j<i;j++){
		    	
			while(device[random_PC_ID].bot_ID == already_register_client[j]){
			device[random_PC_ID].bot_ID = rand() % 15000;
			j=0;
			}
		    
		}
		
    	already_register_client[i] = device[random_PC_ID].bot_ID;
    	already_register_PC[already_register_PC_num] = random_PC_ID;
    	already_register_PC_num++;
    	}
    	
    	
    	/*f = fopen("PC_record.txt", "w");
	if(!f){
	printf("data not exist");
	system("PAUSE");
	}
	for (i = 0 ; i < 100000 ; i++) {
		if(device[i].bot_ID != -1 && device[i].servent_or_client == 1){
		sprintf(record_data, "PC[%d] is servent ,ID: %d \n",i,device[i].bot_ID);
		fwrite( record_data, 1,strlen(record_data), f ); 
		}
		else if(device[i].bot_ID != -1 && device[i].servent_or_client == 0){
		sprintf(record_data, "PC[%d] is client ,ID: %d \n",i,device[i].bot_ID); 
		fwrite( record_data, 1,strlen(record_data), f );
		}
		else if(device[i].bot_ID == -1){
		sprintf(record_data, "PC[%d] is normal \n",i); 
		fwrite( record_data, 1,strlen(record_data), f );
		}
	}
	sprintf(record_data, "already_register_PC_num :%d\n",already_register_PC_num); 
	fwrite( record_data, 1,strlen(record_data), f );
	fclose(f);*/
    	
    		
    	puts("PC random ok1");
    	
    	
    	
    	for (i = 0; i < 20/5 ; i++){// 1 block = 5000 PC , 4 block = 20000 PC
	    
	    static_PC_block_num[i] = rand() % 20;
	    for(j=0;j<i;j++){
		    	
			    while(static_PC_block_num[i] == static_PC_block_num[j]){
			    static_PC_block_num[i] = rand() % 20;
			    j=0;
			    }
		    
		    }
	    
	    
	    
	}
	/*f = fopen("static_PC_record.txt", "w");
	if(!f){
	printf("data not exist");
	system("PAUSE");
	}*/
	
		
	k=0;
		for (i = 0; i < 20/5 ; i++){       // 1 block = 5000 PC , 4 block = 20000 PC
			for (j = 0;k < 100000/5,j < 5000;k++,j++){
				
				    static_PC_boot_id[k] = static_PC_block_num[i]*5000+j;
				    
				    /*sprintf(record_data, "static_PC_boot_id[%d]:!%d!  \n",k,static_PC_boot_id[k]); 
				    fwrite( record_data, 1,strlen(record_data), f );*/
				    if(j==5000){
				    j=0;
				    }
				    
			}
		}
	
	//fclose(f);
	
	puts("PC random ok2");
	
	
	for (l = 0; l < 4; l++){
		for (i = 0; i < 3*20/5 ; i++){// 1 block = 5000 PC , 12 block = 60000 PC
		    
		    dynamic_PC_block_num[l][i] = rand() % 20;
		    
		    for(j=0;j<i;j++){
			    for(k=0;k<20/5;k++){
					while(dynamic_PC_block_num[l][i] == dynamic_PC_block_num[l][j] || dynamic_PC_block_num[l][i] == static_PC_block_num[k]){
					dynamic_PC_block_num[l][i] = rand() % 20;
					j=0;
					k=0;
					}
			    }
		    }
		    
		    
		}	
		
	}
	
	
	for (l = 0; l < 4; l++){
		/*sprintf(path, "dynamic_PC_record%d.txt", l); 
		f = fopen(path, "w");
		if(!f){
		printf("data not exist");
		system("PAUSE");
		}*/
		k=0;
		for (i = 0; i < 3*20/5 ; i++){// 1 block = 5000 PC , 12 block = 60000 PC
			for (j = 0; k < 3*100000/5,j < 5000; k++,j++){
				
				    dynamic_PC_boot_id[l][k] = dynamic_PC_block_num[l][i]*5000+j;
				    
				    /*sprintf(record_data, "dynamic_PC_boot_id[%d][%d]:!%d!  \n",l,k,dynamic_PC_boot_id[l][k]); 
				    fwrite( record_data, 1,strlen(record_data), f );*/
				    if(j==5000){
				    j=0;
				    }	
				    
			}
			
		}
		//fclose(f);
	}
		
		
		
	puts("PC random ok3");
	
	for (i = 0; i < 100000/5 ; i++){
		if(device[static_PC_boot_id[i]].bot_ID != -1 && device[static_PC_boot_id[i]].servent_or_client == 0){ // servent = 1 client = 0
		static_client_boot_id[static_client_boot_num] = device[static_PC_boot_id[i]].bot_ID;
		static_client_boot_num++;
		}
		else if(device[static_PC_boot_id[i]].bot_ID != -1 && device[static_PC_boot_id[i]].servent_or_client == 1){
		static_servent_boot_id[static_servent_boot_num] = device[static_PC_boot_id[i]].bot_ID;
		static_servent_boot_num++; 
		}
	
	}
		/*sprintf(path, "static_bot_record.txt"); 
		f = fopen(path, "w");
		if(!f){
		printf("data not exist");
		system("PAUSE");
		}
		for (i = 0; i < static_client_boot_num ; i++){
		sprintf(record_data, "static_client_boot_id[%d]:!%d!  \n",i,static_client_boot_id[i]); 
	        fwrite( record_data, 1,strlen(record_data), f );
	        }
	        for (i = 0; i < static_servent_boot_num ; i++){
		sprintf(record_data, "static_servent_boot_id[%d]:!%d!  \n",i,static_servent_boot_id[i]); 
	        fwrite( record_data, 1,strlen(record_data), f );
	        }
	        sprintf(record_data, "static_client_boot_num %d!  \n",static_client_boot_num); 
	        fwrite( record_data, 1,strlen(record_data), f );
	        sprintf(record_data, "static_servent_boot_num %d!  \n",static_servent_boot_num); 
	        fwrite( record_data, 1,strlen(record_data), f );
	        fclose(f);*/
	        
	        
	for (l = 0; l < 4; l++){
		for (i = 0; i < 3*100000/5; i++){
			if(device[dynamic_PC_boot_id[l][i]].bot_ID != -1 && device[dynamic_PC_boot_id[l][i]].servent_or_client == 0){
			dynamic_client_boot_id[l][dynamic_client_boot_num[l]] = device[dynamic_PC_boot_id[l][i]].bot_ID;
			dynamic_client_boot_num[l]++;
			}
			else if(device[dynamic_PC_boot_id[l][i]].bot_ID != -1 && device[dynamic_PC_boot_id[l][i]].servent_or_client == 1){
			dynamic_servent_boot_id[l][dynamic_servent_boot_num[l]] = device[dynamic_PC_boot_id[l][i]].bot_ID;
			dynamic_servent_boot_num[l]++; 
			}
		}
	}
	/*for (l = 0; l < 4; l++){
		sprintf(path, "dynamic_bot_record%d.txt",l); 
		f = fopen(path, "w");
		if(!f){
		printf("data not exist");
		system("PAUSE");
		}
		for (i = 0; i < dynamic_client_boot_num[l] ; i++){
		sprintf(record_data, "dynamic_client_boot_id[%d][%d]:!%d!  \n",l,i,dynamic_client_boot_id[l][i]); 
	        fwrite( record_data, 1,strlen(record_data), f );
	        }
	        for (i = 0; i < dynamic_servent_boot_num[l] ; i++){
		sprintf(record_data, "dynamic_servent_boot_id[%d][%d]:!%d!  \n",l,i,dynamic_servent_boot_id[l][i]); 
	        fwrite( record_data, 1,strlen(record_data), f );
	        }
	        sprintf(record_data, "dynamic_client_boot_num[%d]:%d \n",l,dynamic_client_boot_num[l]); 
	        fwrite( record_data, 1,strlen(record_data), f );
	        
	        sprintf(record_data, "dynamic_servent_boot_num[%d]:%d \n",l,dynamic_servent_boot_num[l]); 
	        fwrite( record_data, 1,strlen(record_data), f );
	        fclose(f);
	
	}*/
			
	puts("PC random ok4");
	
	
	
	time(&current);
	info = localtime( &current );
		
	strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
	now_hour = atoi(string_now_hour);	
	strftime(string_now_min,sizeof(string_now_min),"%M",info);
	now_min = atoi(string_now_min);	
	strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
	now_sec = atoi(string_now_sec);	
	now_sec+=(60*now_min)+(60*60*now_hour);
		
	change_period_time[change_period_time_pointer].start_time = now_sec;
		
	

    	
	
	time(&current);
	info = localtime( &current );
		
	strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
	now_hour = atoi(string_now_hour);	
	strftime(string_now_min,sizeof(string_now_min),"%M",info);
	now_min = atoi(string_now_min);	
	strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
	now_sec = atoi(string_now_sec);	
	now_sec+=(60*now_min)+(60*60*now_hour);
	
	program_start_time = now_sec;
    	last_time_record = program_start_time;
    	
    	
	for (i = 0 ; i < static_servent_boot_num ; i++) {
		servent_boot_signal[static_servent_boot_id[i]]=1;	
	}
	/*for (i = 0 ; i < dynamic_servent_boot_num ; i++) {
		servent_boot_signal[dynamic_servent_boot_id[dynamic_pointer][i]]=1;	
	}*/
	
	
	for (i = 0 ; i < static_client_boot_num ; i++) {
		client_boot_signal[static_client_boot_id[i]]=1;	
	}
	/*for (i = 0 ; i < dynamic_client_boot_num ; i++) {
		client_boot_signal[dynamic_client_boot_id[dynamic_pointer][i]]=1;	
	}*/
	
	for (i = NUM_SERVENT_BOTS; i < NUM_FAKE_SERVENT_BOTS+NUM_SERVENT_BOTS+NUM_SENSOR ; i++) {
	servent_boot_signal[i]=1;	
	}
	for (i = 0; i < 50 ; i++) {
	servent_boot_signal[i]=1;	
	}
	
	change_period_time[change_period_time_pointer].end_time = now_sec;
	change_period_time[change_period_time_pointer].peak_to_off_peak = 1;
	change_period_time_pointer++;
	//dynamic_pointer++;
	
	
	pattern_execute_time = 0;
    	while(boot_control_terminate_signal != 1){
    	
    	
    	
	
    	time(&current);
	info = localtime( &current );
		
	strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
	now_hour = atoi(string_now_hour);	
	strftime(string_now_min,sizeof(string_now_min),"%M",info);
	now_min = atoi(string_now_min);	
	strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
	now_sec = atoi(string_now_sec);	
	now_sec+=(60*now_min)+(60*60*now_hour);
	/*printf(" now_time %s:%s:%s now_sec %d!\n",string_now_hour,string_now_min,string_now_sec,now_sec); 
	printf("last_time_record: %d  !\n",last_time_record);

	
	
	printf("pattern_execute_time: %d  !\n",pattern_execute_time);*/
	
	if(now_sec > last_time_record  ){
	
	if((now_sec - last_time_record ) >= NUM_OF_SELECT_PATTERN_TIMES  ){
	last_time_record = now_sec;
	printf(" now_time %s:%s:%s now_sec %d!\n",string_now_hour,string_now_min,string_now_sec,now_sec); 
	if(pattern_execute_time+1 == 44){
	printf("peak to off-peak !\n");
	change_time_period_signal=1;
	}
	else if(pattern_execute_time+1 == 20){
	printf("off-peak to peak !\n");
	change_time_period_signal=1;
	}
	pattern_execute_time++;
	}
	}
	
	else if(now_sec < last_time_record ){
	
	if((now_sec+(86400-last_time_record) ) >= NUM_OF_SELECT_PATTERN_TIMES  ){
	printf(" now_time %s:%s:%s now_sec %d!\n",string_now_hour,string_now_min,string_now_sec,now_sec); 
	last_time_record = now_sec;
	if(pattern_execute_time+1 == 44){
	printf("peak to off-peak !\n");
	change_time_period_signal=1;
	}
	else if(pattern_execute_time+1 == 20){
	printf("off-peak to peak !\n");
	change_time_period_signal=1;
	}
	pattern_execute_time++;
	}
	}
	
	if(pattern_execute_time == 48){
	pattern_execute_time = 0;
	}
	if(pattern_execute_time == 44 && change_time_period_signal == 1){
		printf("off-peak period start !\n");
		
		for (i = 0 ; i < NUM_FAKE_SERVENT_BOTS+NUM_SERVENT_BOTS+NUM_SENSOR ; i++) {
			servent_boot_signal[i]=0;	
		}
		for (i = 0 ; i < NUM_CLIENT_BOTS ; i++) {
			client_boot_signal[i]=0;	
		}
		for (i = 0 ; i < static_servent_boot_num ; i++) {
			servent_boot_signal[static_servent_boot_id[i]]=1;	
		}

		
		for (i = 0 ; i < static_client_boot_num ; i++) {
			client_boot_signal[static_client_boot_id[i]]=1;	
		}
		change_period_time[change_period_time_pointer].start_time = now_sec;
		change_period_time[change_period_time_pointer].end_time = now_sec;
		change_period_time[change_period_time_pointer].peak_to_off_peak = 1;
		change_period_time_pointer++;
		
		change_time_period_signal=0;
	}
	else if(pattern_execute_time == 20 && change_time_period_signal == 1){
		
		start_now_sec=0;
		end_now_sec=0;
		printf("peak period start !\n");
		time(&current);
		info = localtime( &current );
			
		strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
		now_hour = atoi(string_now_hour);	
		strftime(string_now_min,sizeof(string_now_min),"%M",info);
		now_min = atoi(string_now_min);	
		strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
		now_sec = atoi(string_now_sec);	
		now_sec+=(60*now_min)+(60*60*now_hour);

		start_now_sec = now_sec;
		change_period_time[change_period_time_pointer].start_time = start_now_sec;
		
		for (i = 0 ; i < NUM_FAKE_SERVENT_BOTS+NUM_SERVENT_BOTS+NUM_SENSOR ; i++) {
			servent_boot_signal[i]=0;	
		}
		for (i = 0 ; i < NUM_CLIENT_BOTS ; i++) {
			client_boot_signal[i]=0;	
		}
		
		
		for (i = 0 ; i < static_servent_boot_num ; i++) {
			servent_boot_signal[static_servent_boot_id[i]]=1;	
		}

		for (i = 0 ; i < dynamic_servent_boot_num[dynamic_pointer] ; i++) {
			servent_boot_signal[dynamic_servent_boot_id[dynamic_pointer][i]]=1;	
		}
		
		
		for (i = 0 ; i < static_client_boot_num ; i++) {
			client_boot_signal[static_client_boot_id[i]]=1;	

		}
		for (i = 0 ; i < dynamic_client_boot_num[dynamic_pointer] ; i++) {
			client_boot_signal[dynamic_client_boot_id[dynamic_pointer][i]]=1;	
		}
		time(&current);
		info = localtime( &current );
			
		strftime(string_now_hour,sizeof(string_now_hour),"%H",info);
		now_hour = atoi(string_now_hour);	
		strftime(string_now_min,sizeof(string_now_min),"%M",info);
		now_min = atoi(string_now_min);	
		strftime(string_now_sec,sizeof(string_now_sec),"%S",info);
		now_sec = atoi(string_now_sec);	
		now_sec+=(60*now_min)+(60*60*now_hour);

		end_now_sec = now_sec;
		
		if(enumeration_start_time != 0){
		printf("start_now_sec %d!\n",start_now_sec);
    		printf("end_now_sec %d!\n",end_now_sec);
		offpeak_to_peak_time += abs(end_now_sec - start_now_sec );
		printf("offpeak_to_peak_time %d !\n",offpeak_to_peak_time);
		}
		
	
		change_period_time[change_period_time_pointer].end_time = end_now_sec;
		change_period_time[change_period_time_pointer].off_peak_to_peak = 1;
		change_period_time_pointer++;
		dynamic_pointer++;
		if(dynamic_pointer > 3){
		dynamic_pointer = 0;
		}
		change_time_period_signal=0;
	
	}
	
	for (i = NUM_SERVENT_BOTS; i < NUM_FAKE_SERVENT_BOTS+NUM_SERVENT_BOTS+NUM_SENSOR ; i++) {
	servent_boot_signal[i]=1;	
	}
	for (i = 0; i < 50 ; i++) {
	servent_boot_signal[i]=1;	
	}
				
	
	
    	sleep(1);
    	}
	printf(" boot_control_func_thread terminated !\n");
    	pthread_exit(NULL);
	
	
}
int main() {
	
    
    
    srand(time(NULL) );
    
    int rc,return_data;
    int check=1;
    char data[1024];
    char string[100];    
    

    int master_command=-1;
   
    long i=0;
    long j=0;
    int a,b;
    long t;
    if( putenv( "TZ=EST" ) == -1 ){
	printf( "Unable to set TZ\n" );exit( 1 );
    }
    
    int now_hour,now_min,now_sec;
    char string_now_hour[1024],string_now_min[1024],string_now_sec[1024];
    FILE* f;
    char record_data[200];
    int servent_latency_num=0;
    char write_data[100];
    char path[100];
    
	for(i=1;i<=3;i++){
	    
		    
		    if(i==1){
		    sprintf(path, "command%d.txt", i); 
		    f = fopen(path, "w");
		    if(!f){
			printf("data not exist");
			system("PAUSE");
			
		    }
		    strcpy(write_data,"f004;;9999;9999;2022.10.29.86400;7200");
		    fwrite( write_data, 1,strlen(write_data), f );
    		    //puts(write_data);
    		    fclose(f);
		    }
		    if(i==2){
		    sprintf(path, "command%d.txt", i); 
		    f = fopen(path, "w");
		    if(!f){
			printf("data not exist");
			system("PAUSE");
			
		    }
		    strcpy(write_data,"[][f004][9999][9999][2022.10.29.86400][7200]");
		    fwrite( write_data, 1,strlen(write_data), f );
    		    //puts(write_data);
    		    fclose(f);
		    }
		    if(i==3){
		    sprintf(path, "command%d.txt", i); 
		    f = fopen(path, "w");
		    if(!f){
			printf("data not exist");
			system("PAUSE");
			
		    }
		    strcpy(write_data,"2022.10.29.86400||7200||||f004||9999||9999");
		    fwrite( write_data, 1,strlen(write_data), f );
    		    //puts(write_data);
    		    fclose(f);
		    }
		    
		    
	    }
    
    
    
    pthread_attr_init(&attr);       
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 

    rc = pthread_create(&relay_station, &attr, relay_station_func, NULL);  
    if (rc) {
				    printf("ERORR; return code from pthread_create() is %s\n", strerror(rc));
				    exit(EXIT_FAILURE);
				}
    
    for (i = 0; i < NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR; i++) {
    servent[i].detect_signal = 0;
    servent[i].detect_and_reply_signal = 0;
    servent[i].request_signal = 0;
    servent[i].fake_signal = 0;
    servent[i].sensor_signal = 0;
    servent_select_pattern_signal[i]=0;
    servent_last_time_select_pattern[i]=0;
    
    
    }
    


    for (i = 0; i < NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR; i++) {
    	servent_execute_pattern_time[i]=0;
    	servent_pattern[i]=99;
        servent_command_buffer_pointer[i]=0;
        servent_boot_signal[i]=0;
        servent_bot_website_buffer_pointer[i]=0;
        servent_send_trust_message_to_me_pointer[i]=0;
        servent_trust_threshold[i]=0;
        servent_latency_signal[i]=0;
        servent_resurrection_complete_signal[i]=0;
	servent_eliminate_signal[i]=0;
	servent_resurrection_request_peer_signal[i]=0;
	servent_already_turn_to_latency_signal[i]=0;
	
	
	servent_already_execute_resurrection_signal[i] = 0;
        for (j = 0; j < NUM_SERVENT_PEER; j++){
		
		servent_peer_list[i][j].peer_id = -1;
		servent_peer_list[i][j].trust_signal = 0;
		servent_peer_list[i][j].already_send_TM_signal = 0;
		
		
		servent_send_trust_message_to_me[i][j] = -1;
		
	}

     }
     
     
     for (i = 0; i < NUM_CLIENT_BOTS; i++) {
     client[i].detect_signal = 0;
     client[i].detect_and_reply_signal = 0;
     client[i].request_signal = 0;
     client[i].fake_signal = 0;
     client[i].sensor_signal = 0;
     
     
     }
     for (i = 0; i < NUM_CLIENT_BOTS; i++) {
     	client_execute_pattern_time[i]=0;
    	client_pattern[i]=99;
        client_command_buffer_pointer[i]=0;
        client_boot_signal[i]=0;
        client_bot_website_buffer_pointer[i]=0;
        client_eliminate_signal[i]=0;
        client_exchange_servent_target[i]=0;
        client_select_pattern_signal[i] =0;
        client_last_time_select_pattern[i]=0;
        for (j = 0; j < NUM_SERVENT_PEER; j++){
		
		client_master[i][j].master_id = -1;
		
		
	}

    }
    for (i = 0; i < NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR; i++) {
    for (j = 0; j < 1000; j++){
     
     servent_log[i][j].num_pattern = 0;
     servent_log[i][j].communication_target = -1;
     servent_log[i][j].communication_result = 0;
     servent_log[i][j].receive_peer = -1;
     servent_log[i][j].send_peer = -1;
    }
    }
    for (i = 0; i < NUM_CLIENT_BOTS; i++) {
    for (j = 0; j < 1000; j++){
     
     client_log[i][j].num_pattern = 0;
     client_log[i][j].communication_target = -1;
     client_log[i][j].communication_result = 0;
     client_log[i][j].receive_peer = -1;
     client_log[i][j].send_peer = -1;
    }
    }

    init_servent_and_client_information();
    init_servent_website();
    for (i = 0; i < 4800; i++) {
    init_servent_peer_list(i);
    }
    for (i = 0; i < 15000; i++) {
    init_client_master(i);
    }
    
    
    
    
    
    
    				//client_boot_signal[0]=1;
    				/*for (i = 0; i < 1; i++) {
					for (j = 0; j < 9 ; j++){
					        
						if(client_master[i][j].master_id!= -1  ){
						servent_boot_signal[client_master[i][j].master_id] = 0;
						}
						
						
						
					}
				}
				i=0;
				printf("master of client %ld have:", i);
				
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				    
				    
				    if(client_master[i][j].master_id!= -1)
				    	printf("%ld %ld  ", client_master[i][j].master_id, client_master[i][j].reputation_value);
				    
				    }	*/
    				//servent_latency_signal[0]=1;//***-
    				//servent_trust_threshold[0]=5;
				//servent_boot_signal[0]=1;
				/*for (i = 0; i < 1; i++) {
					for (j = 0; j < 9 ; j++){
					        
						if(servent_peer_list[i][j].peer_id != -1  ){
						servent_boot_signal[client_master[i][j].master_id] = 0;
						}
						
						
						
					}
				}*/
				/*for(i = 1; i < 10;i++){
				    
				    
				    printf("peer list of servent %ld have:", i);
				    for (j = 0; j < NUM_SERVENT_BOTS; j++) {
					printf("%ld %ld  ", servent_peer_list[i][j].peer_id, servent_peer_list[i][j].reputation_value);
				    }	
				    puts("");
				    
				}*/ 							
    
    
    servent_thread_num_now = 100;
    client_thread_num_now = 300;
    
    pthread_attr_init(&attr);       
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 
    
    rc = pthread_create(&infect_and_inject_enumeration, &attr, infect_and_inject_enumeration_thread_func, NULL);
        
    if (rc) {
				    printf("ERORR; return code from pthread_create() is %s\n", strerror(rc));
				    exit(EXIT_FAILURE);
				    }
				    
	while(abc_signal!=1)	{
	sleep(1);
	}
    rc = pthread_create(&boot_control, &attr, boot_control_func, NULL);  
    if (rc) {
				    printf("ERORR; return code from pthread_create() is %s\n", strerror(rc));
				    exit(EXIT_FAILURE);
				    }
    
    
    
				    
    pthread_attr_init(&attr);       
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);  
    for (i = 0; i < (NUM_SERVENT_BOTS/50); i++) { // /1000
 	rc = pthread_create(&servent_threads[i], &attr, servent_thread_func, (void *)i);
        
        if (rc) {
				    printf("ERORR; return code from pthread_create() is %s\n", strerror(rc));
				    exit(EXIT_FAILURE);
	}
     }
    
    
    
    
    pthread_attr_init(&attr);       
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 
     for (i = 0; i < (15000/50); i++) {  // /1000
 	rc = pthread_create(&client_threads[i], &attr, client_thread_func, (void *)i);
        
        if (rc) {
				    printf("ERORR; return code from pthread_create() is %s\n", strerror(rc));
				    exit(EXIT_FAILURE);
	}
     }
    
 
    
    while(master_command != 0 && program_terminate_signal != 1){
 
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
	
				
				//servent_pattern[0]=1;
				i=4900;
				servent_pattern[i]=2;
				
				//sleep(2);
				
				break;
				
			case 2:
				///servent_pattern[0]=2;
				//servent_pattern[NUM_SERVENT_BOTS]=2;
				
				//sleep(2);
				i=4900;
				servent_pattern[i]=3;
				
				break;				
			case 3:
				
				//servent_pattern[0]=3;	
				//client_pattern[0]=3;
				for (i = 0; i < (NUM_SERVENT_BOTS); i++) {
				servent_pattern[i]=2;
				}
				//servent_pattern[NUM_SERVENT_BOTS]=3;
				//sleep(2);
					
				break;
			case 4:
				/*for (i = 0; i < (5); i++) {
				servent_latency_signal[servent_peer_list[3000][i].peer_id] = 1;
				}
				for (i = 0; i < (5); i++) {
				servent_latency_signal[servent_peer_list[4800][i].peer_id] = 1;
				}*/
				/*for (i = 0; i < (5); i++) {
				servent_eliminate_signal[client_master[3000][i].master_id] = 1;
				}
				
				for ( i = 0; i < 10; i++) {
				if(client_master[3000][i].master_id!= -1 ){
				printf("id:%d value:%d\n",client_master[3000][i].master_id, client_master[3000][i].reputation_value);
				}
				
				}*/
				
				
				/*for (i = 0; i < (10); i++) {
				
				servent_peer_list[4900][i].peer_id = 4901+i;
				
				}*/
				
				
				
				/*
				for (i = 0; i < (10); i++) {
				client_master[3001][i].master_id = 4800+i;
				}*/
				
				i=3000;
				/*for (j = 0; j < 5; j++) {
				servent_peer_list[i][j].reputation_value = j+1;
				}

				for (j = 5; j < 10; j++) {
				servent_peer_list[i][j].reputation_value = j-5;
				}
				printf("peer list of servent %ld have:\n", i);
				for ( j = 0; j < 10; j++) {
				if(servent_peer_list[i][j].peer_id != -1 ){
				printf("id:%d value:%d\n",servent_peer_list[i][j].peer_id, servent_peer_list[i][j].reputation_value);
				}
				
				}*/
				i=4900;
				/*for ( j = 0; j < 10; j++) {
				if(servent_peer_list[i][j].peer_id != -1 && servent_peer_list[i][j].reputation_value == 5){
				servent_peer_list[i][j].reputation_value--;
				}
				
				}*/
				
				printf("peer list of servent %ld have:\n", i);
				for ( j = 0; j < 10; j++) {
				if(servent_peer_list[i][j].peer_id != -1 ){
				printf("id:%d value:%d\n",servent_peer_list[i][j].peer_id, servent_peer_list[i][j].reputation_value);
				}
				
				}
				break;	
			case 5:
				
				/*i=0;
				for (j = 0; j < 5; j++) {
				client_master[i][j].reputation_value = j+1;
				}
				for (j = 5; j < 10; j++) {
				client_master[i][j].reputation_value = j-5;
				}
				printf("peer list of client %ld have:\n", i);
				for ( j = 0; j < 10; j++) {
				if(client_master[i][j].master_id!= -1 ){
				printf("id:%d value:%d\n",client_master[i][j].master_id, client_master[i][j].reputation_value);
				}
				
				}
				*/
				i=4900;
				servent_peer_list[i][2].reputation_value = 5;
				printf("peer list of servent %ld have:\n", i);
				for ( j = 0; j < 10; j++) {
				if(servent_peer_list[i][j].peer_id != -1 ){
				printf("id:%d value:%d\n",servent_peer_list[i][j].peer_id, servent_peer_list[i][j].reputation_value);
				}
				
				}
				break;	
			case 6:
				
				//servent_pattern[4900]=2;
				/*for (j = 0; j < NUM_SERVENT_PEER; j++) {
				servent_pattern[]=1;
				}*/
				i=3000;
				    printf("before servent_command_buffer_pointer  %d \n", servent_command_buffer_pointer[i]);
				    strcpy(servent_command_buffer[i][servent_command_buffer_pointer[i]].content,"f004;;9999;9999;2022.10.29.0;7200");
				    puts(servent_command_buffer[i][servent_command_buffer_pointer[i]].content);
				    servent_command_buffer_pointer[i]++;
				    printf("after servent_command_buffer_pointer  %d \n", servent_command_buffer_pointer[i]);
				    servent_command_analysis(i,servent_command_buffer[i][servent_command_buffer_pointer[i]-1].content);
				break;
			case 7:
				//servent_pattern[4800]=1;
				//request_time_largest = 1001;
				//for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
				    //printf("servent %ld\n",i);
				    i=3000;
				    printf("before servent_command_buffer_pointer  %d \n", servent_command_buffer_pointer[i]);
				    strcpy(servent_command_buffer[i][servent_command_buffer_pointer[i]].content,"f007;;9999;9999;2022.9.28.0;7200");
				    puts(servent_command_buffer[i][servent_command_buffer_pointer[i]].content);
				    servent_command_buffer_pointer[i]++;
				    printf("after servent_command_buffer_pointer  %d \n", servent_command_buffer_pointer[i]);
				    servent_command_analysis(i,servent_command_buffer[i][servent_command_buffer_pointer[i]-1].content);
				//}
				break;	
			case 8:
				
				/*  i=100;  
				    
				    printf("peer list of servent %ld have:", i);
				    for (j = 0; j < 10; j++) {
					printf("%ld %ld  ", servent_peer_list[i][j].peer_id, servent_peer_list[i][j].reputation_value);
				    }	
				    puts("");
				    
				servent_peer_list[100][9].reputation_value = 5;
				
				
				  i=100;  
				    
				    printf("peer list of servent %ld have:", i);
				    for (j = 0; j < 10; j++) {
					printf("%ld %ld  ", servent_peer_list[i][j].peer_id, servent_peer_list[i][j].reputation_value);
				    }	
				    puts("");
				    
				    
				servent_pattern[100]=2;*/
				i=4900;
				for (j = 0; j < 10; j++) {
				servent_latency_signal[4900+j] = 1;
				}
				
				/*servent_peer_list[1010][9].peer_id = i;
				servent_peer_list[1010][9].reputation_value = 5;
				servent_peer_list[1020][9].peer_id = i;
				servent_peer_list[1020][9].reputation_value = 5;
				servent_peer_list[1030][9].peer_id = i;
				servent_peer_list[1030][9].reputation_value = 5;
				servent_peer_list[1040][9].peer_id = i;
				servent_peer_list[1040][9].reputation_value = 5;*/
				i=4800;
				for (j = 0; j < 10; j++) {
				servent_latency_signal[4800+j] = 1;
				}
				/*servent_peer_list[10][9].peer_id = i;
				servent_peer_list[10][9].reputation_value = 5;
				servent_peer_list[20][9].peer_id = i;
				servent_peer_list[20][9].reputation_value = 5;
				servent_peer_list[30][9].peer_id = i;
				servent_peer_list[30][9].reputation_value = 5;
				servent_peer_list[40][9].peer_id = i;
				servent_peer_list[40][9].reputation_value = 5;*/
				
				
				break;		
			case 9:
				
				
				/*for (i = 50; i < 51; i++) {
					for (j = 0; j < 9 ; j++){
					        
						if(servent_peer_list[i][j].peer_id != -1){
						servent_peer_list[i][j].reputation_value = 5;
						}
						
						
						
					}
				}*/
				//request_time_largest = 2000;
				
				
				
				
				
			
				break;	
			case 10:
				/*for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS;i++){
				servent_last_time_select_pattern[i] = 0;
				if(servent_eliminate_signal[i] != 1){

				printf("servent_last_time_select_pattern[%d] %d !! \n",i,servent_last_time_select_pattern[i]);
				}
				
				}*/
				f = fopen("request_time_record.txt" , "w");
				if(!f){
				printf("data not exist");
				system("PAUSE");
				}
				for(i=0;i<request_time_record_pointer;i++){
				sprintf(string, "%d:request_time_largest.%ld !\n",i+1,request_time_record_array[i]);
				puts(string);
				fwrite( string, 1,strlen(string), f );
				}	

				
				fclose(f);
				
				f = fopen("latency_num_record.txt" , "w");
				if(!f){
				printf("data not exist");
				system("PAUSE");
				}
				for(i=0;i<latency_num_record_pointer;i++){
				sprintf(string, "%d.servent_latency_num:%d,crawler_latency_num:%d,sensor_latency_num:%d !\n"
				,i+1,latency_num_record_array[i].servent_latency_num,latency_num_record_array[i].crawler_latency_num,latency_num_record_array[i].sensor_latency_num);
				
				puts(string);
				fwrite( string, 1,strlen(string), f );
				}	
				fclose(f);
				
				printf("enumeration_start_time %d\n",enumeration_start_time);
				printf("vs:%d !\n",vs);
				printf("request_time_largest:%ld !\n",request_time_largest);
				printf("request_hour:%d !\n",request_hour);
				
				for(i=0;i<request_time_record_pointer;i++){
				printf("request_time_record_array[%d]:%ld !! \n",i,request_time_record_array[i]);
				
				}
				break;						
			default:
				sleep(10);
				break;
				
	    	
		}
		
		
		
		
		
		sleep(0.8);
		
    
	}
	
	/*for(i=0;i<NUM_SERVENT_BOTS;i++){
				    
				    if(servent_eliminate_signal[i]!= 1){
				    printf("peer list of servent %ld have:", i);
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				    if(servent_peer_list[i][j].peer_id != -1){
					printf("%ld %ld  ", servent_peer_list[i][j].peer_id, servent_peer_list[i][j].reputation_value);
					}
				    }	
				    puts("");
				    }
	}
	for(i=NUM_SERVENT_BOTS;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS;i++){
				    
				    if(servent_eliminate_signal[i]!= 1){
				    printf("peer list of fake_servent %ld have:", i);
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				    if(servent_peer_list[i][j].peer_id != -1){
					printf("%ld %ld  ", servent_peer_list[i][j].peer_id, servent_peer_list[i][j].reputation_value);
					}
				    }	
				    puts("");
				    }
	}
		*/
	data_record_terminate_signal=1;	
	int crawler_in_servent_peerlist_num=0,sensor_in_servent_peerlist_num=0,sensor_in_client_peerlist_num=0,crawler_in_client_peerlist_num=0;
	for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
				    
		if(servent_eliminate_signal[i]!= 1){
				    
				    
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				    	if(servent_peer_list[i][j].peer_id != -1 && servent[servent_peer_list[i][j].peer_id].fake_signal == 1){
						crawler_in_servent_peerlist_num++;
						
						break;
					
				    	}
				    	
				    	
				    }	
				    
				   
		}
	}
	for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
				    
		if(servent_eliminate_signal[i]!= 1){
				    
				    
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				    	
				    	if(servent_peer_list[i][j].peer_id != -1 && servent[servent_peer_list[i][j].peer_id].sensor_signal == 1){
						sensor_in_servent_peerlist_num++;
						
						break;
				    	}
				    	
				    }	
				    
				   
		}
	}
	for(i=0;i<NUM_CLIENT_BOTS;i++){
				    
				    
				    
				    
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				        if(client_master[i][j].master_id != -1 && servent[client_master[i][j].master_id].fake_signal == 1){
						crawler_in_client_peerlist_num++;
						break;
					}
					
				    }	
				    
				    
	}
	for(i=0;i<NUM_CLIENT_BOTS;i++){
				    
				    
				    
				    
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				        
					if(client_master[i][j].master_id != -1 && servent[client_master[i][j].master_id].sensor_signal == 1){
						sensor_in_client_peerlist_num++;
						break;
					}
				    }	
				    
				    
	}
	f = fopen("servent_peerlist_record.txt" , "w");
	if(!f){
	printf("data not exist");
	system("PAUSE");
	}
	for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
				    
				    if(servent_eliminate_signal[i]!= 1){
				    if(servent_latency_signal[i]== 1){
				    servent_latency_num++;
				    }
				    //printf("peer list of servent %ld have:", i);
				    sprintf(record_data, "peer list of servent %ld have:",i  );
				    fwrite( record_data, 1,strlen(record_data), f );
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				    if(servent_peer_list[i][j].peer_id != -1){
					//printf("%ld %ld  ", servent_peer_list[i][j].peer_id, servent_peer_list[i][j].reputation_value);
					sprintf(record_data, "%ld %ld  ", servent_peer_list[i][j].peer_id, servent_peer_list[i][j].reputation_value);
					fwrite( record_data, 1,strlen(record_data), f );
					}
				    }	
				    //puts("");
				    fwrite( "\n", 1,1, f );
				    }
	}	
	sprintf(record_data, "latency bot have %d !\n",servent_latency_num  );
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data,"crawler_in_servent_peerlist_num:%d !\n",crawler_in_servent_peerlist_num);
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data,"sensor_in_servent_peerlist_num:%d !\n",sensor_in_servent_peerlist_num);
	fwrite( record_data, 1,strlen(record_data), f );
	fclose(f);
	
	f = fopen("client_peerlist_record.txt" , "w");
	if(!f){
	printf("data not exist");
	system("PAUSE");
	}
	for(i=0;i<NUM_CLIENT_BOTS;i++){
				    
				    
				    
				    sprintf(record_data, "peer list of client %ld have:",i  );
				    fwrite( record_data, 1,strlen(record_data), f );
				    for (j = 0; j < NUM_SERVENT_PEER; j++) {
				    if(client_master[i][j].master_id != -1){
					//printf("%ld %ld  ", servent_peer_list[i][j].peer_id, servent_peer_list[i][j].reputation_value);
					sprintf(record_data, "%ld %ld  ", client_master[i][j].master_id, client_master[i][j].reputation_value);
					fwrite( record_data, 1,strlen(record_data), f );
					}
				    }	
				    //puts("");
				    fwrite( "\n", 1,1, f );
				    
	}	
	sprintf(record_data,"crawler_in_client_peerlist_num:%d !\n",crawler_in_client_peerlist_num);
	fwrite( record_data, 1,strlen(record_data), f );
	sprintf(record_data,"sensor_in_client_peerlist_num:%d !\n",sensor_in_client_peerlist_num);
	fwrite( record_data, 1,strlen(record_data), f );
	fclose(f);
	
	f = fopen("servent_log.txt" , "w");
	if(!f){
	printf("data not exist");
	system("PAUSE");
	}
	for(i=0;i<NUM_SERVENT_BOTS+NUM_FAKE_SERVENT_BOTS+NUM_SENSOR;i++){
	sprintf(record_data,"Log of servent %ld :\n", i);
	fwrite(record_data, 1,strlen(record_data), f );
	for (j = 0; j < servent_execute_pattern_time[i]; j++) {
	if(servent_log[i][j].num_pattern == 3 && servent[i].fake_signal != 1){
	sprintf(record_data,"%d:pattern_num:%d|communication_target:%d|communication_result:%d|receive_peer:%d|send_peer:%d|now_sec:%d !\n",j,servent_log[i][j].num_pattern,servent_log[i][j].communication_target,servent_log[i][j].communication_result,servent_log[i][j].receive_peer,servent_log[i][j].send_peer,servent_log[i][j].now_sec);
	fwrite(record_data, 1,strlen(record_data), f );
	}
	else if(servent_log[i][j].num_pattern == 2 && servent[i].fake_signal == 1){
	sprintf(record_data,"%d:pattern_num:%d|communication_target:%d|communication_result:%d|receive_peer:%d|send_peer:%d|now_sec:%d !\n",j,servent_log[i][j].num_pattern,servent_log[i][j].communication_target,servent_log[i][j].communication_result,servent_log[i][j].receive_peer,servent_log[i][j].send_peer,servent_log[i][j].now_sec);
	fwrite(record_data, 1,strlen(record_data), f );
	}
	else if(servent_log[i][j].num_pattern != 3 && servent[i].fake_signal != 1){
	sprintf(record_data,"%d:pattern_num:%d|communication_target:%d|communication_result:%d|now_sec:%d !\n",j,servent_log[i][j].num_pattern,servent_log[i][j].communication_target
	,servent_log[i][j].communication_result,servent_log[i][j].now_sec);
	fwrite(record_data, 1,strlen(record_data), f );
	}
	
	}
	//puts("");
	}
	fclose(f);
	
	f = fopen("client_log.txt" , "w");
	if(!f){
	printf("data not exist");
	system("PAUSE");
	}
	for(i=0;i<NUM_CLIENT_BOTS;i++){
	sprintf(record_data,"Log of client %ld :\n", i);
	fwrite(record_data, 1,strlen(record_data), f );
	for (j = 0; j < client_execute_pattern_time[i]; j++) {
	if(client_log[i][j].num_pattern == 2 ){
	sprintf(record_data,"%d:pattern_num:%d|communication_target:%d|communication_result:%d|receive_peer:%d|send_peer:%d|now_sec:%d !\n",j,client_log[i][j].num_pattern,client_log[i][j].communication_target,client_log[i][j].communication_result,client_log[i][j].receive_peer,client_log[i][j].send_peer,client_log[i][j].now_sec);
	fwrite(record_data, 1,strlen(record_data), f );
	}
	else if(client_log[i][j].num_pattern != 2 ){
	sprintf(record_data,"%d:pattern_num:%d|communication_target:%d|communication_result:%d|now_sec:%d !\n",j,client_log[i][j].num_pattern,client_log[i][j].communication_target
	,client_log[i][j].communication_result,client_log[i][j].now_sec);
	fwrite(record_data, 1,strlen(record_data), f );
	}
	
	}
	//puts("");
	}
	fclose(f);
	
	
	
	
	f = fopen("request_time_record.txt" , "w");
	if(!f){
	printf("data not exist");
	system("PAUSE");
	}
	for(i=0;i<request_time_record_pointer;i++){
	sprintf(string, "%d:request_time_largest.%ld !\n",i+1,request_time_record_array[i]);
	puts(string);
	fwrite( string, 1,strlen(string), f );
	}	

	
	fclose(f);
	
	f = fopen("latency_num_record.txt" , "w");
	if(!f){
	printf("data not exist");
	system("PAUSE");
	}
	for(i=0;i<latency_num_record_pointer;i++){
	sprintf(string, "%d.servent_latency_num:%d,crawler_latency_num:%d,sensor_latency_num:%d !\n"
	,i+1,latency_num_record_array[i].servent_latency_num,latency_num_record_array[i].crawler_latency_num,latency_num_record_array[i].sensor_latency_num);
	
	puts(string);
	fwrite( string, 1,strlen(string), f );
	}	
	fclose(f);
	
	f = fopen("period_time_record.txt" , "w");
	if(!f){
	printf("data not exist");
	system("PAUSE");
	}
	for(i=0;i<change_period_time_pointer;i++){
	if(change_period_time[i].off_peak_to_peak == 1){
	sprintf(string, "%d:change_period_time:start %d,end %d,off_peak_to_peak !\n",i+1,change_period_time[i].start_time,change_period_time[i].end_time);
	}
	else if(change_period_time[i].peak_to_off_peak == 1){
	sprintf(string, "%d:change_period_time:start %d,end %d,peak_to_off_peak !\n",i+1,change_period_time[i].start_time,change_period_time[i].end_time);
	}
	puts(string);
	fwrite( string, 1,strlen(string), f );
	}	
	fclose(f);

	pthread_join(data_record,NULL);	
	
	printf("vc:%d \n", vc);
	printf("vrc:%d \n", vrc);
	printf("vs:%d \n", vs);
	
	puts("correct111");
	pthread_join(relay_station,NULL);
	puts("correct222");
	pthread_join(infect_and_inject_enumeration,NULL);
	puts("correct333");
	pthread_join(boot_control,NULL);
	puts("correct444");		
	for (t = 0; t < (NUM_SERVENT_BOTS/50); t++){ // /1000
		pthread_join(servent_threads[t],NULL);	
	}
	puts("correct1");
	for (t = 0; t < (NUM_CLIENT_BOTS/50); t++){  // /1000
		pthread_join(client_threads[t],NULL);	
	}
	puts("correct2");
	if(inject_signal==1){  
	for (t = 0; t < (NUM_FAKE_SERVENT_BOTS/5); t++){ // /1000
		pthread_join(fake_servent_threads[t],NULL);	
	}
	for (t = 0; t < (NUM_SENSOR/5); t++){ // /1000
		pthread_join(sensor_threads[t],NULL);	
	}
	}
	puts("correct3");
	
    
    
    pthread_exit(NULL);
	
    return 0; 
    
}
