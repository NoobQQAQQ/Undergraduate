#include "x86.h"
#include "device.h"

extern TSS tss;
extern ProcessTable pcb[MAX_PCB_NUM];
extern int current;

extern int displayRow;
extern int displayCol;

void GProtectFaultHandle(struct StackFrame *sf);

void timerHandle(struct StackFrame *sf);

void syscallHandle(struct StackFrame *sf);

void syscallWrite(struct StackFrame *sf);
void syscallPrint(struct StackFrame *sf);
void syscallFork(struct StackFrame *sf);
void syscallExec(struct StackFrame *sf);
void syscallSleep(struct StackFrame *sf);
void syscallExit(struct StackFrame *sf);

void irqHandle(struct StackFrame *sf) { // pointer sf = esp
	/* Reassign segment register */
	asm volatile("movw %%ax, %%ds"::"a"(KSEL(SEG_KDATA)));
	/*XXX Save esp to stackTop */
	uint32_t tmpStackTop = pcb[current].stackTop;
	pcb[current].prevStackTop = pcb[current].stackTop;
	pcb[current].stackTop = (uint32_t)sf;

	switch(sf->irq) {
		case -1:
			break;
		case 0xd:
			GProtectFaultHandle(sf);
			break;
		case 0x20:
			timerHandle(sf);
			break;
		case 0x80:
			syscallHandle(sf);
			break;
		default:assert(0);
	}
	/*XXX Recover stackTop */ 
	pcb[current].stackTop = tmpStackTop;
}

void GProtectFaultHandle(struct StackFrame *sf) {
	assert(0);
	return;
}

void timerHandle(struct StackFrame *sf) {
	int i;
	uint32_t tmpStackTop;
	i = (current+1) % MAX_PCB_NUM; // i point to next process of current
    // make blocked processes sleep time -1, sleep time to 0, re-run
	
    // time count not max, process continue
	
    // else switch to another process
	
		/* echo pid of selected process */
		
		/*XXX recover stackTop of selected process */
		
        // setting tss for user process
		// switch kernel stack


	// make blocked processes sleep time -1, sleep time to 0, re-run
	while (i != current)
	{
		if (pcb[i].state == STATE_BLOCKED)
		{
			pcb[i].sleepTime -= 1;
			if (pcb[i].sleepTime == 0) pcb[i].state = STATE_RUNNABLE;
		}
		i = (i+1) % MAX_PCB_NUM; // update
	}

	pcb[current].timeCount += 1;
	if (pcb[current].timeCount < MAX_TIME_COUNT) // time count not max, process continue
		return;

	else // time count to max, switch to next runnable process
	{
		pcb[current].timeCount = 0;
		pcb[current].state = STATE_RUNNABLE;

		// find another runnable process, none idle processes have priority
		i = (current+1) % MAX_PCB_NUM;
		if (i == 0) // if next process is idle, skip it and try to find another runnable process
		i = (i+1) % MAX_PCB_NUM;

		while (i != current)
		{
			if (pcb[i].state == STATE_RUNNABLE && i != 0)
				break;
			i = (i+1) % MAX_PCB_NUM;
		}
		if (i == current)
			i = 0;

		pcb[i].state = STATE_RUNNING; // switch to running state
		pcb[i].timeCount = 0; // set time count to 0

		putChar(i + '0'); /* echo pid of selected process */

		/*XXX recover stackTop of selected process */
		tmpStackTop = pcb[i].stackTop;
		pcb[i].stackTop = pcb[i].prevStackTop;

		// setting tss for user process
		tss.esp0 = pcb[i].stackTop;
		// switch to new process
		current = i;
	}
	
		// switch kernel stack
		asm volatile("movl %0, %%esp"::"m"(tmpStackTop)); 

		asm volatile("popl %gs");
		asm volatile("popl %fs");
		asm volatile("popl %es");
		asm volatile("popl %ds");
		asm volatile("popal");
		asm volatile("addl $8, %esp");
		asm volatile("iret");
	
}

