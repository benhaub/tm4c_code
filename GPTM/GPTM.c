#include "tm4c123gh6pm.h"
#include <inttypes.h>
#include <stdlib.h>

/*
 * Initialize and enable General Purpose Timer Module 0, using counter B
 * with inverted output. The output pin is PF4
 *
 * param control:
 *          0x00 - 32-bit timer
 *          0x01 - 32-bit real time clock
 *          0x04 - 16-bit timer
 *
 * param mode:
 *          0x001 - One-shot Timer mode
 *          0x002 - Periodic Timer mode
 *          0x003 - Capture mode
 *          0x009 - PWM mode w/ One-Shot
 *          0x00A - PWM mode w/ periodic
 *
 * param dir:
 *          zero or positive number for up counter, negative number for down counter.
 *
 * param inv:
 *          zero or positive number for non-inverted output (TBPWML), negative for inverted.
 *
 * param frequency (prescaler):
 *          The frequency with which the timer will count. if zero,
 *          the frequency matches the bus frequency, else it is
 *          busFrequency / (prescale + 1). If using PWM mode, the
 *          frequency value is ignored.
 *
 * param period:
 *          When the timer reaches the timeout event TIMER0_TBV = 0x0,
 *          it reloads the value from period. period is 32 bits in length,
 *          so the max amount of time we can wait is 2^32*(1/busFrequency).
 *
 * param width:
 *          Timer match event. Uses low bits Useful values:
 *          0x7FF - 50% duty if using max period (0xFFF).
 *
 * param snap (Pg. 687):
 *          Use snapshot mode for one-shot and period mode. Use a positive
 *          number or 0 to enable, negative to disable. Snap shot save the value
 *          of TBV in GPTMBR when the timeout event occurs. The value
 *          of this parameter does not matter if using PWM mode.
 *
 * param wot (Pg. 671):
 *          Wait on trigger. If negative, the timer begins as soon as TBEN is turned
 *          on (as soon as this function returns). If 0 or greater, does not start counting
 *          until the previous counter in the daisy chain reaches it's timeout event. This
 *          mode is allowed for PWM, but currently has no effect when using this function.
 *
 * param mte (Pg. 688)
 *          An interrupt is generated on MRIS when the match value in MATCHR is reach in one-shot
 *          or periodic modes. Value does not matter in PWM mode.
 */

void init_timer0B_PWMperiodic(unsigned int control, uint32_t period, unsigned int width, int inv) {

    volatile unsigned long delay_clk;
    SYSCTL_RCGCTIMER_R |= 0x01;
    delay_clk = SYSCTL_RCGCTIMER_R; //delay to allow the clock to settle, no operation
    TIMER0_CTL_R &= ~0x0100;
    TIMER0_CFG_R |= control;
    TIMER0_TBMR_R |= 0x00A;
    TIMER0_TBILR_R = period;
    TIMER0_TBMATCHR_R |= width;
    if(inv >= 0)
        TIMER0_CTL_R &= ~0x4000;
    else
        TIMER0_CTL_R |= 0x4000;
    TIMER0_CTL_R |= 0x0100;
}

void init_timer0B_PWMoneShot(unsigned int control, uint32_t period, unsigned int width, int inv) {

    volatile unsigned long delay_clk;
    SYSCTL_RCGCTIMER_R |= 0x01;
    delay_clk = SYSCTL_RCGCTIMER_R; //delay to allow the clock to settle, no operation
    TIMER0_CTL_R &= ~0x0100;
    TIMER0_CFG_R |= control;
    TIMER0_TBMR_R |= 0x009;
    TIMER0_TBILR_R = period;
    TIMER0_TBMATCHR_R |= width;
    if(inv >= 0)
        TIMER0_CTL_R &= ~0x4000;
    else
        TIMER0_CTL_R |= 0x4000;
    TIMER0_CTL_R |= 0x0100;
}

void init_timer0B_oneShot(int dir, int snap, int wot, int mte, uint32_t period) {


}

void init_timer0B_periodic(dir, snap, wot, mte, period) {


}

void init_timer0B_capture() {

    /* TODO: Implement */
}
