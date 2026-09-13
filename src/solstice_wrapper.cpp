#include "Motors/solstice_wrapper.hpp"
#include <iostream>
//#include <pigpio.h>

/*
RMIT Rover

Authors:
* [Software] Greg Miles

*/


SolsticeWrapper::SolsticeWrapper(int wheel, int EN, int IN1, int IN2)
    : wheelNum(wheel), en_pin(EN), in1_pin(IN1), in2_pin(IN2)
{
    printf("Init Solstice Wheel %i (EN=%i, IN1=%i, IN2=%i)\n", wheel, EN, IN1, IN2);
}


void SolsticeWrapper::enable(){
    printf("Wheel %i Enable\n", this->wheelNum);
    printf("Set EN, IN1, IN2 to output\n");
    printf("Write IN1, IN2 to 0\n");
    printf("Write EN to high\n");
}

void SolsticeWrapper::disable(){
    printf("Wheel %i Disable\n", this->wheelNum);
    printf("Write IN1, IN2 to 0\n");
    printf("Write EN to low\n");
    printf("Set EN, IN1, IN2 to input\n");
}

void SolsticeWrapper::calibrate(){
    /* TODO */
}

void SolsticeWrapper::setTorque(float value){
    /* TODO */
}

void SolsticeWrapper::setSpeed(float value){
    /* TODO */
}

void SolsticeWrapper::clearErrors(){
    /* TODO */
}

void SolsticeWrapper::estop(){
    /* TODO */
}

float SolsticeWrapper::getSpeed(){
    /* TODO */
    return lastSpeed;
}

float SolsticeWrapper::getPos(){
    /* TODO */
    return 0.0f;
}