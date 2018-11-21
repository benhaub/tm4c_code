#include "tm4c123gh6pm.h"

void init_sw1_and_sw2() {

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

    /* Enables for switch one - PF4 */
    GPIO_PORTF_PUR_R |= 0x10;
    GPIO_PORTF_DIR_R &= ~0x10;
    GPIO_PORTF_DEN_R |= 0x10;
}

void sw1_and_sw2_interrupt() {

      //interrupts for PF4
      GPIO_PORTF_IM_R &= ~0x10 ; // clear, disable (mask) int when config, p.609
      GPIO_PORTF_IS_R &= ~0x10; // edge sense for PF4
      GPIO_PORTF_IBE_R &= ~0x10; // not both edges for PF4
      GPIO_PORTF_IEV_R &= ~0x10; // falling edge event for PF4
      GPIO_PORTF_ICR_R = 0x10; // clear int flag
      GPIO_PORTF_IM_R |= 0x10; // arm interrupt on PF4 after configured

      //interrupts for PF0 - sw2
      GPIO_PORTF_IM_R &= ~0x01; // Disable interrupts while we set them up
      GPIO_PORTF_IS_R &= ~0x01;
      GPIO_PORTF_IBE_R &= ~0x01; // Do not interrupt on both edges
      GPIO_PORTF_IEV_R &= ~0x01; // Detect an interrupt on the falling edge of switch 2
      GPIO_PORTF_ICR_R = 0x01; //clear interrupts
      GPIO_PORTF_IM_R |= 0x01; //Re-enable interrupts now that set up is done

      // we want INT30, i.e., 4n+2, 4*7+2, bits 23:21, we set priority level to say '5' ==> 101 => 1010 = A
      NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF0FFFFF)|0x00A00000; // priority 5, lower means higher priority
      NVIC_EN0_R = 0x40000000;  // finally enable Interrupt # 30 (i.e., IRQ30 ==> bit 30 of NVIC_EN0), GPIO PortF
}
