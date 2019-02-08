#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hcq.h"
#define INPUT_BUFFER_SIZE 256
#define OUT_BUF_SIZE 1024


int main(){
    Ta *head = malloc(sizeof(Ta));
    head->name = malloc(sizeof(char) *10);
    head->next = NULL;
    strcpy(head->name, "shivam");
    char *mess = print_currently_serving(head);
    printf("%s ", mess);

}