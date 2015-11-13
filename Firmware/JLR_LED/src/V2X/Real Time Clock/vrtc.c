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

typedef struct alarm_node_t alarm_node;
struct alarm_node_t {
	unsigned int alarm_epoch_seconds;
	void * alarm_callback_function;
	alarm_node * prev;
	alarm_node * next;
};


//TODO: Remove for testing
static void alarm(uint32_t time)
{
	ioport_set_pin_level(LED_0_PIN,!ioport_get_pin_level(LED_0_PIN));
	//rtc_set_alarm_relative(1024);
	rtc_set_callback(alarm2);
	rtc_set_alarm_relative(32768*2);
}

//TODO: Remove for testing
static void alarm2(uint32_t time) {
	int i;
	
	for (i=0; i<10; i++) {
		ioport_set_pin_level(LED_0_PIN,!ioport_get_pin_level(LED_0_PIN));
		delay_ms(50);
	}
	rtc_set_callback(alarm);
	rtc_set_alarm_relative(32768);
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
	rtc_set_callback(alarm);
	rtc_set_alarm_relative(32768);
	
}

void setNextAlarmRoutine(rtc_callback_t callback) {
	rtc_set_callback(callback);
}

alarm_node * popNextAlarmNode(alarm_node * head) {

	alarm_node * earliest = head;
	alarm_node * curr = head;

	while (curr) {
		if (curr->alarm_epoch_seconds < earliest->alarm_epoch_seconds) {
			earliest = curr;
		}
		curr = curr->next;
	}

	/* TODO: TEST CASES TO POP EARLIEST*/
	if (earliest == head) {
		head = head->next;
	}

	else if (!earliest->next) {
		earliest->prev = NULL;
	}

	else {
		earliest->prev->next = earliest->next;
		earliest->next->prev = earliest->prev;
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
