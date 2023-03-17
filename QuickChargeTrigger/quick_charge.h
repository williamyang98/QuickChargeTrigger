/*
 * quick_charge.h
 *
 * Created: 17/03/2023 6:01:25 PM
 *  Author: William Yang
 *
 * Quick charge functions
 *
 */ 

#ifndef QUICK_CHARGE_H_
#define QUICK_CHARGE_H_

#include "devinfo.h"
#include <util/delay.h>

// Set a pin high or low
struct Pin {
	volatile uint8_t *PORTx;
	uint8_t PORTxn;
	volatile uint8_t *PINx;
	uint8_t PINxn;
	volatile uint8_t *DDRx;
	uint8_t DDxn;
};

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

// Each data pin is connected to two GPIO pins
// These are connected by a 2.2k and 10k resistor
// With a 3.3V VCC we can create a 0V, 0.6V, 3.3V or floating voltage by setting each GPIO individually
struct DataOut {
	struct Pin high;	// Connected to 50k
	struct Pin low;		// Connected to 10k
};

enum DATA_VOLTAGE { V_FLOAT, V_0, V_0_6, V_3_3 };

void set_data_out(struct DataOut *p, enum DATA_VOLTAGE v) {
	switch (v) {
	case V_FLOAT:
		set_pin(&(p->high), FLOAT);
		set_pin(&(p->low), FLOAT);
		break;
	case V_0:
		set_pin(&(p->high), LOW);
		set_pin(&(p->low), LOW);
		break;
	case V_0_6:
		// 3.3V * 2.2/(2.2+10) = 0.6V
		set_pin(&(p->high), HIGH);
		set_pin(&(p->low), LOW);
		break;
	case V_3_3:
		set_pin(&(p->high), HIGH);
		set_pin(&(p->low), HIGH);
		break;
	default:
		break;
	};
};

// Quick charge protocol sets voltages on the data+ and data- USB pins
// These act like commands to the power source
struct QuickCharge {
	struct DataOut data_pos;
	struct DataOut data_neg;
};

enum QC_VOLTAGE { V5, V9, V12, V20, V_CONTINUOUS };

// Source: docs/quick_charge_3_datasheet.pdf
// Table 3: QC 2.0 control table
// Inform power source that we are quick charge enabled
void init_qc(struct QuickCharge* qc) {
	set_data_out(&(qc->data_pos), V_0_6);
	set_data_out(&(qc->data_neg), V_FLOAT);
	// Minimum initialization time of 1.0-1.4 s
	_delay_ms(1500);
}

// Source: docs/quick_charge_3_datasheet.pdf
// Section: The principle of QC 3.0 charging
// Table 3: QC 3.0 control table
void set_qc_voltage(struct QuickCharge* qc, enum QC_VOLTAGE v) {
	switch (v) {
	case V5:
		set_data_out(&(qc->data_pos), V_0_6);
		set_data_out(&(qc->data_neg), V_0);
		break;
	case V9:
		set_data_out(&(qc->data_pos), V_3_3);
		set_data_out(&(qc->data_neg), V_0_6);
		break;
	case V12:
		set_data_out(&(qc->data_pos), V_0_6);
		set_data_out(&(qc->data_neg), V_0_6);
		break;
	case V20:
		set_data_out(&(qc->data_pos), V_3_3);
		set_data_out(&(qc->data_neg), V_3_3);
		break;
	case V_CONTINUOUS:
		set_data_out(&(qc->data_pos), V_0_6);
		set_data_out(&(qc->data_neg), V_3_3);
		break;
	default:
		break;
	};
	// Minimum wait time of 20-60 ms
	_delay_ms(100);
}

// Source: docs/quick_charge_3_datasheet.pdf
// Section: The principle of QC 3.0 charging
// Figure 4 : Quick Charge 3.0 Ramp Up 3.2V and Ramp Down 3.2V
// When in continuous mode increment and decrement by 200mV by pulsing the data- or data+ pins
// The pulse length has to be at least 200ms in duration
// Pulse data+ pin to increase by 0.2V
void increment_qc(struct QuickCharge* qc) {
	set_data_out(&(qc->data_pos), V_3_3);
	_delay_us(200);
	set_data_out(&(qc->data_pos), V_0_6);
	_delay_us(200);
}
// Pulse data- pin to decrease by 0.2V
void decrement_qc(struct QuickCharge* qc) {
	set_data_out(&(qc->data_neg), V_0_6);
	_delay_us(200);
	set_data_out(&(qc->data_neg), V_3_3);
	_delay_us(200);
}


#endif /* QUICK_CHARGE_H_ */