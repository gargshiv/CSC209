#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    char *ch = "ls -l";
    char store[255];
    //char *e = '\n';
    system(ch);
    while(fgets(store, 255, stdin) != NULL ){
       // printf("%s \n", store);
    }
}//&& strcmp(store, e ) != 0