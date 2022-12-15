/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t Mask_Master; /* IRQs 0-7  */
uint8_t Mask_Slave;  /* IRQs 8-15 */

/* 
 * i8259_init
 *   DESCRIPTION: Initialize the 8259 PIC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: PIC will be initailzed with all IRQs masked,
 *                 except the IRQ2 for the connection to slave PIC.
 *   REFERENCE: https://wiki.osdev.org/8259_PIC
 */
void i8259_init(void)
{
    // Reinitialize Mask
    Mask_Master = 0xFF;
    Mask_Slave = 0xFF;
    
    // Send Masked Mask
    outb(Mask_Master, Master_data);
    outb(Mask_Slave, Slave_data);
    
    // Send Initialization Sequence
    outb(ICW1, Master_command);
    outb(ICW1, Slave_command);

    outb(ICW2_MASTER, Master_data);
    outb(ICW2_SLAVE, Slave_data);

    outb(ICW3_MASTER, Master_data);
    outb(ICW3_SLAVE, Slave_data);

    outb(ICW4, Master_data);
    outb(ICW4, Slave_data);

    // Enable Slave PIC IRQ #
    enable_irq(SLAVE_IRQ);
}

/* 
 * enable_irq
 *   DESCRIPTION: Enable (unmask) the specified IRQ
 *   INPUTS: irq_num - IRQ # to enable
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: PIC will unmask the requested IRQ #.
 *   REFERENCE: https://wiki.osdev.org/8259_PIC
 */
void enable_irq(uint32_t irq_num)
{
    uint16_t port_num;
    uint8_t value;

    // Check if the IRQ # is on primary or slave
    if (irq_num < PIC_SIZE)
    {
        port_num = Master_data;
        value = Mask_Master & ~(1 << irq_num);
        Mask_Master = value;
    }
    else
    {
        port_num = Slave_data;
        irq_num -= PIC_SIZE;
        value = Mask_Slave & ~(1 << irq_num);
        Mask_Slave = value;
    }

    // Send modified mask
    outb (value, port_num);
}

/* 
 * disable_irq
 *   DESCRIPTION: Disable (mask) the specified IRQ
 *   INPUTS: irq_num - IRQ # to disable
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: PIC will mask the requested IRQ #.
 *   REFERENCE: https://wiki.osdev.org/8259_PIC
 */
void disable_irq(uint32_t irq_num)
{
    uint16_t port_num;
    uint8_t value;

    // Check if the IRQ # is on primary or slave
    if (irq_num < PIC_SIZE){
        port_num = Master_data;
        value = Mask_Master | (1 << irq_num);
        Mask_Master = value;
    }
    else{
        port_num = Slave_data;
        irq_num -= PIC_SIZE;
        value = Mask_Slave | (1 << irq_num);
        Mask_Slave = value;
    }

    // Send modified mask
    outb (value, port_num);
}

/* 
 * send_eoi
 *   DESCRIPTION: Send end-of-interrupt signal for the specified IRQ
 *   INPUTS: irq_num - IRQ # to send EOI
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Send EOI signal to PIC for the requested IRQ #.
 *   REFERENCE: https://wiki.osdev.org/8259_PIC
 */
void send_eoi(uint32_t irq_num)
{
    // Master PIC has interrupt number 0-7
    if (irq_num < PIC_SIZE)
    {
        outb(EOI | irq_num, Master_command);
    }
    else
    {
        // Slave pic
        irq_num -= PIC_SIZE;

        // Slave is connect to master's second IRQ 
        outb(EOI | SLAVE_IRQ, Master_command);
        outb(EOI | irq_num, Slave_command);
    }
}
