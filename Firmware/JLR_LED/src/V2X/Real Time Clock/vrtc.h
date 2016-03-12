/*
 * vrtc.h
 *
 * Created: 10/20/2015 12:13:19 PM
 *  Author: Kevin
 */ 


#ifndef VRTC_H_
#define VRTC_H_

#include "../common/v2xcomm.h"
#include "epoch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLOCK_FREQ 32768

typedef struct alarm_node_t alarm_node;
struct alarm_node_t {
	unsigned char expired;
	uint32_t alarm_epoch_seconds;
	void (*alarm_callback_function)(uint32_t);
	alarm_node * prev;
	alarm_node * next;
};

volatile uint16_t soft_counter;
alarm_node * alarm_list_head;// = NULL;
alarm_node * alarm_curr_ref; //reference to current set alarm


void init_vrtc(void);
void setEpochTime(uint32_t);
void setNextAlarmRoutine(void);
alarm_node * peakNextAlarmNode(alarm_node ** head);
void removeExpiredAlarms (void);
alarm_node * extractNextAlarmNode(alarm_node ** head);
void addAlarm(uint32_t, void (*alarmRoutine)(uint32_t));
int epochSec(int y, int m, int d, int h, int min);

#endif /* VRTC_H_ */