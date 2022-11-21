# TorqueTuner: A self contained module for designing rotaryhaptic force feedback for digital musical instruments.

This repository is part of the TorqueTuner project, and contains firmware for the ESP32 microcontroller.

You will need to create file `Firmware/Esp32/wifi_credentials.h` that contains the right values `"..."` for your WiFi settings:
```
const char* SSID     = "...";
const char* PASSWORD = "...";
```