#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hcq.h"
#define INPUT_BUFFER_SIZE 256

/*
 * Return a pointer to the struct student with name stu_name
 * or NULL if no student with this name exists in the stu_list
 */
Student *find_student(Student *stu_list, char *student_name)
{
    Student *curr = stu_list;
    while (curr != NULL)
    {
        if (strcmp((*curr).name, student_name) == 0)
        {
            //checking whether the names are equal
            return curr;
        }
        curr = curr->next_overall;
    }
    //if there is no student found of the same name
    return NULL;
}

/*   Return a pointer to the ta with name ta_name or NULL
 *   if no such TA exists in ta_list. 
 */
Ta *find_ta(Ta *ta_list, char *ta_name)
{
    Ta *curr = ta_list;
    while (curr != NULL)
    {
        if (strcmp(curr->name, ta_name) == 0)
        {
            //checking whether the struct has the same TA name
            return curr;
        }
        curr = curr->next;
    }
    //if no TA of the same name is found
    return NULL;
}

/*  Return a pointer to the course with this code in the course list
 *  or NULL if there is no course in the list with this code.
 */
Course *find_course(Course *courses, int num_courses, char *course_code)
{

    Course *ptr = malloc(sizeof(Course));
    // error check for malloc
    if (ptr == NULL)
    {
        perror("malloc");
        exit(1);
    }
    for (int i = 0; i < num_courses; i++)
    {
        if (strcmp((courses)[i].code, course_code) == 0)
        {
            ptr = &courses[i];
            return ptr; // return the pointer to the course found
        }
    }
    // if no course is found with the same course name
    return NULL;
}

/*
Creates an new student, allocated the memory as required and returns a new student.
**/
Student *create_node(Student *next, char *name, char *co_code, Course *carray, int numc)
{
    Student *new_node = malloc(sizeof(struct student));
    char *stored_name = malloc(sizeof(char *));
    time_t *stored_time = malloc(sizeof(size_t));

    // error check for malloc
    if (stored_name == NULL || stored_time == NULL || new_node == NULL)
    {
        perror("malloc");
        exit(1);
    }

    *stored_time = (time(NULL));
    strcpy(stored_name, name);

    // steps to initialise all the factors of a student
    new_node->name = stored_name;
    new_node->next_overall = next;
    new_node->course = find_course(carray, numc, co_code);
    new_node->arrival_time = stored_time;

    return new_node;
}

/* Add a student to the queue with student_name and a question about course_code.
 * if a student with this name already has a question in the queue (for any
   course), return 1 and do not create the student.
 * If course_code does not exist in the list, return 2 and do not create
 * the student struct.
 * For the purposes of this assignment, don't check anything about the 
 * uniqueness of the name. 
 */
int add_student(Student **stu_list_ptr, char *student_name, char *course_code,
                Course *course_array, int num_courses)
{

    //checking whether the course is valid
    Course *course_found = find_course(course_array, num_courses, course_code);
    if (course_found == NULL)
        return 2;

    //checking whether such a student exists
    Student *student_found = find_student(*stu_list_ptr, student_name);
    if (student_found != NULL)
        return 1;
    
    // If it reaches here, it means the student doesnt exist and the course does
    Student *curr = *stu_list_ptr;

    // if there is no student in the queue
    if (curr == NULL)
    {
        *stu_list_ptr = create_node(*stu_list_ptr, student_name, course_code, course_array, num_courses);
        course_found ->head = *stu_list_ptr;
        course_found ->tail = *stu_list_ptr;
        return 0;
    }
    else // there is already some student in the queue
    {
        Student *previous = *stu_list_ptr;
        // finding the previous student 
        while (curr != NULL)
        { 
            previous = curr;
            curr = curr->next_overall;
        }
        Student *previous_course = *stu_list_ptr;
        //finding the previous student of the same course
        while(previous_course != NULL){
            if(previous_course->course == course_found && previous_course->next_course == NULL){
                break;
            }
            previous_course = previous_course->next_overall;
        }
        // Assigning the new student's pointers
        previous->next_overall = create_node(previous->next_overall, student_name, course_code, course_array, num_courses);
         
        // Assigning the new student's pointers to the course and previous course
        if(course_found->head == NULL){
            course_found ->head = previous->next_overall;
            course_found ->tail = previous->next_overall;
        } else {
            previous_course->next_course = previous->next_overall;
            course_found ->tail = previous->next_overall;
        }
    }
    return 0;
}

