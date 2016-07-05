#include <project.h>
#include <stdlib.h>

#define CapSense_TU_CMD_COMPLETE_BIT    0x8000u
#define CapSense_TU_CMD_SUSPEND_E       0x0001u

#define I2C_SLAVE_ADDRESS (0x8u)
#define BUFFER_SIZE 512

/* Hard-coded Bluetooth address of CapSense target device */
CYBLE_GAP_BD_ADDR_T	targetAddr = {{0xa0, 0xa0, 0xa0, 0x50, 0xa0, 0x00}, 0};

/* Flag set when CapSense target connection has been successfully established */
static uint8 targetConnected = 0;   

/* Values used for enabling notifications from target device */
uint8 cccdNotifEnabledValue[] = {0x01, 0x00};
CYBLE_GATT_VALUE_T cccdNotifFlagSetStruct =
{
    cccdNotifEnabledValue,
    2,
    0
};

/* Copy of data from target's CapSense data struct CapSense_dsRam */
static uint8 capSenseDataBuffer[BUFFER_SIZE];

/* 16-bit I2C "Sub-adress" used as offset into capSenseDataBuffer */
static uint16 i2cOffset = 0;   

/* Buffer used to receive data from I2C writes from CapSense Tuner */
uint8 i2cWriteBuffer[BUFFER_SIZE]; 

/* Queue to hold pending write transactions for sending to target over BLE */
#define QUEUE_DEPTH 10
static uint8 *i2cWriteQueue[QUEUE_DEPTH] = { 0 };
static uint8 wp = 0; /* write pointer for queue */
static uint8 rp = 0;    

static inline uint8 i2cWriteQueueNotEmpty(void)
{
    return (rp != wp);
}

static inline uint8 i2cWriteQueueFull(void)
{
    uint8 nextwp;

    nextwp = wp + 1;
    if (nextwp == QUEUE_DEPTH)
        nextwp = 0;
    return (nextwp == rp);
}

static inline void i2cWriteQueueClear(void)
{
    rp = wp = 0;
}

static uint8 *i2cWriteQueueGet(void)
{
    uint8 intState;
    uint8 *packet = NULL;
    intState = CyEnterCriticalSection();
    if (rp != wp) /* not empty */
    {
        packet = i2cWriteQueue[rp++];
        if (rp == QUEUE_DEPTH) 
            rp = 0;
    }
    CyExitCriticalSection(intState);
    return packet;
}

static uint8 i2cWriteQueuePut(uint8 *packet)
{
    uint8 intState;
    uint8 retval = CYRET_MEMORY;
    uint8 nextwp;

    intState = CyEnterCriticalSection();
    nextwp = wp + 1;
    if (nextwp == QUEUE_DEPTH)
        nextwp = 0;
    if (nextwp != rp) /* not full */
    {
        i2cWriteQueue[wp] = packet;
        wp = nextwp; 
        retval = CYRET_SUCCESS;
    }
    CyExitCriticalSection(intState);
    return retval;
}

/* Retrieves I2C transfer from i2cWriteBuffer, queues it for BLE transfer and updates global I2C offset pointer */
uint8 ProcessI2cWriteXfer(void)
{
    uint8 i;
    uint8 retval = CYRET_BAD_DATA;
    uint8 *i2cWriteXfer = NULL;
    uint16 num_bytes = I2C_I2CSlaveGetWriteBufSize();

    if (num_bytes >= 2) /* Valid write means >= two bytes for sub-address */ 
    {
        i2cOffset = (i2cWriteBuffer[0] << 8) + i2cWriteBuffer[1];
   
        if (num_bytes > 2)  /* Actual data, not just sub-address */
        {
            i2cWriteXfer = (uint8 *)malloc(num_bytes + 2);
            if (i2cWriteXfer == NULL)
            {
                CyHalt(1);  /* Out of memory error */
            }
            else
            {
                i2cWriteXfer[0] = (num_bytes >> 8) & 0xFF;
                i2cWriteXfer[1] = num_bytes & 0xFF;
                i2cWriteXfer[2] = i2cWriteBuffer[0];
                i2cWriteXfer[3] = i2cWriteBuffer[1];
                for (i=0; i < num_bytes-2 && ( (i + i2cOffset) < BUFFER_SIZE ); i++)
                {
                    capSenseDataBuffer[i + i2cOffset] = i2cWriteBuffer[i + 2];
                    i2cWriteXfer[i + 4] = i2cWriteBuffer[i + 2];
                }
                retval = i2cWriteQueuePut(i2cWriteXfer);
            }
        }
    }

    /* Clear slave write buffer and status */
    I2C_I2CSlaveClearWriteBuf();
    (void) I2C_I2CSlaveClearWriteStatus();

    return retval;
}

static uint32 AddressAccepted(void)
{
    uint32 response;
    uint8 activeAddress = I2C_I2C_NAK_ADDR;

    /* Check for unprocessed writes */
    if (0u != (I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_WR_CMPLT))
        ProcessI2cWriteXfer();

    activeAddress = I2C_GET_I2C_7BIT_ADDRESS(I2C_RX_FIFO_RD_REG);
    if (activeAddress == I2C_SLAVE_ADDRESS)
    {
        I2C_I2CSlaveInitReadBuf (capSenseDataBuffer + i2cOffset,
                BUFFER_SIZE - i2cOffset);

        if (!i2cWriteQueueFull())
            I2C_I2CSlaveInitWriteBuf(i2cWriteBuffer, BUFFER_SIZE);
        else
            I2C_I2CSlaveInitWriteBuf(i2cWriteBuffer, 0);  /* NAK I2C Writes */
            
        response = I2C_I2C_ACK_ADDR;
    }
    
    return (response);
}

