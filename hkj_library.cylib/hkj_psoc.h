typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned long   uint32;
typedef signed   char   int8;
typedef signed   short  int16;
typedef signed   long   int32;
typedef          float  float32;

#define CY_ISR(FuncName)        void FuncName (void)
#define CY_ISR_PROTO(FuncName)  void FuncName (void)

#define CyGlobalIntEnable           do                      \
                                    {                       \
                                        __asm("CPSIE   i"); \
                                    } while ( 0 )

#define CyGlobalIntDisable          do                      \
                                    {                       \
                                        __asm("CPSID   i"); \
                                    } while ( 0 )

typedef void (* cyisraddress)(void);

cyisraddress CyIntSetSysVector(uint8 number, cyisraddress address);
cyisraddress CyIntGetSysVector(uint8 number);
