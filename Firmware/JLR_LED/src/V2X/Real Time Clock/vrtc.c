/*
 * CFile1.c
 *
 * Created: 10/22/2015 3:24:03 PM
 *  Author: Kevin
 */ 


#include "vrtc.h"

//static struct rtc_data_struct rtc_data;
//const int months[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/*
 * ***CURRENT SETTINGS FOR CLOCK PRESCALAR AND RTC SOURCE***
 *  **CAN BE CHANGED (see definitions)** 
 *
 *	Content of conf_rtc.h:
 *  #define CONFIG_RTC_PRESCALER       RTC_PRESCALER_DIV1024_gc
 *	Needed in conf_clock.h:
 *	#define CONFIG_RTC_SOURCE          SYSCLK_RTCSRC_ULP
 *	
 */

void init_vrtc(){
	//real time clock inits
	pmic_init();
	sysclk_init();
	sleepmgr_init();
	rtc_init();	
	cpu_irq_enable();
	
	//v2x init
	soft_counter = 0x00;
	alarm_list_head = NULL;
	//rtc_set_callback(alarm1);
	//rtc_set_alarm_relative(32768);
	
}

void setNextAlarmRoutine() {
	alarm_node * next_alarm_node;
	next_alarm_node = peakNextAlarmNode(&alarm_list_head);
	
	//check if there's another alarm to be set
	if (next_alarm_node) {
		rtc_set_callback(next_alarm_node->alarm_callback_function);
		rtc_set_alarm(next_alarm_node->alarm_epoch_seconds);
	}
	
	// clear the alarm routine callback
	else 
		rtc_set_callback(NULL);
	
	//update the current reference
	alarm_curr_ref = next_alarm_node;
}

void addAlarm(uint32_t eSeconds, void (*alarmRoutine)(uint32_t) ) {

	//clean up expired nodes
	removeExpiredAlarms();

	alarm_node * newAlarmNode= (alarm_node *)malloc(sizeof(alarm_node));
	newAlarmNode->alarm_epoch_seconds = eSeconds;
	newAlarmNode->alarm_callback_function = alarmRoutine;
	newAlarmNode->expired = 0;

	//no list instantiated, begin new one
	if (!alarm_list_head) {
		alarm_list_head = newAlarmNode;
		alarm_list_head->next = NULL;
		alarm_list_head->prev = NULL;
		
		// initiate the new alarm and return
		alarm_curr_ref = alarm_list_head;
		rtc_set_callback(alarm_list_head->alarm_callback_function);
		rtc_set_alarm(alarm_list_head->alarm_epoch_seconds);
		return;
	}
	
	//find position to insert
	alarm_node * curr = alarm_list_head;

	//MUTEX ACQUIRE NEEDED HERE?
	//CONCURRENCY IS POSSIBLE, BUT WE USE ONLY 1 THREAD FOR V2X APPLICATION

	if (curr->alarm_epoch_seconds > newAlarmNode->alarm_epoch_seconds) {
		alarm_list_head->prev = newAlarmNode;
		newAlarmNode->prev = NULL;
		newAlarmNode->next = alarm_list_head;
		alarm_list_head = newAlarmNode;
		return;
	}

	while (curr->next) {
		curr = curr->next;
		//position was found
		if (curr->alarm_epoch_seconds > newAlarmNode->alarm_epoch_seconds) {
			newAlarmNode->next = curr;
			newAlarmNode->prev = curr->prev;
			(curr->prev)->next = newAlarmNode;
			curr->prev = newAlarmNode;
			return;
		}
	}
	
	//insert at tail
	curr->next = newAlarmNode;
	newAlarmNode->prev = curr;
	newAlarmNode->next = NULL;
	
	//MUTEX RELEASE NEEDED HERE?
	
}

//find next unexpired alarm
alarm_node * peakNextAlarmNode(alarm_node ** head) {
	uint32_t curr_time = (uint32_t)((uint32_t)soft_counter << 16) + (uint32_t)rtc_data.alarm_high;
	
	if (!(*head))
		return NULL;

	if (!(*head)->expired && (*head)->alarm_epoch_seconds >= curr_time )
		return *head;

	alarm_node * curr = *head;

	while (curr->next) {
		curr = curr->next;
		if (!curr->expired && curr->alarm_epoch_seconds >= curr_time)
			return curr;
	}
	return NULL;
}

