#include "../Motor/Motor.h"

Motor::Motor(PinName in1, PinName in2, PinName pwm) : _in1(in1), _in2(in2), _pwm(pwm){
}

void Motor::drive(int speed){

	if( speed >= 0 ) {
		// forward
		_in1 = 0;
		_in2 = 1;
		_pwm.write((float)speed);
	} else {
		// reverse
		_in1 = 1;
		_in2 = 0;
		_pwm.write((float)-speed);
	}
}

void Motor::stop(void){
	_in1 = 0;
	_in2 = 0;
	_pwm.write((float)0);
}
