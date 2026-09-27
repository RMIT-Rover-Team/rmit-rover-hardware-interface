#include "Motors/solstice_wrapper.hpp"
#include <cstdio>       // printf
#include <algorithm>    // std::min, std::max 
#include <cmath>        // std::fabs           

/*
RMIT Rover

Authors:
* [Software] Greg Miles

*/

#ifdef SOLSTICE_HW
    #include <lgpio.h>
    namespace {
        const int   GPIOCHIP_DEV = 0;        // /dev/gpiochip0
        const float PWM_FREQ_HZ  = 1000.0f;  // starting point - tune on blocks
    }
#endif

// Namespace for pin modes. lgpio doesn't have INPUT/OUTPUT constants
namespace {
    const int INPUT  = 0;
    const int OUTPUT = 1;
}


// Constructor: Stores config only, doesn't touch pins.
SolsticeWrapper::SolsticeWrapper(int wheel, int EN, int IN1, int IN2, int dir)
    : wheelNum(wheel), en_pin(EN), in1_pin(IN1), in2_pin(IN2),
      direction(dir >= 0 ? 1 : -1)
{
    printf("Init Solstice Wheel %i (EN=%i, IN1=%i, IN2=%i, dir=%i)\n",
           wheel, EN, IN1, IN2, this->direction);
}


// Chip handle
// Every lgpio function returns a negative number on failure, and
// lguErrorText() turns that number into a readable message
bool SolsticeWrapper::openChip(){
#ifdef SOLSTICE_HW
    chip = lgGpiochipOpen(GPIOCHIP_DEV);
    if (chip < 0) {
        printf("Wheel %i: can't open gpiochip%i: %s\n",
               wheelNum, GPIOCHIP_DEV, lguErrorText(chip));
        return false;
    }
#else
    chip = 0;
#endif
    return true;
}

void SolsticeWrapper::closeChip(){
#ifdef SOLSTICE_HW
    if (chip >= 0) lgGpiochipClose(chip);
#endif
    chip = -1;
}


// Pin helpers
bool SolsticeWrapper::setMode(int pin, int mode){
#ifdef SOLSTICE_HW
    int errorCode;
    lgGpioFree(chip, pin);
    if (mode == OUTPUT) {
        errorCode = lgGpioClaimOutput(chip, 0, pin, 0);
        if(errorCode < 0) {
            printf("Error on Wheel %i: pin %i: %s\n",
                wheelNum, pin, lguErrorText(errorCode));
            return false;
        }
    }
    else {
        errorCode = lgGpioClaimInput(chip, 0, pin);
        if(errorCode < 0) {
            printf("Error on Wheel %i: pin %i: %s\n",
                wheelNum, pin, lguErrorText(errorCode));
            return false;
        }
    }
#else
    printf("Wheel %i: pin %i -> %s\n",
        wheelNum, pin, mode == OUTPUT ? "OUTPUT" : "INPUT");
#endif
    return true;
}

void SolsticeWrapper::writePin(int pin, int level){
#ifdef SOLSTICE_HW
    int errorCode = lgGpioWrite(chip, pin, level);
    if(errorCode < 0) {
        printf("Error on Wheel %i: pin %i: %s\n",
            wheelNum, pin, lguErrorText(errorCode));
    }
#else
    printf("Wheel %i: write pin %i -> %i\n", wheelNum, pin, level);
#endif
}

bool SolsticeWrapper::startPWM(int pin){
#ifdef SOLSTICE_HW
    int errorCode = lgGpioClaimOutput(chip, 0, pin, 0);
    if(errorCode < 0) {
        printf("Error on Wheel %i: pin %i: %s\n",
            wheelNum, pin, lguErrorText(errorCode));
        return false;
    }
    errorCode = lgTxPwm(chip, pin, PWM_FREQ_HZ, 0, 0, 0);
    if(errorCode < 0) {
        printf("Error on Wheel %i: pin %i: %s\n",
            wheelNum, pin, lguErrorText(errorCode));
        return false;
    }
#else
    printf("Wheel %i: start PWM on pin %i\n", wheelNum, pin);
#endif
    return true;
}

