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

/* cdc/usb support */
#include "conf_usb.h"
#include "ui.h"
#include "uart.h"

static volatile bool main_b_cdc_enable = false;


//TODO: Remove, for testing power sequencing
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
	
	//USB cdc
	irq_initialize_vectors();
	// cpu_irq_enable();	/* taken care of in init_vrtc below */
	
	//v2x power sequencing
	init_power_seq();
	
	//v2x real time clock
	init_vrtc();

/* already done above */
/*	kevinC
#if !SAM0
	sysclk_init();
	board_init();
#else
	system_init();
#endif
*/ //end kevinC
	ui_init();
	ui_powerdown();
	// Start USB stack to authorize VBus monitoring
	udc_start();
	
	
	/************
	
	MAIN IS CURRENTLY BEING USED FOR INITIALIZATION AND TESTING
	
	*************/
	
	//currently every  high_counter=.5t
	uint32_t test_time = 1448436421;
	setEpochTime(test_time);
	addAlarm(test_time+2, alarm1);	// blink
	addAlarm(test_time+4, alarm2);	// fast burst
	addAlarm(test_time+5, alarm3);	// slow burst
	//addAlarm(test_time+14, alarm3);	// slow burst
	//addAlarm(test_time+8, alarm3);	// slow burst
	/*
	unsigned char z = 0;
	unsigned char a = 0;
	*/
	while(1){
		
		// Sleep between each triggered alarm
		sleepmgr_enter_sleep();
		
		// Can extend alarm test here to create additional alarms
		// relative to the current time.
		/*
		if (!z && rtc_data.counter_high > 6) {
			addAlarm(test_time+8, alarm2);
			z = 1;
		}
		else if (!a && rtc_data.counter_high > 9) {
			addAlarm(test_time+11, alarm2);
			a = 1;
		}
		*/
	}
}

void main_suspend_action(void)
{
	ui_powerdown();
}

void main_resume_action(void)
{
	ui_wakeup();
}

void main_sof_action(void)
{
	if (!main_b_cdc_enable)
		return;
	ui_process(udd_get_frame_number());
}

#ifdef USB_DEVICE_LPM_SUPPORT
void main_suspend_lpm_action(void)
{
	ui_powerdown();
}

void main_remotewakeup_lpm_disable(void)
{
	ui_wakeup_disable();
}

void main_remotewakeup_lpm_enable(void)
{
	ui_wakeup_enable();
}
#endif

bool main_cdc_enable(uint8_t port)
{
	main_b_cdc_enable = true;
	// Open communication
	uart_open(port);
	return true;
}

void main_cdc_disable(uint8_t port)
{
	main_b_cdc_enable = false;
	// Close communication
	uart_close(port);
}

void main_cdc_set_dtr(uint8_t port, bool b_enable)
{
	if (b_enable) {
		// Host terminal has open COM
		ui_com_open(port);
	}else{
		// Host terminal has close COM
		ui_com_close(port);
	}
}

/**
 * \mainpage ASF USB Device CDC
 *
 * \section intro Introduction
 * This example shows how to implement a USB Device CDC
 * on Atmel MCU with USB module.
 * The application note AVR4907 provides more information
 * about this implementation.
 *
 * \section desc Description of the Communication Device Class (CDC)
 * The Communication Device Class (CDC) is a general-purpose way to enable all
 * types of communications on the Universal Serial Bus (USB).
 * This class makes it possible to connect communication devices such as
 * digital telephones or analog modems, as well as networking devices
 * like ADSL or Cable modems.
 * While a CDC device enables the implementation of quite complex devices,
 * it can also be used as a very simple method for communication on the USB.
 * For example, a CDC device can appear as a virtual COM port, which greatly
 * simplifies application development on the host side.
 *
 * \section startup Startup
 * The example is a bridge between a USART from the main MCU
 * and the USB CDC interface.
 *
 * In this example, we will use a PC as a USB host:
 * it connects to the USB and to the USART board connector.
 * - Connect the USART peripheral to the USART interface of the board.
 * - Connect the application to a USB host (e.g. a PC)
 *   with a mini-B (embedded side) to A (PC host side) cable.
 * The application will behave as a virtual COM (see Windows Device Manager).
 * - Open a HyperTerminal on both COM ports (RS232 and Virtual COM)
 * - Select the same configuration for both COM ports up to 115200 baud.
 * - Type a character in one HyperTerminal and it will echo in the other.
 *
 * \note
 * On the first connection of the board on the PC,
 * the operating system will detect a new peripheral:
 * - This will open a new hardware installation window.
 * - Choose "No, not this time" to connect to Windows Update for this installation
 * - click "Next"
 * - When requested by Windows for a driver INF file, select the
 *   atmel_devices_cdc.inf file in the directory indicated in the Atmel Studio
 *   "Solution Explorer" window.
 * - click "Next"
 *
 * \copydoc UI
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB Device stack and CDC modules:
 *   <br>services/usb/
 *   <br>services/usb/udc/
 *   <br>services/usb/class/cdc/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>manages UI
 *      <br>
 *    - uart_xmega.c,
 *      <br>implementation of RS232 bridge for XMEGA parts
 *    - uart_uc3.c,
 *      <br>implementation of RS232 bridge for UC3 parts
 *    - uart_sam.c,
 *      <br>implementation of RS232 bridge for SAM parts
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (leds,buttons...)
 */



