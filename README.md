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

### Program mappings on you computer

### Install Python3
on Linux:

    sudo apt install python3 python-is-python3
    
#### Install libmapper

on MacOs:

    git clone https://github.com/libmapper/libmapper.git
    cd libmapper
    ./autogen.sh
    make
    make install

#### Installing and running webmapper

To install webmapper, you'll need to have installed libmapper before. After that, you'll have to install a few dependencies:
- pyserial (python should have already been installed):   `sudo apt-get install python3-serial`
- liblo:      `sudo apt install liblo-dev`
- swig:       `sudo apt install swig`

If any other installations are required, follow the same instructions, ie. `sudo apt install ...`

Copy `_mapper.cpython-38-x86_64-linux-gnu.so` and `mapper.py` from the directory /libmapper/swig into /webmapper directory.

Now you can run webmapper by running `webmapper.py` from terminal (from inside the /webmapper directory). The terminal will display "serving at port #####", and a browser window should be opened automatically and directed to the correct port.


### Program embedded devices
#### Install libmapper arduino library for ESP32
<update to include a release.zip as submodule>
The libmapper arduino library needs to be compiled and moved to the Arduino libraries folder. See [instructions](https://gitlab.com/idmil/haptics/libmapper-arduino/-/tree/rotarep)

#### Upload firmware

Upload mechaduino firmware
1. Open software/Mechaduino-firmware/Mechaduino/Mechaduino.ino in arduino  
2. Choose board -> arduino zero  
3. Upload  

Upload ESP32 firmware

1. Install [arduino-ESP32](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md) using boards manager
2. In Arduino IDE, go to preferences, click under "More preferences cab be edited directlyh in the file".   packages>esp32>hardware>esp32>1.0.6>tools>sdk>include>lwip>arpa and access the inet.h file. Apply the following modifications:
change `#include "../../../lwip/src/include/lwip/inet.h"` to `#include "lwip/inet.h"` .
3. Install Pyserial (if not already installed): `sudo apt-get install python3-serial` 
4. Open software/TorqueTuner-ESP32/firmware/ESP32/ESP32.ino in arduino
5. Change part of the code as displayed [here](https://gitlab.com/idmil/haptics/TorqueTuner-ESP32/-/commit/84bfbb211ef643d7faa20d303f8e1e38cba5f807) 
6. Choose board -> TinyPico 
7. Download TinyPico Arduino Helper (https://github.com/tinypico/tinypico-arduino/tree/master/TinyPICO-Helper) to the Arduino libraries folder, or directly through the Arduino IDE Library Manager.
8. Run `update-library.sh`  in the libmapper directory. (torquetuner>software>libmapper_arduino)
9. Modify WIFI credentials correspondingly (see around line 15-20, edit SSID and PASSWORD)
10. Upload. *(in case of error: _selected serial port could not open port /dev/ttyUSB0_, make sure that access to USB is not restricted. To solve this, run `sudo usermod -a -G dialout $USER` and restart).

#### Calibrate
Follow [instructions](https://github.com/jcchurch13/Mechaduino-Firmware#calibration-routine) on calibrating the magnetic encoder with the known step angle given. 



#### Install SuperCollider

```
sudo apt install supercollider supercollider-ide
```
#### Install MapperUGen


```
sudo apt install supercollider-dev
```

in `software/MapperUGen`:
```
mkdir -p build
cd build
cmake -DSUPERNOVA=ON -DSC_PATH=/usr/include/SuperCollider -DCMAKE_INSTALL_PREFIX=/usr ..
make
sudo checkinstall --pkgname=MapperUGen --pkgversion=0.0.1 --pkgrelease=0 --requires="libmapper-dev" --exclude="/home" -y
```

#### Load demos

Load [demos](demos) and do mappings manually with `scide` (supercollider-ide).

_General Tips for testing demos:_

- When creating a signal in superCollider, if they don't appear in the Mapper GUI, they might appear after relaunching the Mapper GUI.
 
- When the motor goes off at uncontrollable speed, mapping "angle" to "Damping" generally works to bring it back to still.

##### **1. Turntable**

_A turntable-like application, where playback can be stopped by stalling the motor, and scratching can be performed by twisting it back and forth._

A. Mapping `velocity` to `playback speed`, in haptic mode : Spin.

- Upload this [.json file](https://gitlab.com/idmil/haptics/torquetuner/-/blob/Rotarep/demos/Turntable.json) into SuperCollider.

- In this example, a .wav file sampling a couple of drums should play at a playback speed that varies with the velocity of the knob applied by the user. When the knob is turned clockwise by the user, you should observe an increase in the speed at which the drums.wav file is played. Similarly, when the knob is turned counterclockwise, you should observe an increase in the speed at which the drums.wav file is played in reverse playback.

##### **2. PitchWheel**

_An application reminiscent of the pitch wheel common in many keyboard synthesizers. When twisting, TorqueTuner displays an exponentially increasing force, when released it returns to idle._

A. Mapping `angle` to the `Filter Cutoff` frequency, in haptic mode : Spring.

- Upload this [.json file](https://gitlab.com/idmil/haptics/torquetuner/-/blob/Rotarep/demos/Pitchwheel_a.json) into SuperCollider.

- In this example, the user turns the knob to observe the effect of sweeping the Cutoff frequency of a filter. The user should observe an angle at which there is a discontinuity, where the cutoff frequency goes back to its minimum value, right after having reached it maximum value (and vice versa).

B. Adding the mapping between `speed` and `resonance` to demo 2.A, in haptic mode : Spring.

- Upload this [.json file](https://gitlab.com/idmil/haptics/torquetuner/-/blob/Rotarep/demos/Pitchwheel_b.json) into SuperCollider.

- Similarly to demo 2.a, the user turns the knob to observe the effect of sweeping the Cutoff frequency of a filter. The difference from demo 2.a, is that the user should notice the addition of resonance, whose intensity varies with the speed applied on the knob. The best way to observe this behaviour is to turn the knob back and forth at varying speed. 

##### **3. String-Plucker**

_Detents serve as haptic feedback on virtual string plucking, and twisting in-between detents produce vibrato-like modulation._

A. Mapping `trigger` to `excitation`, in haptic mode : Detents.

- Upload this [.json file](https://gitlab.com/idmil/haptics/torquetuner/-/blob/Rotarep/demos/string-plucker_a.json) into SuperCollider.

- In this configuration, the user should experience a similar sensation to plucking a string, where turning the knob will trigger the sound of an individual string being excited by a plucking, at each detent.

B. Adding the mapping between `angle` and `frequency` to demo 3.A, in haptic mode : Detents.

- Upload this [.json file](https://gitlab.com/idmil/haptics/torquetuner/-/blob/Rotarep/demos/string-plucker_b.json) into SuperCollider.

- In this configuration, the user should experience the same sensation as in demo 3.a, only this time the frequency of the note being played will change progressively with the angle of the knob. As a result, the user should observe a progressive increase in the frequency of the note as the knob is turned clock-wise. The user should observe an angle at which there is a frequency discontinuity, where the frequency goes back to its minimum value, right after having reached it maximum value (and vice versa). 




### Generate documentation (optional)

Install graphviz: `sudo apt install graphviz`

Install doxygen: `sudo apt install doxygen`

Open a terminal in `torquetuner\software`:
```
doxygen TorqueTuner.doxygen
```

(`TorqueTuner.doxygen` initially generated by: `doxygen -g TorqueTuner.doxygen` then adapted).

Then explore with a web browser the generated documentation, for example: 
- Class Hierarchy: [software/html/inherits.html](software/html/inherits.html)
- Class Index: [software/html/classes.html](software/html/classes.html)
-  TorqueTuner Class Reference: [software/html/classTorqueTuner.html](software/html/classTorqueTuner.html)


## Usage
TODO Write about how to use libmapper, supported audio environments and examples

Note that the ESP32 board needs a USB cable connected for power supply (also useful for flashing with Arduino IDE and debugging with serial monitor.

## Choice of Motor-Controller system to replace the Mechaduino: uStepperS vs. Moteus r4.5
Context, background, things to keep in mind:      
The TorqueTuner was originally designed and implemented using the Mechaduino system (out of stock, as of August 2021). The designed firmware for the Mechaduino implementation made use of torque control to control the motor, and relied on the Arduino IDE. 

The system to replace the Mechaduino will be chosen according to a couple of factors such as, but not limited to: adaptibility to the previously designed firmware (intended for Mechaduino), price, and availability.

#### uStepperS
_Price main components: motor (EUR 15) + board inc. mounting, magnetic encoder, heat sink (EUR 47) = EUR 62_       
_Price accessories: 12V 2A Power Supply (EUR 12) = EUR 12_

_Price developer kit: EUR 96_

**PROS:**
- Uses the Arduino IDE;
- Compact in size, would be easier to physically integrate with the T-stick;
- Cheapest option.

**CONS:**
- No torque control. uStepperS is mainly designed for position or velocity control, so the firmware requires some fair modifications and could perhaps resurface problems in the future?
- Stepper motor, can sense clogging.

#### Moteus r4.5
_Price main components: motor (USD 55) + board (USD 84) = USD 139_

_Price developer kit: USD 174_

**PROS:**
- Uses Torque control;
- Brushless motor, less clogging.

**CONS:**
- Has it's own GUI, not directly compatible with Arduino IDE, requires some work around;
- Expensive.



## Building the hardware

## T-stick integration

<div align="center">

<img src="/images/HapticKnob_tstick_exploded1.png" height="600">
</div>

<div align ="center">

<img src="/images/TorqueTuner+T-stick.png" height="400">
</div>
