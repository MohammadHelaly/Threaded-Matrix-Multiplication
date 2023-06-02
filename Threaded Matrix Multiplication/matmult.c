#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>

#define Item(X, I, J, NCOL)      X[((J) + (I) * (NCOL))]

/*
 * argument struct
 * contains agruments passed to thread function
 * pthread_create() accepts only a single pointer to argument of thread function 
 * we use argument struct to pass multiple arguments to thread function 
 */
struct arg_struct
{
    int* A;
    int* B;
    int* C;
    int l;
    int m;
    int n;
    int tno;
    int tno2;
};

/*
 * matrix multiplication
 *     C = A*B
 *     A has l rows and m cols
 *     B has m rows and n cols
 *     C has l rows and n cols
 */
void matmult(int* A, int* B, int* C, int l, int m, int n)
{
    for(int i=0; i<l; i++)
        for(int j=0; j<n; j++)
        {
            int sum = 0;
            for(int k=0; k<m; k++)
                sum += Item(A, i, k, m) * Item(B, k, j, n);
            Item(C, i, j, n) = sum;
        }
}

/*
 * thread function for version 1 matrix multiplication
 * similar to original algorithm but with small modifications
 * accepts pointer to argument struct passed to it by pthread_create() 
 * among arguments accepted are thread number 1 and thread number 2 i.e. tid[i][j]
 * each thread is assigned an element to calculate in the result matrix according to thread number tid[i][j] 
 */ 
 void * mv1(void *arguments)
{
    struct arg_struct *args = arguments;
    int i,j,k;
    int sum=0;
    for(k=0;k<args->m;k++)
    {
    sum += Item(args->A, args->tno, k, args->m) * Item(args->B, k, args->tno2, args->n);
    }
    Item(args->C, args->tno,args->tno2, args->n) = sum;
}
 
/*
 * version 1 matrix multiplication
 * creates 2D array of structs to pass arguments to threads created for calculating each element in the result matrix 
 * defines 2D array of pthread_t for thread ids of each thread 
 * performs 2 nested for loops to create a new thread calling thread function for calculating each element in the result matrix
 * performs 2 nested for loops to join all the created threads
 * join is performed to wait for all threads to complete before proceeding
 */  
void matmult_v1(int* A, int* B, int* C, int l, int m, int n)
{
    struct arg_struct args[l][n];
    pthread_t tid [l][n];
    for(int v=0;v<l;v++)
    {
        for(int g=0;g<n;g++)
        {   
    	args[v][g].A = A;
        args[v][g].B = B;
        args[v][g].C = C;
        args[v][g].l = l;
        args[v][g].m = m;
        args[v][g].n = n;
        args[v][g].tno=v;
        args[v][g].tno2=g;
    	pthread_create(&tid[v][g],NULL,mv1,&args[v][g]);
        }
    }	
    for(int x=0;x<l;x++)
    {
        for(int y=0;y<n;y++)
        {
    	pthread_join(tid[x][y],NULL);
    	}
    }	
}

/*
 * thread function for version 2 matrix multiplication
 * similar to original algorithm but with small modifications
 * accepts pointer to argument struct passed to it by pthread_create() 
 * among arguments accepted is thread number i.e. tid[i]
 * each thread is assigned a row for which to calculate elements in the result matrix according to thread number tid[i] 
 */  
void * mv2(void *arguments)
{
    struct arg_struct *args = arguments;
    int i,j,k;
    	for(j=0;j<args->n;j++)
    	{
    	    int sum=0;
    	    for(k=0;k<args->m;k++)
    	    {
    	    	 sum += Item(args->A, args->tno, k, args->m) * Item(args->B, k, j, args->n);
    	    }
    	    Item(args->C, args->tno,j, args->n) = sum;
    	}
}
 
/*
 * version 2 matrix multiplication
 * creates 1D array of structs to pass arguments to threads created for calculating each row in the result matrix 
 * defines 1D array of pthread_t for thread ids of each thread 
 * performs for loop to create a new thread calling thread function for calculating each row in the result matrix
 * performs for loop to join all the created threads
 * join is performed to wait for all threads to complete before proceeding
 */   
void matmult_v2(int* A, int* B, int* C, int l, int m, int n)
{   
    pthread_t tid [l];
    struct arg_struct args[l];
    for(int v=0;v<l;v++)
    {
    args[v].A = A;
    args[v].B = B;
    args[v].C = C;
    args[v].l = l;
    args[v].m = m;
    args[v].n = n;
    args[v].tno=v;    	
    pthread_create(&tid[v],NULL,mv2,&args[v]);
    }	
    for(int j=0;j<l;j++)
    	pthread_join(tid[j],NULL);	
}


