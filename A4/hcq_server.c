#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "hcq.h"

#ifndef PORT
#define PORT 30000
#endif
#define MAX_BACKLOG 5
#define MAX_CONNECTIONS 200
#define BUF_SIZE 1024

struct sockname
{
    int sock_fd;
    char *username;
    char *TA_or_Student;
    int no_of_reads;
    char *after;
    char buf[BUF_SIZE];
    int inbuf;
    int room;
    int nbytes;
};
Course *courses;
Ta *head_ta = NULL;
Student *head_student = NULL;

/* Accept a connection. Note that a new file descriptor is created for
 * communication with the client. The initial socket descriptor is used
 * to accept connections, but the new socket is used to communicate.
 * Return the new client's file descriptor or -1 on error.
 */
int accept_connection(int fd, struct sockname *usernames)
{
    int user_index = 0;
    while (user_index < MAX_CONNECTIONS && usernames[user_index].sock_fd != -1)
    {
        user_index++;
    }

    if (user_index == MAX_CONNECTIONS)
    {
        fprintf(stderr, "server: max concurrent connections\n");
        return -1;
    }

    int client_fd = accept(fd, NULL, NULL);
    if (client_fd < 0)
    {
        perror("server: accept");
        close(fd);
        exit(1);
    }

    usernames[user_index].sock_fd = client_fd;
    usernames[user_index].username = NULL;
    return client_fd;
}
/* Return the index where the buffer has a newline. If it doesn't, return -1.
*/
int find_newline(const char *buf, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (buf[i] == '\n')
        {
            return i;
        }
    }
    return -1;
}

/*  This is the major part of the assignment. It is responsible to read from every client,
    check what does the input mean, call appropriate methods from hcq.c and display the
    answer to the client. Also, it is implemented as a partial reads system so that the
    buffer of every client is treated independently. Return 0 if everything is okay, -1
    is there are no more reads or return the fd if the client has a too long username or
    if the student has been taken by a TA.
 */
