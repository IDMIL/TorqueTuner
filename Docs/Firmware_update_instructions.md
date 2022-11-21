# Torquetuner Firmware Update Guide (firmware version: 20221020)

- [Torquetuner (Firmware version: 20221020)](#torquetuner-firmware-update-guide-firmware-version-20221020)
  - [First time firmware upload instructions](#first-time-firmware-upload-instructions)
    - [Flash Moteus Board](#flash-moteus-board)
      - [Recompiling the Moteus firmware](#recompiling-the-moteus-firmware)
      - [Setup flash with debug USB stick:](#setup-flash-with-debug-usb-stick)
    - [Flash TinyPico Board](#flash-tinypico-board)
      - [Install PlatformIO](#install-platformio)
      - [Clone the Torquetuner repository](#clone-the-torquetuner-repository)
      - [Open software project and flash it to the Torquetuner](#open-software-project-and-flash-it-to-the-torquetuner)
      - [Test Torquetuner](#test-torquetuner)
  - [Other Documentation](#other-documentation)
  - [Firmware information](#firmware-information)

## First time firmware upload instructions
[Modified from the T-Stick firmware update instructions](https://github.com/aburt2/T-Stick/blob/master/Docs/Firmware_update_instructions.md)
_INSTALL ALL DEPENDENCIES AND REAL ALL OBSERVATIONS BEFORE UPLOAD !_
### Flash Moteus Board

#### Recompiling the Moteus firmware:
1. Install [OpenOCD](https://openocd.org/pages/getting-openocd.html) on your device.
2. Clone moteus: https://github.com/IDMIL/Moteus/tree/RotaRep branch.
3. In the `moteus` directory, build the moteus firmware:
```
tools/bazel build --config=target //:target --verbose_failures
```
4. (optional) test `tools/bazel test --config=target //:target`

#### Setup flash with debug USB stick:

5. Connect the power cable to power input of the controller (with the power supply unplugged from the wall).
6. Connect the canfd connector to the controller.
7. Connect the fdcanusb to the usb port of the PC.
8. You may now plug the power supply into the wall.
9. A green light on the controller should now appear.
10. Connect the USB stick to the board using the JST ZH-6 and plug in the USB stick into the computer.

11. Flash with debug USB stick: `./fw/flash.py`

Upon succesful flash, the following lines will be output:
```
** Programming Finished **
** Verify Started **
** Verified OK **
** Resetting Target **
...
shutdown command invoked
```

Note: if you encounter the error message:
```
Info : STLINK V2J29S7 (API v2) VID:PID 0483:3748
Info : Target voltage: 3.223809
Error: init mode failed (unable to connect to the target)
```
check the different connections related to the power supply.

10. Install tview using pip: `pip install moteus-gui`
11. Launch Moteus GUI: `python -m moteus_gui.tview --devices=1`
12. In the Moteus GUI, check the I2C state and status:
```
d i2c.state
d i2c.status
```
For now state should be READY or LISTEN and status ERROR or BUSY (with no other i2c device connected).

### Flash TinyPico Board

#### Install PlatformIO

1. To download and install PlatformIO, follow the instructions at [https://platformio.org/platformio-ide](https://platformio.org/platformio-ide). We recomment using PlatformIO under Visual Studio Code, but you can also choose another editor.

#### Clone the Torquetuner repository

2. Clone this repository using `git clone https://github.com/IDMIL/TorqueTuner.git`. Alternatively, you can download the repository as a zip file at [https://github.com/IDMIL/TorqueTuner](https://github.com/IDMIL/TorqueTunerk). Take note of the folder location.

#### Open software project and flash it to the Torquetuner

3. Open the Torquetuner **software** folder in VSC/PlatformIO. You can get help on how to use PlatformIO at [https://docs.platformio.org/en/latest/core/quickstart.html](https://docs.platformio.org/en/latest/core/quickstart.html)
4. Open the `config.json` file in the Data folder to make changes before uploading.
5. If it is the first time flashing, you may see an error pointing to the ESP32 inet.h file. The file requires manual fixing. Check the issue at [https://github.com/mathiasbredholt/libmapper-arduino/issues/3](https://github.com/mathiasbredholt/libmapper-arduino/issues/3)

When ready, you need to flash both the firmware and the filesystem image. Choose the proper platform accordingly (*tinypico*) and use the PlatformIO menu to flash the image to the Torquetuner.

#### Test Torquetuner

After flashing, you can use the VSC/PlatformIO serial monitor to check if the Torquetuner module is booting properly. You should see Torquetuner booting process.

You can also interact with the controller using the following commands:

- 'reboot' to reboot

## Other Documentation

[Torquetuner connection guide – v1.0](./connection_guide_v1.md)

[How to build a Torquetuner Module](./Build_guide_v1.md)

## Firmware information

Torquetuner V1 - TinyPico - USB - WiFi
Input Devices and Music Interaction Laboratory (IDMIL)  