void syscallHandle(struct StackFrame *sf) {
	switch(sf->eax) { // syscall number
		case 0:
			syscallWrite(sf);
			break; // for SYS_WRITE
		case 1:
			syscallFork(sf);
			break; // for SYS_FORK
		case 2:
			syscallExec(sf);
			break; // for SYS_EXEC
		case 3:
			syscallSleep(sf);
			break; // for SYS_SLEEP
		case 4:
			syscallExit(sf);
			break; // for SYS_EXIT
		default:break;
	}
}

void syscallWrite(struct StackFrame *sf) {
	switch(sf->ecx) { // file descriptor
		case 0:
			syscallPrint(sf);
			break; // for STD_OUT
		default:break;
	}
}

void syscallPrint(struct StackFrame *sf) {
	int sel = sf->ds; //TODO segment selector for user data, need further modification
	char *str = (char*)sf->edx;
	int size = sf->ebx;
	int i = 0;
	int pos = 0;
	char character = 0;
	uint16_t data = 0;
	asm volatile("movw %0, %%es"::"m"(sel));
	for (i = 0; i < size; i++) {
		asm volatile("movb %%es:(%1), %0":"=r"(character):"r"(str+i));
		if(character == '\n') {
			displayRow++;
			displayCol=0;
			if(displayRow==25){
				displayRow=24;
				displayCol=0;
				scrollScreen();
			}
		}
		else {
			data = character | (0x0c << 8);
			pos = (80*displayRow+displayCol)*2;
			asm volatile("movw %0, (%1)"::"r"(data),"r"(pos+0xb8000));
			displayCol++;
			if(displayCol==80){
				displayRow++;
				displayCol=0;
				if(displayRow==25){
					displayRow=24;
					displayCol=0;
					scrollScreen();
				}
			}
		}
		//asm volatile("int $0x20"); //XXX Testing irqTimer during syscall
		//asm volatile("int $0x20":::"memory"); //XXX Testing irqTimer during syscall
	}
	
	updateCursor(displayRow, displayCol);
	//TODO take care of return value
	return;
}

void syscallFork(struct StackFrame *sf) {
    // find empty pcb
	int i, j;
	for (i = 0; i < MAX_PCB_NUM; i++) {
		if (pcb[i].state == STATE_DEAD)
			break;
	}
	if (i != MAX_PCB_NUM) {
		/*XXX copy userspace
		  XXX enable interrupt
		 */
		enableInterrupt();
		for (j = 0; j < 0x100000; j++) {
			*(uint8_t *)(j + (i+1)*0x100000) = *(uint8_t *)(j + (current+1)*0x100000);
			//asm volatile("int $0x20"); //XXX Testing irqTimer during syscall
		}
		/*XXX disable interrupt
		 */
		disableInterrupt();
		/*XXX set pcb
		  XXX pcb[i]=pcb[current] doesn't work
		*/

		for (j = 0; j < MAX_STACK_SIZE; j++)
			pcb[i].stack[j] = pcb[current].stack[j];

		pcb[i].stackTop = (uint32_t)&(pcb[i].regs);
		pcb[i].prevStackTop = (uint32_t)&(pcb[i].stackTop);

		pcb[i].state = STATE_RUNNABLE;

		// use Linux V2.6's schedule strategy
		int time_rest = MAX_TIME_COUNT - pcb[current].timeCount;
		if ((time_rest % 2) == 0) // if rest time is a even number, bisects it
		{
			pcb[i].timeCount = pcb[current].timeCount + time_rest / 2;
			pcb[current].timeCount += time_rest / 2;
		}
		else // otherwise make father process 1 more time slice
		{
			pcb[i].timeCount = pcb[current].timeCount + time_rest / 2 + 1;
			pcb[current].timeCount += time_rest / 2;
		}
		
		pcb[i].sleepTime = 0;
		pcb[i].pid = i;
		
		/*XXX set regs */
		pcb[i].regs.gs = USEL(2 * i + 2);
		pcb[i].regs.fs = USEL(2 * i + 2);
		pcb[i].regs.es = USEL(2 * i + 2);
		pcb[i].regs.ds = USEL(2 * i + 2);

		pcb[i].regs.edi = pcb[current].regs.edi;
		pcb[i].regs.esi = pcb[current].regs.esi;
		pcb[i].regs.ebp = pcb[current].regs.ebp;
		pcb[i].regs.xxx = pcb[current].regs.xxx;
		pcb[i].regs.ebx = pcb[current].regs.ebx;
		pcb[i].regs.edx = pcb[current].regs.edx;
		pcb[i].regs.ecx = pcb[current].regs.ecx;

		pcb[i].regs.irq = pcb[current].regs.irq;
		pcb[i].regs.error = pcb[current].regs.error;

		pcb[i].regs.eip = pcb[current].regs.eip;
		pcb[i].regs.cs = USEL(2 * i + 1);
		pcb[i].regs.eflags = pcb[current].regs.eflags;
		pcb[i].regs.esp = pcb[current].regs.esp;
		pcb[i].regs.ss = USEL(2 * i + 2);
		/*XXX set return value */
		pcb[i].regs.eax = 0;
		pcb[current].regs.eax = i;
	}
	else {
		pcb[current].regs.eax = -1;
	}
	return;
}

