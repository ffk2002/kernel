#include "sched.h"

.globl cpu_switch_to
cpu_switch_to:
	mov	x10, #THREAD_CPU_CONTEXT
	add	x8, x0, x10
	mov	x9, sp
	stp	x19, x20, [x8], #16		// store callee-saved registers
	stp	x21, x22, [x8], #16
	stp	x23, x24, [x8], #16
	stp	x25, x26, [x8], #16
	stp	x27, x28, [x8], #16
	stp	x29, x9, [x8], #16
	str	x30, [x8]				// x30=LR. this saves in task_struct the "return-to" address, i.e. the instruction right after the cpu_switch_to() callsite 
	add	x8, x1, x10
	ldp	x19, x20, [x8], #16		// restore callee-saved registers
	ldp	x21, x22, [x8], #16
	ldp	x23, x24, [x8], #16
	ldp	x25, x26, [x8], #16
	ldp	x27, x28, [x8], #16
	ldp	x29, x9, [x8], #16
	ldr	x30, [x8]				// load LR from memory ...
	mov	sp, x9
	ret							// branch to LR (which was just loaded from memory)

.globl get_cnt_frq
get_cnt_frq:
	mrs x0, CNTFRQ_EL0
	ret

.globl get_cnt_pct
get_cnt_pct:
	mrs x0, CNTPCT_EL0
	ret
