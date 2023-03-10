#include "sched.h"
#include "irq.h"
#include "printf.h"
#include "timer.h"

static struct task_struct init_run_task = INIT_RUN_TASK;
struct task_struct *current = &(init_run_task);
struct task_struct * task[NR_TASKS] = {&(init_run_task), };
int nr_tasks = 1;
// static struct task_struct init_wait_task = INIT_WAIT_TASK;
// struct task_struct * wait_task[NR_TASKS] = {&(init_wait_task),};
// int nr_wait_task = 0;


void _schedule(void)
{
	int next, c;
	struct task_struct * p;
	// struct task_struct * w;
	while (1) {
		c = -1;	// the maximum counter found so far
		next = 0;

		// for (int i=0; i<NR_TASKS;i++){
		// 	printf("check wait");
		// 	w = task[i];
		// 	if (get_current_time1()>=w->resume){
		// 		w->state=TASK_READY;
		// 		// switch_to(wait_task[i]); 
		// 		break;
		// 	}else if(w->state==TASK_READY){
		// 		next=i;
		// 		if(c){
		// 			break;
		// 		}
		// 	}
		// }

		/* Iterates over all tasks and tries to find a task in 
		TASK_RUNNING state with the maximum counter. If such 
		a task is found, we immediately break from the while loop 
		and switch to this task. */

		for (int i = 0; i < NR_TASKS; i++){
			p = task[i];
			if (p && (p->state == TASK_RUNNING) && p->counter > c) {
				c = p->counter;
				next = i;
			// }else i f (p->state == TASK_WAIT){
				// printf("task in wait\n");
				// wait_task[nr_wait_task] = p;
				// nr_wait_task++;
			}
		}
		if (c) {
			break;
		}


		next = 3;
		/* If no such task is found, this is either because i) no 
		task is in TASK_RUNNING state or ii) all such tasks have 0 counters.
		in our current implemenation which misses TASK_WAIT, only condition ii) is possible. 
		Hence, we recharge counters. Bump counters for all tasks once. */
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p) {
				p->counter = (p->counter >> 1) + p->priority; // The increment depends on a task's priority.
			}
			
		}

		/* loops back to pick the next task */
	}
	switch_to(task[next]);
}

void schedule(void)
{
	current->counter = 0;
	_schedule();
}

void switch_to(struct task_struct * next) 
{
	if (current == next) 
		return;
	struct task_struct * prev = current;
	current = next;
	cpu_switch_to(prev, next);
}

void schedule_tail(void) {
	/* nothing */
}