void SolsticeWrapper::writePWM(int pin, int duty){
#ifdef SOLSTICE_HW
    int errorCode = lgTxPwm(chip, pin, PWM_FREQ_HZ, duty, 0, 0);
    if(errorCode < 0) {
        printf("Error on Wheel %i: pin %i: %s\n",
            wheelNum, pin, lguErrorText(errorCode));
    }
#else
    printf("Wheel %i: PWM pin %i -> %i%%\n", wheelNum, pin, duty);
#endif
}

void SolsticeWrapper::stopPWM(int pin){
#ifdef SOLSTICE_HW
    int errorCode = lgTxPwm(chip, pin, 0, 0, 0, 0);
    if(errorCode < 0) {
        printf("Error on Wheel %i: pin %i: %s\n",
            wheelNum, pin, lguErrorText(errorCode));
    }
#else
    printf("Wheel %i: stop PWM on pin %i\n", wheelNum, pin);
#endif
}


// enable()
// Open the chip, IN pins idle (coast) FIRST, then EN high.
void SolsticeWrapper::enable(){
    printf("Wheel %i Enable\n", wheelNum);

    if (!openChip()) return;   // no chip = nothing else can work

    if(!startPWM(in1_pin)) { closeChip(); return; }
    if(!startPWM(in2_pin)) { closeChip(); return; }
    if(!setMode(en_pin, OUTPUT)) { closeChip(); return; }
    writePin(en_pin, 1);
}


// disable()  - reverse of enable()
void SolsticeWrapper::disable(){
    printf("Wheel %i Disable\n", wheelNum);

    // Coast: both IN duties to 0
    writePWM(in1_pin, 0);
    writePWM(in2_pin, 0);
    // Driver off: EN low
    writePin(en_pin, 0);
    // Release pins: stop PWM on both IN pins, then all three to INPUT
    stopPWM(in1_pin);
    stopPWM(in2_pin);
    setMode(in1_pin, INPUT);
    setMode(in2_pin, INPUT);
    setMode(en_pin, INPUT);
    // Set lastSpeed to 0
    this->lastSpeed = 0;
    closeChip();   // last: hand the chip back
}


// setSpeed()
void SolsticeWrapper::setSpeed(float value){
    // Clamp value to [-1, 1]
    float clampedSpeed = std::max(-1.0f, std::min(value, 1.0f));
    // Set lastSpeed
    this->lastSpeed = clampedSpeed;
    // Apply direction
    float motorVelocity = clampedSpeed * this->direction;
    // Convert to duty
    int duty = static_cast<int>(std::fabs(motorVelocity) * PWM_RANGE);
    // Set pins relative to truth table (0 first, then duty)
    if (motorVelocity > 0) {
        writePWM(in2_pin, 0);
        writePWM(in1_pin, duty);
    }
    else if (motorVelocity < 0) {
        writePWM(in1_pin, 0);
        writePWM(in2_pin, duty);
    }
    else{
        writePWM(in1_pin, 0);
        writePWM(in2_pin, 0);
    }
    // NOTE: setSpeed called before enable?
}


// estop()
void SolsticeWrapper::estop(){
    writePWM(in1_pin, PWM_RANGE);
    writePWM(in2_pin, PWM_RANGE);
    this->lastSpeed = 0;
    // NOTE: Should this stay braked, or brake and disable?
    // NOTE: Does GPIO27 cut power? Is this a second layer
    // or does this need to trigger GPIO27?
}


// getSpeed()
// This is the commanded speed, not the measured speed
float SolsticeWrapper::getSpeed(){
    return lastSpeed;
}

// Extras
void SolsticeWrapper::calibrate(){
    printf("Wheel %i: No encoder to calibrate\n",
        wheelNum);
}

void SolsticeWrapper::clearErrors(){
    printf("Wheel %i: Nothing to clear\n",
    wheelNum);
    // NOTE: Check if nFAULT is wired to the Pi
}

void SolsticeWrapper::setTorque(float value){
    setSpeed(value); // Open loop: no torque control, so treat torque as speed.
}

float SolsticeWrapper::getPos(){
    return 0.0f;
}