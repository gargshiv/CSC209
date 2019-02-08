#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include "freq_list.h"
#include "worker.h"

/* Initialise the values of the FreqRecord array for the given size.
*/
void initialise_array( FreqRecord **freqarray, int size){

    FreqRecord *freq_array = *freqarray;
    for (int i = 0; i < size; i++)
    {
        freq_array[i].freq = 0;
        strcpy(freq_array[i].filename, "");
    }
}

/* Sort the array of FreqRecord elements in order of frequency from highest to lowest.
 *  Take special care when the size exceeds the MAXRECORDS specification, since the
 *  maximum number of records in the array can only be MAXRECORDS.
 */
void sort_the_array(FreqRecord **freqarray, int size)
{
  if(size == 1){
    return;
  }
    FreqRecord *freq_array = *freqarray;
    //Using Bubble Sort : Time Complexity -> O(n^2), Space Complexity-> O(1)
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size - i; j++)
        {
            if (freq_array[j].freq < freq_array[j+1].freq) //Camparing of highest to lowest
            {
                char temp_word[MAXWORD] = "";
                int temp_freq = 0;
                //Swapping values using temporary variables
                strcpy(temp_word, freq_array[j].filename);
                temp_freq = freq_array[j].freq;

                freq_array[j].freq = freq_array[j+1].freq;
                strcpy(freq_array[j].filename, freq_array[j+1].filename);

                freq_array[j+1].freq = temp_freq;
                strcpy(freq_array[j+1].filename, temp_word);
            }
        }
    }
    // Setting the rest of the elements to 0
    if(size < MAXRECORDS){
        for (int i = size; i < MAXRECORDS; i++)
        {
        strcpy(freq_array[i].filename, "");
        freq_array[i].freq = 0;
        }
    }
    //If size exceeds MAXRECORDS, then setting the rest of the sorted elements to 0 and empty string
    else
    {
        for (int i = MAXRECORDS; i < size; i++)
        {
            strcpy(freq_array[i].filename, "");
            freq_array[i].freq = 0;
        }
    }
}

/* Returns an array of FreqRecord elements that contain frequency and filename
*  for a particular word in the linked list. If there is no such word and/or to 
*  indicate the end of valid records, return an array of elements that has the 
*  last frequency as 0 and an empty string as its file name.
*/
FreqRecord *get_word(char *word, Node *head, char **file_names)
{
    int count = 0;
    while (file_names[count] != NULL) //counting the number of files
        count++;
    //Declaring an array of count + 1 elements
    FreqRecord *freq_record = malloc(sizeof(FreqRecord) * (count + 1));
    //Assigning the first element of the linked list
    Node *curr = head;
    while (curr != NULL)
    {
        if (strcmp(curr->word, word) == 0)
        {
            //printf("wor %s \n", word);
            for (int i = 0; i < count; i++)
            {
                //if(curr->freq[i] != 0){
                (freq_record[i]).freq = curr->freq[i];
        
               // printf("word frequency is %d \n", freq_record[i].freq);
                strcpy(freq_record[i].filename, file_names[i]);
            }
            //Assigning the last element to 0 and empty string
            (freq_record[count]).freq = 0;
            strcpy(freq_record[count].filename, "");
            return freq_record;
        }
        curr = curr->next;
    }
    // If there was no such word in the linked list
    free(freq_record);
    //Reinitialise the freq record array
    freq_record = malloc(sizeof(FreqRecord));
    (freq_record[0]).freq = 0;
    strcpy(freq_record[0].filename, "");

    return freq_record;
}

/* Print to standard output the frequency records for a word.
* Use this for your own testing and also for query.c
*/
void print_freq_records(FreqRecord *frp)
{
    int i = 0;
    while (frp != NULL && frp[i].freq != 0)
    {
        printf("%d    %s\n", frp[i].freq, frp[i].filename);
        i++;
    }
}

/*  Uses the path to a directory (that contain index and filenames)
*   to load the indexer. Using file descriptors in and out, read one word, 
*   look for the word in the index, and write to out one FreqRecord for 
*   each file in which the word has a non-zero frequency. Then write one 
*   final FreqRecord with a frequency of zero and an empty string for the filename.
*/

void run_worker(char *dirname, int in, int out)
{
    // Useful variables :)
    char *word = malloc(sizeof(char) * MAXWORD);
    char *indexfile = malloc(sizeof(char) * MAXLINE);
    char *filenamefile = malloc(sizeof(char) * MAXLINE);
    // Create Start Node
    Node *start = create_node("", 0, 0);
    // Initilaise the filenames
    char **filenames = init_filenames();

    // Set the index and filenames file
    strncpy(indexfile, dirname, strlen(dirname) + 1);
    strncpy(filenamefile, dirname, strlen(dirname) + 1);
    strncat(indexfile, "/index", strlen(indexfile) + strlen("/index") + 1);
    strncat(filenamefile, "/filenames", strlen(filenamefile) + strlen("/filenames") + 1);

    // Load the index into a data structure
    read_list(indexfile, filenamefile, &start, filenames);

    // Read a Word
    if (read(in, word, MAXWORD) == -1)
    {
        perror("read");
        exit(1);
    }

    char passing_word[32] = "";
    strncpy(passing_word, word, strlen(word) - 1);

    // Get the records from get_word
    FreqRecord *freqrecord = malloc(sizeof(FreqRecord) * MAXFREQRECORDS);
    freqrecord = get_word(passing_word, start, filenames);

    for (int i = 0; i <MAXFREQRECORDS; i++)
    {   
        // Indicating end of RECORDS
        if (strcmp(freqrecord[i].filename, "") == 0)
        {
            break;
        }
        // Writing a freqrecord for a non zero frequency
        if (freqrecord[i].freq > 0)
        {
            FreqRecord *writing_out = malloc(sizeof(FreqRecord));
            strcpy(writing_out->filename, freqrecord[i].filename);
            writing_out->freq = freqrecord[i].freq;

            if (write(out, writing_out, sizeof(FreqRecord)) == -1)
            {
                perror("write to pipe");
                exit(1);
            }
            free(writing_out);
        }
    }
    free(freqrecord);
    // Write one Final FreqRecord having frequency 0 and filename as empty string
    FreqRecord *ending_freqrecord = malloc(sizeof(FreqRecord));
    ending_freqrecord->freq = 0;
    strcpy(ending_freqrecord->filename, "");

    if (write(out, ending_freqrecord, sizeof(FreqRecord)) == -1)
    {
        perror("write to pipe");
        exit(1);
    }
    // Free space of the malloc calls
    free(ending_freqrecord);
    free(indexfile);
    free(filenamefile);

}


