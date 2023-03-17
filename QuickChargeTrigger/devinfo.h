/*
 * devinfo.h
 *
 * Created: 17/03/2023 6:04:06 PM
 *  Author: William Yang
 *
 * Attiny85 constants
 *
 */ 

#ifndef DEVINFO_H_
#define DEVINFO_H_

// NOTE: We are using the inbuilt 1MHz oscillator since we are not using an external crystal
#ifndef F_CPU
#define F_CPU 1000000
#endif

#endif /* DEVINFO_H_ */