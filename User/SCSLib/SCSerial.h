#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>


void ftUart_Send(uint8_t *nDat, int nLen);
int ftUart_Read(uint8_t *nDat, int nLen);
void ftBus_Delay(void);

#ifdef __cplusplus
}
#endif