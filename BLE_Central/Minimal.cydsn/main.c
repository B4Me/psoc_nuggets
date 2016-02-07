#include <project.h>

CYBLE_GAP_BD_ADDR_T	connectPeriphDevice = {{0x56, 0x34, 0x12, 0x50, 0xa0, 0x00}, 0};
CYBLE_CONN_HANDLE_T connHandle = {0, 0};
CYBLE_GATTC_WRITE_REQ_T writeReqParam;
CYBLE_GATT_VALUE_T writeValue;

uint16 myCharHandle = 0x0E;
uint8 writeData = 0;
uint8 readData = 0;
uint8 rspReceived = 0;

void BleEventHandler(uint32 event, void *eventParam)
{
    CYBLE_GATTC_READ_RSP_PARAM_T *rsp = (CYBLE_GATTC_READ_RSP_PARAM_T *)eventParam;

    switch(event)
	{
        case CYBLE_EVT_STACK_ON:
            CyBle_GapcConnectDevice(&connectPeriphDevice);
            break;

        case CYBLE_EVT_GATT_CONNECT_IND:
            connHandle = *(CYBLE_CONN_HANDLE_T *)eventParam;
            break;

        case CYBLE_EVT_GATTC_READ_RSP:
            readData = rsp->value.val[0];
            rspReceived = 1;
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

    writeData = 0x01; /* A value of 1 will turn on LED on Minimal_Peripheral dongle*/
    writeValue.val = &writeData;
    writeValue.len = sizeof(writeData);
    writeValue.actualLen = sizeof(writeData);
    writeReqParam.value = writeValue;        
    writeReqParam.attrHandle = myCharHandle;
    CyBle_GattcWriteWithoutResponse(connHandle, &writeReqParam); /* Write to server */
    CyBle_ProcessEvents();
    
    CyBle_GattcReadCharacteristicValue(connHandle, myCharHandle); /* Read from server */
    while(!rspReceived)
        CyBle_ProcessEvents();
    
    if (readData != writeData) 
        CyHalt(1);
        
    return 0;
}