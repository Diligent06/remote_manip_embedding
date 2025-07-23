/*
 * SCServo.c
 * ���ض��Ӳ���ӿڲ����
 * ����: 2024.12.2
 * ����: txl
 */
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "SCSerial.h"

uint8_t wBuf[128];
uint8_t wLen = 0;

void ftUart_Send(uint8_t *nDat, int nLen);
int ftUart_Read(uint8_t *nDat, int nLen);
void ftBus_Delay(void);

//UART �������ݽӿ�
int readSCS(unsigned char *nDat, int nLen)
{
	return ftUart_Read(nDat, nLen);
}

//UART �������ݽӿ�
int writeSCS(unsigned char *nDat, int nLen)
{
	while(nLen--){
		if(wLen<sizeof(wBuf)){
			wBuf[wLen] = *nDat;
			wLen++;
			nDat++;
		}
	}
	return wLen;
}

int writeByteSCS(unsigned char bDat)
{
	if(wLen<sizeof(wBuf)){
		wBuf[wLen] = bDat;
		wLen++;
	}
	return wLen;
}

//���ջ�����ˢ��
void rFlushSCS()
{
	ftBus_Delay();
}

//���ͻ�����ˢ��
void wFlushSCS()
{
	if(wLen){
		ftUart_Send(wBuf, wLen);
		wLen = 0;
	}
}

#ifdef __cplusplus
}
#endif