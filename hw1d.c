/*
Ece Duzgec 20220702045
hw1d.c - Single Process Matrix Multiplication
All computation is done in a single process without creating child processes or threads.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define MAX_N 10000

// Global arrays to avoid overflow for large matrices
int A[MAX_N][MAX_N];
int B[MAX_N][MAX_N];
int C[MAX_N][MAX_N];

// Function to generate random matrix with values between 1 and 10
void random_matrix(int matrix[MAX_N][MAX_N], int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

// Function to multiply matrices
void multiply_matrices(int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0;
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int n;
    struct timeval start, end;
    
    // Get matrix dimension from user
    printf("Enter matrix dimension N (max %d): ", MAX_N);
    scanf("%d", &n);
    
    if (n <= 0 || n > MAX_N) {
        fprintf(stderr, "Error: Matrix dimension must be between 1 and %d\n", MAX_N);
        return 1;
    }
    
    printf("\nSingle Process Matrix Multiplication\n");
    printf("Matrix dimension: %dx%d\n\n", n, n);
    
    srand(time(NULL));
    
    // Generate random values for A and B
    printf("Generating %dx%d matrices...\n", n, n);
    random_matrix(A, n);
    random_matrix(B, n);
    
    // Start timing
    gettimeofday(&start, NULL);
    
    // Matrix multiplication
    multiply_matrices(n);
    
    // End timing
    gettimeofday(&end, NULL);
    
    // Calculate time passed
    double time_passed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    
    // Print result matrix
    printf("\nResult Matrix C:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }
    
    printf("\nExecution Time: %.4f seconds\n", time_passed);
    
    return 0;
}