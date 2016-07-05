#include <project.h>

extern CapSense_RAM_STRUCT CapSense_dsRam_mirror;

void Tuner_Init(void (*refresh)(void), void (*send)(void));
void Tuner_RunTuner(void);
