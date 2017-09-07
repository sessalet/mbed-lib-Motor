#ifndef MOTOR_H
#define MOTOR_H

#include "mbed.h"
#include "platform.h"
#include "PwmOutResetSync.h"

class Motor
{

	public:
	Motor(PinName in1, PinName in2, PinName pwm);
	void drive(int speed);
	void stop(void);

	private:

	protected:
	DigitalOut _in1;
	DigitalOut _in2;
	PwmOutResetSync _pwm;
};

#endif
