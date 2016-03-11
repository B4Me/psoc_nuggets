#include "hkj_library.h"

#define UART_UartPutChar(ch) UART_SpiUartWriteTxData((uint32)(ch))
extern void UART_SpiUartWriteTxData(uint32 txData);

int _write(int file, char *ptr, int len)
{
    (void)file;
    int i;

    if (ptr)
    {
        for (i = 0; i < len; i++)
            UART_UartPutChar(*ptr++);
    }
    return len;
}

void hkj_debug_init(void)
{
    _write(0, 0, 0); /* Dummy call to force linking of own _write() */
    hkj_ble_events_log_init();
}

