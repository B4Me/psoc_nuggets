#include <project.h>
#define DEBUGPRINTS
#include <hkj_library.h>

uint8 notificationsEnabled = 0;
uint32 bytesRx = 0;
uint32 bytesTx = 0;
uint8 sequenceNr = 0;

void BleEventHandler(uint32 event, void *eventParam)
{
	CYBLE_GATT_HANDLE_VALUE_PAIR_T endpointInCCCD;
    CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;
    CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T *wrCmdReqParam;
	uint8 endpointInCCCDvalue[2];

    switch(event)
	{
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
        case CYBLE_EVT_STACK_ON:
        case CYBLE_EVT_TIMEOUT:
            sequenceNr = 0;    
            notificationsEnabled = 0;
    		endpointInCCCDvalue[0] = notificationsEnabled;
    		endpointInCCCDvalue[1] = 0x00;
    		
    		endpointInCCCD.attrHandle = CYBLE_ENDPOINT_IN_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    		endpointInCCCD.value.val = endpointInCCCDvalue;
    		endpointInCCCD.value.len = sizeof(endpointInCCCDvalue);
    		
    		CyBle_GattsWriteAttributeValue(&endpointInCCCD, 0, &cyBle_connHandle, \
                                            CYBLE_GATT_DB_LOCALLY_INITIATED);
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            break;

        case CYBLE_EVT_GATTS_WRITE_CMD_REQ:
            wrCmdReqParam = (CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T *) eventParam;
            if (wrCmdReqParam->handleValPair.attrHandle == CYBLE_ENDPOINT_OUT_CHAR_HANDLE)
            {
                bytesRx += wrCmdReqParam->handleValPair.value.len;
                if (wrCmdReqParam->handleValPair.value.val[0] != sequenceNr ||
                    wrCmdReqParam->handleValPair.value.val[wrCmdReqParam->handleValPair.value.len - 1] != sequenceNr) 
                {
                    debug_print("Data sequence nr: (%u, %u) != %u - disconnecting\r\n", wrCmdReqParam->handleValPair.value.val[0], 
                        wrCmdReqParam->handleValPair.value.val[wrCmdReqParam->handleValPair.value.len - 1], sequenceNr);
                    CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
                }
                else
                {
                    sequenceNr++;
                }
            }
            break;
            
        case CYBLE_EVT_GATTS_WRITE_REQ: 							
            wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
            if(wrReqParam->handleValPair.attrHandle == CYBLE_ENDPOINT_IN_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE)
            {
                notificationsEnabled = wrReqParam->handleValPair.value.val[0];
        		endpointInCCCDvalue[0] = notificationsEnabled;
        		endpointInCCCDvalue[1] = 0x00;
        		
        		endpointInCCCD.attrHandle = CYBLE_ENDPOINT_IN_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
        		endpointInCCCD.value.val = endpointInCCCDvalue;
        		endpointInCCCD.value.len = sizeof(endpointInCCCDvalue);
        		
        		CyBle_GattsWriteAttributeValue(&endpointInCCCD, 0, &cyBle_connHandle, \
                                                CYBLE_GATT_DB_LOCALLY_INITIATED);
            }
            CyBle_GattsWriteRsp(cyBle_connHandle);
			break;            
            
        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
            {
                CYBLE_GATT_XCHG_MTU_PARAM_T *param = (CYBLE_GATT_XCHG_MTU_PARAM_T *)eventParam;
                debug_print("CYBLE_EVT_GATTS_XCNHG_MTU_REQ MTU=%u\r\n", param->mtu);
            }
            break;
            
        default:
            break;
	}
}

uint8 endpointInData[CYBLE_GATT_MTU-3] = {};
    
int main()
{
    CYBLE_API_RESULT_T result;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T endpointInNotification;	
    
	CyGlobalIntEnable;

    UART_Start();
    hkj_debug_init();
    printf("Starting GAP Peripheral\r\n");
    CyBle_Start(BleEventHandler);

    endpointInNotification.attrHandle = CYBLE_ENDPOINT_IN_CHAR_HANDLE;
    endpointInNotification.value.val = endpointInData;
    endpointInNotification.value.len = sizeof(endpointInData);

    while(1)
    {
        CyBle_ProcessEvents();
        
        if(CyBle_GattGetBusStatus() != CYBLE_STACK_STATE_BUSY)
        {
            if (notificationsEnabled)
            {
                endpointInData[0]++;
    	        result = CyBle_GattsNotification(cyBle_connHandle, &endpointInNotification);
                if (result == CYBLE_ERROR_OK)
                    bytesTx += endpointInNotification.value.len;
            }
        }
	}
}
