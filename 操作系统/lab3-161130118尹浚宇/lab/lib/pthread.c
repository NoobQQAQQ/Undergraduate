#include "pthread.h"
#include "lib.h"
/*
 * pthread lib here
 * 用户态多线程写在这
 */
 
ThreadTable tcb[MAX_TCB_NUM];
int current;

// switch current pthread to i pthread
void switch_to(int i)
{
    uint32_t ebp;
    // save current regs
    asm volatile("movl %%edi, %0":"=m"(tcb[current].cont.edi));
	asm volatile("movl %%esi, %0":"=m"(tcb[current].cont.esi));
	asm volatile("movl %%ebx, %0":"=m"(tcb[current].cont.ebx));
	asm volatile("movl %%edx, %0":"=m"(tcb[current].cont.edx));
    asm volatile("movl %%ecx, %0":"=m"(tcb[current].cont.ecx));
    asm volatile("movl %%eax, %0":"=m"(tcb[current].cont.eax));

    // very important
    //asm volatile("movl (%ebp), %eax");
    //asm volatile("movl %%eax, %0":"=m"(tcb[current].cont.ebp));

    //asm volatile("leal 0xc(%ebp), %eax");
    //asm volatile("movl %%eax, %0":"=m"(tcb[current].cont.esp));

    //asm volatile("movl 0x4(%ebp), %eax");
    //asm volatile("movl %%eax, %0":"=m"(tcb[current].cont.eip));

    // use stack instead of reg
    asm volatile("movl %%ebp, %0":"=m"(ebp));
    tcb[current].cont.ebp = *(uint32_t*)ebp;
    tcb[current].cont.eip = *((uint32_t*)ebp+1);
    tcb[current].cont.esp = ebp + 0xc;
    // ************
    
    current = i;
    // switch regs jmp to pthread i
    asm volatile("movl %0, %%esp"::"m"(tcb[i].cont.esp));

    asm volatile("pushl %0"::"m"(tcb[i].cont.eip));
    asm volatile("pushl %0"::"m"(tcb[i].cont.eax));
    asm volatile("pushl %0"::"m"(tcb[i].cont.ecx));
    asm volatile("pushl %0"::"m"(tcb[i].cont.edx));
    asm volatile("pushl %0"::"m"(tcb[i].cont.ebx));
    asm volatile("pushl %0"::"m"(tcb[i].cont.ebp));
    asm volatile("pushl %0"::"m"(tcb[i].cont.esi));
    asm volatile("pushl %0"::"m"(tcb[i].cont.edi));

    asm volatile("popl %edi");
    asm volatile("popl %esi");
    asm volatile("popl %ebp");
    asm volatile("popl %ebx");
    asm volatile("popl %edx");
    asm volatile("popl %ecx");
    asm volatile("popl %eax");
    asm volatile("ret");


    /*
    asm volatile("movl %0, %%edi"::"m"(tcb[i].cont.edi));
	asm volatile("movl %0, %%esi"::"m"(tcb[i].cont.esi));

	asm volatile("movl %0, %%edx"::"m"(tcb[i].cont.edx));
    asm volatile("movl %0, %%ecx"::"m"(tcb[i].cont.ecx));
    asm volatile("movl %0, %%eax"::"m"(tcb[i].cont.eax));
    asm volatile("")

    asm volatile("movl %0, %%ebx"::"m"(tcb[i].cont.ebx));
    asm volatile("movl %0, %%ebp"::"m"(tcb[i].cont.ebp));
    asm volatile("call *%0"::"m"(tcb[i].cont.eip));
    */
}

void pthread_initial(void){
    int i;
    for (i = 0; i < MAX_TCB_NUM; i++) {
        tcb[i].state = STATE_DEAD;
        tcb[i].joinid = -1;
    }
    tcb[0].state = STATE_RUNNING;
    tcb[0].pthid = 0;
    current = 0; // main thread
    return;
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)
{
    // try to find an empty tcb
    int i;
	for (i = 0; i < MAX_TCB_NUM; i++) 
    {
		if (tcb[i].state == STATE_DEAD)
			break;
	}

    // have found an empty tcb, creat a pthread
    if (i != MAX_TCB_NUM)
    {
        tcb[i].stackTop = (uint32_t)&(tcb[i].cont);
        tcb[i].state = STATE_RUNNABLE; // switch state to runnable
        tcb[i].joinid = -1;
        tcb[i].pthid = i; // pthid = i
        tcb[i].pthArg = (uint32_t)arg;
        *thread = i;

        tcb[i].cont.eip = (uint32_t)start_routine;
        tcb[i].cont.ebp = (uint32_t)&(tcb[i].cont);
        tcb[i].stack[MAX_STACK_SIZE - 1] = (uint32_t)arg;
        tcb[i].cont.esp = (uint32_t)&(tcb[i].stack[MAX_STACK_SIZE - 2]);
        return 0;
    }

    // all tcbs have been occupied, creating a pthread triggers an error
    else
        return -1;
     
    // return 0;
}

