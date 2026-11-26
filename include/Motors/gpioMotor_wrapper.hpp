#pragma once
#include "drive_motor_wrapper.hpp"

/* 
RMIT Rover

Authors:
* [Electrical] Kaelan Grainger

*/


class GPIOMotorWrapper: public  DriveMotorWrapper {
    private:
        int fwPin;
        int bwPin;
        int enPin;
        int dir;

        double speed;
        double pos;

        double lastTime;

    public:
        //We need to share the controller with other instances
        GPIOMotorWrapper(int fwPin, int bwPin, int enPin, int dir);


        void enable() override;
        void disable() override;
        void calibrate() override;
        void setTorque(float value)  override;
        void setSpeed(float value)  override;
        void clearErrors() override;
        void estop() override;
        float getSpeed() override;
        float getPos() override;
};
