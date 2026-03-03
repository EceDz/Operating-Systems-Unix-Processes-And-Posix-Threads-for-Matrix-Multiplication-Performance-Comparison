/*
Ece Duzgec 20220702045
hw1a.c-Multiprocess Matrix Multiplication
Each child process computes one row of the result matrix.N child processes are created for an NxN matrix.
 */
 
#include <stdio.h> 
#include <stdlib.h>     // For exit, rand, srand, malloc
#include <unistd.h>     // For fork, getpid
#include <sys/wait.h>   // For wait (to wait for child processes)
#include <sys/shm.h>    // For shared memory functions (shmget, shmat, shmctl)
#include <sys/time.h>   // For gettimeofday (timing)
#include <time.h>       // For time() used with srand

#define MAX_N 10000 

// Global arrays to avoid overflow for large matrices
int A[MAX_N][MAX_N];
int B[MAX_N][MAX_N];

// Create random matrix nxn with values between 1 and 10
void random_matrix(int matrix[MAX_N][MAX_N], int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

// Function to compute one row of the result matrix
void calculate_row(int row, int n, int *C_row) {
    for (int j = 0; j < n; j++) {
        C_row[j] = 0;
        for (int k = 0; k < n; k++) {
            C_row[j] += A[row][k] * B[k][j];
        }
    }
}

int main(int argc, char *argv[]) {
    int n;
    struct timeval start, end;
    
    // Get matrix dimension N value from user
    printf("Enter matrix dimension N (max %d): ", MAX_N);
    scanf("%d", &n);
    
    //Prints an error id it is not an acceptable N value
    if (n <= 0 || n > MAX_N) {
        fprintf(stderr, "Error: Matrix dimension must be between 1 and %d\n", MAX_N);
        return 1;
    }
    //Prints a summary of what happens 
    printf("\nMultiprocess Matrix Multiplication\n");
    printf("Matrix dimension: %dx%d\n", n, n);
    printf("Number of processes: %d (1 parent + %d children)\n\n", n + 1, n);
    
    srand(time(NULL));
    
    // Generate random values for A and B
    printf("Generating %dx%d matrices...\n", n, n);
    random_matrix(A, n);
    random_matrix(B, n);
    
    // Create shared memory for result matrix C
    int shmid = shmget(IPC_PRIVATE, n * n * sizeof(int), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        return 1;
    }
    //Error check for shared memory
    int *C = (int *)shmat(shmid, NULL, 0);
    if (C == (int *)-1) {
        perror("shmat failed");
        return 1;
    }
    
    // Timing start
    gettimeofday(&start, NULL);
    
    // Clears output buffers before forking to prevent duplication in child processes
    fflush(stdout);
    fflush(stderr);
    
    // Create N child processes each computing one row
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("fork failed");
            return 1;
        }
        else if (pid == 0) {
            // Child process - compute row i and exit immediately
            int *C_row = C + (i * n);
            calculate_row(i, n, C_row);
            exit(0);  // If child exits don't continue the loop
        }
        // Parent continues to next iteration to create next child
    }
    
    // Parent process waits for all children to complete
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }
    
    // End timing
    gettimeofday(&end, NULL);
    
    // Calculate time passed
    double passed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    
    // Print result matrix
    printf("\nResult Matrix C:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", C[i * n + j]);
        }
        printf("\n");
    }
    
    printf("\nExecution Time: %.4f seconds\n", passed_time);
    
    // Cleanup
    shmdt(C);
    shmctl(shmid, IPC_RMID, NULL);
    
    return 0;
}