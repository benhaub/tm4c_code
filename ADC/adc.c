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
