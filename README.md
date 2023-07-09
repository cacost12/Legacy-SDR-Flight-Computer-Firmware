<h1>Full-Feature Flight Computer Firmware</h1>

<p>PCB: Full-Feature Flight Computer A0002</p>
<p>MCU: STM32H750VBT6 </p>
<p>MPU Architecture: ARM Cortex-M7</p>

<p>
Description: The full feature flight computer is a complete solution for parachute ejection and data logging of high power rocketry flights. The computer is equipped with the base flight computer features such as a barometric pressure sensor for measuring altitude, two pyro ports for dual-deploy recovery, and a USB interface for programming. In addition to these base features, the full feature flight computer contains an IMU for full state measurement using an accelerometer, gyroscope, and magnetometer. The board also contains a high-G accelerometer in order to measure high accelerations during the rocket boost phase. To support staging and redundant ejection setups, the board is equipped with two auxiliary pyro ports that are software configurable.
</p>

<h2>Working Directory Structure</h2>

<p>
app: application code for the flight computer containing source directories

auto: auto-generated code from STM32CubeMX (not compiled into application)

init: Microcontroller initialization and configuration code

lib: third-party libraries for device drivers and middleware, microcontroller pin and peripheral configurations 

mod: Hardware modules containing hardware specific code for SDR boards

test: Test code
</p>

<h2>Source Directories:</h2>
<p>
blink:blinks status LED to test programmer and board setup 

data-logger: firmware to collect data during test flights 

dual-deploy: firmware for dual-deploy parachute recovery 

terminal: firmware to allow terminal access to all PCB hardware 
</p>
