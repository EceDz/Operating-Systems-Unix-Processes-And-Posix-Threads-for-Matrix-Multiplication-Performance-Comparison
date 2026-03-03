/*
Ece Duzgec 20220702045
hw1c.c - Multithreaded Matrix Multiplication without Mutex
Each thread computes one row of the result matrix without mutex. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

#define MAX_N 10000

// Global arrays to avoid overflow for large matrices
int A[MAX_N][MAX_N];
int B[MAX_N][MAX_N];
int C[MAX_N][MAX_N];
int N;

// Structure to pass row information to thread
typedef struct {
    int row;
} thread_data_t;

// Function to generate random matrix with values between 1 and 10
void random_matrix(int matrix[MAX_N][MAX_N], int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

// Thread function to compute one row
void *calculate_row(void *arg) {
    thread_data_t *mydata = (thread_data_t *)arg;
    int row = mydata->row;
    
    // No mutex - direct access to shared matrices
    for (int j = 0; j < N; j++) {
        C[row][j] = 0;
        for (int k = 0; k < N; k++) {
            C[row][j] += A[row][k] * B[k][j];
        }
    }
    
    free(mydata);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    struct timeval start, end;
    
    // Get matrix dimension from user
    printf("Enter matrix dimension N (max %d): ", MAX_N);
    scanf("%d", &N);
    
    if (N <= 0 || N > MAX_N) {
        fprintf(stderr, "Error: Matrix dimension must be between 1 and %d\n", MAX_N);
        return 1;
    }
    
    printf("\nMultithreaded Matrix Multiplication without Mutex\n");
    printf("Matrix dimension: %dx%d\n", N, N);
    printf("Number of threads: %d\n\n", N);
    
    srand(time(NULL));
    
    // Generate random values for A and B
    printf("Generating %dx%d matrices...\n", N, N);
    random_matrix(A, N);
    random_matrix(B, N);
    
    // Initialize result matrix C to zero
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
        }
    }
    
    // Start timing
    gettimeofday(&start, NULL);
    
    // Create N threads
    pthread_t threads[MAX_N];
    
    for (int i = 0; i < N; i++) {
        thread_data_t *mydata = (thread_data_t *)malloc(sizeof(thread_data_t));
        mydata->row = i;
        
        if (pthread_create(&threads[i], NULL, calculate_row, (void *)mydata) != 0) {
            perror("pthread_create failed");
            return 1;
        }
    }
    
    // Wait for all threads to complete
    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // End timing
    gettimeofday(&end, NULL);
    
    // Calculate elapsed time
    double time_passed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    
    // Print result matrix
    printf("\nResult Matrix C:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }
    
    printf("\nExecution Time: %.4f seconds\n", time_passed);
    
    return 0;
}