#include <stdio.h>
#include <string.h>
#include <stdlib.h>



char* basename(char *path){
    int len = strlen(path);
    char message[len];
    char *r = malloc(sizeof(char) * len);
    strncpy(message, path, len);
    for(int i = 0; i < len; i ++){
        if(message[i] == "/"){
            printf("%d", message[i]);
        }
    }
    return r;
}
int main () {


char *path1 = "/usr/include";
char *result1 = basename(path1);
char *path2 = "/usr/include/";
char *result2 = basename(path2);
char *path3 = "file.txt";
char *result3 = basename(path3);
char *path4 = "./a2/test/test1.in";
char *result4 = basename(path4);
printf("%s, %s, %s, %s\n",result1, result2, result3, result4);
}