void give_up_waiting_stats(Student *curr)
{
    curr->course->bailed = curr->course->bailed + 1;
    curr->course->wait_time = curr->course->wait_time + difftime(time(NULL), *(curr->arrival_time));
}
/* Student student_name has given up waiting and left the help centre
 * before being called by a Ta. Record the appropriate statistics, remove
 * the student from the queues and clean up any no-longer-needed memory.
 *
 * If there is no student by this name in the stu_list, return 1.
 */

int give_up_waiting(Student **stu_list_ptr, char *student_name)
{

    if (find_student(*stu_list_ptr, student_name) == NULL)
        return 1;

    Student *curr = *stu_list_ptr;
    Student *prev = *stu_list_ptr;
    //SPECIAL CASE: if the student who gives up is at the first in the queue
    if (strcmp((curr->name), student_name) == 0)
    {
        give_up_waiting_stats(curr);

        // reassigning the course head or tail
        if(curr ->course->head == curr){
            if(curr ->course->head->next_course == NULL){
                curr->course->head = NULL;
                curr->course->tail = NULL;
            }
         else {
            curr->course->head = curr->course->head->next_course;
            }
        } 
        // Re assigning the pointers to the first student in the queue
        if (curr->next_overall != NULL)
            *stu_list_ptr = curr->next_overall;
        else
            *stu_list_ptr = NULL;
        //free the memory not useful anymore
        free(curr->name);
        free(curr->arrival_time);
        free(curr);
        return 0;
    }

    // All other cases, i.e student is either second or after
    while (curr != NULL)
    {
        if (strcmp(curr->name, student_name) == 0)
        {
            give_up_waiting_stats(curr);

            // reassigning the next overall student
            if (curr->next_overall != NULL)
                prev->next_overall = curr->next_overall;
            else
                prev->next_overall = NULL;
            
            // re assigning the next course if it is the head
            if(curr->course->head == curr){
                if(curr->course->head->next_course != NULL){
                    curr->course->head = curr->course->head->next_course;
                } else{
                    curr->course->head = NULL;
                    curr->course->tail = NULL;
                }
            } // reassigning the next course if it is the tail
            else if(curr->course->tail == curr){
                Student *temp = *stu_list_ptr;
                // finding the previous student with the same course
                while(temp!= NULL){
                    if(temp->course == curr->course && temp->next_course == curr){
                        break;
                    }
                    temp = temp->next_overall;
                }
                curr->course->tail = temp;
                temp->next_course = NULL;
            } else{ // reassigning if it is somewhere in between
                Student *temp = *stu_list_ptr;
                //finding the previous student with the same course 
                while(temp!= NULL){
                    if(temp->course == curr->course && temp->next_course == curr){
                        break;
                    }
                    temp = temp->next_overall;
                }
                // assigning it to the next student in the queue after the current
                temp->next_course = curr->next_course;

            }
            curr = NULL;
            free(curr);
            break;
        }
        prev = curr;
        curr = curr->next_overall;
    }

    return 0;
}

/* Create and prepend Ta with ta_name to the head of ta_list. 
 * For the purposes of this assignment, assume that ta_name is unique
 * to the help centre and don't check it.
 */
void add_ta(Ta **ta_list_ptr, char *ta_name)
{
    // first create the new Ta struct and populate
    Ta *new_ta = malloc(sizeof(Ta));
    if (new_ta == NULL)
    {
        perror("malloc for TA");
        exit(1);
    }
    new_ta->name = malloc(strlen(ta_name) + 1);
    if (new_ta->name == NULL)
    {
        perror("malloc for TA name");
        exit(1);
    }
    strcpy(new_ta->name, ta_name);
    new_ta->current_student = NULL;

    // insert into front of list
    new_ta->next = *ta_list_ptr;
    *ta_list_ptr = new_ta;
}

/* The TA ta is done with their current student. 
 * Calculate the stats (the times etc.) and then 
 * free the memory for the student. 
 * If the TA has no current student, do nothing.
 */
