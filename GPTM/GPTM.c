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
/*
 * In one shot mode, the timer counts until the timeout event, then stops.
 * The timer is enabled at the end of this routine
 *
 * param config:
 *          1 for 32 bit timer
 *          0 for 16 bit timer
 *
 * param dir:
 *          1 for up counter
 *          0 for down counter
 *
 * param snap (Pg. 687):
 *          Use snapshot mode for one-shot and period mode. Use 1 to
 *          enable, 0 to disable. Snap shot saves the value
 *          of TBV in GPTMBR when the timeout event occurs.
 *
 * param wot (Pg. 671):
 *          Wait on trigger. If 0, the timer begins as soon as TBEN is turned
 *          on (as soon as this function returns). If 1, does not start counting
 *          until the previous counter in the daisy chain reaches it's timeout event.
 *
 * param load:
 *          The load value for the time out even in the interval load register.
 *          If counting up, count until this number. If counting down, start at
 *          this number.
 *
 * The timeout event is dependent on the direction of the timer. If counting
 * up, the timer starts at zero, and counts until the value in ILR. If counting
 * down, the timer starts at ILR, and counts to 0 (Pg. 710).
 */
void init_timer0B_oneShot(int config, int dir, int snap, int wot, int load) {

    volatile unsigned long delay_clk;
    SYSCTL_RCGCTIMER_R |= 0x01;
    delay_clk = SYSCTL_RCGCTIMER_R; //delay to allow the clock to settle, no operation
    /* Disable TimerB for setup */
    TIMER0_CTL_R &= ~0x0100; //Pg. 690
    if(config)
        TIMER0_CFG_R |= 0x000; //Pg. 680
    else
        TIMER0_CFG_R |= 0x004;
    TIMER0_TBMR_R |= 0x01;
    if(snap)
        TIMER0_TBMR_R |= 0x080;
    else
        TIMER0_TBMR_R &= ~0x080;
    if(wot)
        TIMER0_TBMR_R |= 0x040;
    else
        TIMER0_TBMR_R &= ~0x040;
    TIMER0_TBILR_R = load;
    /* Enable the timer */
    TIMER0_CTL_R |= 0x0100;
}
/*
 * Once the timeout event is reached, the counter is reset, and starts counting
 * again.
 *
 * param config:
 *          1 for 32 bit timer
 *          0 for 16 bit timer
 *
 * param dir:
 *          1 for up counter
 *          0 for down counter
 *
 * param snap (Pg. 687):
 *          Use snapshot mode for one-shot and period mode. Use 1 to
 *          enable, 0 to disable. Snap shot saves the value
 *          of TBV in GPTMBR when the timeout event occurs.
 *
 * param wot (Pg. 671):
 *          Wait on trigger. If 0, the timer begins as soon as TBEN is turned
 *          on (as soon as this function returns). If 1, does not start counting
 *          until the previous counter in the daisy chain reaches it's timeout event.
 *
 * param load:
 *          The load value for the time out even in the interval load register.
 *          If counting up, count until this number. If counting down, start at
 *          this number.
 *
 * The timeout event is dependent on the direction of the timer. If counting
 * up, the timer starts at zero, and counts until the value in ILR. If counting
 * down, the timer starts at ILR, and counts to 0.
 */
void init_timer0B_periodic(int config, int dir, int snap, int wot, int load) {

    volatile unsigned long delay_clk;
    SYSCTL_RCGCTIMER_R |= 0x01;
    delay_clk = SYSCTL_RCGCTIMER_R; //delay to allow the clock to settle, no operation
    /* Disable TimerB for setup */
    TIMER0_CTL_R &= ~0x0100;
    if(config)
        TIMER0_CFG_R |= 0x000;
    else
        TIMER0_CFG_R |= 0x004;
    TIMER0_TBMR_R |= 0x02;
    if(dir)
        TIMER0_TBMR_R |= 0x010;
    else
        TIMER0_TBMR_R &= ~0x010;
    if(snap)
        TIMER0_TBMR_R |= 0x080;
    else
        TIMER0_TBMR_R &= ~0x080;
    if(wot)
        TIMER0_TBMR_R |= 0x040;
    else
        TIMER0_TBMR_R &= ~0x040;
    TIMER0_TBILR_R = load;
    /* Enable the timer */
    TIMER0_CTL_R |= 0x0100;
}
