#include "STS_control.h"



uint8_t servo_ID[1] = {1 };
uint16_t servo_position[sizeof(servo_ID)];
uint16_t servo_speed[sizeof(servo_ID)];
uint8_t servo_rxPacket[4];
uint16_t servo_set_position[sizeof(servo_ID)];
uint16_t servo_set_speed[sizeof(servo_ID)];
uint16_t servo_set_acc[sizeof(servo_ID)];
uint16_t default_speed = 3250;
uint16_t default_acc = 0;
uint8_t servo_can_rx[8] = {0};
uint8_t servo_can_tx[8] = {0};
uint8_t servo_read_flag_12 = 0;
uint8_t servo_read_flag_3 = 0;
uint8_t servo_write_flag = 0;


char debug_message[32];
void STS_control_init(void) {
    // update read buffer length
    syncReadBegin(sizeof(servo_ID), sizeof(servo_rxPacket));
    // set default servo move speed and acc
    for(uint8_t i = 0; i < sizeof(servo_ID); i++) {
        servo_set_speed[i] = default_speed;  // V = 60*0.732 = 43.92rpm
        servo_set_acc[i] = default_acc;    // A = 50*8.7 = 435 deg/s^2
    }
}

void STS_syn_read(void) {
    syncReadPacketTx(servo_ID, sizeof(servo_ID), SMS_STS_PRESENT_POSITION_L, sizeof(servo_rxPacket));
    for(uint8_t i=0; i<sizeof(servo_ID); i++){

      if(!syncReadPacketRx(servo_ID[i], servo_rxPacket)){
        printf("ID:%d sync read error!\n", servo_ID[i]);
        continue;
      }
      servo_position[i] = syncReadRxPacketToWrod(15);
      servo_speed[i] = syncReadRxPacketToWrod(15);
      sprintf(debug_message, "ID:%d Position:%d Speed:%d", servo_ID[i], servo_position[i], servo_speed[i]);
    //   CDC_Transmit_FS((uint8_t*)debug_message, strlen(debug_message));
    }
}

void STS_syn_write(void) { 
    SyncWritePosEx(servo_ID, sizeof(servo_ID), servo_set_position, servo_set_speed, servo_set_acc);
}


uint16_t calc_move_time(int16_t delta_position) {
    uint16_t abs_delta = abs(delta_position);
    return (abs_delta * 1000 / (default_speed * 50) + (default_speed * 50) * 10 / default_acc + 50);
}

// FT servo communication interfaces
int fputc(int c, FILE *f)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)&c, 1, 100);
	return c;
}

void ftUart_Send(uint8_t *nDat , int nLen)
{
	HAL_UART_Transmit(&huart2, nDat, nLen, 100);
}

int ftUart_Read(uint8_t *nDat, int nLen)
{
	if(HAL_OK!=HAL_UART_Receive(&huart2, nDat, nLen, 100)){
		return 0;
	}else{
		return nLen;
	}
}

void ftBus_Delay(void)
{
	HAL_Delay(1);
}
