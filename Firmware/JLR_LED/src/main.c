/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * This is a bare minimum user application template.
 *
 * For documentation of the board, go \ref group_common_boards "here" for a link
 * to the board-specific documentation.
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to board_init()
 * -# Basic usage of on-board LED and button
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <v2x.h>

#define SEQ_SIZE 32

uint16_t values [SEQ_SIZE] =	{
								0X8000, 0X4000, 0X2000, 0X1000, 0X0800, 0X0400, 0X0200, 0X0100,
	
								0x0080, 0x0040, 0x0020, 0x0010, 0x0008, 0x0004, 0x0002, 0x0001, 
							
								0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080, 
							
								0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000 
								};
														
struct step seq[SEQ_SIZE];
uint32_t x = 0;
uint32_t y = 0;

//TODO: Remove, for testing alarms functionality
static void alarm1(uint32_t time)
{
	//LED BLIP
	ioport_set_pin_level(LED_0_PIN,LED_0_ACTIVE);
	delay_ms(200);
	ioport_set_pin_level(LED_0_PIN,LED_0_INACTIVE);

}

//TODO: Remove, for testing alarm functionality
static void alarm2(uint32_t time) {
	int i;
	
	// LED FAST BURST
	for (i=0; i<10; i++) {
		ioport_set_pin_level(LED_0_PIN,!ioport_get_pin_level(LED_0_PIN));
		delay_ms(50);
	}
	//ioport_set_pin_level(LED_0_PIN,LED_0_INACTIVE);
}

//TODO: Remove, for testing alarm functionality
static void alarm3(uint32_t time) {
	int i;
	
	// LED SLOW BURST
	for (i=0; i<5; i++) {
		ioport_set_pin_level(LED_0_PIN,!ioport_get_pin_level(LED_0_PIN));
		delay_ms(150);
	}
	ioport_set_pin_level(LED_0_PIN,LED_0_INACTIVE);
}

int main (void)
{
	//board-specific
	board_init();
	ioport_init();
	ssd1306_init();
	
	//v2x power sequencing
	init_power_seq();
	
	//v2x real time clock
	init_vrtc();
	
	
	
	/************
	
	MAIN IS CURRENTLY BEING USED FOR INITIALIZATION AND TESTING
	
	*************/
	
	//currently every  high_counter=.5t
	addAlarm(2, alarm2);	// fast burst
	addAlarm(4, alarm1);	// blink
	//addAlarm(12, alarm2);	// fast burst
	//addAlarm(14, alarm3);	// slow burst
	//addAlarm(8, alarm3);	// slow burst
	
	// fast, blink, slow, fast, slow
	unsigned char z = 0;
	unsigned char a = 0;
	
	while(1){
		
		// Sleep between each triggered alarm
		sleepmgr_enter_sleep();
		
		// Can extend alarm test here to create additional alarms
		// relative to the current time.
		if (!z && rtc_data.counter_high > 6) {
			addAlarm(8, alarm2);
			z = 1;
		}
		else if (!a && rtc_data.counter_high > 9) {
			addAlarm(11, alarm2);
			a = 1;
		}

	}
}



