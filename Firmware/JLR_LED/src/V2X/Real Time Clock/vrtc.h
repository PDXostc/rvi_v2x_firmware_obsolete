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

static void alarm(uint32_t time);
static void alarm2(uint32_t time);
void init_vrtc(void);
void setNextAlarmRoutine(rtc_callback_t callback);
int epochSec(int y, int m, int d, int h, int min);

#endif /* RTC_H_ */