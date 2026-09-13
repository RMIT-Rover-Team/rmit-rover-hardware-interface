#pragma once
#include "drive_motor_wrapper.hpp"



/*
RMIT Rover

Authors:
* [Software] Greg Miles

*/

class SolsticeWrapper: public DriveMotorWrapper {
    private:
        int wheelNum;    
        int en_pin;
        int in1_pin;
        int in2_pin;
        float lastSpeed = 0.0f;
        // TODO(getPos): std::chrono::steady_clock::time_point

    public:
        SolsticeWrapper(int wheel, int EN, int IN1, int IN2);

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