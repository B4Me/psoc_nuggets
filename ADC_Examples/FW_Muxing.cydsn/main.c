#include <project.h>

#define MUX_FW_AMUXBUSA_VMINUS (1 << 20)
#define NEG_SEL_MASK 0x00000E00;

volatile int16 result[4] = { 0 }; 

int main()
{
    uint32 temp = 0;
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    ADC_Start();

    for(;;)
    {
        /* Disconnect VREF from negative input of SARADC */
        temp = CY_GET_REG32(CYREG_SAR_CTRL);
        temp &= ~NEG_SEL_MASK;
        temp |= ADC_NEG_VSSA_KELVIN;
        CY_SET_REG32(CYREG_SAR_CTRL, temp);

        /* Connect AMUXBUS_A to negative input af SARADC */
        CY_SET_XTND_REG32((void CYFAR *)CYREG_SAR_MUX_SWITCH0, MUX_FW_AMUXBUSA_VMINUS);

        /* 1. TOP - MIDDLE */
        AMux_Select(1);
        ADC_StartConvert();
        ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
        result[0] = ADC_GetResult16(0);
        result[0] = ADC_CountsTo_mVolts(0, result[0]);

        /* 2. BOTTOM - MIDDLE */
        AMux_Select(2);
        ADC_StartConvert();
        ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
        result[1] = ADC_GetResult16(0);
        result[1] = ADC_CountsTo_mVolts(0, result[1]);

        /* Disconnect AMUXBUS_A from negative input af SARADC */
        CY_SET_XTND_REG32((void CYFAR *)CYREG_SAR_MUX_SWITCH_CLEAR0, MUX_FW_AMUXBUSA_VMINUS);

        /* Connect VREF to negative input of SARADC */
        temp = CY_GET_REG32(CYREG_SAR_CTRL);
        temp &= ~NEG_SEL_MASK;
        temp |= ADC_NEG_VREF;
        CY_SET_REG32(CYREG_SAR_CTRL, temp);

        /* 3. BOTTOM (single ended)*/
        AMux_Select(2);
        ADC_StartConvert();
        ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
        result[2] = ADC_GetResult16(0);
        result[2] = ADC_CountsTo_mVolts(0, result[2]);
        result[2] += 1024;  /* Add VREF for result relative to VSS */
        
        /* 4. DieTemp (single ended) */
        AMux_Select(0);
        ADC_StartConvert();
        ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
        result[3] = ADC_GetResult16(0);
        result[3] = DieTemp_CountsTo_Celsius(result[3]);
    }
}