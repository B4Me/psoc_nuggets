#include <project.h>
#include <hkj_library.h>

CYBLE_GAP_BD_ADDR_T	connectPeriphDevice = {{0x25, 0x43, 0x65, 0x50, 0xa0, 0x00}, 0};
uint8 shouldConnect = TRUE;
CYBLE_GATT_HANDLE_VALUE_PAIR_T myHandle;	

void ApplicationEventHandler(uint32 event, void *eventparam)
{
    CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T *wrCmdReqParam;

    hkj_ble_events_log_add(event, eventparam);
	switch(event)
	{
		case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
			shouldConnect = TRUE;
			break;
			          
        case CYBLE_EVT_GATTS_WRITE_CMD_REQ:
            wrCmdReqParam = (CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T *) eventparam;
            myHandle.attrHandle = wrCmdReqParam->handleValPair.attrHandle;
            myHandle.value.val = wrCmdReqParam->handleValPair.value.val;
            myHandle.value.len = wrCmdReqParam->handleValPair.value.len;
            myHandle.value.actualLen = wrCmdReqParam->handleValPair.value.actualLen;            
            CyBle_GattsWriteAttributeValue(&myHandle, FALSE, &cyBle_connHandle, FALSE); 
            //Store value-15 in handle 0x0c for testing purposes
            myHandle.attrHandle = 0x0c;
            myHandle.value.val[0] -= 15;
            CyBle_GattsWriteAttributeValue(&myHandle, FALSE, &cyBle_connHandle, FALSE); 
            
            if (wrCmdReqParam->handleValPair.attrHandle == 0x10)
                CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
            break;
            
		default:
    		break;
	}
}

int main()
{
    UART_Start();
    hkj_debug_init();
    hkj_ble_events_log_init();
    CyGlobalIntEnable; 
    debug_print("\r\nINFO: Starting GATT Server on GAP Central\r\n");

	CyBle_Start(ApplicationEventHandler);	
	
    for(;;)
    {
        CyBle_ProcessEvents();
  		if(shouldConnect)
		{
            hkj_ble_events_log_debug_print();
            debug_print("INFO: Trying to connect - CyBle_GapcConnectDevice\r\n");
			CyBle_GapcConnectDevice(&connectPeriphDevice);
            shouldConnect = FALSE; /* Avoid duplicate connect API calls */
            hkj_ble_events_log_clear();
        }
    }
}