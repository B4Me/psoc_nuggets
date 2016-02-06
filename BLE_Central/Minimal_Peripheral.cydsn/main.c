#include <project.h>

void BleEventHandler(uint32 event, void *eventParam)
{
    CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T *wrCmdReqParam;

    switch(event)
	{
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
        case CYBLE_EVT_STACK_ON:
        case CYBLE_EVT_TIMEOUT:
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            break;

        case CYBLE_EVT_GATTS_WRITE_CMD_REQ:
            wrCmdReqParam = (CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T *) eventParam;
            if (wrCmdReqParam->handleValPair.attrHandle == \
                cyBle_customs[CYBLE_MYCUSTOMSERVICE_SERVICE_INDEX].\
                customServInfo[CYBLE_MYCUSTOMSERVICE_MYCUSTOMCHARACTERISTIC_CHAR_INDEX].customServCharHandle)
            {
                    CyBle_GattsWriteAttributeValue(&wrCmdReqParam->handleValPair, 0, &cyBle_connHandle, CYBLE_GATT_DB_PEER_INITIATED); 
            }
            break;
                
        default:
            break;
	}
}

int main()
{
	CyGlobalIntEnable;

    CyBle_Start(BleEventHandler);
    
    while(1)
    {
        CyBle_ProcessEvents();
	}
}
