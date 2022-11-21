# TorqueTuner: A self contained module for designing rotaryhaptic force feedback for digital musical instruments.

TorqueTuner  is  an  embedded  module  that  allows  Digital Musical Instrument (DMI) designers to map sensors to parameters  of  haptic  effects  and  dynamically  modify  rotary force  feedback  in  real-time. It comes with an embedded collection of haptic effects, and a is wireless bi-directional interface through [libmapper](https://github.com/libmapper/libmapper).

## Demonstration Video

[![IMAGE ALT TEXT](https://img.youtube.com/vi/KY3mpczKI3k/0.jpg)](https://www.youtube.com/watch?v=KY3mpczKI3k)

The hardware is based on the ESP32 microcontroller and the [moteus](https://mjbots.com/) platform, to implement a force feedback rotary encoder with 3600 PPR ~= 0.1 degree resolution that can display forces up to 45 Ncm (63.7oz.in).
## Documentation:
[Build Guide](./Docs/Build_guide_v1.md)

[Torquetuner Connection Guide](./Docs/connection_guide_v1.md)

[Torquetuner Firmware Update Guide](./Docs/Firmware_update_instructions.md)

[Torquetuner OSC Namespace](./Docs/TorquetunerOSC.md)

## Project Info:
Project webpage at IDMIL: http://www.idmil.org/project/torquetuner/
Official repository: https://github.com/IDMIL/torquetuner

IDMIL - Input Devices and Music Interaction Laboratory - http://www-new.idmil.org/

Metalab - Society for Arts and Technology - https://sat.qc.ca/fr/recherche/metalab

CIRMMT - Centre for Interdisciplinary Research in Music Media and Technology - http://www.cirmmt.org/

Schulich School of Music - McGill University