void release_current_student(Ta *ta)
{
    // only does something if the ta has some current student
    if (ta->current_student != NULL)
    {
        // calculating some quick statictics
        ta->current_student->course->helped = ta->current_student->course->helped + 1;
        ta->current_student->course->help_time = ta->current_student->course->help_time + difftime(time(NULL), *(ta->current_student->arrival_time));
        free(ta->current_student);
        ta->current_student = NULL;
    }
}

/* Remove this Ta from the ta_list and free the associated memory with
 * both the Ta we are removing and the current student (if any).
 * Return 0 on success or 1 if this ta_name is not found in the list
 */
int remove_ta(Ta **ta_list_ptr, char *ta_name)
{
    Ta *head = *ta_list_ptr;
    if (head == NULL)
    {
        return 1;
    }
    else if (strcmp(head->name, ta_name) == 0)
    {
        // TA is at the head so special case
        *ta_list_ptr = head->next;
        release_current_student(head);
        // memory for the student has been freed. Now free memory for the TA.
        free(head->name);
        free(head);
        return 0;
    }
    while (head->next != NULL)
    {
        if (strcmp(head->next->name, ta_name) == 0)
        {
            Ta *ta_tofree = head->next;
            //  We have found the ta to remove, but before we do that
            //  we need to finish with the student and free the student.
            //  You need to complete this helper function
            release_current_student(ta_tofree);

            head->next = head->next->next;
            // memory for the student has been freed. Now free memory for the TA.
            free(ta_tofree->name);
            free(ta_tofree);
            return 0;
        }
        head = head->next;
    }
    // if we reach here, the ta_name was not in the list
    return 1;
}

/* TA ta_name is finished with the student they are currently helping (if any)
 * and are assigned to the next student in the full queue. 
 * If the queue is empty, then TA ta_name simply finishes with the student 
 * they are currently helping, records appropriate statistics, 
 * and sets current_student for this TA to NULL.
 * If ta_name is not in ta_list, return 1 and do nothing.
 */
int take_next_overall(char *ta_name, Ta *ta_list, Student **stu_list_ptr)
{

    Ta *find = find_ta(ta_list, ta_name);
    if (find == NULL)
        return 1;

    Student *curr = *stu_list_ptr;
    //if there is no student
    if (curr == NULL)
    {
        release_current_student(find);
        find->current_student = NULL;
        return 0;
    }
    //if the TA has no student
    if (find->current_student == NULL)
    {
        find->current_student = curr;
        // re assigning the course head and tail
        if (curr->course->head == curr && curr->course->head->next_course != NULL)
        {
            curr->course->head = curr->next_course;
        }
        else if (curr->course->head == curr && curr->course->head->next_course == NULL)
        {
            curr->course->head = NULL;
            curr->course->tail = NULL;
        }
        // calculating some quick statistics again
        find->current_student->course->wait_time = find->current_student->course->wait_time + difftime(time(NULL),  *(find->current_student->arrival_time));
        *(find->current_student->arrival_time) = time(NULL); // the arrival time becomes the time at which the student arrived to the TA
        curr = curr->next_overall;
        if (curr == NULL)
            *stu_list_ptr = NULL;
        else
            *stu_list_ptr = curr;
        return 0;
    } 
    // All other cases

    Student *next_renew = find->current_student->next_overall;
    release_current_student(find);
    // re assingnig the course's head and tail
    find->current_student = next_renew;
    if (next_renew->course->head == next_renew && next_renew->course->head->next_course != NULL)
    {
        next_renew->course->head = next_renew->next_course;
    }
    else if (next_renew->course->head == next_renew && next_renew->course->head->next_course == NULL)
    {
        next_renew->course->head = NULL;
        next_renew->course->tail = NULL;
    }

    find->current_student->course->wait_time = find->current_student->course->wait_time + difftime(time(NULL), *(find->current_student->arrival_time));
    *(find->current_student->arrival_time) = time(NULL);
    // changing the initial pointer to point to the next overall student
    *stu_list_ptr = curr->next_overall;

    return 0;
}

/* TA ta_name is finished with the student they are currently helping (if any)
 * and are assigned to the next student in the course with this course_code. 
 * If no student is waiting for this course, then TA ta_name simply finishes 
 * with the student they are currently helping, records appropriate statistics,
 * and sets current_student for this TA to NULL.
 * If ta_name is not in ta_list, return 1 and do nothing.
 * If course is invalid return 2, but finish with any current student. 
 */
