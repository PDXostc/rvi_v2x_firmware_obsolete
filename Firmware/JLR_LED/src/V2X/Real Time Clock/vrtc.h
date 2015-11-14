/*
 * rtc.h
 *
 * Created: 10/20/2015 12:13:19 PM
 *  Author: Kevin
 */ 


#ifndef RTC_H_
#define RTC_H_

#include "../common/v2xcomm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <time.h>

volatile uint16_t soft_counter;

typedef struct alarm_node_t alarm_node;
struct alarm_node_t {
	unsigned int alarm_epoch_seconds;
	void (*alarm_callback_function)(uint32_t);
	alarm_node * prev;
	alarm_node * next;
};

alarm_node * alarm_list_head;// = NULL;

static void alarm(uint32_t time);
static void alarm2(uint32_t time);
void init_vrtc(void);
void setNextAlarmRoutine(void);
alarm_node * extractNextAlarmNode(alarm_node ** head);
void addAlarm(uint32_t, void (*alarmRoutine)(uint32_t));
int epochSec(int y, int m, int d, int h, int min);

#endif /* RTC_H_ */