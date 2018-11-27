#include "tm4c123gh6pm.h"
#include <inttypes.h>

/*
 * Initialize and execute General Purpose Timer Module 0, using counter B
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
 *          0x7FF - 50% duty
 */
void init_timer0B(unsigned int control, unsigned int mode, int dir, int inv, unsigned int frequency, uint32_t period, unsigned int width) {

    volatile unsigned long delay_clk;
    SYSCTL_RCGCTIMER_R |= 0x01;
    delay_clk = SYSCTL_RCGCTIMER_R; //delay to allow the clock to settle, no operation
    /* Disable timerB while we set it up */
    if(mode == 0x00A) {
        init_timer0B_PWM(control, mode, period, width, inv);
        return;
    }
    TIMER0_CTL_R &= ~0x0100;
    TIMER0_CFG_R |= control;
    TIMER0_TBMR_R |= mode;
    if(dir >= 0)
        TIMER0_TBMR_R |= 0x10;
    else
        TIMER0_TBMR_R &= ~0x10;
    /* Pre-scale. Controls the timer frequency to be bus frequency / prescale + 1 */
    TIMER0_TBPR_R = frequency;
    TIMER0_TBILR_R = period;
    TIMER0_TBMATCHR_R |= width;
    /* In PWM mode, PMR is used alongside MATCHR to change the value of */
    /* the PWML bit */
    if(inv >= 0)
        TIMER0_CTL_R &= ~0x4000;
    else
        TIMER0_CTL_R |= 0x4000;
    TIMER0_CTL_R |= 0x0100;
}
/*
 * Helper function for init_timer0B to start up the pulse width modulator.
 * Only these registers are enabled when using PWM capability. Enabling
 * extra registers will disable the PWM.
 */
void init_timer0B_PWM(unsigned int control, unsigned int mode, uint32_t period, unsigned int width, int inv) {

    TIMER0_CTL_R &= ~0x0100;
    TIMER0_CFG_R |= control;
    TIMER0_TBMR_R |= mode;
    TIMER0_TBILR_R = period;
    TIMER0_TBMATCHR_R |= width;
    if(inv >= 0)
        TIMER0_CTL_R &= ~0x4000;
    else
        TIMER0_CTL_R |= 0x4000;
    TIMER0_CTL_R |= 0x0100;
}
