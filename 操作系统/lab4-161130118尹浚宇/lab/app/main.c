#include "lib.h"
#include "types.h"

int data = 0;
#define TEST 2

#if TEST == 1
int uEntry(void) {
    
    int i = 4;
    int ret = 0;
    sem_t sem;
    printf("Father Process: Semaphore Initializing.\n");
    ret = sem_init(&sem, 2);
    if (ret == -1) {
        //printf("pid %d.\n", getpid());
        printf("Father Process: Semaphore Initializing Failed.\n");
        exit();
    }

    ret = fork();
    if (ret == 0) {
        while( i != 0) {
            i --;
            //printf("pid %d.\n", getpid());
            printf("Child Process: Semaphore Waiting.\n");
            sem_wait(&sem);
            printf("Child Process: In Critical Area.\n");
        }
        printf("Child Process: Semaphore Destroying.\n");
        sem_destroy(&sem);
        exit();
    }
    else if (ret != -1) {
        while( i != 0) {
            i --;
            //printf("pid %d.\n", getpid());
            printf("Father Process: Sleeping.\n");
            sleep(128);
            printf("Father Process: Semaphore Posting.\n");
            sem_post(&sem);
        }
        printf("Father Process: Semaphore Destroying.\n");
        sem_destroy(&sem);
        exit();
    }
    
    return 0;
}
#endif

#if TEST == 2
void producer(pid_t pid, sem_t mutex, sem_t full)
{
    for (int k = 1; k <= 8; k++)
    {
        // produce()
        printf("pid %d, producer %d, produce, product %d\n", pid, pid-1, k);
        sleep(3);
        printf("pid %d, producer %d, try lock\n", pid, pid-1);
        sem_wait(&mutex);
        printf("pid %d, producer %d, locked\n", pid, pid-1);
        sem_post(&mutex);
        printf("pid %d, producer %d, unlock\n", pid, pid-1);

        sem_post(&full);
    }
    //exit();
    //while(1);
}

void consumer(pid_t pid, sem_t mutex, sem_t full)
{
    for (int k = 1; k <= 4; k++)
    {
        printf("pid %d, consumer %d, try consume, product %d\n", pid, pid-3, k);
        sem_wait(&full);

        printf("pid %d, consumer %d, try lock\n", pid, pid-3);
        sem_wait(&mutex);
        printf("pid %d, consumer %d, locked\n", pid, pid-3);
        sem_post(&mutex);
        printf("pid %d, consumer %d, unlock\n", pid, pid-3);

        printf("pid %d, consumer %d, consumed, product %d\n", pid, pid-3, k);
        sleep(3);
    }
    //exit();
    //while(1);
}

int uEntry(void)
{
    printf("this is test 1.3\n");
    int ret, i;
    sem_t mutex, full; // full represents number of products
    sem_init(&mutex, 1);
    sem_init(&full, 0);
    // fork 6 processes
    for (i = 0; i < 6; i++)
    {
        ret = fork();
        if (ret == 0 || ret == -1) break;
    }

    if (ret == -1) // error, should not happen here
    {
        printf("fork error.\n");
        exit();
    }

    else if (ret == 0) // child processes
    {
        pid_t pid = getpid();
        if ((pid > 1) && (pid < 4)) // process 2, 3 used as producer
            producer(pid, mutex, full);
        else // process 4-7 used as consumer
            consumer(pid, mutex, full); 
        exit();
    }

    else // father processes
    {
        exit();
    }
    return 0;
}
#endif