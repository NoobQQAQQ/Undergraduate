#ifndef __PTHREAD_H__
#define __PTHREAD_H__

#include "types.h"

#define MAX_TCB_NUM 4           // Why?
#define MAX_STACK_SIZE 1536     // Why? try other numbers

#define STATE_RUNNABLE 0 // create set this state
#define STATE_RUNNING 1 
#define STATE_BLOCKED 2 // yield and joined set this state
#define STATE_DEAD 3 // only exit can set this state


struct Context {
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t eip;
};

struct ThreadTable {
    uint32_t stack[MAX_STACK_SIZE]; // user stack
    struct Context cont;            // pthread context
    uint32_t retPoint;              // the entry to exit the thread
    uint32_t pthArg;                // the arg to pass into thread
    uint32_t stackTop;              // point to where?
    int state;                      // state
    uint32_t pthid;                 // pthread id
    uint32_t joinid;                // target pthread when call pthread_join, valid when state = BLOCK
};
typedef struct ThreadTable ThreadTable;


/**
 * when you create a Process in your OS, there is no thread, you need initial these Process as a thread, so when you create a thread, they are in same structure.
**/
void pthread_initial(void); 

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);

void pthread_exit(void *retval);

int pthread_join(pthread_t thread, void **retval);

int pthread_yield(void);


#endif
