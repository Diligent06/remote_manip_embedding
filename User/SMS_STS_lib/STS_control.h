#ifndef STS_CONTROL_H
#define STS_CONTROL_H 

#include "SCServo.h"
#include "SMS_STS.h"
#include "main.h"
#include "stdio.h"
#include "usart.h"
#include "usb_device.h"
#include <stdlib.h>

void STS_control_init(void);
void STS_syn_read(void);
void STS_syn_write(void);
uint16_t calc_move_time(int16_t delta_position);

extern uint16_t servo_position[];
extern uint16_t servo_speed[];
extern uint16_t servo_set_position[];
extern uint16_t default_speed;
extern uint16_t default_acc;
extern uint8_t servo_can_rx[8];
extern uint8_t servo_can_tx[8];
extern uint8_t servo_read_flag_12;
extern uint8_t servo_read_flag_3;
extern uint8_t servo_write_flag;
extern uint8_t servo_ID[3];

#endif