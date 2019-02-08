#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>

#include "freq_list.h"
#include "worker.h"

/* Write data on the file decriptor.
 */
void WRITE(int file_descriptor, const void* buf, size_t n){
    if(write(file_descriptor, buf, n) == -1){
        perror("write");
        exit(1);
    }
}

/* Close the file descriptor.
 */
void CLOSE( int file_descriptor){
    if(close(file_descriptor) == -1){
        perror("close");
        exit(1);
    }
}

/*  Return 1 if the dirent has index files, filenames file, regularfiles, git files, etc, which are 
 *  files not required for the execution of A3.
 *  Return 0 if the dirent has no such file.
 */
int if_on_dir_names(struct dirent *dp1)
{
    if (strcmp(dp1->d_name, ".") == 0 ||
        strcmp(dp1->d_type, ".txt") == 0 ||
        strcmp(dp1->d_name, "..") == 0 ||
        strcmp(dp1->d_name, ".svn") == 0 ||
        strcmp(dp1->d_name, ".git") == 0 ||
        strcmp(dp1->d_name, ".gitkeep") == 0 ||
        strcmp(dp1->d_name, ".txt") == 0 ||
        strcmp(dp1->d_name, "regular_file") == 0 ||
        strcmp(dp1->d_name, "index") == 0 ||
        strcmp(dp1->d_name, "filenanames") == 0 ||
        strcmp(dp1->d_name, ".gitignore") == 0)
    {
        return 1;
    }
    return 0;
}

/* The "FUN" part of A3. This program works like Google's search engine! It's reads input from the user,
 * creates multiple processes for each worker, the worker calls run_worker to get the freq_records,
 * writes to the parent, the parent reads the freq_records produced by all workers, creates a sorted
 * master_frequency_array, and prints the result using print_records.
 */
int main(int argc, char **argv)
{

    char ch;
    char *startdir = ".";
    int no_of_subdirectories = 0;
    char *input_word;


    /* Using getopt to process command-line flags and arguments */
    while ((ch = getopt(argc, argv, "d:")) != -1)
    {   
        switch (ch)
        {
        case 'd':
            startdir = optarg;
            break;
        default:
            fprintf(stderr, "Usage: queryone [-d DIRECTORY_NAME]\n");
            exit(1);
        }
    }

    // Open the directory provided by the user (or current working directory)
    DIR *dirp1;
    if ((dirp1 = opendir(startdir)) == NULL)
    {
        perror("opendir");
        exit(1);
    }

    // Counting the number of subdirectories, to determine the number of processes to be created
    struct dirent *dp1;
    while ((dp1 = readdir(dirp1)) != NULL)
    {
        if (if_on_dir_names(dp1) == 1) // Checking whether it's a valid directory
            continue;
        no_of_subdirectories++;
    }
    // Closing first instance of Directory
    if (closedir(dirp1) < 0)
        perror("closedir");

    // "Fun"
    while (1)
    {
        DIR *dirp2;
        char path[PATHLENGTH];
        FreqRecord *master_frequency_array = malloc(sizeof(FreqRecord *) * MAXRECORDS * no_of_subdirectories);
        initialise_array(&master_frequency_array, no_of_subdirectories * MAXRECORDS);
        
        //Opening the Target Directory
        if ((dirp2 = opendir(startdir)) == NULL)
        {
            perror("opendir");
            exit(1);
        }

        int fd[no_of_subdirectories][2];
        int r = 0;
        input_word = malloc(sizeof(char) * MAXWORD);

        // Reading the word from the STDIN
        if (read(STDIN_FILENO, input_word, MAXWORD) == -1)
        {
            perror("read");
            exit(1);
        }

        struct dirent *dp;
        // Looping over each Subdirectory
        for (int number = 0; number < no_of_subdirectories; number++)
        {
            // Opening the Subdirectory
            dp = readdir(dirp2);
        
            if (dp == NULL)
                break;

            if (if_on_dir_names(dp) == 1)
            {
                number--; // Because we don't want this dirent to be counted as a directory
                continue;
            }

            strncpy(path, startdir, PATHLENGTH);
            strncat(path, "/", PATHLENGTH - strlen(path));
            strncat(path, dp->d_name, PATHLENGTH - strlen(path));
            path[PATHLENGTH - 1] = '\0';

            struct stat sbuf;
            if (stat(path, &sbuf) == -1)
            {
                // This should only fail if we got the path wrong
                // or we don't have permissions on this entry.
                perror("stat");
                exit(1);
            }

            if (pipe(fd[number]) == -1)
            {
                perror("pipe");
                exit(1);
            }

            r = fork();
            // PARENT
            if (r > 0)
            {
                // Writing to the Child
                WRITE(fd[number][1], input_word, MAXWORD);
                
                // Closing the writing end since we don't need to write anymore
                CLOSE(fd[number][1]);

                // Waiting for child processes to end
                int status;
                if(wait(&status) == -1){
                    perror("wait");
                    exit(1);
                }

            }
            // CHILD
            else if (r == 0)
            {
                // call run_worker to process the index file contained in the directory
                if (S_ISDIR(sbuf.st_mode))
                    run_worker(path, fd[number][0], fd[number][1]);

                // Close all the reading ends for the other children to avoid incorrect data
                for (int i = 0; i <= number; i++)
                    CLOSE(fd[i][0]);


                // Close writing end of the child
                CLOSE(fd[number][1]);

                // Exit to not create any child processes of the child process
                exit(0);
            }
            else
            {   // In case, stuff doesn't go right :)
                perror("fork");
                exit(1);
            }
        }

        FreqRecord *retrieval = malloc(sizeof(FreqRecord));
        int current_size = 0;

        for (int i = 0; i < no_of_subdirectories; i++)
        {
            while (read(fd[i][0], retrieval, sizeof(FreqRecord)) > 0)
            {
                // If read does indicate the end of the pipe by passing empty string and 0
                if (strcmp(retrieval->filename, "") == 0)
                    break;
                // If frequency of the read freqrecord is non zero and the filename isnt empty
                if ((retrieval->freq > 0) && (strcmp(retrieval->filename, "") != 0))
                {
                    master_frequency_array[current_size].freq = retrieval->freq;
                    strcpy(master_frequency_array[current_size].filename, retrieval->filename);
                    current_size++; // To store next Freqrecord
                }
            }
        }

        // Sort the array using Current Size. Special cases are handled in the function
        sort_the_array(&master_frequency_array, current_size);

        // Print the freq records which are ordered by frequency
        print_freq_records(master_frequency_array);

        // Free malloc calls so that it can be utilised in the next RUN
        free(master_frequency_array);
        free(retrieval);
        free(input_word);

        // Close the write ends
        CLOSE(fd[0][0]);
        CLOSE(fd[1][0]);
        CLOSE(fd[2][0]);

        if (closedir(dirp2) < 0)
            perror("closedir");
    } // And, Repeat until it ends.
    
    return 0;
}