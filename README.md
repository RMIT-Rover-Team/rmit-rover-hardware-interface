# Mini Rover Hardware Interface
Hardware interface for the Research Mini Rover project

## Installation
The RaspberryPi requires the libgpiod-dev and gpiod packages, to install
```sh
sudo apt update && sudo apt upgrade
sudo apt install gpiod libgpiod-dev
```

This hardware interface uses the WiringPi library, to install:
```sh 
wget https://github.com/WiringPi/WiringPi/releases/download/v3.19/wiringpi_3.19_arm64.deb
sudo apt install ./wiringpi_3.19_arm64.deb
```

The Solstice mini-rover drive (`SolsticeWrapper`) uses the lgpio library instead. It's only needed on the Pi that drives Solstice:
```sh
sudo apt install liblgpio-dev
```
If apt can't find it, build from source:
```sh
wget https://github.com/joan2937/lg/archive/master.zip
unzip master.zip && cd lg-master
make && sudo make install
```
Check it's installed: `ls /usr/include/lgpio.h`

## TODO: Compiling


# Solstice Mini-Rover (SolsticeWrapper)
Drive motor wrapper for the Solstice mini-rover: one DRV8952 channel per wheel, open loop.
- `include/Motors/solstice_wrapper.hpp`, `src/solstice_wrapper.cpp`
- Pins are BCM numbers (the GPIO numbers on the schematic).
- Each wheel opens its own lgpio handle in `enable()` and closes it in `disable()`. No setup is needed in `main()`.

**Two build modes:**
- **Pi (real hardware):** TODO: finish compile notes
- **Anywhere else (e.g. WSL):** build normally. No lgpio needed; pin actions are printed instead of driving hardware.

To test with the torque handler test:
```
cd test/testTorqueHandler
make -B && ./TestTorque
```

# (Websocket)
To Build:
```
bash build_pycontrol.sh
```
To enable virtual CAN for testing:
```
bash SetupVCAN.sh
```
Alternatively, The torque controller can be configured to use fakemotor odrive emulators in header files in include/

To Run:
```
cd dist/
python3 main.py <caninterface>
```
Such as:
```
python3 main.py can0
```


# Direct Drive
Build files under test/
For each test dir:
```
make -j4
./TestDrive <caninterface>
./TestDrive can0
```


# (ROS) RAT3 Diff Drive
RAT3 ROS2 Control Integration and Canbus Hardware Interface for Differential Drive Robot with 6 wheels.

Use the following commands to build and launch with the bringup package.

```
colcon build
ros2 launch rat3_bringup launch_robot.launch.py
```

Important commands for setting up Canbus:

```
sudo ip link set can0 type can bitrate 1000000
sudo ip link set can0 up
sudo ip link set can0 txqueuelen 1000
```

After launch, use the following command for teleop control:

```
ros2 run teleop_twist_keyboard teleop_twist_keyboard -r cmd_vel:=/diff_cont/cmd_vel_unstamped
```