void removeExpiredAlarms () {

	if (!alarm_list_head)
		return;

	alarm_node * curr = alarm_list_head;

	uint32_t curr_time = (uint32_t)((uint32_t)soft_counter << 16) + (uint32_t)rtc_data.counter_high;

	do {
		
		if (curr->alarm_epoch_seconds <= curr_time ) {
			if (curr->next) {
				alarm_list_head = curr->next;
				free(curr);
				alarm_list_head->prev = NULL;
				curr = alarm_list_head;
			}
			else {
				free(curr);
				alarm_list_head = NULL;
				return;
			}
		}
		else
			return;
		
	} while (curr);
}


alarm_node * extractNextAlarmNode(alarm_node ** head) {

	alarm_node * earliest = *head;
	alarm_node * curr = *head;

	while (curr->next) {
		curr = curr->next;
		if (curr->alarm_epoch_seconds < earliest->alarm_epoch_seconds) {
			earliest = curr;
		}
	}

	//check if there's a left node
	//if there's a right node, then we reassign
	// NODE-> <-EARLIEST-> <-NODE
	if (earliest->prev && earliest->next) {
		(earliest->next)->prev = earliest->prev;
		(earliest->prev)->next = earliest->next;
	}
	//check if head and if there's any other alarm nodes
	else if (earliest == *head) {
		if ((*head)->next) {
			*head = (*head)->next;
			(*head)->prev = NULL;
		}
	}
	//check if tail
	else if (!earliest->next) {
		(earliest->prev)->next = NULL;
	}

	return earliest;
}

//Redefined from rtc.c
ISR(RTC_OVF_vect)
{
	//if (rtc_data.counter_high == 0)
	//	rtc_data.counter_high = 65533;
	
	if (++(rtc_data.counter_high) == 0) {
		//ioport_set_pin_level(LED_0_PIN,!ioport_get_pin_level(LED_0_PIN));
		soft_counter++;
	}
	//ioport_set_pin_level(LED_0_PIN,!ioport_get_pin_level(LED_0_PIN));
}

//Redefined from rtc.c
//Should wake the device at regular intervals RTC_COMP_vect

ISR(RTC_COMP_vect)
{
	//RTC.CNT is a 32 bit word register holding the value of the clock (or the pre-scaled clock??)
	
	// uint32_t curr_time =	(uint32_t)((uint32_t)soft_counter << 16) |
	// 						(uint32_t)((uint32_t)rtc_data.counter_high * 2) |
	// 						(uint32_t)RTC.CNT; // frequency????********;	

	uint32_t curr_time = (uint32_t)((uint32_t)soft_counter << 16) | (uint32_t)rtc_data.counter_high;

	// currently curr_time represents half-seconds
	// therefore divide this quantity by 2
	uint32_t alarm_time = ((uint32_t)((uint32_t)rtc_data.alarm_high << 16) | (uint32_t)rtc_data.alarm_low) / 2;

	if (curr_time > alarm_time && alarm_curr_ref && !alarm_curr_ref->expired) {
		RTC.INTCTRL = RTC_OVERFLOW_INT_LEVEL;
		if (rtc_data.callback) {
			uint32_t count = ((uint32_t)rtc_data.counter_high << 16)
					| RTC.CNT;
	
			//previously alarm_low referenced the physical clock cycles
			uint32_t alarm = ((uint32_t)rtc_data.alarm_high << 16)
					| rtc_data.alarm_low;
			/* Workaround for errata. Count might not be updated
			 * when waking up from sleep, so in this case use alarm
			 * time plus one.
			 */
			if (alarm >= count)
				count = alarm + 1;
			rtc_data.callback(count);
			alarm_curr_ref->expired = 1;
			
			/* 
			 * kevinC
			 * Need to check a queue here to see 
			 * if there is another alarm to be set
			 * peak from priority queue (pq) which  will hold
			 * tuples of (time,func*). The key for pq elements
			 * is time.
			 */
			setNextAlarmRoutine();
		}
	}
}