static void onTargetConnect()
{
    targetConnected = 1;
    BLUE_LED_Write(1);
    i2cWriteQueueClear();
    i2cOffset = 0;
    I2C_SetI2cAddressCustomInterruptHandler(&AddressAccepted);
    I2C_I2CSlaveInitReadBuf (capSenseDataBuffer, BUFFER_SIZE);
    I2C_I2CSlaveInitWriteBuf(i2cWriteBuffer, BUFFER_SIZE);
    I2C_Enable();
}

static void onTargetDisconnect()
{
    targetConnected = 0;
    BLUE_LED_Write(0);
    I2C_Stop();
}

void BleEventHandler(uint32 event, void *eventParam)
{
    uint8 intState;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T *hvpPtr;
    CYBLE_API_RESULT_T result;
    CYBLE_GATT_DB_ATTR_HANDLE_T descHandle;
    CYBLE_GATTC_WRITE_REQ_T writeReqParam;

    switch(event)
	{
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
        case CYBLE_EVT_STACK_ON:
        case CYBLE_EVT_TIMEOUT:
            onTargetDisconnect();
            CyBle_GapcConnectDevice(&targetAddr);
            break;

        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            result = CyBle_GattcStartDiscovery(cyBle_connHandle); 
            if (result != CYBLE_ERROR_OK)
                CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
            break;

        case CYBLE_EVT_GATTC_ERROR_RSP:
            CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
            break;
            
        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
            result = CyBle_GattcExchangeMtuReq(cyBle_connHandle, CYBLE_GATT_MTU);
            if (result != CYBLE_ERROR_OK)
                CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
            break;
            
        case CYBLE_EVT_GATTC_XCHNG_MTU_RSP:
            if ( ((CYBLE_GATT_XCHG_MTU_PARAM_T *)eventParam)->mtu != CYBLE_GATT_MTU )
                CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
            descHandle = cyBle_customCServ[CYBLE_CUSTOMC_CAPSENSE_TUNER_SERVICE_INDEX].\
                    customServChar[CYBLE_CUSTOMC_CAPSENSE_TUNER_DATA_BUFFER_CHAR_INDEX].\
                    customServCharDesc[CYBLE_CUSTOMC_CAPSENSE_TUNER_DATA_BUFFER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX].\
                    descHandle;
                    
            writeReqParam.attrHandle = descHandle; 
            writeReqParam.value = cccdNotifFlagSetStruct;        
            CyBle_GattcWriteCharacteristicDescriptors(cyBle_connHandle, &writeReqParam);                    
            break;
            
        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:
            hvpPtr = &((CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *)eventParam)->handleValPair;
            if (hvpPtr->attrHandle == \
                    cyBle_customCServ[CYBLE_CUSTOMC_CAPSENSE_TUNER_SERVICE_INDEX].\
                    customServChar[CYBLE_CUSTOMC_CAPSENSE_TUNER_DATA_BUFFER_CHAR_INDEX].customServCharHandle )
            {
                intState = CyEnterCriticalSection();
                do
                {
                    CyExitCriticalSection(intState);
                    intState = CyEnterCriticalSection();
                } while ( (I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_RD_BUSY) || \
                        (I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_WR_BUSY));
                
                memcpy(capSenseDataBuffer, hvpPtr->value.val, hvpPtr->value.len);

                CyExitCriticalSection(intState);

                if (!targetConnected)
                    onTargetConnect();
            }
            break;
            
		default:
    		break;
	}
}

int main()
{
    uint8 intState;
    CYBLE_GATTC_WRITE_CMD_REQ_T writeCmdReqParam;
    CYBLE_GATT_VALUE_T writeValue;
    CYBLE_API_RESULT_T result;
    uint8 *i2cWriteXfer = NULL;
    CyGlobalIntEnable; 

    UART_Start();
    I2C_Init();
	CyBle_Start(BleEventHandler);	

    while(1)
    {
        CyBle_ProcessEvents();
        
        /* Check for new data written from  I2C master */
        intState = CyEnterCriticalSection();
        do
        {
            CyExitCriticalSection(intState);
            intState = CyEnterCriticalSection();
        } while ( (I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_RD_BUSY) || \
                (I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_WR_BUSY));

        if (0u != (I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_WR_CMPLT))
        {
            ProcessI2cWriteXfer();  
        }
        CyExitCriticalSection(intState);

        /* Check for pending I2C writes that should be sent to CapSense target */
        if (i2cWriteQueueNotEmpty())
        {
            if ( targetConnected && (CyBle_GattGetBusyStatus() != CYBLE_STACK_STATE_BUSY) )
            {
                i2cWriteXfer = i2cWriteQueueGet();
                
                writeValue.len = i2cWriteXfer[0] << 8; 
                writeValue.len += i2cWriteXfer[1];
                writeValue.val = i2cWriteXfer + 2;
                
                writeCmdReqParam.attrHandle = \
                        cyBle_customCServ[CYBLE_CUSTOMC_CAPSENSE_TUNER_SERVICE_INDEX].\
                        customServChar[CYBLE_CUSTOMC_CAPSENSE_TUNER_EZI2C_WRITE_CHAR_INDEX].\
                        customServCharHandle;
                writeCmdReqParam.value = writeValue;    
                result = ~CYBLE_ERROR_OK;
                while (result != CYBLE_ERROR_OK)
                {
                    /* Send I2C write to CapSense target */
                    result = CyBle_GattcWriteWithoutResponse(cyBle_connHandle, &writeCmdReqParam);
                    CyBle_ProcessEvents();
                }
                free(i2cWriteXfer);
            }
        }
    }
}
    