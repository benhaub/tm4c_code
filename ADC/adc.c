#include "tm4c123gh6pm.h"
#include <inttypes.h>
#include <stdlib.h>

/*
 * Initialize the Analog to Digital Converter 0 with sample interrupt enable,
 * end of sequence, and sample sequencer 3. Differential input and temperature are off.
 * Interrupts are off.
 *
 * param samplingRate
 *          0x01 - 125ksps
 *          0x03 - 250ksps
 *          0x05 - 500ksps
 *          0x07 - 1Msps
 *
 * param trigger
 *          0x00 - Processor (Default)
 *          0x01 - Analog comparator 0
 *          0x02 - Analog comparator 1
 *          0x04 - External (GPIO Pins)
 *          0x05 - Timer
 *          0x0F - Always (continuously sample)
 *
 * param sampleSelect
 *          0x01 - 0x09 corresponds to AIN0 - AIN9
 *          See table 21-5 on p.1135 of data sheet
 */
void init_adc0(unsigned int samplingRate, unsigned int trigger, unsigned int sampleSelect) {

    /* See page 463 of Valvano text for init procedure */

    volatile unsigned long delay_clk;

    SYSCTL_RCGCADC_R |= 0x01; //p.322 - to enable clock for ADC module 0
    delay_clk = SYSCTL_RCGCADC_R; //dummy operation for clock to settle
    delay_clk = SYSCTL_RCGCADC_R; //dummy operation for clock to settle

    ADC0_PC_R |= samplingRate;  //p.840 - to select sampling rate
    ADC0_SSPRI_R |= 0x0123;   //p.791 - to select SS priority. ADC3 has the highest priority
                      //The priority doesn't matter since we are only using one ADC. But set
                      // anyway since they all must have a different priority.

    ADC0_ACTSS_R &= ~0x0008; //p.774 - disable ADC during setup

    //p.785 - set trigger mode
    switch(trigger) {

        case 0x00:  ADC0_EMUX_R &= ~0xF000;
                    break;
        case 0x01:  ADC0_EMUX_R |= 0x1000;
                    break;
        case 0x02:  ADC0_EMUX_R |= 0x2000;
                    break;
        case 0x04:  ADC0_EMUX_R |= 0x4000;
                    break;
        case 0x05:  ADC0_EMUX_R |= 0x5000;
                    break;
        case 0x0F:  ADC0_EMUX_R |= 0xF000;
                    break;
        default:    exit(EXIT_FAILURE);
    }
    ADC0_SSMUX3_R = (ADC0_SSMUX3_R & 0xFFFFFFF0) + sampleSelect; //Clear SS3 field
    //ADC0_SSMUX3_R |= sampleSelect;
    ADC0_SSCTL3_R |= 0b0110; //p.826 - to configure sequencer control
    ADC0_IM_R &= ~0x0008; //disable SS3 interrupts

    /* re-initiate after setup */
    ADC0_ACTSS_R |= 0x0008; //p.774 -
}

/*
 * Read the value sampled by ADC0 from SSFIFO3
 */
