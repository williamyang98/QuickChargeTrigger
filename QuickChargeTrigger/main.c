/*
 * main.c
 *
 * Created: 21/02/2021 5:14:13 PM
 * Author : William Yang
 */ 

#include "devinfo.h"
#include "quick_charge.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>

struct QuickCharge quick_charge = {
	.data_pos = {
		.low = {
			&PORTB, PB4,
			&PINB, PINB4,
			&DDRB, DDB4
		},
		.high = {
			&PORTB, PB3,
			&PINB, PINB3,
			&DDRB, DDB3
		}
	},
	.data_neg = {
		.low = {
			&PORTB, PB0,
			&PINB, PINB0,
			&DDRB, DDB0
		},
		.high = {
			&PORTB, PB1,
			&PINB, PINB1,
			&DDRB, DDB1
		}
	}
};

void system_sleep(void) {
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	sleep_mode();
	sleep_disable();
}

int main(void) {
	init_qc(&quick_charge);
	// NOTE: I am just using quick charge to get 12V to power a fan
	set_qc_voltage(&quick_charge, V12);
	system_sleep();
}

// NOTE: Demo with incrementing and decrementing quick charge voltage up and down
//       DO NOT USE this with a load connected, it is just for testing
void run_qc_test(struct QuickCharge* qc) {
	set_qc_voltage(qc, V5);
	_delay_us(1000);
	set_qc_voltage(qc, V9);
	_delay_us(1000);
	set_qc_voltage(qc, V12);
	_delay_us(1000);
	set_qc_voltage(qc, V20);
	_delay_us(1000);
	
	// to go from 5 to 20V in 0.2V increments
	// 1V = 5 * 0.2V
	// 20V-5V = 15
	// therefore 5*15 = 75 increments and decrements to ramp up and down
	const int total_increments = 75;
	set_qc_voltage(qc, V5);
	set_qc_voltage(qc, V_CONTINUOUS);
	while (1) {
		// Increase voltage
		for (int i = 0; i < total_increments; i++) {
			increment_qc(qc);
		}
		
		_delay_us(1000);
		 
		// Decrease voltage
		for (int i = 0; i < total_increments; i++) {
			decrement_qc(qc);
		}
	}
	
	set_qc_voltage(qc, V5);
}

