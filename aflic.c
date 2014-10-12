/* --------------------------------------------------------------------------------------
 Shek Yin Tam
 CSE321 AFLIC Project
 Note: This program is designed to only work for one runway with n planes.
 To start: type ./a.out #ofplanes(int)
 --------------------------------------------------------------------------------------- */
#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <pthread.h>    /* POSIX Threads */
#include <string.h>     /* String handling */
#include "pthread.h"

// STATES
#define REGISTER 1
#define AT_GATE 2
#define GETS_CLEARENCE 3
#define TAKEOFF 4
#define CRUISING_ALT 5
#define DELETED 6
#define LANDING 8
#define AIR 9
#define ROUTINE 10
#define AIR2 11

typedef struct plane_arg
{
    int plane_id;
    int plane_status;
    char plane_purp;
} plane_arg;

int max;
int count = 1;
pthread_mutex_t mutexRun = PTHREAD_MUTEX_INITIALIZER;
pthread_attr_t attr;
pthread_cond_t myconvar = PTHREAD_COND_INITIALIZER;

void foo()
{
    printf("\n");
};

void *whereAmi(void *plane_args)
{
    int state;
    int pid;
    char p;
    plane_arg *str_ptr;
    
    str_ptr = (plane_arg*) plane_args;          // str_ptr points to struct plane_args
    pid = str_ptr -> plane_id;                  // id = str_ptr.plane_id
    state = str_ptr -> plane_status;
    p = str_ptr -> plane_purp;
    int gap = rand() % 4;
    srand(time(NULL));
    
    while(1) {
        switch (state)
        {
            default: case REGISTER :{
                printf("\nPlane %d is registered in the AFLIC system. ", pid);
                foo();
                state = AT_GATE;
                sleep(1);
            } break;
                
            case AT_GATE :{
                printf("\nPlane %d is at the gate. ", pid);
                state = GETS_CLEARENCE;
                sleep(gap);
            } break;
                
            case GETS_CLEARENCE :{
                printf("\nPlane %d is getting clearence for runway.", pid);
                if (count == 2) {
                    state = AIR;
                    printf("\nThis is the count atm: %d.", count); // count condition to indicate mutex use
                } else {
                    pthread_mutex_lock(&mutexRun);
                    printf("\nPlane %d has the runway.", pid);
                    count++;
                    state = TAKEOFF;
                    sleep(gap);
                    printf("\nPlane %d is at takeoff.", pid);
                    pthread_mutex_unlock(&mutexRun);
                    count--;
                    sleep(1);
                }
            } break;
                
            case TAKEOFF :{
                state = CRUISING_ALT;
            } break;
                
            case CRUISING_ALT :{
                sleep(2);
                printf("\nPlane %d has reached its cruising altitude.", pid);
                state = DELETED;
            } break;
                
            case DELETED :{
                printf("\nPlane %d has been deleted.", pid);
                foo();
                sleep(gap + 2);
                state = REGISTER;
            } break;
                
            case LANDING :{
                printf("\nPlane %d has landed", pid);
                foo();
                state = ROUTINE;
                sleep(gap);
            } break;
                
            case AIR :{
                printf("\nWARNING: Collision detected! WARNING: Collision detected! \nPlane %d is in the air waiting for open slot", pid);
                sleep(gap);
                pthread_mutex_lock(&mutexRun);
                state = TAKEOFF;
                pthread_mutex_unlock(&mutexRun);
                sleep(1);
                // pthread_cond_wait(&myconvar, &mutexRun);
            } break;
                
            case ROUTINE :{
                printf("\nPlane %d is registered in the AFLIC system. ", pid);
                foo();
                sleep(1);
                printf("\nPlane %d is getting clearence.", pid);
                if (count == 2) {
                    state = AIR2;
                } else {
                    pthread_mutex_lock(&mutexRun);
                    count++;
                    sleep(3);
                    state = AT_GATE;
                    sleep(gap);
                    printf("\nPlane %d is on time.", pid);
                    pthread_mutex_unlock(&mutexRun);
                    count--;
                    sleep(1);

                }
            } break;
                
            case AIR2 :{
                printf("\nWARNING: Collision detected! WARNING: Collision detected! \nPlane %d is in the air waiting for open slot", pid);
                sleep(gap);
                pthread_mutex_lock(&mutexRun);
                state = AT_GATE;
                pthread_mutex_unlock(&mutexRun);
                sleep(1);
                // pthread_cond_wait(&myconvar, &mutexRun);
            } break;
        }
    }
    pthread_exit(NULL);
};

int main(int argc, char* argv[])
{
    printf("Welcome to AFLIC! \n");
    max = atoi(argv[1]); //accepting two arguments, space to seperate
    if (max == 0){ // terminates on this condition
        printf("The number of planes cannot be 0. Program terminates \n");
        exit(1);
    }
    
    plane_arg paperPlanes[max]; // struct created
    pthread_t threads[max];
    pthread_mutex_init(&mutexRun, NULL);
    int rc, z, x;
    long t;
    
    
    
    for(t=1;t <= max;t++)
    {
        int stat = rand(); // generate random event for start state
        srand(time(NULL));
        paperPlanes[t].plane_id = t;
        
        if(stat%2 == 0)
        {
            paperPlanes[t].plane_purp = 'T';
            paperPlanes[t].plane_status = 1;
        } else {
            paperPlanes[t].plane_purp = 'L';
            paperPlanes[t].plane_status = 8;
        }
        
        printf("Creating thread %ld\n", t);
        rc = pthread_create(&threads[t], NULL, whereAmi, (void *)&paperPlanes[t]);
        if (rc != 0)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
        printf("Thread# = %ld is created \n", t);
    }
    
    for(z = 1;z<=max; z++)
    {
        pthread_join(threads[z], NULL);
    }
    foo();
    pthread_cond_destroy(&myconvar);
    pthread_mutex_destroy(&mutexRun);
    pthread_exit(NULL);
    return 0;
}