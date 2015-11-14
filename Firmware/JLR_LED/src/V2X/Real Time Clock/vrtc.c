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

//TODO: Remove for testing
static void alarm1(uint32_t time)
{
	ioport_set_pin_level(LED_0_PIN,!ioport_get_pin_level(LED_0_PIN));
	//rtc_set_alarm_relative(1024);
	setNextAlarmRoutine();
}

//TODO: Remove for testing
static void alarm2(uint32_t time) {
	int i;
	
	for (i=0; i<10; i++) {
		ioport_set_pin_level(LED_0_PIN,!ioport_get_pin_level(LED_0_PIN));
		delay_ms(50);
	}
	setNextAlarmRoutine();
}

static void alarm3(uint32_t time) {
	
	return;
	
}

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
	rtc_set_callback(alarm1);
	rtc_set_alarm_relative(32768);
	
}

void setNextAlarmRoutine() {
	alarm_node * next_alarm_node;
	next_alarm_node = extractNextAlarmNode(&alarm_list_head);
	
	//check if there's another alarm to be set
	if (next_alarm_node) {
		rtc_set_callback(next_alarm_node->alarm_callback_function);
		rtc_set_alarm(next_alarm_node->alarm_epoch_seconds);
	}
}

void addAlarm(uint32_t eSeconds, void (*alarmRoutine)(uint32_t) ) {
	alarm_node * newAlarmNode= (alarm_node *)malloc(sizeof(alarm_node));
	newAlarmNode->alarm_epoch_seconds = eSeconds;
	newAlarmNode->alarm_callback_function = alarmRoutine;
	
	//find position to insert
	alarm_node * curr = alarm_list_head;
	
	//MUTEX ACQUIRE NEEDED HERE?
	
	while (curr->next) {
		curr = curr->next;
		//position was found
		if (curr->alarm_epoch_seconds > newAlarmNode->alarm_epoch_seconds) {
			
			if (curr == alarm_list_head) {
				alarm_list_head->prev = newAlarmNode;
				newAlarmNode->prev = NULL;
				newAlarmNode->next = alarm_list_head;
				alarm_list_head = newAlarmNode;
			}
			
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
	ioport_set_pin_level(LED_0_PIN,!ioport_get_pin_level(LED_0_PIN));
	rtc_data.counter_high++;
	soft_counter++;
	/*
	if (rtc_data.counter_high++ == 0){
		//ioport_set_pin_level(LED_0_PIN,!ioport_get_pin_level(LED_0_PIN));
		soft_counter++;
	}
	*/
}
