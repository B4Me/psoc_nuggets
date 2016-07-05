#include <project.h>
#include "tuner.h"

static uint8 notificationsEnabled = 0;

void BleEventHandler(uint32 event, void *eventParam)
{
	CYBLE_GATT_HANDLE_VALUE_PAIR_T dataBufferCCCD;
    CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;
    CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T *wrCmdReqParam;
	uint8 dataBufferCCCDvalue[2];
    uint16 offset;
    uint8 i;

    switch(event)
	{
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
        case CYBLE_EVT_STACK_ON:
        case CYBLE_EVT_TIMEOUT:
            notificationsEnabled = 0;
    		dataBufferCCCDvalue[0] = notificationsEnabled;
    		dataBufferCCCDvalue[1] = 0x00;
    		
    		dataBufferCCCD.attrHandle = CYBLE_CAPSENSE_TUNER_DATA_BUFFER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    		dataBufferCCCD.value.val = dataBufferCCCDvalue;
    		dataBufferCCCD.value.len = sizeof(dataBufferCCCDvalue);
    		
    		CyBle_GattsWriteAttributeValue(&dataBufferCCCD, 0, &cyBle_connHandle, \
                                            CYBLE_GATT_DB_LOCALLY_INITIATED);
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            break;

        case CYBLE_EVT_GATTS_WRITE_CMD_REQ:
            wrCmdReqParam = (CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T *) eventParam;
            if (wrCmdReqParam->handleValPair.attrHandle == CYBLE_CAPSENSE_TUNER_EZI2C_WRITE_CHAR_HANDLE)
            {
                if (wrCmdReqParam->handleValPair.value.len >= 2)
                {
                    offset = (wrCmdReqParam->handleValPair.value.val[0] << 8) + \
                            wrCmdReqParam->handleValPair.value.val[1];

                    for (i=0; i < wrCmdReqParam->handleValPair.value.len-2 && \
                            ( (i + offset) < sizeof(CapSense_dsRam) ); i++)
                    {
                        ((uint8 *)(&CapSense_dsRam))[i + offset] = wrCmdReqParam->handleValPair.value.val[i + 2];
                    }
                }
            }
            break;

        case CYBLE_EVT_GATTS_WRITE_REQ: 							
            wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
            if(wrReqParam->handleValPair.attrHandle == CYBLE_CAPSENSE_TUNER_DATA_BUFFER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE)
            {
                notificationsEnabled = wrReqParam->handleValPair.value.val[0];
        		dataBufferCCCDvalue[0] = notificationsEnabled;
        		dataBufferCCCDvalue[1] = 0x00;
        		
        		dataBufferCCCD.attrHandle = CYBLE_CAPSENSE_TUNER_DATA_BUFFER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
        		dataBufferCCCD.value.val = dataBufferCCCDvalue;
        		dataBufferCCCD.value.len = sizeof(dataBufferCCCDvalue);
        		
        		CyBle_GattsWriteAttributeValue(&dataBufferCCCD, 0, &cyBle_connHandle, \
                                                CYBLE_GATT_DB_LOCALLY_INITIATED);
            }
            CyBle_GattsWriteRsp(cyBle_connHandle);
			break;            

        default:
            break;
	}
}

void BleHandler_Init(void)
{
   	CYBLE_GATT_HANDLE_VALUE_PAIR_T dataBuffer;

    CyBle_Start(BleEventHandler);
    CyBle_ProcessEvents();
        		
	dataBuffer.attrHandle = CYBLE_CAPSENSE_TUNER_DATA_BUFFER_CHAR_HANDLE;
	dataBuffer.value.val = (uint8 *)&CapSense_dsRam;
	dataBuffer.value.len = sizeof(CapSense_dsRam);
	
	CyBle_GattsWriteAttributeValue(&dataBuffer, 0, &cyBle_connHandle,
            CYBLE_GATT_DB_LOCALLY_INITIATED);
};

void BleHandler_RefreshBuffer(void)
{
    CyBle_ProcessEvents();
}

void BleHandler_SendBuffer(void)
{
    CYBLE_API_RESULT_T result = ~CYBLE_ERROR_OK;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T dataBufferNotification;	
    
    dataBufferNotification.attrHandle = CYBLE_CAPSENSE_TUNER_DATA_BUFFER_CHAR_HANDLE;
    dataBufferNotification.value.val = (uint8 *)&CapSense_dsRam;
    dataBufferNotification.value.len = sizeof(CapSense_dsRam);

    while (result != CYBLE_ERROR_OK && notificationsEnabled)
    {
        if (CyBle_GattGetBusStatus() != CYBLE_STACK_STATE_BUSY)
        {
            result = CyBle_GattsNotification(cyBle_connHandle, &dataBufferNotification);
        }
        CyBle_ProcessEvents();
    }
    CyBle_ProcessEvents();
};
