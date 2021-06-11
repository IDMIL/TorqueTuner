# TorqueTuner: A self contained module for designing rotary haptic force feedback for digital musical instruments.

TorqueTuner  is  an  embedded  module  that  allows  Digital Musical Instrument (DMI) designers to map sensors to parameters  of  haptic  effects  and  dynamically  modify  rotary force  feedback  in  real-time. It comes with an embedded collection of haptic effects, and a is wireless bi-directional interface through [libmapper](https://github.com/libmapper/libmapper).

[Demonstration video](https://vimeo.com/404592134)  

<div align="center">

<img src="/images/standalone_knob.jpg" height="400" >
<img src="/images/standalone_knob_open.jpg" height="400" >
</div>

The hardware is based on the ESP32 microcontroller and the [mechaduino](https://github.com/jcchurch13/Mechaduino-Hardware) platform, to implement a force feedback rotary encoder with 3600 PPR ~= 0.1 degree resolution that can display forces up to 45 Ncm (63.7oz.in).

## Installation

Clone this repository and all of its submodules:
	
    git clone --recursive https://gitlab.com/idmil/haptics/torquetuner

## libmapper

on MacOs:

    git clone https://github.com/libmapper/libmapper.git
    cd libmapper
    ./autogen.sh
    make
    make install

## libmapper arduino library 
<update to include a release.zip as submodule>
The libmapper arduino library needs to be compiled and moved to the Arduino libraries folder. See [instructions](https://github.com/mathiasbredholt/libmapper_arduino)

## Upload firmware

Upload mechaduino firmware
1. Open software/Mechaduino-firmware/Mechaduino/Mechaduino.ino in arduino  
2. Choose board -> arduino zero  
3. Upload  

Upload ESP32 firmware

1. Install [arduino-ESP32](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md) using boards manager 
2. Open software/TorqueTuner-ESP32/firmware/ESP32/ESP32.ino in arduino 
3. Choose board -> TinyPico 
4. Download TinyPico Arduino Helper (https://github.com/tinypico/tinypico-arduino/tree/master/TinyPICO-Helper) to the Arduino libraries folder, or directly through the Arduino IDE Library Manager.
5. Modify WIFI credentials correspondingly (see around line 14-18)
6. Upload  

## Calibration
Follow [instructions](https://github.com/jcchurch13/Mechaduino-Firmware#calibration-routine) on calibrating the magnetic encoder with the known step angle given. 

## Usage
TODO Write about how to use libmapper, supported audio environments and examples

## Building the hardware

## T-stick integration

<div align="center">

<img src="/images/HapticKnob_tstick_exploded1.png" height="600">
</div>

<div align ="center">

<img src="/images/TorqueTuner+T-stick.png" height="400">
</div>
