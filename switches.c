#include "tm4c123gh6pm.h"
#include <stdlib.h>

void init_sw1() {

    volatile unsigned long delay_clk; // delay for clock, must have 3 sys clock delay, p.424
    SYSCTL_RCGC2_R |= 0x00000020; // clock enable for port F, p.424
    delay_clk = SYSCTL_RCGC2_R; // must delay for the clock to settle, no-operation

    /* Enables for switch one - PF4 */
    GPIO_PORTF_PUR_R |= 0x10;
    GPIO_PORTF_DIR_R &= ~0x10;
    GPIO_PORTF_DEN_R |= 0x10;
}

void init_sw2() {

    volatile unsigned long delay_clk; // delay for clock, must have 3 sys clock delay, p.424
    SYSCTL_RCGC2_R |= 0x00000020; // clock enable for port F, p.424
    delay_clk = SYSCTL_RCGC2_R; // must delay for the clock to settle, no-operation

    // the following two lines are needed to unlock SW2
    GPIO_PORTF_LOCK_R  |= 0x4C4F434B; // to unlock SW2, p.637-8
    GPIO_PORTF_CR_R |= 0x01; // to unlock SW2

    /* Enables for switch two - PF0 */
    GPIO_PORTF_PUR_R |= 0x01;
    GPIO_PORTF_DIR_R &= ~0x01;
    GPIO_PORTF_DEN_R |= 0x01;
    GPIO_PORTF_AFSEL_R |= 0x00; /* Pins function as GPIO */
}
/*
 * Don't forget to add the interupt handler - GPIOPortF_Handler(void) {
 * 
 * 					      }
 * 
 * param edgeSense:
 *          0 edge sense,
 *          1 for level sense
 *
 * param bothEdges:
 *          0 do not interrupt on both edges
 *          1 interrupt on both rising and falling edges of the clock
 *
 * param edgeEvent:
 *          0 interrupt on the falling edge of the clock
 *          1 interrupt on the rising edge of the clock
 *
 */
void sw1_interrupt(int edgeSense, int bothEdges, int edgeEvent, int pri) {

    int priority;

    //interrupts for PF4
    GPIO_PORTF_IM_R &= ~0x10 ; // clear, disable (mask) int when config, p.609
    if(!edgeSense)
        GPIO_PORTF_IS_R &= ~0x10; // edge sense for PF4
    else
        GPIO_PORTF_IS_R |= 0x10;
    if(!bothEdges)
        GPIO_PORTF_IBE_R &= ~0x10; // not both edges for PF4
    else
        GPIO_PORTF_IBE_R |= 0x10;
    if(!edgeEvent)
        GPIO_PORTF_IEV_R &= ~0x10; // falling edge event for PF4
    else
        GPIO_PORTF_IEV_R |= 0x10;

    GPIO_PORTF_ICR_R = 0x10; // clear int flag
    GPIO_PORTF_IM_R |= 0x10; // arm interrupt on PF4 after configured

    /* we want INT30, i.e., 4n+2, 4*7+2, bits 23:21, we set priority level to */
    /* say '5' ==> 101 => 1010 = A. We choose 4n + 2 because we need an equation */
    /* that gives n as a whole number and a valid priority number. */
    priority = pri << 21;
    NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF0FFFFF) | priority; // priority 5, lower means higher priority
    NVIC_EN0_R = 0x40000000;  // finally enable Interrupt # 30 (i.e., IRQ30 ==> bit 30 of NVIC_EN0), GPIO PortF
}

/*
 * Don't forget to add the interupt handler - GPIOPortF_Handler(void) {
 * 
 * 					      }
 * 
 * param edgeSense:
 *          0 edge sense,
 *          1 for level sense
 *
 * param bothEdges:
 *          0 do not interrupt on both edges
 *          1 interrupt on both rising and falling edges of the clock
 *
 * param edgeEvent:
 *          0 interrupt on the falling edge of the clock
 *          1 interrupt on the rising edge of the clock
 *
 * param priority
 *          The priority of the interrupt, from 0 to 7. The lower the number,
 *          the higher the priority.
 *
 */
void sw2_interrupt(int edgeSense, int bothEdges, int edgeEvent, int pri) {

      int priority;

      //interrupts for PF0 - sw2
      GPIO_PORTF_IM_R &= ~0x01; // Disable interrupts while we set them up
      if(!edgeSense)
          GPIO_PORTF_IS_R &= ~0x01;
      else
          GPIO_PORTF_IS_R |= 0x01;
      if(!bothEdges)
          GPIO_PORTF_IBE_R &= ~0x01; // Do not interrupt on both edges
      else
          GPIO_PORTF_IBE_R |= 0x01;
      if(!edgeEvent)
          GPIO_PORTF_IEV_R &= ~0x01; // Detect an interrupt on the falling edge of switch 2
      else
          GPIO_PORTF_IEV_R |= 0x01;

      GPIO_PORTF_ICR_R = 0x01; //clear interrupts
      GPIO_PORTF_IM_R |= 0x01; //Re-enable interrupts now that set up is done

      // we want INT30, i.e., 4n+2, 4*7+2, bits 23:21, we set priority level to say '5' ==> 101 => 1010 = A
      /* We choose 4n + 2 because we need an equation that gives n as a whole number and a valid priority number. */
      priority = pri << 21;
      NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF0FFFFF) | priority;
      NVIC_EN0_R = 0x40000000;  // finally enable Interrupt # 30 (i.e., IRQ30 ==> bit 30 of NVIC_EN0), GPIO PortF
}