uint32_t ADC0_InSeq3(void) {

    uint32_t result;

    ADC0_PSSI_R |= 0x0008; //p.795 - Begin sampling on sample sequencer 3

    /* Poll the conversion completion bit of SS3 */
    while((ADC0_RIS_R & 0x08) == 0x00);

    /* Read FIFO for the value, which is the value obtained by the ADC */
    result = ADC0_SSFIFO3_R & 0xFFF;

    ADC0_ISC_R = 0x0008; //Clear the bit by writing to it.

    return result;
}
/*
 * enable interrupts on ADC0. Don't forget to include the handler
 * ADC0Seqn_Handler(), where n is the sequencer being used. An exemplary
 * use of this might be to interrupt whenever the digital comparator detects
 * a value in a certain range. In this case, you would not want to use SS
 * interrupts.
 *
 * param SSI:
 *          Sample Sequencer interrupt. Use 0, 1, 2, or 3 for Sample Sequencer
 *          0, 1, 2, and 3 respectively. Use -1 if not using. Note that the
 *          sample sequencer interrupt is turned on every time an ADC conversion
 *          is complete, so it may not be desirable to use SS interrupts.
 *
 * param DCSS:
 *          Digital Comparator interrupt for sample sequencer.
 *          Use 0, 1, 2, or 3 for SS 0, 1, 2, and 3 respectively. Use -1 if
 *          not using.
 *
 * param SS:
 *          The sample Sequencer being used on ADC0.
 *
 * param DC:
 *          The digital comparator being used. There are 8 comparators labeled
 *          0 to 7. -1 if not using DC.
 *
 * param CIE:
 *          Comparison interrupt enable. Use 1 to enable, 0 to disable
 *
 * param CIC:
 *          Comparison interrupt condition. The operation region in which
 *          the interrupt will be generated when the ADC conversion is
 *          comparared agains the values of COMP0 and COMP1 as defined
 *          in the ADCDCCMP registers. Valid values are:
 *
 *          0x0 - Low Band
 *                  ADC Data < COMP0 <= COMP1
 *
 *          0x1 - Mid Band
 *                  COMP0 <= ADC Data < COMP1
 *
 *          0x3 - High Band
 *                  COMP0 < COMP1 <= ADC Data
 *
 * param CIM:
 *          Comparison interrupt mode;
 *
 *          0x0 - Interrupt every time the ADC conversion data falls within
 *          the selected operational region as defined by CIC and COMP0, COMP1.
 *
 *          0x1 - Interrupt the first time that the ADC conversion data enters
 *          the selected operational region
 *
 *          0x2 - Interrupt when the ADC conversion data falls within the selected
 *          operational region and continues to generate the interrupt until the
 *          condition is cleared by entering the opposite operational region
 *
 *          0x03 - Interrupt the first time the the ADC conversion data falls  within
 *          the selected operational region. No additional interrupts are
 *          generated until the hysterisis condition is cleared by entering
 *          the opposite operational region
 *
 * param pri:
 *          The priority of the interrupt. values range from 0 to 7, with 0
 *          being the highest priority and 7 being the lowest.
 *
 * param COMP0, COMP1:
 *          The values that define the operation region for interrupts on the
 *          digital comparator. Max value is 2^12 (4096). COMP1 must be greater
 *          than or equal to COMP0 or unexpected results can occur.
 *
 */
