#include <project.h>
#include "tuner.h"
#include "blehandler.h"

int main()
{
    uint8 interruptState;

    CyGlobalIntEnable; /* Enable global interrupts */
    
    EZI2C_Start(); /* Start EZI2C component */
    
    /*
    * Set up communication and initialize data buffer to CapSense data structure
    * to use Tuner application
    */
    EZI2C_EzI2CSetBuffer1(sizeof(CapSense_dsRam), sizeof(CapSense_dsRam),
                            (uint8 *)&CapSense_dsRam);

    CapSense_Start(); /* Initialize component */

    BleHandler_Init();
    Tuner_Init(BleHandler_RefreshBuffer, BleHandler_SendBuffer);

    CapSense_ScanAllWidgets(); /* Scan all widgets */

    for(;;)
    {
        /* Do this only when a scan is done */
        interruptState = CyEnterCriticalSection();
        if(CapSense_NOT_BUSY == CapSense_IsBusy())
        {
            CyExitCriticalSection(interruptState);
            CapSense_ProcessAllWidgets(); /* Process all widgets */
            Tuner_RunTuner();

            if (CapSense_IsAnyWidgetActive()) /* Scan result verification */
            {
                /* add custom tasks to execute when touch detected */
            }
            CapSense_ScanAllWidgets(); /* Start next scan */
        }
        CySysPmSleep(); /* Sleep until scan is finished */
        CyExitCriticalSection(interruptState);
    }
}