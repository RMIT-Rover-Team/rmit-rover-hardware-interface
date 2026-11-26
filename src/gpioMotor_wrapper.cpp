#include "Motors/gpioMotor_wrapper.hpp"
#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>
#include <chrono>

/* 
RMIT Rover

Authors:
* [Electrical] Kaelan Grainger

*/

double monotonic_ms()
{
    using namespace std::chrono;

    return duration<double, std::milli>(
        steady_clock::now().time_since_epoch()
    ).count();
}


GPIOMotorWrapper::GPIOMotorWrapper(int fwPin, int bwPin, int enPin, int dir){
     printf("Init GPIO Wheel FWPin={}, BWPin={}, ENPin={}\n", fwPin, bwPin, enPin);
     this->fwPin = fwPin;
     this->bwPin = bwPin;
     this->enPin = enPin;
     this->dir = dir;

     this->speed = 0;
     this->pos = 0;

     if (wiringPiSetupGpio() == -1){
            std::cerr << "Failed to initialize wiringPi\n";
    }

     // Set the Pinmodes
     pinMode(enPin, OUTPUT);
     digitalWrite(enPin, LOW);

     softPwmCreate(fwPin, 0, 100);
     softPwmCreate(bwPin, 0, 100);

     softPwmWrite(fwPin, 0);
     softPwmWrite(bwPin, 0);

     this->lastTime = monotonic_ms();

}


void GPIOMotorWrapper::enable(){
    printf("Wheel %i Enable\n", this->fwPin);
    digitalWrite(enPin, HIGH);
}

void GPIOMotorWrapper::disable(){
    printf("Wheel %i Disable\n", this->fwPin);
    digitalWrite(enPin, LOW);
}

void GPIOMotorWrapper::calibrate(){
    printf("Wheel %i Calibrate\n", this->fwPin);
}

void GPIOMotorWrapper::setTorque(float value) {
    printf("Wheel %i SetTorque %f\n", this->fwPin, value);
    //make some shit up
    
}

void GPIOMotorWrapper::setSpeed(float value) {
    printf("Wheel %i SetVelocity %f\n", this->fwPin, value);
    
     //Update the pos with the integral
    
    this->pos += this->speed * ((monotonic_ms() - this->lastTime) / 1000);

    this->lastTime = monotonic_ms();

    this->speed = value;

    value = value * this->dir;

    // Write PWM
    if (value >= 0){
        softPwmWrite(fwPin, (int)value);
        softPwmWrite(bwPin, 0);
    }
    else{
        softPwmWrite(fwPin, 0);
        softPwmWrite(bwPin, -1 * (int)value);
    }

}

void GPIOMotorWrapper::clearErrors(){
    printf("Wheel %i Clear Error\n", this->fwPin);
}

void GPIOMotorWrapper::estop(){
    printf("Wheel %i ESTOP\n", this->fwPin);
}

float GPIOMotorWrapper::getSpeed(){
    printf("Wheel %i Getspeed %f\n", this->fwPin, this->speed);
    this->pos += this->speed * ((monotonic_ms() - this->lastTime) / 1000);

    this->lastTime = monotonic_ms();
    return this->speed * this->dir;
}

float GPIOMotorWrapper::getPos(){
    printf("Wheel %i Getpos %f\n", this->fwPin, this->pos);
    this->pos += this->speed * ((monotonic_ms() - this->lastTime) / 1000);

    this->lastTime = monotonic_ms();
    return this->pos * this->dir;
}
