#include "sched.h"
#include "irq.h"
#include "printf.h"
#include "fork.h"
#include "peripherals/timer.h"
#include "sched.h"
#include "utils.h"
#include "mm.h"


static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct * task[NR_TASKS] = {&(init_task), };
static struct task_switch init_switch = INIT_SWITCH;
struct task_switch *_switch = &(init_switch);
struct task_switch *switches[50] = {&(init_switch),};
struct task_switch *s;
int switch_ct = 1;

int nr_tasks = 1;


void preempt_disable(void)
{
	current->preempt_count++;
}

void preempt_enable(void)
{
	current->preempt_count--;
}


void _schedule(void)
{
	/* ensure no context happens in the following code region
		we still leave irq on, because irq handler may set a task to be TASK_RUNNING, which 
		will be picked up by the scheduler below */
	preempt_disable(); 
	int next,c;
	struct task_struct * p;
	while (1) {
		c = -1; // the maximum counter of all tasks 
		next = 0;

		/* Iterates over all tasks and tries to find a task in 
		TASK_RUNNING state with the maximum counter. If such 
		a task is found, we immediately break from the while loop 
		and switch to this task. */

		for (int i = 0; i < NR_TASKS; i++){
			p = task[i];
			if (p && p->state == TASK_RUNNING && p->counter > c) {
				c = p->counter;
				next = i;
			}
		}
		if (c) {
			break;
		}

		/* If no such task is found, this is either because i) no 
		task is in TASK_RUNNING state or ii) all such tasks have 0 counters.
		in our current implemenation which misses TASK_WAIT, only condition ii) is possible. 
		Hence, we recharge counters. Bump counters for all tasks once. */
		
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p) {
				p->counter = (p->counter >> 1) + p->priority;
			}
		}
	}

	switch_to(task[next]);
	switches[switch_ct] = s;
	switch_ct+=1;
	preempt_enable();
}

void schedule(void)
{
	if (switch_ct<=50){
		current->counter = 0;
		_schedule();
	}else{
		printf("printing stack trace...");
		for (int i=0; i<50; i++){
			printf("t=%d	from pid %d (PC 0x%x SP 0x%x)	to pid %d (PC 0x%x\tSP 0x%x) \n", switches[i]->timestamp, switches[i]->prev_pid, switches[i]->prev_pc, switches[i]->prev_sp, switches[i]->next_pid, switches[i]->next_pc, switches[i]->next_sp);
		}
		switch_ct=1;
	}

}

int get_time_ms(void){
	// unsigned long count = (get32(TIMER_CHI) << 31) + get32(TIMER_CLO);
	unsigned long count = get_cnt_pct();
	unsigned long freq = get_cnt_frq();
	// printf("--%d/%d--", count, freq);
	return (count*1000)/freq;
}

void switch_to(struct task_struct * next) 
{
	if (current == next) 
		return;
	struct task_struct * prev = current;
	current = next;
	s = (struct task_switch *) get_free_page();
	s->timestamp = get_time_ms();
	asm volatile("mrs %0, elr_el1" : "=r" (s->prev_pc));
	asm volatile("mov %0, sp" : "=r" (s->prev_sp));
	s->prev_pid=prev->pid;
	s->next_pid=next->pid;

	cpu_switch_to(prev, next);
}

void get_next_pc_sp(void){
	asm volatile("mrs %0, elr_el1" : "=r" (s->next_pc));
	asm volatile("mov %0, sp" : "=r" (s->next_sp));
}

void schedule_tail(void) {
	preempt_enable();
}

int get_pid(void){
	return current->pid;
}


void timer_tick()
{
	--current->counter;
	if (current->counter>0 || current->preempt_count>0) {
		return;
	}
	current->counter=0;
	enable_irq();
	// printf("||||| %d |||||", get_time_ms());
	printf("|%d ms|", get_time_ms());

	_schedule();

	// printf("||pid: %d||", get_pid());
	disable_irq();

}
