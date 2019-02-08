#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include "freq_list.h"
#include "worker.h"
// --------------------TESTING PART 1----------------------

void my_own_tests(){
    char **fnames = malloc(sizeof(char *) * 2);
    char *f1 = "file1";
    char *f2 = "file2";
    fnames[0] = f1;
    fnames[1] = f2;

    Node *first = malloc(sizeof(Node));
    first->freq[0] = 3;
    first->freq[1] = 4;
    strcpy(first->word, "i am");

    Node *second = malloc(sizeof(Node));
    second->freq[0] = 1;
    second->freq[1] = 2;
    strcpy(second->word, "done");
    first->next = second;

    Node *third = malloc(sizeof(Node));
    third->freq[0] = 5;
    third->freq[1] = 6;
    strcpy(third->word, "with");
    second ->next = third;

    Node *fourth = malloc(sizeof(Node));
    fourth->freq[0] = 3;
    fourth->freq[1] = 5;
    strcpy(fourth->word, "part 1");
    third->next = fourth;

    char word[MAXWORD] = "hi";

    FreqRecord *fre = get_word(word, first, fnames);
    print_freq_records(fre);
    printf("\n");

    strcpy(word, "i am");
    fre = get_word(word, first, fnames);
    print_freq_records(fre);
    printf("\n");

    strcpy(word, "part 1");
    fre = get_word(word, first, fnames);
    print_freq_records(fre);
    printf("\n");

    Node *fifth = malloc(sizeof(Node));
    fifth->freq[0] = 60;
    fifth->freq[1] = 70;
    strcpy(fifth->word, "i love");
    fourth->next = fifth;

    Node *sixth = malloc(sizeof(Node));
    sixth->freq[0] = 10;
    sixth->freq[1] = 110;
    strcpy(sixth->word, "blueberry");
    fifth->next = sixth;

    strcpy(word, "i am");
    fre = get_word(word, first, fnames);
    print_freq_records(fre);
    printf("\n");

    strcpy(word, "done");
    fre = get_word(word, first, fnames);
    print_freq_records(fre);
    printf("\n");

    strcpy(word, "with");
    fre = get_word(word, first, fnames);
    print_freq_records(fre);
    printf("\n");

    Node *seventh = malloc(sizeof(Node));
    seventh->freq[0] = 90;
    seventh->freq[1] = 2160;
    strcpy(seventh->word, "snowcones");
    sixth->next = seventh;

    strcpy(word, "random");
    fre = get_word(word, first, fnames);
    print_freq_records(fre);
    printf("\n");

    strcpy(word, "snowcones");
    fre = get_word(word, first, fnames);
    print_freq_records(fre);
    printf("\n");

    free(first);
    free(second);
    free(third);
    free(fourth);
    free(fifth);
    free(sixth);
    free(seventh);
}

void test_on_the_picture_given_on_quercus(){

    char **fnames = malloc(sizeof(char *) * 2);
    char *f1 = "Menu1";
    char *f2 = "Menu2";
    fnames[0] = f1;
    fnames[1] = f2;

    Node *first = malloc(sizeof(Node));
    first->freq[0] = 0;
    first->freq[1] = 1;
    strcpy(first->word, "pepper");

    Node *second = malloc(sizeof(Node));
    second->freq[0] = 11;
    second->freq[1] = 3;
    strcpy(second->word, "potato");
    first->next = second;

    Node *third = malloc(sizeof(Node));
    third->freq[0] = 4;
    third->freq[1] = 0;
    strcpy(third->word, "salad");
    second ->next = third;

    Node *fourth = malloc(sizeof(Node));
    fourth->freq[0] = 2;
    fourth->freq[1] = 6;
    strcpy(fourth->word, "spinach");
    third->next = fourth;

    char word[MAXWORD] = "spinach";

    FreqRecord *fre = get_word(word, first, fnames);
    print_freq_records(fre);
    printf("\n");

    strcpy(word, "pepper");
    fre = get_word(word, first, fnames);
    print_freq_records(fre);
    printf("\n");

    strcpy(word, "potato");
    fre = get_word(word, first, fnames);
    print_freq_records(fre);
    printf("\n");


    free(first);
    free(second);
    free(third);
    free(fourth);

}

void test_on_zero_frequencies(){
    char **fnames = malloc(sizeof(char *) * 2);
    char *f1 = "A3";
    char *f2 = "A4";
    fnames[0] = f1;
    fnames[1] = f2;

    Node *first = malloc(sizeof(Node));
    first->freq[0] = 30;
    first->freq[1] = 10;
    strcpy(first->word, "A1 was amazing");

    Node *second = malloc(sizeof(Node));
    second->freq[0] = 40;
    second->freq[1] = 50;
    strcpy(second->word, "A2 was great");
    first->next = second;

    Node *third = malloc(sizeof(Node));
    third->freq[0] = 20;
    third->freq[1] = 40;
    strcpy(third->word, "A4 hmm?");
    second ->next = third;

    Node *fourth = malloc(sizeof(Node));
    fourth->freq[0] = 100;
    fourth->freq[1] = 100;
    strcpy(fourth->word, "A3 i'll do well!");
    third->next = fourth;

    char word[MAXWORD] = "A3 i'll do well!";

    FreqRecord *fre = get_word(word, first, fnames);
    print_freq_records(fre);
    printf("\n");

    strcpy(word, "A2 was great");
    fre = get_word(word, first, fnames);
    print_freq_records(fre);
    printf("\n");

    free(first);
    free(second);
    free(third);
    free(fourth);
    
}

int main(){
    printf("--------------------NEW TEST------------------ \n");

    /* Works fine since all frequencies are above 0 and words are not null
    */
    my_own_tests();

    printf("--------------------NEW TEST------------------ \n");

    test_on_the_picture_given_on_quercus();

    /* Works fine since the value returned are the same as the one in the picture
    */ 

    printf("--------------------NEW TEST------------------\n");

    /* Works as expected. The key thing to note here is that if a frequency is 0 and there
    is a frequency after that that is non zero, then that non zero frequency will not be 
    printed since freq_records only checks up till it receives a 0. Although all the non
    zero values are returned, it isnt printed. I take care of this in the run worker function
    since there i check whether is frequency is above 0 or not.*/
    test_on_zero_frequencies();

   // exit(0);

}
