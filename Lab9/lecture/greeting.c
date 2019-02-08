#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

char name[32];

void sing(int code){
    
    if(code != SIGINT){
    printf("happy birthday TO YOU %s \n", name);
    sleep(10);
    }
    

}

int main(int argc, char **argv){

    struct sigaction newact;
    newact.sa_handler = sing;
    newact.sa_flags = 0;
    sigemptyset(&newact.sa_mask);
    sigaction(SIGUSR1, &newact, NULL);

    if(argc > 2){
        exit(0);
    }

    strcpy(name, argv[1]);
    while(1){

    }
    
}
