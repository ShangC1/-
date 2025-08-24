#ifndef __MOTOR_H
#define __MOTOR_H

#define FAN_ON		50

#define FAN_OFF		0

extern uint8_t fan_speed;

void Motor_Init(void);

void Motor_SetSpeed(uint8_t Speed);

#endif
