# Building a Torquetuner Module (v1.0)

## Bill of Materials

Qty   | Description
------|---------------------
01    | [Moteus Development Kit](https://shop.bela.io/products/trill-craft/)     
01    | [TinyPICO](https://www.adafruit.com/product/4335)        
01    | [Adafruit LCD I2C Shield Kit](https://www.adafruit.com/product/714)
01    | [5V Step up Regulator](https://www.robotshop.com/ca/en/5v-1a-step-up-step-down-voltage-regulator-s13v10f5.html)
01    | [JST - ZHR 4 pin cable ](https://www.robotshop.com/ca/en/breadboard-to-jst-zhr-cable-4-pin-x-15mm-pitch.html)
08    | M2 Screws
24    | 12x1.5mm Magnets

### Printing and preparing the Parts

Print one of each of the [3d printing files](../3D_printing/). Unscrew the Moteus board from its stand.
#### Firmware

Instructions to upload (flash) the Torquetuner and the Moteus board firmware and configuration files can be found [here](./Firmware_update_instructions.md).

Read and follow __ALL__ the instructions at before upload it to the board. There are some steps to prepare your machine and the ESP32 before flash it for the first time.

### Glueing Magnets

Glue magnets to the bottom of the electronics case and the top of the PSU case. These magnets need to be able to connect together.

![magnetsoncase](/images/magnets.jpg "magnets on the bottom of case")
Figure 1: Magnets on the power supply and electronics case
### Soldering Components
![Overview](/images/torquetuner_overview.jpg "3d printing overview")
Figure 2: All the electronic components

1. Start with the Moteus board. 
2. Solder a wire from a wire from the 3V pin to the VIN pin on the regulator. 
3. Screw the moteus board onto the top plate of the electronics case.

![moteusoncase](/images/moteus_onplate.jpg "moteus board on case")
Figure 3: Moteus board screwed onto the top plate of the electronics case.

4. Assemble the Adafruit LCD I2C Shield according to the [instructions](https://learn.adafruit.com/rgb-lcd-shield/assembly) given on the adafruit website. If your M2 Screws are too short, you can screw the shield onto the case before soldering the LCD as shown in figure 4.

![finisedLCD](/images/assembled_TopPlate.jpg "lcd and moteus board on case")
Figure 4: Moteus board and Adafruit LCD I2C shield screwed onto the top plate of the electronics case.

5. Connect the JST-ZHR cable to the moteus board. The pins from the JST ZH cable (from left-to-right, that is starting closest to the ABS label on the board) are:
3.3V - SCL - SDA - GND
6. Solder the 3.3V pin to the 3.3V pin on the TinyPico. 
7. Twist another wire onto the SCL, SDA and GND wires. We will use solder these extra wires onto the LCD shield
8. Solder wires for SDA, SCL, GND and 5V as shown the __Detached Usage__ section on the [Adafruit product page](https://learn.adafruit.com/rgb-lcd-shield/using-the-rgb-lcd-shield).
9. Solder the 5V wire to the VOUT pin on the 5V regulator
10. Solder the a wire from the GND pin of the 5V regulator to the other ground pin on the TinyPico. 
11. Solder the SDA, SCL and GND wires coming from the Moteus board and the LCD Shield onto pins 21, 22 and GND respectively on the TinyPico.
12. Slot the TinyPico on the opening.
13. Attach the Stub at the end to stop the TinyPico from sliding out of the opening.
14. Close the electronics case and screw 4 M2 screws on top of the case.
15. Power on the motor to test the LCD turns on. Note you may need to change the contrast in order to see the words on the LCD.

![finishedTorquetuner](/images/torquetuner_finished.jpg "lcd and moteus board on case")
Figure 5: Completed Torquetuner module


## Document info

Version 1.0: Albert-Ngabo Niyonsenga - albert-ngabo.niyonsenga@mail.mcgill.ca

__Input Devices and Music Interaction Laboratory__
http://www.idmil.org\
Schulich School of Music\
McGill University\
550 Rue Sherbrooke Ouest, Suite 500\
Montreal, QC, H3A 1E\
Canada

