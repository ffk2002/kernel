#include "printf.h"
#include "utils.h"
#include "timer.h"
#include "irq.h"
#include "fork.h"
#include "sched.h"
#include "mini_uart.h"


void process(char *array)
{
	for (int i=0;i<5;i++){
		for (int i = 0; i < 5; i++){
			uart_send(array[i]);
			delay(5000000);
		}
	}
}

void process2(char *array)
{
	while (1) {
		// printf("|pid: %d|", get_pid());
		for (int i = 0; i < 5; i++){
			uart_send(array[i]);
			delay(5000000);
		}
	}
}




void kernel_main(void)
{
	uart_init();
	init_printf(0, putc);

	printf("kernel boots\n");

	irq_vector_init();
	generic_timer_init();
	enable_interrupt_controller();
	enable_irq();

	int res = copy_process((unsigned long)&process, (unsigned long)"12345");
	if (res != 0) {
		printf("error while starting process 1");
		return;
	}
	res = copy_process((unsigned long)&process2, (unsigned long)"abcde");
	if (res != 0) {
		printf("error while starting process 2");
		return;
	}
	res = copy_process((unsigned long)&process, (unsigned long)"67890");
	if (res != 0){
		printf("error while starting process 3");
		return;
	}
	res = copy_process((unsigned long)&process, (unsigned long)"fghij");
	if (res != 0){
		printf("error while starting process 3");
		return;
	}

	while (1){
		schedule();
	}	
}
