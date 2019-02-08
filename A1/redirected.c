#include <stdio.h>
int main(){
    char message[1024];
    fgets(message, 1024, stdin);
    char c = fgetc(stdin);
    while (c  != EOF){
        printf("%s", message);
    //fgets(message, 1024, stdin);
   // c =fgetc(stdin);
    
}
//fclose(stdin);
}