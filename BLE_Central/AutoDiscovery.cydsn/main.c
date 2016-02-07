#include <project.h>

CYBLE_GAP_BD_ADDR_T	connectPeriphDevice = {{0x56, 0x34, 0x12, 0x50, 0xa0, 0x00}, 0};
CYBLE_CONN_HANDLE_T connHandle = {0, 0};

CYBLE_GATTC_WRITE_REQ_T writeReqParam;
CYBLE_GATT_VALUE_T writeValue;
uint8 writeData = 0;

uint16 ledStateHandle = 0;

void BleEventHandler(uint32 event, void *eventParam)
{
    switch(event)
	{
        case CYBLE_EVT_STACK_ON:
            CyBle_GapcConnectDevice(&connectPeriphDevice);
            break;

        case CYBLE_EVT_GATT_CONNECT_IND:
            connHandle = *(CYBLE_CONN_HANDLE_T *)eventParam;
            break;

        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
            ledStateHandle = cyBle_customCServ[CYBLE_CUSTOMC_LEDSERVICE_SERVICE_INDEX].\
                                customServChar[CYBLE_CUSTOMC_LEDSERVICE_LEDSTATE_CHAR_INDEX].customServCharHandle;
            break;
            
		default:
    		break;
	}
}

int main()
{
    CyGlobalIntEnable; 
	CyBle_Start(BleEventHandler);	
    while(connHandle.bdHandle == 0) /* Wait for connection to GATT server */
        CyBle_ProcessEvents();

    CyBle_GattcStartDiscovery(connHandle); /* Attempt discovery of configured services */

    while(ledStateHandle == 0) /* Wait for discovery */
        CyBle_ProcessEvents();
        
    writeData = 0x01; /* Turn on LED on Minimal_Peripheral dongle */
    writeValue.val = &writeData;
    writeValue.len = sizeof(writeData);
    writeValue.actualLen = sizeof(writeData);
    writeReqParam.value = writeValue;        
    writeReqParam.attrHandle = ledStateHandle; /* Use discovered handle */
    CyBle_GattcWriteWithoutResponse(connHandle, &writeReqParam); /* Write to server */
    CyBle_ProcessEvents();
    
    return 0;
}