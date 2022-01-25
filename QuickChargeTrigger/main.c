/*
 * QuickChargeTrigger.c
 *
 * Created: 21/02/2021 5:14:13 PM
 * Author : acidi
 */ 

#ifndef F_CPU
#define F_CPU 1000000
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>

struct Pin {
	volatile uint8_t *PORTx;
	uint8_t PORTxn;
	volatile uint8_t *PINx;
	uint8_t PINxn;
	volatile uint8_t *DDRx;
	uint8_t DDxn;
};

struct DataOut {
	struct Pin high;
	struct Pin low;	
};

struct DataOut D_POS = {
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
};

struct DataOut D_NEG = {
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
};

enum DATA_VOLTAGE { V_FLOAT, V_0, V_0_6, V_3_3 };
enum PIN_VOLTAGE { LOW, HIGH, FLOAT };

void set_pin(struct Pin *p, enum PIN_VOLTAGE v) {
	switch (v) 
	{
	case LOW:
		*(p->PORTx) &= ~(1 << p->PORTxn); 
		*(p->DDRx) |= (1 << p->DDxn);
		break;
	case HIGH:
		*(p->PORTx) |= (1 << p->PORTxn);
		*(p->DDRx) |= (1 << p->DDxn);
		break;
	case FLOAT:
		*(p->PORTx) &= ~(1 << p->PORTxn);
		*(p->DDRx) &= ~(1 << p->DDxn);
		break;
	};
}

void set_data_out(struct DataOut *p, enum DATA_VOLTAGE v) {
	switch (v) 
	{
	case V_FLOAT:
		set_pin(&(p->high), FLOAT);	
		set_pin(&(p->low), FLOAT);
		break;
	case V_0:
		set_pin(&(p->high), LOW);
		set_pin(&(p->low), LOW);
		break;
	case V_0_6:
		set_pin(&(p->high), HIGH);
		set_pin(&(p->low), LOW);
		break;
	case V_3_3:
		set_pin(&(p->high), HIGH);
		set_pin(&(p->low), HIGH);
		break;
	};	
};

void init_qc() {
	set_data_out(&D_POS, V_0_6);
	set_data_out(&D_NEG, V_FLOAT);
	// max init time 1.0-1.4 s
	_delay_ms(1500);
}

enum QC_VOLTAGE { V5, V9, V12, V_CONTINUOUS, V_RESERVE };
	
void set_qc_voltage(enum QC_VOLTAGE v) {
	switch (v) 
	{
	case V5:
		set_data_out(&D_POS, V_0_6);
		set_data_out(&D_NEG, V_0);
		break;
	case V9:
		set_data_out(&D_POS, V_3_3);
		set_data_out(&D_NEG, V_0_6);
		break;
	case V12:
		set_data_out(&D_POS, V_0_6);
		set_data_out(&D_NEG, V_0_6);
		break;
	case V_CONTINUOUS:
		set_data_out(&D_POS, V_0_6);
		set_data_out(&D_NEG, V_3_3);
		break;
	// keep previous state
	case V_RESERVE:
		set_data_out(&D_POS, V_3_3);
		set_data_out(&D_NEG, V_3_3);
		break;
	};
	// mode change debuonce time 20-60 ms
	_delay_ms(100);
}

// continuous mode debounce time 100-200 uS
// increment and decrement by 200mV
void increment_qc() {
	set_data_out(&D_POS, V_3_3);
	_delay_us(200);
	set_data_out(&D_POS, V_0_6);
	_delay_us(200);
}

void decrement_qc() {
	set_data_out(&D_NEG, V_0_6);
	_delay_us(200);
	set_data_out(&D_NEG, V_3_3);
	_delay_us(200);
}

void system_sleep() {
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	sleep_mode();
	sleep_disable();
}

int main(void)
{
	init_qc();
	
	set_qc_voltage(V12);
	system_sleep();
	
	//set_qc_voltage(V5);
	//set_qc_voltage(V_CONTINUOUS);
    //while (1) 
    //{	
		//// to go from 5 to 12V in 0.2V increments
		//// 5 increments per volt
		//// 7 volt difference
		//// therefore 35 increments
		//for (int i = 0; i < 35; i++) {
			//increment_qc();
			//_delay_ms(500);
		//}
		// _delay_ms(1000);
		//for (int i = 0; i < 35; i++) {
			//decrement_qc();
			//_delay_ms(500);
		//}
		// _delay_ms(1000);
    //}
}