void pthread_exit(void *retval)
{
    tcb[current].state = STATE_DEAD;
    //*(int*)retval = 666; // *retval records the thread number of the thread who calls the function pthread_exit()

    for (int i = 0; i < MAX_TCB_NUM; i++)
        if (tcb[i].joinid == current)
        {
            tcb[i].joinid = -1;
            tcb[i].state = STATE_RUNNABLE;
        }
    
    int i;
    i = (current + 1) % MAX_TCB_NUM;
    while (tcb[i].state != STATE_RUNNABLE)
        i = (i + 1) % MAX_TCB_NUM;
    

    // switch to pthread i
    // current = i;
    current = i;
    //tcb[current].retPoint = (uint32_t)retval;

    // switch regs jmp to pthread i
    /*
    asm volatile("movl %0, %%edi"::"m"(tcb[i].cont.edi));
	asm volatile("movl %0, %%esi"::"m"(tcb[i].cont.esi));
	asm volatile("movl %0, %%ebp"::"m"(tcb[i].cont.ebp));
	asm volatile("movl %0, %%esp"::"m"(tcb[i].cont.esp));
	asm volatile("movl %0, %%ebx"::"m"(tcb[i].cont.ebx));
	asm volatile("movl %0, %%edx"::"m"(tcb[i].cont.edx));
    asm volatile("movl %0, %%ecx"::"m"(tcb[i].cont.ecx));
    asm volatile("movl %0, %%eax"::"m"(tcb[i].cont.eax));
    asm volatile("call *%0"::"m"(tcb[i].cont.eip));
    */
    
    asm volatile("movl %0, %%esp"::"m"(tcb[i].cont.esp));

    asm volatile("pushl %0"::"m"(tcb[i].cont.eip));
    asm volatile("pushl %0"::"m"(tcb[i].cont.eax));
    asm volatile("pushl %0"::"m"(tcb[i].cont.ecx));
    asm volatile("pushl %0"::"m"(tcb[i].cont.edx));
    asm volatile("pushl %0"::"m"(tcb[i].cont.ebx));
    asm volatile("pushl %0"::"m"(tcb[i].cont.ebp));
    asm volatile("pushl %0"::"m"(tcb[i].cont.esi));
    asm volatile("pushl %0"::"m"(tcb[i].cont.edi));
    /*
    asm volatile("movl %0, %%esp"::"m"(tcb[target].cont.esp));

    asm volatile("pushl %0"::"m"(tcb[target].cont.eip));
    asm volatile("pushl %0"::"m"(tcb[target].cont.eax));
    asm volatile("pushl %0"::"m"(tcb[target].cont.ecx));
    asm volatile("pushl %0"::"m"(tcb[target].cont.edx));
    asm volatile("pushl %0"::"m"(tcb[target].cont.ebx));
    asm volatile("pushl %0"::"m"(tcb[target].cont.ebp));
    asm volatile("pushl %0"::"m"(tcb[target].cont.esi));
    asm volatile("pushl %0"::"m"(tcb[target].cont.edi));
    */
    asm volatile("popl %edi");
    asm volatile("popl %esi");
    asm volatile("popl %ebp");
    asm volatile("popl %ebx");
    asm volatile("popl %edx");
    asm volatile("popl %ecx");
    asm volatile("popl %eax");
    asm volatile("ret");
    // return;
}

int pthread_join(pthread_t thread, void **retval)
{
    if (tcb[thread].state != STATE_DEAD)
    {
        tcb[current].state = STATE_BLOCKED;
        tcb[current].joinid = thread;

        // switch to pthread thread
        tcb[thread].state = STATE_RUNNING;
        switch_to(thread);

        if (retval != NULL)
            *retval = (void*)tcb[current].retPoint;
        /*
        // return orinal pthread
        for (int i = 0; i < MAX_TCB_NUM; i++)
            if (tcb[i].joinid == thread)
            {
                tcb[i].joinid = -1;
                tcb[i].state = STATE_RUNNABLE;
            }
        */
    }
    return 0;
}

int pthread_yield(void)
{
    tcb[current].state = STATE_RUNNABLE;

    int i;
    i = (current + 1) % MAX_TCB_NUM;
    while (tcb[i].state != STATE_RUNNABLE)
        i = (i + 1) % MAX_TCB_NUM;
    
    // switch to pthread i
    tcb[i].state = STATE_RUNNING;

    switch_to(i);

    return 0;
}
