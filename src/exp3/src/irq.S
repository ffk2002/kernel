.globl irq_vector_init
irq_vector_init:
    adr x0, vectors     // load VBAR_EL1 with virtual
    msr vbar_el1, x0        // vector table address
    ret

// .globl irq_vector_el2_init
// irq_vector_el2_init:
// 	adr x0, vectors_el2     // load VBAR_EL1 with virtual
// 	msr vbar_el2, x0        // vector table address
// 	ret

.globl enable_irq
enable_irq:
    msr daifclr, #2
    ret

.globl disable_irq
disable_irq:
    msr daifset, #0xf
    ret
