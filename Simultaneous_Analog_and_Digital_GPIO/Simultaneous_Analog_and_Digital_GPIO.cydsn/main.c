#include <project.h>

int main()
{
    uint16 adcResult;
    uint8 digitalIn;
    
    CyGlobalIntEnable; /* Enable global interrupts */

    ADC_Start();
    
    P2_0_Write(1); /* Drive pin high via resistive pull-up */
    ADC_StartConvert();
    ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
    adcResult = ADC_GetResult16(0);
    digitalIn = Status_Reg_Read();
    if (digitalIn == 0 || adcResult < 0x7ff/2)
        CyHalt(1); /* Error: both analog and digital should be high */

    P2_0_Write(0); /* Drive pin low */
    ADC_StartConvert();
    ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
    adcResult = ADC_GetResult16(0);
    digitalIn = Status_Reg_Read();
    if (digitalIn != 0 || adcResult > 0x7ff/2)
        CyHalt(1); /* Error: both analog and digital should be low */
        
    CyHalt(0); /* Success */        
    return 0;
}