void syscallExec(struct StackFrame *sf) {
	return;
}

void syscallSleep(struct StackFrame *sf) {
	pcb[current].state = STATE_BLOCKED; // process blocks itself
	pcb[current].sleepTime = sf->ecx; // set sleep time slices

	// find another runnable process, none idle processes have priority
	int i = (current+1) % MAX_PCB_NUM;
	if (i == 0) // if next process is idle, skip it and try to find another runnable process
		i = (i+1) % MAX_PCB_NUM;

	while (i != current)
	{
		if (pcb[i].state == STATE_RUNNABLE && i != 0)
			break;
		i = (i+1) % MAX_PCB_NUM;
	}
	if (i == current)
		i = 0;


	uint32_t tmpStackTop;
	pcb[i].state = STATE_RUNNING; // switch to running state
	pcb[i].timeCount = 0; // set time count to 0

	putChar(i + '0'); /* echo pid of selected process */

	/*XXX recover stackTop of selected process */
	tmpStackTop = pcb[i].stackTop;
	pcb[i].stackTop = pcb[i].prevStackTop;

	// setting tss for user process
	tss.esp0 = pcb[i].stackTop;
	// switch to new process
	current = i;

	// switch kernel stack
	asm volatile("movl %0, %%esp"::"m"(tmpStackTop)); 
	asm volatile("popl %gs");
	asm volatile("popl %fs");
	asm volatile("popl %es");
	asm volatile("popl %ds");
	asm volatile("popal");
	asm volatile("addl $8, %esp");
	asm volatile("iret");	
	//return;
}

void syscallExit(struct StackFrame *sf) {
	pcb[current].state = STATE_DEAD; // switch to dead state

	// recover resources logically, that means doing nothing



	// find another runnable process, none idle processes have priority
	int i = (current+1) % MAX_PCB_NUM;
	if (i == 0) // if next process is idle, skip it and try to find another runnable process
		i = (i+1) % MAX_PCB_NUM;

	while (i != current)
	{
		if (pcb[i].state == STATE_RUNNABLE && i != 0)
			break;
		i = (i+1) % MAX_PCB_NUM;
	}
	if (i == current)
		i = 0;


	uint32_t tmpStackTop;
	pcb[i].state = STATE_RUNNING; // switch to running state
	pcb[i].timeCount = 0; // set time count to 0

	putChar(i + '0'); /* echo pid of selected process */

	/*XXX recover stackTop of selected process */
	tmpStackTop = pcb[i].stackTop;
	pcb[i].stackTop = pcb[i].prevStackTop;

	// setting tss for user process
	tss.esp0 = pcb[i].stackTop;
	// switch to new process
	current = i;

	// switch kernel stack
	asm volatile("movl %0, %%esp"::"m"(tmpStackTop)); 
	asm volatile("popl %gs");
	asm volatile("popl %fs");
	asm volatile("popl %es");
	asm volatile("popl %ds");
	asm volatile("popal");
	asm volatile("addl $8, %esp");
	asm volatile("iret");	
	//return;
}