int read_from(int client_index, struct sockname *usernames)
{
    setbuf(stdout, NULL);

    usernames[client_index].after = usernames[client_index].buf;
    // Outer loop to take care of partial reads
    while ((usernames[client_index].nbytes = read(usernames[client_index].sock_fd, usernames[client_index].after, usernames[client_index].room)) > 0)
    {

        usernames[client_index].inbuf += usernames[client_index].nbytes;
        int where;
        // inner loop to check if there was a newline found
        while ((where = find_newline(usernames[client_index].buf, usernames[client_index].inbuf)) > 0)
        {
            usernames[client_index].buf[where] = '\0';
            usernames[client_index].no_of_reads++;
            // if this is the first read from the client
            if (usernames[client_index].no_of_reads == 1)
            {
                usernames[client_index].username = malloc(sizeof(char) * BUF_SIZE);
                if(usernames[client_index].username ==  NULL){
                    perror("malloc");
                    exit(1);
                }
                if(strlen(usernames[client_index].buf) > 30){ //too long username
                    return client_index + 1;
                }
                strcpy(usernames[client_index].username, usernames[client_index].buf);
                char *tex = "Are you a TA or a Student (enter T or S)?\n";
                if (write(usernames[client_index].sock_fd, tex, strlen(tex)) == -1)
                {
                    perror("write");
                    exit(1);
                }
                
            }
            // if this is the second read from the client
            else if (usernames[client_index].no_of_reads == 2)
            {
                usernames[client_index].TA_or_Student = malloc(sizeof(char) * BUF_SIZE);
                if(usernames[client_index].TA_or_Student ==  NULL){
                    perror("malloc");
                    exit(1);
                }
                strncpy(usernames[client_index].TA_or_Student, usernames[client_index].buf, 1);
                // Segragating output based on whether it is a T or a S
                if (usernames[client_index].buf[0] == 'T')
                {
                    char *note = "Valid commands for a TA:\n\tstats\n\tnext\n\t(or use Ctrl-C to leave)\n";
                    if (write(usernames[client_index].sock_fd, note, strlen(note)) == -1)
                    {
                        perror("write");
                        exit(1);
                    }
                    add_ta(&head_ta, usernames[client_index].username);
                    
                }
                else if (usernames[client_index].buf[0] == 'S')
                {
                    char *note = "Valid courses: CSC108, CSC148, CSC209\nWhich course are you asking about?\n";
                    if (write(usernames[client_index].sock_fd, note, strlen(note)) == -1)
                    {
                        perror("write");
                        exit(1);
                    }
                } else {
                    char *note = "Invalid role (enter T or S)\n";
                    if (write(usernames[client_index].sock_fd, note, strlen(note)) == -1)
                    {
                        perror("write");
                        exit(1);
                    }
                    usernames[client_index].no_of_reads --;
                    
                }
                
            }
            // if this is the third read from the student
            else if (usernames[client_index].no_of_reads == 3 && usernames[client_index].TA_or_Student[0] == 'S')
            {
                char co_code[7];
                strncpy(co_code, usernames[client_index].buf, 6);
                co_code[6] = '\0';

                if (add_student(&head_student, usernames[client_index].username, co_code, courses, 3) == 0)
                {
                    char *not = "You have been entered into the queue. While you wait, you can use the command stats to see which TAs are currently seeing students.\n";
                    if (write(usernames[client_index].sock_fd, not, strlen(not)) == -1)
                    {
                        perror("write");
                        exit(1);
                    }
                }
                else
                {
                    char *not = "This is not a valid course. Good-bye\n";
                    if (write(usernames[client_index].sock_fd, not, strlen(not)) == -1)
                    {
                        perror("write");
                        exit(1);
                    }
                    return -1;
                }
            }
            // for all other reads from the client
            else if (usernames[client_index].no_of_reads >= 3)
            {
                if (usernames[client_index].TA_or_Student[0] == 'S')
                {
                    if (strncmp(usernames[client_index].buf, "stats", 5) == 0)
                    {
                        char *mess= malloc(sizeof(char) * BUF_SIZE);
                        if(mess == NULL){
                            perror("malloc");
                            exit(1);
                        }
                        mess = print_currently_serving(head_ta);
                        
                        if (write(usernames[client_index].sock_fd, mess, strlen(mess)) == -1)
                        {
                            perror("write");
                            exit(1);
                        }
                        free(mess);
                    }
                    else
                    {
                        char *mess = "Incorrect Syntax\n";
                        if (write(usernames[client_index].sock_fd, mess, strlen(mess)) == -1)
                        {
                            perror("write");
                            exit(1);
                        }
                    }
                }
                else if (usernames[client_index].TA_or_Student[0] == 'T')
                {
                   
                    if (strncmp(usernames[client_index].buf, "stats", 5) == 0)
                    {

                        char *mess= malloc(sizeof(char) * BUF_SIZE);
                        if(mess == NULL){
                            perror("malloc");
                            exit(1);
                        }
                        mess = print_full_queue(head_student);
                        
                        if (write(usernames[client_index].sock_fd, mess, strlen(mess)) == -1)
                        {
                            perror("write");
                            exit(1);
                        }
                        free(mess);
                    }
                    else if (strncmp(usernames[client_index].buf, "next", 4) == 0)
                    {

                        Student *head = head_student;
                        char *mess = "Your turn to see the TA\nWe are disconnecting you from the server now. Press Ctrl-C to close nc\n";
                        if(head != NULL){
                            // checking which client needs to be disconnected
                        for (int i = 0; i < MAX_CONNECTIONS; i++)
                        {
                            if (usernames[i].sock_fd != -1)
                            {
                                if (strcmp(usernames[i].username, head->name) == 0)
                                {
                                    if (write(usernames[i].sock_fd, mess, strlen(mess)) == -1)
                                    {
                                        perror("write");
                                        exit(1);
                                    }
                                    
                                    next_overall(usernames[client_index].username, &head_ta, &head_student);
                                    return i+1;
                                }
                            }
                        }
                        }
                    }
                    else
                    {
                        char *mess = "Incorrect Syntax\n";
                        if (write(usernames[client_index].sock_fd, mess, strlen(mess)) == -1)
                        {
                            perror("write");
                            exit(1);
                        }
                    }
                }
                
            }
            // moving the buffer after the read has been done
            memmove(&usernames[client_index].buf[0], &usernames[client_index].buf[where], usernames[client_index].inbuf - where );
            usernames[client_index].inbuf = usernames[client_index].inbuf - where;
            usernames[client_index].buf[0] = '\0';
            usernames[client_index].room = BUF_SIZE;
            usernames[client_index].after = NULL;
        
            return 0;
        }
        
        // updating inbuf and room
        usernames[client_index].after = &usernames[client_index].buf[usernames[client_index].inbuf];
        usernames[client_index].room = BUF_SIZE - usernames[client_index].inbuf;
       
    }

    
    return -2;
}

