#include <project.h>
#define DEBUGPRINTS
#include <hkj_library.h>

#define NUM_BYTES_TX 500000
#define CYBLE_ENDPOINT_OUT_CHAR_HANDLE 0x13

CYBLE_GAP_BD_ADDR_T	connectPeriphDevice = {{0x01, 0x00, 0x00, 0x50, 0xa0, 0x00}, 0};
CYBLE_CONN_HANDLE_T connHandle = {0, 0};

uint8 writeData[CYBLE_GATT_MTU-3] = { 0 };

uint8 mtuConfigured = 0;
uint8 sequenceNr = 0;

static void BleEventHandler(uint32 event, void *eventParam)
{    
    switch(event)
	{
        case CYBLE_EVT_STACK_ON:
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            sequenceNr = 0;
            connHandle = (CYBLE_CONN_HANDLE_T){0, 0};
            CyBle_GapcConnectDevice(&connectPeriphDevice);
            break;
        case CYBLE_EVT_GATT_CONNECT_IND:
            connHandle = *(CYBLE_CONN_HANDLE_T *)eventParam;
            debug_print("Connected to GAP Peripheral\r\n");
            break;
        case CYBLE_EVT_GATTC_XCHNG_MTU_RSP:
            mtuConfigured = 1;
            break;            
		default:
    		break;
	}
}

static CYBLE_API_RESULT_T measureTx(uint32 numBytes, uint16 attMtu, uint16 connMaxTxOctets)
{
    (void)connMaxTxOctets; /* connMaxTxOctets not supported yet */
    CYBLE_API_RESULT_T result;
    CYBLE_GATTC_WRITE_REQ_T writeReqParam;

    hkj_timer t;
    
    uint32 bytesTx = 0;
    uint32 throughput = 0;
    uint32 time = 0;
    uint16 mtu = 0;
    uint i = 0;
    
    hkj_timer_ms_init(&t);

    CyBle_GattGetMtuSize(&mtu);
    if (mtu != attMtu)
    {
        mtuConfigured = 0;
        result = CyBle_GattcExchangeMtuReq(connHandle, attMtu);
        if (result != CYBLE_ERROR_OK)
            return result;
        while (!mtuConfigured)
            CyBle_ProcessEvents();    
        CyBle_GattGetMtuSize(&mtu);
    }

    writeReqParam.value.val = writeData;
    writeReqParam.value.len = mtu - 3;
    writeReqParam.attrHandle = CYBLE_ENDPOINT_OUT_CHAR_HANDLE;

    /* 100 initial dummy writes to avoid measuring buffer filling */
    while (i < 100)
    {
        CyBle_ProcessEvents();        
        if (CyBle_GattGetBusStatus() != CYBLE_STACK_STATE_BUSY)
        {
            writeData[0] = writeData[writeReqParam.value.len - 1] = sequenceNr;
            result = CyBle_GattcWriteWithoutResponse(connHandle, &writeReqParam);
            if (result == CYBLE_ERROR_OK)
            {
                i++;
                sequenceNr++;
            }
        }
    }

    hkj_timer_ms_reset_delta(&t);
    while(bytesTx < numBytes)
    {
        if(CyBle_GattGetBusStatus() != CYBLE_STACK_STATE_BUSY)
        {
            writeData[0] = writeData[writeReqParam.value.len - 1] = sequenceNr;
            result = CyBle_GattcWriteWithoutResponse(connHandle, &writeReqParam);
            if (result == CYBLE_ERROR_OK)
            {
                bytesTx += writeReqParam.value.len;
                sequenceNr++;
            }
        }
        CyBle_ProcessEvents();        
    }
    time = hkj_timer_ms_get_delta(&t);
    throughput = ( (uint64)bytesTx * 8 * 1000 ) / time;
    printf("%u,%lu,%lu,%lu\r\n", mtu, bytesTx, time, throughput);

    return CYBLE_ERROR_OK;
}

int main()
{
    uint16 connMaxTxOctets = CYBLE_LL_MIN_SUPPORTED_TX_PAYLOAD_SIZE;
    uint16 attMtu = CYBLE_GATT_MTU;
    uint i = 0;

    CyGlobalIntEnable; 
    UART_Start();
    hkj_debug_init();
    printf("Starting GAP Central\r\n");
    CyBle_Start(BleEventHandler);	

    while (connHandle.bdHandle == 0)
        CyBle_ProcessEvents();

    for (i=0; i<10; i++)
    {
        CyBle_ProcessEvents();
        CyDelay(300);
    }

    printf("ATT_MTU,Bytes Sent,Time(ms),Throughput(kbps)\r\n");
    
    connMaxTxOctets = 27;
    attMtu = 23;
    measureTx(NUM_BYTES_TX, attMtu, connMaxTxOctets);
    measureTx(NUM_BYTES_TX, attMtu, connMaxTxOctets);
    measureTx(NUM_BYTES_TX, attMtu, connMaxTxOctets);

    connMaxTxOctets = 251;
    attMtu = 247;
    measureTx(NUM_BYTES_TX, attMtu, connMaxTxOctets);
    measureTx(NUM_BYTES_TX, attMtu, connMaxTxOctets);
    measureTx(NUM_BYTES_TX, attMtu, connMaxTxOctets);

    return 0;    
}