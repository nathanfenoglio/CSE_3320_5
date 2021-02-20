#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{
    double *a;
    double *b;
    double sum;
    int veclen;
} DOTDATA;

#define NUMTHRDS 4
#define VECLEN 100

DOTDATA dotstr;
pthread_t callThd[NUMTHRDS];
pthread_mutex_t mutexsum;

void* dotprod(void *arg){
    //define and use local variables for convenience it says
    int i, start, end, len;
    long offset;
    double mysum, *x, *y;
    offset = (long) arg;

    len = dotstr.veclen;
    start = offset * len;
    end = start + len;
    x = dotstr.a;
    y = dotstr.b;

    mysum = 0;
    for(i = start; i < end; i++){
        mysum += (x[i] * y[i]);
    }

    //lock a mutex prior to updating the value in the shared structure, 
    //and unlock it upon updating
    pthread_mutex_lock(&mutexsum);
    dotstr.sum += mysum;
    pthread_mutex_unlock(&mutexsum);

    pthread_exit((void *) 0);
}

int main(int argc, char *argv[]){
    long i;
    double *a, *b;
    void *status;
    pthread_attr_t attr;

    //assign storage 
    a = (double *)malloc(NUMTHRDS * VECLEN * sizeof(double));
    b = (double *)malloc(NUMTHRDS * VECLEN * sizeof(double));

    //intialize values. Seems that they are all being initialized to just 1.0
    for(i = 0; i < (VECLEN * NUMTHRDS); i++){
        a[i] = 1.0;
        b[i] = a[i];
    }

    dotstr.veclen = VECLEN;
    dotstr.a = a;
    dotstr.b = b;
    dotstr.sum = 0;

    pthread_mutex_init(&mutexsum, NULL);

    //create threads to perfrom the dotproduct
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(i = 0; i < NUMTHRDS; i++){
        //each thread works on a different set of data
        //the offset is specified by 'i'
        //the size of the data for each thread is indicated by VECLEN
        pthread_create(&callThd[i], &attr, dotprod, (void *) i);
    }

    pthread_attr_destroy(&attr);

    //wait on the other threads
    for(i = 0; i < NUMTHRDS; i++){
        pthread_join(callThd[i], &status);
    }

    //after joining, print out the results and cleanup
    printf("Sum = %f\n", dotstr.sum);
    free(a);
    free(b);
    pthread_mutex_destroy(&mutexsum);
    pthread_exit(NULL);
}

