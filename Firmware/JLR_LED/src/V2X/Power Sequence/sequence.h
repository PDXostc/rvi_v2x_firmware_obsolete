/*
 * IncFile1.h
 *
 * Created: 10/15/2015 2:48:04 PM
 *  Author: Kevin
 */ 


#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include "../common/v2xcomm.h"


#define SEQ_MOSI EXT1_PIN_16	//DS
#define SR_LATCH EXT1_PIN_12	//ST_CP
#define SEQ_SCK EXT1_PIN_18		//SH_CP
#define SR_CLEAR EXT1_PIN_11	//MR

#define A 0X8000
#define B 0X4000
#define C 0X2000
#define D 0X1000
#define E 0X0800
#define F 0X0400
#define G 0X0200
#define H 0X0100
#define I 0x0080
#define J 0x0040
#define K 0x0020
#define L 0x0010
#define M 0x0008
#define N 0x0004
#define O 0x0002
#define P 0x0001


typedef struct step {
	uint16_t value;    // output value
	uint16_t interval; // interval in ms
} step_t;

typedef struct power_state {
	uint16_t curr;	//current bit state
} power_state_t;

struct power_state power;

void init_power_seq(void);
void bits_To_Shift_Register(uint16_t bits);
void run_sequence(struct step sequence[], int size);
void turn_on (uint16_t pin_name_on);
void turn_off(uint16_t pin_name_off);

static uint16_t state = 0x0000;

#endif /* SEQUENCE_H_ */