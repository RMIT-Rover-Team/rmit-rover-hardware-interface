#include "Motors/gpioMotor_wrapper.hpp"
#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>

/* 
RMIT Rover

Authors:
* [Electrical] Kaelan Grainger

*/


GPIOMotorWrapper::GPIOMotorWrapper(int fwPin, int bwPin, int enPin, int dir){
     printf("Init GPIO Wheel FWPin={}, BWPin={}, ENPin={}\n", fwPin, bwPin, enPin);
     this->fwPin = fwPin;
     this->bwPin = bwPin;
     this->enPin = enPin;
}


void GPIOMotorWrapper::enable(){
    printf("Wheel %i Enable\n", this->wheelID);
}

void GPIOMotorWrapper::disable(){
    printf("Wheel %i Disable\n", this->wheelID);
}

void GPIOMotorWrapper::calibrate(){
    printf("Wheel %i Calibrate\n", this->wheelID);
}

void GPIOMotorWrapper::setTorque(float value) {
    printf("Wheel %i SetTorque %f\n", this->wheelID, value);
    //make some shit up
    this->speed = 200*value;
}

void GPIOMotorWrapper::setSpeed(float value) {
    printf("Wheel %i SetVelocity %f\n", this->wheelID, value);
    this->speed = value;
     //Update the pos with the integral
    this->pos += value;
}

void GPIOMotorWrapper::clearErrors(){
    printf("Wheel %i Clear Error\n", this->wheelID);
}

void GPIOMotorWrapper::estop(){
    printf("Wheel %i ESTOP\n", this->wheelID);
}

float GPIOMotorWrapper::getSpeed(){
    printf("Wheel %i Getspeed %f\n", this->wheelID, this->speed);
    return this->speed;
}

float GPIOMotorWrapper::getPos(){
    printf("Wheel %i Getpos %f\n", this->wheelID, this->pos);
    return this->pos;
}