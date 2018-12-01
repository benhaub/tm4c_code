#include "tm4c123gh6pm.h"
#include <stdlib.h>
#include <math.h>

/*
 * Set uart1 for a 16MHz clock
 *
 * param baud:
 *          The baud rate
 */
void UART1_init(int baud)
{
    SYSCTL_RCGC1_R |= 0x02; // p.421, activate UART1
    SYSCTL_RCGC2_R |= 0x02; // p.424, activate clock gating for Port B

    UART1_CTL_R &= ~0x01; // p.868, disable UART1 during config

    double IB = 16E6/16/baud; //Calculate the baud rate
    int IB_whole = IB; //Truncate off the decimal.
    float FB = IB - IB_whole;

    //Convert the fraction into binary with 8 bits of precision.
    int i;
    unsigned int FB_whole;
    int bin;
    for(i = 7; i >=0; i--) {

        FB = FB * 2;
        FB_whole = FB; //integer truncation
        if(FB_whole != 0) {
            bin = bin + pow(2, i);
            FB = FB - FB_whole;
        }
    }

    UART1_IBRD_R = IB_whole;
    UART1_FBRD_R = bin;
    UART1_LCRH_R |= 0x60; // p.866, word length 8 bit, all other default, 8N1
    UART1_CTL_R |= 0x01; // Enable UART1 after config

    // Configure PB0 and PB1 as UART ports
    GPIO_PORTB_AFSEL_R |= 0x03; // p.624, enable alternate func for PA0 and PA1
    GPIO_PORTB_DEN_R |= 0x03; // p.636, enable DEN for PA0 and PA1
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFFFF0F) + 0x00000010; //PB1 port control encoding for U1 pg.1135
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFFFFF0) + 0x00000001;
// GPIO_PORTA_DIR is not needed since inputs and outputs for uart pins are predefined in table 14-1
}

/* UART send character function */
void UART1_send(unsigned char uartSendData)
{
    while((UART1_FR_R & 0x20) != 0); // wait if Transmit FIFO is full, TXFF
    UART1_DR_R = uartSendData;

}

/* UART receive character function */
unsigned char UART1_recieve(void)
{
    while((UART1_FR_R & 0x10) != 0);  // wait until receive FIFO is empty, RXFE
    return((unsigned char)(UART1_DR_R & 0xFF)); // return the received character (only 8-bit)
}

/* Function to convert lower case character to upper case and return */
unsigned char ToUpperCase(unsigned char lowerCase)
{
    unsigned char upperCase=lowerCase;
    if(lowerCase >=0x61 && lowerCase <=0x7A)
    {
        upperCase=lowerCase-0x20;
    }
    return upperCase;
}
