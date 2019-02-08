#include <stdio.h>

/* Each of the n elements of array elements, is the address of an
 * array of n integers.
 * Return 0 if every integer is between 1 and n^2 and all
 * n^2 integers are unique, otherwise return 1.
 */
int check_group(int **elements, int n) {
    // TODO: replace this return statement with a real function body
    
    int all_numbers[n*n];
    for (int i =0; i < n*n; i ++){
        all_numbers[i] =  i+1;
        
    }
    
    
    for(int i = 0; i < n*n; i ++){

        

      for (int j = 0; j < n*n; j ++) {
          //printf("all number : %d \n", all_numbers[j]);
          //printf("element : %d \n", elements[0][i]);
          if (all_numbers[j] == elements[0][i]){

          all_numbers[j] = 0;
          break;
          }
      }

    
           
        
        
    }

    

    for (int i = 0; i < n*n; i++){
        //printf(" %d", all_numbers[i]);
        if (all_numbers[i] != 0)
        return 1;
    }
    //printf("\n");
    return 0;
}

/* puzzle is a 9x9 sudoku, represented as a 1D array of 9 pointers
 * each of which points to a 1D array of 9 integers.
 * Return 0 if puzzle is a valid sudoku and 1 otherwise. You must
 * only use check_group to determine this by calling it on
 * each row, each column and each of the 9 inner 3x3 squares
 */
int check_regular_sudoku(int **puzzle) {
    
    //ROWS
    int *row;
   int **send;
    for (int i =0; i < 9; i ++){
        row = puzzle[i];
        send = &row;
        int answer = check_group(send, 3);
        //printf(" answer returned: %d \n", answer);
        if (answer == 1){
          return 1;
       }
        
    }
    //COLUMNS
     int col = 0;
     int columns[9];
     int *ptr[1];
     for(int i = 0; i < 9; i ++){
         
         for(int j = 0; j < 9; j ++){
            columns[j] = puzzle[j][col];
         }
         col ++;
         ptr[0] = columns;
         
        int answer = check_group(ptr, 3);
        //printf(" answer returned: %d \n", answer);
        if (answer == 1){
          return 1;
        }
     }
     //SQUARE 1
     int square[9];
     int start = 0;
     int pos = 0;
     
     
    for(int k = 0; k < 3; k ++){
     for(int j = 0; j < 9; j ++){
         square[j] = puzzle[pos][start];
         start++;
         if (start == 3){
             pos ++;
             start = 0;
         }

     }
     ptr[0] = square;
     int answer = check_group(ptr, 3);
    if (answer == 1){
         return 1;
    }
    }

    //SQUARE AREA 2
    pos = 0;
    start = 3;
    for(int k = 0; k < 3; k ++){
     for(int j = 0; j < 9; j ++){
         square[j] = puzzle[pos][start];
         start++;
         if (start == 6){
             pos ++;
             start = 3;
         }

     }
     ptr[0] = square;
     int answer = check_group(ptr, 3);
    //printf(" answer returned: %d \n", answer);
    if (answer == 1){
         return 1;
    }
    }
    //SQUARE 3
    pos = 0;
    start = 6;
    for(int k = 0; k < 3; k ++){
     for(int j = 0; j < 9; j ++){
         square[j] = puzzle[pos][start];
         start++;
         if (start == 9){
             pos ++;
             start = 6;
         }

     }
     ptr[0] = square;
     int answer = check_group(ptr, 3);
   // printf(" answer returned: %d \n", answer);
    if (answer == 1){
          return 1;
    }
    }
    return 0;
}
