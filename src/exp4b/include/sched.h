#ifndef _SCHED_H
#define _SCHED_H

#define THREAD_CPU_CONTEXT			0 		// offset of cpu_context in task_struct 

#ifndef __ASSEMBLER__

#define THREAD_SIZE				4096

#define NR_TASKS				64 

#define FIRST_TASK task[0]
#define LAST_TASK task[NR_TASKS-1]

#define TASK_RUNNING				0

extern struct task_struct *current;
extern struct task_struct * task[NR_TASKS];
extern int nr_tasks;

// extern struct task_switch * _switch;
extern struct task_switch * switches[50];
extern int switch_ct;


// int get_pid(void);

struct cpu_context {
	unsigned long x19;
	unsigned long x20;
	unsigned long x21;
	unsigned long x22;
	unsigned long x23;
	unsigned long x24;
	unsigned long x25;
	unsigned long x26;
	unsigned long x27;
	unsigned long x28;
	unsigned long fp;
	unsigned long sp;
	unsigned long pc;
};

struct task_struct {
	struct cpu_context cpu_context;
	long state;	
	long counter; /* countdown for scheduling. higher value means having run for less. recharged in schedule(). decremented in timer_tick(). always non negative */
	long priority;
	long preempt_count;
	int pid;
};

struct task_switch {
	unsigned long timestamp;
	unsigned long prev_pc;
	unsigned long next_pc;
	unsigned long prev_sp;
	unsigned long next_sp;
	int prev_pid;
	int next_pid;
};

extern void sched_init(void);
extern void schedule(void);
extern void timer_tick(void);
extern void preempt_disable(void);
extern void preempt_enable(void);
extern void switch_to(struct task_struct* next);
extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
extern int get_pid(void);
extern unsigned long get_cnt_frq();
extern unsigned long get_cnt_pct();
extern void get_next_pc_sp(void);
int get_time_ms(void);

#define INIT_TASK \
/*cpu_context*/	{ {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state etc */	0,0,1, 0 \
}

#define INIT_SWITCH \
{0,0,0,0,0,0}


#endif
#endif
