#ifndef WORKER_H
#define WORKER_H

#define PATHLENGTH 128
#define MAXRECORDS 100
#define MAXFREQRECORDS 100
#define MAXWORKERS 10

// This data structure is used by the workers to prepare the output
// to be sent to the master process.

typedef struct {
    int freq;
    char filename[PATHLENGTH];
} FreqRecord;

FreqRecord *get_word(char *word, Node *head, char **file_names);
void print_freq_records(FreqRecord *frp);
void run_worker(char *dirname, int in, int out);
void sort_the_array(FreqRecord **freqarray, int size);
void initialise_array(FreqRecord **freqarray, int size);

#endif /* WORKER_H */