void ADC0_interrupt(int SSI, int DCSS, int SS, int DC, int CIE, int CIC, int CIM, int pri, int COMP0, int COMP1) {

    int priority;

    /* Disable interrupts during setup */
    if(SSI >= 0) {
        switch (SSI) {

               case 0: ADC0_IM_R &= ~0x01;
                   break;

               case 1: ADC0_IM_R &= ~0x02;
                   break;

               case 2: ADC0_IM_R &= ~0x04;
                   break;

               case 3: ADC0_IM_R &= ~0x08;
                   break;

               default:    exit(EXIT_FAILURE);
        }
    }

    /* More disables for setup */
    if(DCSS >= 0) {
        switch (DCSS) {

            case 0: ADC0_IM_R &= ~0x00010000;
                break;

            case 1: ADC0_IM_R &= ~0x00020000;
                break;

            case 2: ADC0_IM_R &= ~0x00040000;
                break;

            case 3: ADC0_IM_R &= ~0x00080000;
                break;

            default:    exit(EXIT_FAILURE);
        }
    }

    /* Set the desired bits for the desired comparator */
    /* TODO: Need to do enables on Register 37, Pg. 829. and 828. */
    /* Note that if a sample is sent to a digital comparator, it is */
    /* not sent to the respective FIFO */
    if(DC >= 0) {
        switch (DC) {

            case 0: ADC0_DCCTL0_R |= 0x10;
                    ADC0_DCCTL0_R |= CIM;
                    ADC0_DCCTL0_R |= (CIC << 2);
                    ADC0_DCCMP0_R |= COMP0;
                    ADC0_DCCMP0_R |= (COMP1 << 16);
                    /* enable the digital comparator on the sample sequencer */
                    if(0 == SS) {
                        ADC0_SSOP0_R |= 0b1;
                        ADC0_SSDC0_R |= 0x0;
                    }
                    else if(1 == SS) {
                        ADC0_SSOP1_R |= 0b1;
                        ADC0_SSDC0_R |= 0x0;
                    }
                    else if(2 == SS) {
                        ADC0_SSOP2_R |= 0b1;
                        ADC0_SSDC0_R |= 0x0;
                    }
                    else /*(3 == SS) */ {
                        ADC0_SSOP3_R |= 0b1;
                        ADC0_SSDC0_R |= 0x0;
                    }
                    break;

            case 1: ADC0_DCCTL1_R |= 0x10;
                    ADC0_DCCTL1_R |= CIM;
                    ADC0_DCCTL1_R |= (CIC << 2);
                    ADC0_DCCMP1_R |= COMP0;
                    ADC0_DCCMP1_R |= (COMP1 << 16);
                    /* enable the digital comparator on the sample sequencer */
                    if(0 == SS) {
                        ADC0_SSOP0_R |= 0b1;
                        ADC0_SSDC0_R |= 0x0;
                    }
                    else if(1 == SS) {
                        ADC0_SSOP1_R |= 0b1;
                        ADC0_SSDC1_R |= 0x0;
                    }
                    else if(2 == SS) {
                        ADC0_SSOP2_R |= 0b1;
                        ADC0_SSDC1_R |= 0x0;
                    }
                    else /*(3 == SS) */ {
                        ADC0_SSOP3_R |= 0b1;
                        ADC0_SSDC1_R |= 0x0;
                    }
                    break;

            case 2: ADC0_DCCTL2_R |= 0x10;
                    ADC0_DCCTL2_R |= CIM;
                    ADC0_DCCTL2_R |= (CIC << 2);
                    ADC0_DCCMP2_R |= COMP0;
                    ADC0_DCCMP2_R |= (COMP1 << 16);
                    break;

            case 3: ADC0_DCCTL3_R |= 0x10;
                    ADC0_DCCTL3_R |= CIM;
                    ADC0_DCCTL3_R |= (CIC << 2);
                    ADC0_DCCMP3_R |= COMP0;
                    ADC0_DCCMP3_R |= (COMP1 << 16);
                    break;

            case 4: ADC0_DCCTL4_R |= 0x10;
                    ADC0_DCCTL4_R |= CIM;
                    ADC0_DCCTL4_R |= (CIC << 2);
                    ADC0_DCCMP4_R |= COMP0;
                    ADC0_DCCMP4_R |= (COMP1 << 16);
                    break;

            case 5: ADC0_DCCTL5_R |= 0x10;
                    ADC0_DCCTL5_R |= CIM;
                    ADC0_DCCTL5_R |= (CIC << 2);
                    ADC0_DCCMP5_R |= COMP0;
                    ADC0_DCCMP5_R |= (COMP1 << 16);
                    break;

            case 6: ADC0_DCCTL6_R |= 0x10;
                    ADC0_DCCTL6_R |= CIM;
                    ADC0_DCCTL6_R |= (CIC << 2);
                    ADC0_DCCMP6_R |= COMP0;
                    ADC0_DCCMP6_R |= (COMP1 << 16);
                    break;

            case 7: ADC0_DCCTL7_R |= 0x10;
                    ADC0_DCCTL7_R |= CIM;
                    ADC0_DCCTL7_R |= (CIC << 2);
                    ADC0_DCCMP7_R |= COMP0;
                    ADC0_DCCMP7_R |= (COMP1 << 16);
                    break;
        }
    }

    /* Enable interrupts last */
    if(SSI >= 0) {
        switch (SSI) {

            case 0: ADC0_IM_R |= 0x01;
                priority = (pri << 21); /* 4n+2. bits 23:21. n = 3*/
                NVIC_PRI3_R |= priority;
                NVIC_EN0_R |= 0x00004000; /* bit 14 */
                break;

            case 1: ADC0_IM_R |= 0x02;
                priority = (pri << 29); /* 4n+3. bits 31:29 */
                NVIC_PRI3_R |= priority;
                NVIC_EN0_R |= 0x00008000; /* bit 15 */
                break;

            case 2: ADC0_IM_R |= 0x04;
                priority = (pri << 5); /* 4n. bits 7:5. n = 4 */
                NVIC_PRI4_R |= priority;
                NVIC_EN0_R |= 0x00010000; /* bit 16 */
                break;

            case 3: ADC0_IM_R |= 0x08;
                priority = (pri << 13); /* 4n+1. bits 15:13. n = 4 */
                NVIC_PRI4_R |= priority;
                NVIC_EN0_R |= 0x00020000; /* bit 17 */
                break;

            default:    exit(EXIT_FAILURE);
        }
    }

    /* Enable interrupts last */
    if(DCSS >= 0) {
        switch (DCSS) {

            case 0: ADC0_IM_R |= 0x00010000;
                break;

            case 1: ADC0_IM_R |= 0x00020000;
                break;

            case 2: ADC0_IM_R |= 0x00040000;
                break;

            case 3: ADC0_IM_R |= 0x00080000;
                break;

            default:    exit(EXIT_FAILURE);
        }
    }
}