int take_next_course(char *ta_name, Ta *ta_list, Student **stu_list_ptr, char *course_code, Course *courses, int num_courses)
{
    Ta *find = find_ta(ta_list, ta_name);
    // checks whether there is the TA with this name
    if (find == NULL)
        return 1;
    Course *course_found = find_course(courses, num_courses, course_code);
    // checks whether there is no course with the same name
    if (course_found == NULL)
    {
        release_current_student(find);
        find->current_student = NULL;
        return 2;
    }
    // useful variables i will be using
    Student *curr = *stu_list_ptr;
    Student *prev = *stu_list_ptr;
    Student *set = NULL;

    //SPECIAL CASE FIRST i.e when student is at the head of the list
    if (curr->course == course_found)
    {
        release_current_student(find);
        find->current_student = curr;
        // check specific cases if the student was a head or a tail
        if (curr->course->head != NULL)
            curr->course->head = curr->next_course;
        else
        {
            curr->course->head = NULL;
            curr->course->tail = NULL;
        }
       
        find->current_student->course->wait_time = find->current_student->course->wait_time + difftime(time(NULL), *(find->current_student->arrival_time));
        *(find->current_student->arrival_time) = time(NULL);
        // reassigning the current positions of the pointer
        if (curr->next_overall != NULL)
            *stu_list_ptr = curr->next_overall;
        else
            *stu_list_ptr = NULL;
        return 0;
    }
    //  All other cases
    curr = *stu_list_ptr;
    prev = *stu_list_ptr;
    set = NULL;

    while (curr != NULL)
    {
        if (curr->course == course_found)
        {
            set = curr; //found the course with the same name as input
            break;
        }
        prev = curr;
        curr = curr->next_overall;
    }

    //if the course is not found
    if (set == NULL)
    {
        release_current_student(find);
        find->current_student = NULL;
        return 0;
    }
    else
    {
        // setting the head or the tail
        if (set->next_course == NULL)
        {
            set->course->head = NULL;
            set->course->tail = NULL;
        }
        else
        {
            set->course->head = set->next_course;
        }
        release_current_student(find);
        find->current_student = set;
        // calculating some quick stats, yeah

        find->current_student->course->wait_time = find->current_student->course->wait_time + difftime(time(NULL), *(find->current_student->arrival_time));
        *(find->current_student->arrival_time) = time(NULL);
        // reassigning the student structs

        prev->next_overall = set->next_overall;
    }
    return 0;
}

/* For each course (in the same order as in the config file), print
 * the <course code>: <number of students waiting> "in queue\n" followed by
 * one line per student waiting with the format "\t%s\n" (tab name newline)
 * Uncomment and use the printf statements below. Only change the variable
 * names.
 */
void print_all_queues(Student *stu_list, Course *courses, int num_courses)
{
    int count;
    // iterate over each course
    for (int i = 0; i < num_courses; i++)
    {
        count = 0;
        Student *curr = stu_list;
        // first we calculate how many students in each course
        while (curr != NULL)
        {
            if (strcmp((courses)[i].code, curr->course->code) == 0)
            {
                count++;
            }
            curr = curr->next_overall;
        }
        printf("%s: %d in queue\n", (courses)[i].code, count);
        curr = stu_list;
        // then we print the names for each course
        while (curr != NULL)
        {
            if (strcmp((courses)[i].code, curr->course->code) == 0)
            {
                printf("\t%s\n", curr->name);
            }
            curr = curr->next_overall;
        }
    }
}

/*
 * Print to stdout, a list of each TA, who they are serving at from what course
 * Uncomment and use the printf statements 
 */
void print_currently_serving(Ta *ta_list)
{
    if (ta_list == NULL)
        printf("No TAs are in the help centre.\n");
    else
    {
        Ta *curr = ta_list;
        while (curr != NULL)
        {
            if (curr->current_student == NULL)
            {

                printf("TA: %s has no student\n", curr->name);
            }
            else
            {
                printf("TA: %s is serving %s from %s\n", curr->name, curr->current_student->name, curr->current_student->course->code);
            }
            curr = curr->next;
        }
    }
}

/*  list all students in queue (for testing and debugging)
 *   maybe suggest it is useful for debugging but not included in marking? 
 */
