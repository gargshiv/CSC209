#include <stdio.h>
#include <string.h>
int main(int argc, char **argv){
   
    if(argc > 2){
        fprintf(stderr, "USAGE: most_processes[ppid]\n");
        return 1;
    }

    if(argc == 1){

    char message[1024];
    char name[31];
    char display_name[31];
    char prev[31];
    int i = 0;
    int count_current = 1;
    int display_count = 1;
    int file_not_empty = 0;
    int f = 0;
    while(fgets(message, 1024, stdin) != NULL){

        file_not_empty = 1;
        for(int k = 0; k < 31; k ++){
            name[k] = '\0';
        }
        while(message[i] != ' '){
            name[i] = message[i];
            i ++;
        }

        
        if(strcmp(name, prev) == 0){
            count_current ++;
        }else {
            count_current = 1;
            
        }

         int loca = 0;
         if(display_count < count_current){
             f = 1;
             display_count = count_current;
             for(int l = 0; l < 20; l ++){
                 display_name[l] = '\0';
             }
             while(prev[loca] != '\0'){
             display_name[loca] = prev[loca];
             loca++;
                
             }
         }


        for(int k = 0; k < 31; k ++){
        prev[k] = '\0';
        }
         for(int k = 0; k < i; k ++){
             prev[k] = name[k];
         }
     
        i = 0;
    }

    int loc = 0;
    if(f == 1 && file_not_empty == 1){
    while(display_name[loc] != '\0'){
        
        printf("%c", display_name[loc]);
        loc++;
        
    }
    
    printf(" %d", display_count);
    }
   
    fclose(stdin);
    }


    else if(argc == 2){
    char message[1024];
    char ppid[31];
    int count = 1;
    int display_count = 1;
    char display_name[31] = {};
    char current_name[31] = {};
    char prev[31];
    int f = 0;
    int file_not_empty = 0;
    while(fgets(message, 1024, stdin) != NULL){
        file_not_empty = 1;
        
        for(int i = 0; i < 31; i ++){
            current_name[i] = '\0';
            ppid[i] = '\0';
        }
        
        int derive = 0;
        int pos = 0;
        while(message[derive] != '\0'){
            if(message[derive] == ' '){
                derive ++;
                break;
            }
            current_name[pos] = message[derive];
            pos++;
            derive++;
        }
        int flag = 0;
        while(message[derive] != '\0'){
            if(message[derive] >= 48 && message[derive] <=57 ){
                flag = 1;
                derive++;
                
            }else if(message[derive] == ' ' && flag == 0){
                derive++;
                
            }else if(message[derive] == ' ' && flag == 1){
                break;
            }
            
        }
        derive++;
        int loc = 0;
        flag = 0;
        while(message[derive] != '\0'){
            if(message[derive] == ' ' && flag == 0){
                derive++;
            }
            else if (message[derive] >= 48 && message[derive] <= 57){
                ppid[loc] = message[derive];
                loc++;
                derive++;
                flag = 1;
            }else
                break;
        }
        //printf("ppid %s", ppid);
        //printf("\n");
        
        if(strcmp(ppid, argv[1]) == 0){
            if(strcmp(prev, current_name) == 0){
                count ++;
            }
            else{
                count = 1;
            }
            if(display_count < count){
                f = 1;
                display_count = count;
                for(int l = 0; l < 20; l ++){
                 display_name[l] = '\0';
             }
                for(int i = 0; i < 31; i ++){
                    display_name[i] = prev[i];
                    
                }
            }
            for(int i = 0; i < 31; i ++){
                prev[i] = current_name[i];
            }
                     
        }        

    }
    int loc = 0;
    
    if(f == 1 && file_not_empty == 1){while(display_name[loc] != '\0'){
        
        printf("%c", display_name[loc]);
        loc++;
        
    }
    
    printf(" %d", display_count);
    
   
    fclose(stdin);
    
    }
    }
    printf("\n");
    return 0;
}