/*
 * Initialize the Analog to Digital Converter 1 with sample interrupt enable,
 * end of sequence, and sample sequencer 2. temperature is enabled.
 * Interrupts are off.
 *
 * param samplingRate
 *          0x01 - 125ksps
 *          0x03 - 250ksps
 *          0x05 - 500ksps
 *          0x07 - 1Msps
 *
 * param trigger
 *          0x00 - Processor (Default)
 *          0x01 - Analog comparator 0
 *          0x02 - Analog comparator 1
 *          0x04 - External (GPIO Pins)
 *          0x05 - Timer
 *          0x0F - Always (continuously sample)
 *
 * param sampleSelect
 *          0x01 - 0x09 corresponds to AIN0 - AIN9
 *          See table 21-5 on p.1135 of data sheet
 */
void init_adc1(unsigned int samplingRate, unsigned int trigger, unsigned int sampleSelect) {

    volatile unsigned long delay_clk;

    SYSCTL_RCGCADC_R |= 0x02; //p.322 - to enable clock for ADC module 1
    delay_clk = SYSCTL_RCGCADC_R; //dummy operation for clock to settle
    delay_clk = SYSCTL_RCGCADC_R; //dummy operation for clock to settle

    ADC1_PC_R |= samplingRate;  //p.840 - to select sampling rate
    ADC1_SSPRI_R |= 0x0123;   //p.791 - to select SS priority. ADC3 has the highest priority
    ADC1_ACTSS_R &= ~0x0004; //p.774 - disable ADC during setup

    //p.785 - set trigger mode
    switch(trigger) {

        case 0x00:  ADC1_EMUX_R &= ~0xF000;
                    break;
        case 0x01:  ADC1_EMUX_R |= 0x1000;
                    break;
        case 0x02:  ADC1_EMUX_R |= 0x2000;
                    break;
        case 0x04:  ADC1_EMUX_R |= 0x4000;
                    break;
        case 0x05:  ADC1_EMUX_R |= 0x5000;
                    break;
        case 0x0F:  ADC1_EMUX_R |= 0xF000;
                    break;
        default:    exit(EXIT_FAILURE);
    }
    ADC1_SSMUX2_R = (ADC1_SSMUX2_R & 0xFFFFF0FF) + sampleSelect; //Clear SS2 field
    ADC1_SSCTL2_R |= 0b0110; //p.826 - to configure sequencer control
    ADC1_IM_R &= ~0x0004; //p. 777 - disable SS2 interrupts

    /* re-initiate after setup */
    ADC1_ACTSS_R |= 0x0004; //p.774 -
}

/*
 * Read the value sampled by ADC1 from SSFIFO2
 */
uint32_t ADC1_InSeq2(void) {

    uint32_t result;

    ADC1_PSSI_R |= 0x0004; //p.795 - Begin sampling on sample sequencer 2

    /* Poll the conversion completion bit of SS2 */
    while((ADC1_RIS_R & 0x04) == 0x00);

    /* Read FIFO for the value, which is the value obtained by the ADC */
    result = ADC1_SSFIFO2_R & 0xFFF;

    ADC1_ISC_R = 0x0004; //Clear the bit by writing to it.

    return result;
}
