#pragma once
#include "drive_motor_wrapper.hpp"

/*
RMIT Rover

Authors:
* [Software] Greg Miles

SolsticeWrapper - one DRV8952-driven DC motor on the Solstice mini-rover.

GPIO library: lgpio. Pins are BCM numbers - the GPIO numbers on the
schematic. lgpio uses these directly, no setup call needed.

Each wheel opens its own handle to the GPIO chip in enable() and closes
it in disable(), so nothing outside this class has to set up lgpio.

Build modes:
  SOLSTICE_HW defined     -> real lgpio calls (on the Pi)
  SOLSTICE_HW not defined -> pin actions printed instead
*/

class SolsticeWrapper: public DriveMotorWrapper {
    public:
        // Duty range 0-100. lgpio takes duty as a percentage
        // Public so the torque handler can use it.
        static const int PWM_RANGE = 100;

    private:
        int wheelNum;
        int en_pin;
        int in1_pin;
        int in2_pin;
        int direction;          // +1 normal, -1 reversed (wheel mounted mirrored)
        float lastSpeed = 0.0f; // last COMMANDED speed - there's no encoder
        int chip = -1;          // lgpio handle for the GPIO chip (-1 = not open)

        // Helpers. Every public method talks to hardware ONLY through
        // these. Each has a real (#ifdef SOLSTICE_HW) version and a
        // printf (#else) version in the .cpp.
        //
        //   openChip  -> lgGpiochipOpen     (start of enable())
        //   closeChip -> lgGpiochipClose    (end of disable())
        //   setMode   -> lgGpioClaimOutput / lgGpioClaimInput
        //   writePin  -> lgGpioWrite        (EN pin only - high or low)
        //   startPWM  -> claim + lgTxPwm    (IN pins, in enable())
        //   writePWM  -> lgTxPwm            (IN pins, duty 0..PWM_RANGE)
        //   stopPWM   -> lgTxPwm, freq 0    (IN pins, in disable())
        bool openChip();
        void closeChip();
        bool setMode(int pin, int mode);
        void writePin(int pin, int level);
        bool startPWM(int pin);
        void writePWM(int pin, int duty);
        void stopPWM(int pin);

    public:
        // direction defaults to +1 so existing calls with 4 args still compile.
        SolsticeWrapper(int wheel, int EN, int IN1, int IN2, int dir = 1);

        void enable() override;
        void disable() override;
        void calibrate() override;
        void setTorque(float value) override;
        void setSpeed(float value) override;
        void clearErrors() override;
        void estop() override;
        float getSpeed() override;
        float getPos() override;
};