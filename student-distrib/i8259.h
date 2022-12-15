/* i8259.h - Defines used in interactions with the 8259 interrupt
 * controller
 * vim:ts=4 noexpandtab
 */

#ifndef _I8259_H
#define _I8259_H

#include "types.h"

/* PIC Port Size */
#define PIC_SIZE 8

/* Slave PIC IRQ # */
#define SLAVE_IRQ 2

/* Ports that each PIC sits on */
#define MASTER_8259_PORT    0x20
#define SLAVE_8259_PORT     0xA0
#define Master_command	MASTER_8259_PORT
#define Master_data	(MASTER_8259_PORT+1)
#define Slave_command	SLAVE_8259_PORT
#define Slave_data	(SLAVE_8259_PORT+1)

/* Initialization control words to init each PIC.
 * See the Intel manuals for details on the meaning
 * of each word */               // command wording meaning

/*	|7|6|5|4|3|2|1|0|  ICW1
	 | | | | | | | `---- 1=ICW4 is needed, 0=no ICW4 needed
	 | | | | | | `----- 1=single 8259, 0=cascading 8259's
	 | | | | | `------ 1=4 byte interrupt vectors, 0=8 byte int vectors
	 | | | | `------- 1=level triggered mode, 0=edge triggered mode
	 | | | `-------- must be 1 for ICW1 (port must also be 20h or A0h)
	 `------------- must be zero for PC systems
*/

/*
	|7|6|5|4|3|2|1|0|  ICW2
	 | | | | | `-------- 000= on 80x86 systems
	 `----------------- A7-A3 of 80x86 interrupt vector
*/

/*
	|7|6|5|4|3|2|1|0|  ICW3 for Master Device
	 | | | | | | | `---- 1=interrupt request 0 has slave, 0=no slave
	 | | | | | | `----- 1=interrupt request 1 has slave, 0=no slave
	 | | | | | `------ 1=interrupt request 2 has slave, 0=no slave
	 | | | | `------- 1=interrupt request 3 has slave, 0=no slave
	 | | | `-------- 1=interrupt request 4 has slave, 0=no slave
	 | | `--------- 1=interrupt request 5 has slave, 0=no slave
	 | `---------- 1=interrupt request 6 has slave, 0=no slave
	 `----------- 1=interrupt request 7 has slave, 0=no slave

	|7|6|5|4|3|2|1|0|  ICW3 for Slave Device
	 | | | | | `-------- master interrupt request slave is attached to
	 `----------------- must be zero
*/

/*
	|7|6|5|4|3|2|1|0|  ICW4
	 | | | | | | | `---- 1 for 80x86 mode, 0 = MCS 80/85 mode
	 | | | | | | `----- 1 = auto EOI, 0=normal EOI
	 | | | | `-------- slave/master buffered mode (see below)
	 | | | `--------- 1 = special fully nested mode (SFNM), 0=sequential
	 `-------------- unused (set to zero)
*/
#define ICW1                0x11 // ICW4 needed
#define ICW2_MASTER         0x20 // interrupt vector address
#define ICW2_SLAVE          0x28 // interrupt vector address
#define ICW3_MASTER         0x04 // IR input S3 has slave 
#define ICW3_SLAVE          0x02 // Slave ID 
#define ICW4                0x01 // we are in 8086/8088 mode

/* End-of-interrupt byte.  This gets OR'd with
 * the interrupt number and sent out to the PIC
 * to declare the interrupt finished */
#define EOI                 0x60

#ifndef ASM

/* Externally-visible functions */

/* Initialize both PICs */
void i8259_init(void);
/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num);
/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num);
/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num);



#endif /* ASM */
#endif /* _I8259_H */