void print_full_queue(Student *stu_list)
{

    // really didnt use it much because i used the gdb to debug most of my code. print statements were occasional
}

/* Prints statistics to stdout for course with this course_code
 * See example output from assignment handout for formatting.
 *
 */
int stats_by_course(Student *stu_list, char *course_code, Course *courses, int num_courses, Ta *ta_list)
{
    int students_waiting = 0;
    int students_being_helped = 0;
    Student *curr = stu_list;
    Ta *TAs = ta_list;

    // finding the course from the array
    Course *found = find_course(courses, num_courses, course_code);
    if (found == NULL)
        return 1;

    // first we calculate all the possible students waiting
    while (curr != NULL)
    {
        if (strcmp(curr->course->code, course_code) == 0)
        {
            students_waiting++;
        }
        curr = curr->next_overall;
    }
    // here we calculate the students being helped by a TA
    
    while (TAs != NULL)
    {   
        if (TAs->current_student != NULL && strcmp(TAs->current_student->course->code, course_code) == 0)
            students_being_helped++;
        TAs = TAs->next;
    }
    //  PRINTING AS PER THE INSTRUCTIONS
    
    // You MUST not change the following statements or your code
    //  will fail the testing.
    printf("%s:%s \n", found->code, found->description);
    printf("\t%d: waiting\n", students_waiting);
    printf("\t%d: being helped currently\n", students_being_helped);
    printf("\t%d: already helped\n", found->helped);
    printf("\t%d: gave_up\n", found->bailed);
    printf("\t%f: total time waiting\n", found->wait_time);
    printf("\t%f: total time helping\n", found->help_time);

    return 0;
}

/* Dynamically allocate space for the array course list and populate it
 * according to information in the configuration file config_filename
 * Return the number of courses in the array.
 * If the configuration file can not be opened, call perror() and exit.
 */
int config_course_list(Course **courselist_ptr, char *config_filename)
{
    int no_of_courses;
    char num_of_courses[1];
    char course_code[7];
    char line[INPUT_BUFFER_SIZE];
    char **description;
    int i = 0;
    int desnum = 0;
    int pos;
    int pos2;
    int loop = 0;
    FILE *ptr;
    
    ptr = fopen(config_filename, "r");
    // Error check for incorrect file
    if (ptr == NULL)
    {
        perror("file cannot be opened");
        exit(1);
    }
    // Error check for lines in the file
    if (fgets(num_of_courses, 2, ptr) == NULL)
    {
        fprintf(stderr, "no lines in the file \n");
    }
    no_of_courses = strtol(num_of_courses, NULL, 10);
    // Error check for number of courses
    if (no_of_courses < 0)
    {
        fprintf(stderr, "number of courses should be positive");
        exit(1);
    }
    description = malloc(sizeof(char *) * no_of_courses);
    *courselist_ptr = malloc(sizeof(Course) * no_of_courses);
    // Error check for malloc system calls
    if (description == NULL || courselist_ptr == NULL)
    {
        fprintf(stderr, "failed to allocate memory on the heap \n");
        exit(1);
    }
    
    // We now begin to read from the configuration file
    while (fgets(line, INPUT_BUFFER_SIZE, ptr) != NULL)
    {
        // dont want to read the first line of the file
        if (loop == 0)
        {
            loop++;
            continue;
        }
        // Allocating memory for description of the course
        description[desnum] = malloc(sizeof(char) * INPUT_BUFFER_SIZE);
        pos = 0;
        pos2 = 0;
        while (line[pos] != ' ')
        {
            course_code[pos] = line[pos];
            pos++;
        }
        pos++;
        course_code[pos] = '\0';

        while (line[pos] != '\0')
        {
            description[desnum][pos2] = line[pos];
            pos++;
            pos2++;
        }

        strcpy((*courselist_ptr)[i].code, course_code);
        (*courselist_ptr)[i].description = description[desnum];
        // making the space empty again for re-use
        pos2 = 0;
        while (course_code[pos2] != '\0')
        {
            course_code[pos2] = '\0';
            pos2++;
        }

        i++;
    }
    
    if (fclose(ptr) != 0)
    {
        fprintf(stderr, "failed to close the file");
        exit(1);
    }

    return no_of_courses;
}
