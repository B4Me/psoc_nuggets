#include <stdio.h>
#include <event_strings.h>

#ifdef DEBUGPRINTS
#define debug_print(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define debug_print(fmt, ...) do {} while (0)
#endif
