#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int main(){
    FILE* f;
    char data[1024]; 
    int receive_ID[5];
    int bot_command;
    int i=0;
    int int_data;

    f = fopen("command.txt" , "r");
    if(!f){
        printf("data not exist");
        system("PAUSE");
        return 0;
    }
    fgets(data, 1024, f);
    for (i = 0; i < 5; i++) {
		fgets(data, 1024, f);
		int_data = (data[0]-48);
		if(int_data != -3){
			receive_ID[i] = data[0] -48;
			printf("1receive_ID: %d\n", receive_ID[i]);
		}
		else{
			receive_ID[i] = data[0] -46;
			printf("2receive_ID: %d\n", receive_ID[i]);
			break;
		}
	}

    
    
    fclose(f);




}
