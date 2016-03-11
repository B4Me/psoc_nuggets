#include <project.h>
#define DEBUGPRINTS
#include <hkj_library.h>

void BleEventHandler(uint32 event, void *eventParam)
{
    CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T *wrCmdReqParam;

    hkj_ble_events_log_add(event, eventParam);
    switch(event)
	{
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
        case CYBLE_EVT_STACK_ON:
        case CYBLE_EVT_TIMEOUT:
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            break;

        case CYBLE_EVT_GATTS_WRITE_CMD_REQ:
            WRITE_CMD_PIN_Write(1);    
            wrCmdReqParam = (CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T *) eventParam;
            if (wrCmdReqParam->handleValPair.attrHandle == \
                cyBle_customs[CYBLE_LEDSERVICE_SERVICE_INDEX].\
                customServInfo[CYBLE_LEDSERVICE_LEDSTATE_CHAR_INDEX]. \
                customServCharHandle)
            {
                CyBle_GattsWriteAttributeValue(&wrCmdReqParam->handleValPair, \
                    0, &cyBle_connHandle, CYBLE_GATT_DB_PEER_INITIATED); 
            }
            break;
                
        default:
            break;
	}
    hkj_ble_events_log_debug_print();
}

int main()
{
    CYBLE_STACK_LIB_VERSION_T stackVersion;
    
	CyGlobalIntEnable;
    UART_Start();
    hkj_debug_init();
    CyBle_GetStackLibraryVersion(&stackVersion);
    debug_print("Latency Peripheral   Stack: %u.%u.%u.%u\r\n", \
        stackVersion.majorVersion, stackVersion.minorVersion, \
        stackVersion.patch, stackVersion.buildNumber);
    CyBle_Start(BleEventHandler);
    
    while(1)
    {
        CyBle_ProcessEvents();
        WRITE_CMD_PIN_Write(0);    
	}
}
