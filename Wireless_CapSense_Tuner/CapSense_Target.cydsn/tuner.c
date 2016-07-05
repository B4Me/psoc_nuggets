#include "tuner.h"

static void (*Tuner_RefreshBuffer)(void);
static void (*Tuner_SendBuffer)(void);

void Tuner_Init(void (*refresh)(void), void (*send)(void))
{
    Tuner_RefreshBuffer = refresh;
    Tuner_SendBuffer = send;
}

void Tuner_RunTuner(void)
{
    uint8 interruptState;
    uint8 suspended = 0;
    uint16 command;
    uint16 previousCompleteBit; /* COMPLETE_BIT in tunerCmd at time of last send to tuner host */

    Tuner_SendBuffer();

    /* Handle suspend command to avoid deadlock later in CapSense_RunTuner */
    do 
    {
        Tuner_RefreshBuffer();
        interruptState = CyEnterCriticalSection();  /* Avoid ints between read and modify tunerCmd */
        command = CapSense_dsRam.tunerCmd;
        switch (command)
        {
            case CapSense_TU_CMD_SUSPEND_E:
                suspended = 1;
                CapSense_dsRam.tunerCmd |= CapSense_TU_CMD_COMPLETE_BIT;
                CyExitCriticalSection(interruptState); /* Enable ints during SendBuffer */
                Tuner_SendBuffer(); /* Send buffer with updated COMPLETE_BIT to tuner host */
                interruptState = CyEnterCriticalSection();
                break;
                
            case CapSense_TU_CMD_RESUME_E:    
            case CapSense_TU_CMD_RESTART_E:
            case CapSense_TU_CMD_RUN_SNR_TEST_E:
                suspended = 0;
                break;
                
            default:
                break;
        }
        CyExitCriticalSection(interruptState);
    } while (suspended); 

    previousCompleteBit = CapSense_dsRam.tunerCmd & CapSense_TU_CMD_COMPLETE_BIT;
    CapSense_RunTuner(); 
    
    if ( previousCompleteBit != (CapSense_dsRam.tunerCmd & CapSense_TU_CMD_COMPLETE_BIT) )
        Tuner_SendBuffer(); /* Send buffer with updated COMPLETE_BIT to tuner host */
}