int main(void)
{
    // Initialising the number of clients. I decided to keep this as an array since there would be lesser 
    // changes that would need to be made in the entire run.
    struct sockname usernames[MAX_CONNECTIONS];
    for (int index = 0; index < MAX_CONNECTIONS; index++)
    {
        usernames[index].sock_fd = -1;
        usernames[index].username = NULL;
        usernames[index].TA_or_Student = NULL;
        usernames[index].buf[0] = '\0';
        usernames[index].after = usernames[index].buf;
        usernames[index].no_of_reads = 0;
        usernames[index].room = BUF_SIZE;
        usernames[index].inbuf = 0;
        usernames[index].nbytes = -1;
    }

    // Create the socket FD.
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        perror("server: socket");
        exit(1);
    }

    // Set information about the port (and IP) we want to be connected to.
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    // This should always be zero. On some systems, it won't error if you
    // forget, but on others, you'll get mysterious errors. So zero it.
    memset(&server.sin_zero, 0, 8);

    int on = 1;
    int status = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR,
                            (const char *)&on, sizeof(on));
    if (status == -1)
    {
        perror("setsockopt -- REUSEADDR");
    }
    // Bind the selected port to the socket.
    if (bind(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("server: bind");
        close(sock_fd);
        exit(1);
    }

    // Announce willingness to accept connections on this socket.
    if (listen(sock_fd, MAX_BACKLOG) < 0)
    {
        perror("server: listen");
        close(sock_fd);
        exit(1);
    }

    // The client accept - message accept loop. First, we prepare to listen to multiple
    // file descriptors by initializing a set of file descriptors.
    int max_fd = sock_fd;
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(sock_fd, &all_fds);
    int current_number_of_clients = 0;

    if ((courses = malloc(sizeof(Course) * 3)) == NULL)
    {
        perror("malloc for course list\n");
        exit(1);
    }
    strcpy(courses[0].code, "CSC108");
    strcpy(courses[1].code, "CSC148");
    strcpy(courses[2].code, "CSC209");

    while (1)
    {
        // select updates the fd_set it receives, so we always use a copy and retain the original.

        fd_set listen_fds = all_fds;
        int nready = select(max_fd + 1, &listen_fds, NULL, NULL, NULL);
        if (nready == -1)
        {
            perror("server: select");
            exit(1);
        }

        // Is it the original socket? Create a new connection ...
        if (FD_ISSET(sock_fd, &listen_fds))
        {
            int client_fd = accept_connection(sock_fd, usernames);
            if (client_fd > max_fd)
            {
                max_fd = client_fd;
            }
            FD_SET(client_fd, &all_fds);

            usernames[current_number_of_clients].sock_fd = client_fd;
            
                char *text = "Welcome to the Help Centre, what is your name?\n";
                if (write(usernames[current_number_of_clients].sock_fd, text, strlen(text)) == -1)
                {
                    perror("write");
                    exit(1);
                }
            current_number_of_clients++;
        }

        // Next, check the clients.
        // NOTE: We could do some tricks with nready to terminate this loop early
        
        for (int index = 0; index < current_number_of_clients; index++)
        {
            if (usernames[index].sock_fd > -1 && FD_ISSET(usernames[index].sock_fd, &listen_fds))
            {
                // Note: never reduces max_fd
                int client_closed = read_from(index, usernames);

                if (client_closed == -1)
                {
                    
                    FD_CLR(usernames[index].sock_fd, &all_fds);
                    close(usernames[index].sock_fd); 
                    usernames[index].sock_fd = -1;
                    free(usernames[index].username);
                    free(usernames[index].TA_or_Student);
                    strcpy(usernames[index].buf, "\0");
                    usernames[index].after = NULL;
                    usernames[index].inbuf = 0;
                    usernames[index].nbytes = 0;
                    usernames[index].no_of_reads = 0;
                    usernames[index].room = BUF_SIZE;
                    usernames[index].username = NULL;
                    usernames[index].TA_or_Student = NULL;
                 } 
                 else if (client_closed > 0){
                     
                    FD_CLR(usernames[client_closed - 1].sock_fd, &all_fds);
                    close(usernames[client_closed - 1].sock_fd ); 
                    usernames[client_closed - 1].sock_fd = -1;
                    free(usernames[client_closed - 1].username);
                    free(usernames[client_closed - 1].TA_or_Student);
                    strcpy(usernames[client_closed-1].buf, "\0");
                    usernames[client_closed-1].after = NULL;
                    usernames[client_closed-1].inbuf = 0;
                    usernames[client_closed-1].nbytes = 0;
                    usernames[client_closed-1].no_of_reads = 0;
                    usernames[client_closed-1].room = BUF_SIZE;
                    usernames[client_closed -1].TA_or_Student = NULL;
                    usernames[client_closed-1].username = NULL;

                } else if (client_closed == -2){
                    FD_CLR(usernames[index].sock_fd, &all_fds);
                    close(usernames[index].sock_fd); 
                    usernames[index].sock_fd = -1;
                    
                    if(usernames[index].TA_or_Student[0] == 'S'){
                        Student *start = head_student;
                        while(start != NULL){
                            if(strcmp(start->name, usernames[index].username) == 0){
                                give_up_waiting(&head_student, start->name);
                                break;
                            }
                            start = start->next_overall;
                        }
                        
                    } else{
                        Ta *start = head_ta;
                        while(start != NULL){
                            if(strcmp(start->name, usernames[index].username) == 0){
                                remove_ta(&head_ta, start->name);
                                break;
                            }
                            start = start->next;
                        }
                    }
                    free(usernames[index].username);
                    free(usernames[index].TA_or_Student);
                    strcpy(usernames[index].buf, "\0");
                    usernames[index].after = NULL;
                    usernames[index].inbuf = 0;
                    usernames[index].nbytes = 0;
                    usernames[index].no_of_reads = 0;
                    usernames[index].room = BUF_SIZE;
                    usernames[index].username = NULL;
                    usernames[index].TA_or_Student = NULL;

                }
            }
        } 
    }

    // Should never get here.

    return 1;
}
