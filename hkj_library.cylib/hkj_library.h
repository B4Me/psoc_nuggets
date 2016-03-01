#include "hkj_psoc.h"
#include <stdio.h>

#ifndef FALSE
    #define FALSE 0
#endif
#ifndef TRUE
    #define TRUE 1
#endif

typedef struct {
    uint32 start_ms_tick;
    uint32 delta_start_ms_tick;
} hkj_timer;

void hkj_timer_ms_init(hkj_timer *t);
uint32 hkj_timer_ms_get_tick(hkj_timer *t);
void hkj_timer_ms_reset_delta(hkj_timer *t);
uint32 hkj_timer_ms_get_delta(hkj_timer *t);

const char *hkj_ble_events_get_name(int event);
void hkj_ble_events_log_init(void);
void hkj_ble_events_log_clear(void);
void hkj_ble_events_log_add(int event, void *eventparam);
void hkj_ble_events_log_debug_print(void);

void hkj_debug_init(void);

#ifdef DEBUGPRINTS
#define debug_print(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define debug_print(fmt, ...) do {} while (0)
#endif

