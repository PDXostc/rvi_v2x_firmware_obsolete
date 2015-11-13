/*
 * powerSeq.c
 *
 * Created: 10/15/2015 2:45:54 PM
 *  Author: Spencer
 */ 

#include "sequence.h"


void init_power_seq() {
	 
	ioport_configure_pin(SEQ_SCK, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	gpio_set_pin_low(SEQ_SCK);
	ioport_configure_pin(SEQ_MOSI, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	gpio_set_pin_low(SEQ_MOSI);
	ioport_configure_pin(SR_LATCH, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	gpio_set_pin_low(SR_LATCH);
	ioport_configure_pin(SR_CLEAR, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	gpio_set_pin_high(SR_CLEAR);
	
}
void set_register() {
	gpio_set_pin_low(SR_CLEAR);
	gpio_set_pin_high(SR_CLEAR);
	gpio_set_pin_high(SEQ_SCK);
	gpio_set_pin_low(SEQ_SCK);
}

void bits_To_Shift_Register(uint16_t bits) {
	

	uint16_t preserve= bits;
	for (int i=0; i<16; i++) {
		if (bits>>i & 1)
		gpio_set_pin_high(SEQ_MOSI);
		else
		gpio_set_pin_low(SEQ_MOSI);
		
		gpio_set_pin_high(SEQ_SCK);
		gpio_set_pin_low(SEQ_SCK);
	}
	gpio_set_pin_high(SR_LATCH);
	gpio_set_pin_low(SR_LATCH);
	state = preserve;
}

void run_sequence(struct step sequence[], int size) {
	int i;
	
	for (i=0; i<size; i++) {
		delay_ms(sequence[i].interval);
		bits_To_Shift_Register(sequence[i].value);
	}
}


void turn_on (uint16_t pin_name_on) {
	state = pin_name_on | state;
	bits_To_Shift_Register(state);
}

void turn_off(uint16_t pin_name_off)
{
	uint16_t compare_matrix = pin_name_off ^ 0xffff;
	state = compare_matrix & state;
	bits_To_Shift_Register(state);
}
