#include "hkj_library.h"

extern void CySysTickStart(void);
extern void CySysTickEnableInterrupt(void);
extern void CySysTickDisableInterrupt(void);

#define CY_INT_SYSTICK_IRQN (15u)

static volatile uint32 ms_tick = 0;
static int started = FALSE;

CY_ISR(hkj_ms_isr)
{
    ms_tick++;
}

void hkj_timer_ms_init(hkj_timer *t)
{
    if (!started)
    {
        CySysTickStart();
        CyIntSetSysVector(CY_INT_SYSTICK_IRQN, hkj_ms_isr);
        CySysTickEnableInterrupt();
        started = TRUE;
    }
    CySysTickDisableInterrupt();
    t->start_ms_tick = ms_tick;
    CySysTickEnableInterrupt();
    t->delta_start_ms_tick = t->start_ms_tick;
}

uint32 hkj_timer_ms_get_tick(hkj_timer *t)
{
    uint32 temp = 0;
    CySysTickDisableInterrupt();
    temp = ms_tick;
    CySysTickEnableInterrupt();
    return temp - t->start_ms_tick;    
}

void hkj_timer_ms_reset_delta(hkj_timer *t)
{
    t->delta_start_ms_tick = hkj_timer_ms_get_tick(t);
}

uint32 hkj_timer_ms_get_delta(hkj_timer *t)
{
    return hkj_timer_ms_get_tick(t) - t->delta_start_ms_tick;
}