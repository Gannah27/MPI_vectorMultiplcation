#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<iostream>
#include <mpi.h>
using namespace std;
//function to fill the matrix
int* createMatrix(int nrows, int ncols) {
    int* matrix;
    int h;
    srand(nrows);
    if ((matrix = (int*)malloc(nrows * ncols * sizeof(int))) == NULL) {
        printf("Malloc error");
        exit(1);
    }

    for (h = 0; h < nrows * ncols; h++) {
        matrix[h] = rand()%1000;
    }

    return matrix;
}
//function to fill the vector
int* createvector( int ncols) {
    int* matrix;
    int h;
    srand(ncols);
    if ((matrix = (int*)malloc( ncols * sizeof(int))) == NULL) {
        printf("Malloc error");
        exit(1);
    }

    for (h = 0; h <  ncols; h++) {
        matrix[h] = rand();
    }

    return matrix;
}
//function to print array
void printArray(int* row, int nElements) {
    int i;
    for (i = 0; i < nElements; i++) {
        printf("%d ", row[i]);
    }
    printf("\n");
}
//function to print vector
void printvector(int* row, int nElements) {
    int i;
    for (i = 0; i < nElements; i++) {
        printf("%d\n", row[i]);

    }
    printf("\n");
}

int main(int argc, char** argv) {

    if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
        perror("Error initializing MPI");
        exit(1);
    }

    int size, id,p=0;
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &id); // Get own ID

    int* matrix=nullptr;
    int* vector =  NULL;
    if (id == 0) {
        printf("enter size :\n");
        cin >> p;
        matrix = createMatrix(p, p); // Master process creates matrix
        vector = createvector(p);
        printf("Initial matrix:\n");
        printArray(matrix, p* p);
        printf("initial vector:\n");
        printvector(vector, p);
       
    }
    //broadcast the size of matrix
    MPI_Bcast(&p, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //broadcast vector 
    if (vector != NULL) {
        MPI_Bcast(vector, p, MPI_INT, 0, MPI_COMM_WORLD);
       
    }
    else {
        vector= (int*)malloc(sizeof(int) * (p));
        MPI_Bcast(vector, p, MPI_INT, 0, MPI_COMM_WORLD);
    }
   
    int* procRow = (int*)malloc(sizeof(int) * (p*p/size)); // received row will contain p integers
    if (procRow == NULL) {
        perror("Error in malloc 3");
        exit(1);
    }

    if (MPI_Scatter(matrix, p*p/size, MPI_INT, 
        procRow, p*p/size, MPI_INT, // received data  contains p*p/size integers
        0, MPI_COMM_WORLD) != MPI_SUCCESS) {

        perror("Scatter error");
        exit(1);
    }
   

    printf("Process %d received elements: ", id);

   int* localarray = (int*)malloc(sizeof(int) * ( p / size));
    for (int i = 0; i < p/size; i++) {
        localarray[i] = 0;
        for (int j = 0; j < p; j++) {
            localarray[i] = localarray[i] + (procRow[j + (i * p )] * vector[j]);
            
        }
    }
    printArray(localarray, p / size);

    int* finalans = NULL;
    if (id == 0) {
        finalans = (int*)malloc(sizeof(int) * (p));
    }
    
    //gather local computation
    MPI_Gather(localarray, p / size, MPI_INT, finalans, p / size,MPI_INT, 0, MPI_COMM_WORLD);
    if (id == 0) {
        cout << "final computation:\n";
        printvector(finalans, p);
    }
    MPI_Finalize();
   
    return 0;
}