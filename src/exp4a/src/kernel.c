#include "printf.h"
#include "utils.h"
#include "timer.h"
#include "irq.h"
#include "fork.h"
#include "sched.h"
#include "mini_uart.h"

#ifdef USE_LFB
#include "lfb.h"
#endif

#ifdef USE_QEMU
#define CHAR_DELAY (5 * 5000000)
#else
#define CHAR_DELAY (1000000)
#endif

void sleep(int T){
	current->state = TASK_WAIT;
	current->sleep_time = T;
	schedule();
}

void idle(){
	while(1){
		printf("|idling...");
		//wfi
		asm("WFI");
		schedule();}
}

void process(char *array)
{
	while (1){
		for (int i = 0; i < 5; i++){
			uart_send(array[i]);
			delay(CHAR_DELAY);
		} 
		// uart_send('\n');
		enable_irq();
		sleep(3);
		schedule(); // yield
	}
}

void kernel_main(void)
{
	uart_init();
	init_printf(0, putc);
	irq_vector_init();
	generic_timer_init();
	enable_interrupt_controller();
	printf("kernel boots\r\n");	

#ifdef USE_LFB // (optional) init output to the graphical console
	lfb_init(); 
	lfb_showpicture();
	lfb_print(0, 240, "kernel boots");
#endif		

	int res = copy_process((unsigned long)&process, (unsigned long)"12345");
	// printf("run1\n");
	if (res != 0) {
		printf("error while starting process 1");
		return;
	}
	
	res = copy_process((unsigned long)&process, (unsigned long)"abcde");
	if (res != 0) {
		printf("error while starting process 2");
		return;
	}

	res = copy_process((unsigned long)&idle, (unsigned long)"wfi");
	if (res != 0) {
		printf("error while starting process 3");
		return;
	}

	while (1){
		schedule();
	}	
}
