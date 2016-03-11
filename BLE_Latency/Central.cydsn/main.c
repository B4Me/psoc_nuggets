#include <project.h>
#define DEBUGPRINTS
#include <hkj_library.h>

CYBLE_GAP_BD_ADDR_T	connectPeriphDevice = \
    {{0x56, 0x34, 0x12, 0x50, 0xa0, 0x00}, 0};
CYBLE_CONN_HANDLE_T connHandle = {0, 0};

void BleEventHandler(uint32 event, void *eventParam)
{
    hkj_ble_events_log_add(event, eventParam);
    switch(event)
	{
        case CYBLE_EVT_STACK_ON:
            CyBle_GapcConnectDevice(&connectPeriphDevice);
            break;

        case CYBLE_EVT_GATT_CONNECT_IND:
            connHandle = *(CYBLE_CONN_HANDLE_T *)eventParam;
            break;

		default:
    		break;
	}
}

int main()
{
    hkj_timer t;
    const uint16 myCharHandle = 0x0E;
    uint8 writeData = 0;
    CYBLE_GATT_VALUE_T writeValue = \
        { &writeData, sizeof(writeData), sizeof(writeData) };
    CYBLE_GATTC_WRITE_REQ_T writeReqParam = \
        { .value = writeValue, .attrHandle = myCharHandle };

    CYBLE_STACK_LIB_VERSION_T stackVersion;

    CyGlobalIntEnable;
    UART_Start();
    hkj_debug_init();
    CyBle_GetStackLibraryVersion(&stackVersion);
    debug_print("Latency Central      Stack: %u.%u.%u.%u\r\n", \
        stackVersion.majorVersion, stackVersion.minorVersion, \
        stackVersion.patch, stackVersion.buildNumber);
    CyBle_Start(BleEventHandler);

    hkj_timer_ms_init(&t);
    while(1)
    {
        CyBle_ProcessEvents();
        if (connHandle.bdHandle != 0)
        {
            if (hkj_timer_ms_get_delta(&t) > 500)
            {
                WRITE_CMD_PIN_Write(1);
                CyBle_GattcWriteWithoutResponse(connHandle, &writeReqParam);
                hkj_timer_ms_reset_delta(&t);
                CyDelay(2); 
                WRITE_CMD_PIN_Write(0);
            }
        }
        hkj_ble_events_log_debug_print();
    }
}