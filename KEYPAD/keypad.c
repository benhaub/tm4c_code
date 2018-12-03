#include "tm4c123gh6pm.h"
#include <stdbool.h>

/*
 * 1ms delay on a 16MHz clock
 */
void scankey_delay() {

    int i;
    for(i = 0; i < 16000; i++);
}

void init_keypad() {

    volatile unsigned long delay_clk; // delay for clock, must have 3 sys clock delay, p.424
    SYSCTL_RCGC2_R |= 0x00000004; // enable clock gating for port C
    SYSCTL_RCGC2_R |= 0x00000010; // enable clock gating for port E
    delay_clk = SYSCTL_RCGC2_R; // this is a dummy instruction to allow clock to settle, no-operation

    /* Ports for 4x4 keypad. Connections are particular to the development */
    /* board */
    /* PORTC initialization. AFSEL and DIR default values (GPIO and input) are fine */
    /* Only pins PC4-PC7 are used */
    GPIO_PORTC_DEN_R |= 0xF0;
    GPIO_PORTC_PUR_R |= 0x10;

    /* PORTE initialization. Only using PE3-PE0 */
    GPIO_PORTE_DIR_R = 0x00;
    GPIO_PORTE_AFSEL_R = 0x00;
    GPIO_PORTE_DEN_R |= 0x0F;
    GPIO_PORTE_PDR_R |= 0x0F;
}
/*
 * Scans the keypad for presses, and returns the ASCII value of the character
 * that was pressed.
 */
unsigned int scan_key() {

    /* Hex values for each key on the keypad*/
    /* The first row is labelled for clarification of the layout */

                                   /*'D'   '#'   '0'    '*' */
    unsigned int keypadMap[4][4] = {{0x44, 0x23, 0x30, 0x2A},
                                    {0x43, 0x39, 0x38, 0x37},
                                    {0x42, 0x36, 0x35, 0x34},
                                    {0x41, 0x33, 0x32, 0x31}};

    bool keepLooking = true;
    int i, j;

    /*
     * clear bit clears the the last active pull-up
     * resistor bit
     */
    unsigned int clearPUR = 0x10;

    /*
     * Checks the next bit of PORTE
     */
    unsigned int checkPORTE = 0x01;

    while(keepLooking) {

        for(i = 0; i < 4; i++) {

            /* Janky fix for a problem with buttons A,B and C not activating.*/
            //SysTick_Wait10ms(1);
            scankey_delay();

            /* reset PORTE check back to checking bit 0 */
            if(checkPORTE >= 0x08) {

                     checkPORTE = 0x01;
            }

            for(j = 0; j < 4; j++) {

                if(GPIO_PORTE_DATA_R & checkPORTE) {

                    /* Prevent press an hold keypad entry */
                    while(GPIO_PORTE_DATA_R & checkPORTE);

                   /*
                    * The counters i and j will reset when the function leaves
                    * but the value of PORTC will remain, so we should reset it
                    * as well to keep it lined up with the counters.
                    */
                    GPIO_PORTC_PUR_R &= ~0xE0;
                    GPIO_PORTC_PUR_R |= 0x01;

                    return keypadMap[i][j];
                }
                else {

                    checkPORTE *= 0x02;
                }
            }
            /* Enable 1 pull up resistor at a time, then check the */
            /* corresponding row for a button press */

            /* clear the last active bit */
            GPIO_PORTC_PUR_R &= ~clearPUR;

            /* Multiply the bit by 2 to move to the next PUR bit. Do not do
             * this if we have just reset, and */
                if(clearPUR == 0x80) {

                    clearPUR = 0x10;
                }
                else {

                    clearPUR *= 0x02;
                }

            /* Set GPIO to activate the next PUR */
            GPIO_PORTC_PUR_R |= clearPUR;

            /* We only want to activate up to PC7, then restart */
            if(clearPUR > 0x80) {

                clearPUR = 0x10;
            }
        }
    }
    return 10; /* Never reached */
}
