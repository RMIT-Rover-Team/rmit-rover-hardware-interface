#pragma once
#include "Motors/drive_motor_wrapper.hpp"
#include <thread>

/* 
RMIT Rover

Authors:
* [Electrical] Kaelan Grainger

*/


//Function Flags
// IN TestMode, Myactuator or ODrive motors are replaced with FakeMotor objects that print the output of each operation. No CANBUs communications occur
//#define TorqueHandler_TestMode 1


#define RAT_ODRIVE 0
#define EQUINOX_ODRIVE 1
#define EQUNIOX_TWO_MYACTUATOR 2
#define SOLSTICE_MINIROVER 3

//#define ROVER_BUILD_TARGET EQUINOX_TWO_MYACTUATOR
#define ROVER_BUILD_TARGET SOLSTICE_MINIROVER

// The various configurations
#if ROVER_BUILD_TARGET == RAT_ODRIVE

    #define USE_ODRIVE
    #define MotorCount 6
    #define MotorsPerSide 3
    const int MotorIDs[MotorCount] = {1,2,3,4,5,6};
    const int LeftMotorIndexes[MotorsPerSide] = {0,1,2}; //These are indexes of the above motor IDs                    
    const int RightMotorIndexes[MotorsPerSide] = {3,4,5}; //These are indexes of the above motor IDs                   

    const int MotorDirs[MotorCount] = {-1,-1,-1,1,1,1};

#elif ROVER_BUILD_TARGET == EQUINOX_ODRIVE

    #define USE_ODRIVE
    #define MotorCount 4
    #define MotorsPerSide 2
    const int MotorIDs[MotorCount] = {1,2,3,4};
    const int LeftMotorIndexes[MotorsPerSide] = {0,1}; //These are indexes of the above motor IDs                                                  
    const int RightMotorIndexes[MotorsPerSide] = {2,3}; //These are indexes of the above motor IDs                                                 

    const int MotorDirs[MotorCount] = {-1,1,1,-1};

#elif ROVER_BUILD_TARGET == EQUNIOX_TWO_MYACTUATOR

    #define USE_MyActuator
    #define MotorCount 4
    #define MotorsPerSide 2
    const int MotorIDs[MotorCount] = {1,2,3,4};
    const int LeftMotorIndexes[MotorsPerSide] = {0,1}; //These are indexes of the above motor IDs                                                  
    const int RightMotorIndexes[MotorsPerSide] = {2,3}; //These are indexes of the above motor IDs                                                 

    const int MotorDirs[MotorCount] = {-1,-1,1,1};

#elif ROVER_BUILD_TARGET == SOLSTICE_MINIROVER

    #define USE_GPIO
    #define MotorCount 4
    #define MotorsPerSide 2

    const int LeftMotorIndexes[MotorsPerSide] = {1,2}; //These are indexes of the above motor IDs                                                   
    const int RightMotorIndexes[MotorsPerSide] = {0,3}; //These are indexes of the above motor IDs                                                 
    const int GPIO_FWPins[MotorCount] = {15, 18, 13, 19};
    const int GPIO_BWPins[MotorCount] = {14, 17, 12, 16};
    const int GPIO_ENPins[MotorCount] = {3,4,21,20};

    const int MotorDirs[MotorCount] = {1,1,-1,-1};

#endif

#define ControlLoopTimestep 0.1


enum TorqueDriveMode : int
{
    UNLOCKED_VELOCITY = 0x0, // Independent wheels (Regular driving) - Direct Drive
    UNLOCKED_TORQUE   = 0x1, // Deprecated do not use
    LOCKED_VELOCITY   = 0x2, // Traction control in velocity mode, wheels are matched to slowest, best for sand / uneven terrain
};

//Semophore for the Motor wrappers
enum MotorAction: int
{
    CONTROL_LOOP = 0x0,
    CALIBRATE    = 0x1,
    ENABLE       = 0x2,
    DISABLE      = 0x3,
    ESTOP        = 0x4
};

//The return type from Left and Right odom
typedef struct 
{
    float leftPos;
    float rightPos;
    float leftSpeed;
    float rightSpeed;
} OdomReading;


#ifdef TorqueHandler_TestMode
    #include "Motors/fake_wrapper.hpp"
#else
    #ifdef USE_ODrive
        #include "Motors/odrive_wrapper.hpp"
        #include "Motors/odrive_control.hpp"
    #endif
    #ifdef USE_MyActuator
        #include "Motors/MyActuator_wrapper.hpp"
        #include "CanComms/GenericCan.h"
        #include "CanComms/SocketCanWrapper.h"
    #endif
    #ifdef USE_GPIO
        #include "Motors/gpioMotor_wrapper.hpp"
    #endif
    
#endif

class TorqueHandler{
    public:
        TorqueHandler(const char* canInterface);

        void setSpeed(float leftSpeed, float rightSpeed);
        void calibrate();
        void enable();
        void disable();
        void estop();

        void setMode(TorqueDriveMode mode);

        OdomReading getOdom();

        ~TorqueHandler();

    private:
        //The communication handlers
        #ifndef TorqueHandler_TestMode
            //The unified controller for Odrive motors
            #ifdef USE_ODrive
                ODriveController* myController;
            #endif
            //The unified controller for the MyActuator motors
            #ifdef USE_MyActuator
                GenericCan* myCan;
            #endif
        #endif

        //Target Speeds
        volatile float leftSpeed, rightSpeed;

        //Async handler for PID
        std::thread controlThread;

        //The motors on the rover and their Mathematical Models
        DriveMotorWrapper* myMotors[MotorCount];
        
        //Flag for Async instructions
        volatile int actionFlag;

        //The drive mode
        TorqueDriveMode mode;

        //The private control loop stuff
        void controlLoop();
        void handlePID();
        int loopControlFlag;
};
