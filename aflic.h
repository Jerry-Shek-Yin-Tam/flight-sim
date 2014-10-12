#ifndef _aflic_h
#define _aflic_h

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


#endif
