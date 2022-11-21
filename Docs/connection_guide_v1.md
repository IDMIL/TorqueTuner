# Torquetuner Connection Guide V1.0

## Option 1: Wireless Connection
### Get your network details
1. Connect to the network you will be connecting the Torquetuner module to.
2. Note the SSID (network name) and SSID Password (network password).
3. Get your computers IP address while connected to this network. Below are linked some support articles for Windows and MacOS on how to find your computers IP address.
    - [Find your IP Address Windows](https://support.microsoft.com/en-us/windows/find-your-ip-address-in-windows-f21a9bbc-c582-55cd-35e0-73431160a1b9)
    - [Find your IP Address MacOS](https://discussions.apple.com/thread/253927735)
    - [Find your IP Address Linux](https://opensource.com/article/18/5/how-find-ip-address-linux)

### Connect to the Torquetuner Module.
4. Power on your Torquetuner module and wait until the boot sequence is complete.
5. Connect to the TorqueTuner_XXX wifi where XXX is the ID of the Torquetuner module. ie: TorqueTuner_001. By default the password is mappings.
6. Open your browser and go to [http://TorqueTuner_XXX.local/](http://TorqueTuner_XXX.local/) or [http://192.168.4.1](http://192.168.4.1), where XXX is the ID of the Torquetuner module.
![Torquetuner setup page](/images/torquetuner_setup.png "Torquetuner wifi setup page")
7. In the __Network__ section write the network name and password in the __SSID__ and __SSID Password__ fields. 
8. In the __OSC send settings__ put in your computer's IP address optained in Step 3 in the __Primary IP__ field.
8. Click the green __Save__ button.
6. Click __Config__ on the top of the page to return to the orginal menu.
7. Then click the __Close and Reboot__ button at the bottom of the page.

## Option 2: Puara Serial Manager
1. Download or clone the Puara client repository at https://github.com/Puara/puara-client. Instructions for how to clone github repositories can be found at the links below
    - [Cloning a Git Repository](https://docs.github.com/en/repositories/creating-and-managing-repositories/cloning-a-repository).
2. Open `config_template.json`.
3. Put in the network name you want the Torquetuner to connect to in the __wifiSSID__ field.
4. Put the network password in the __wifiPSK__ field.
5. Save and close `config_template.json`.
6. Run the `puara_serial_manager.py` script.
7. Connect the Torquetuner module to your PC using a USB cable.
8. The script should auto detect and configure the Torquetuner module. If the Torquetuner module is not being detected you may need to download the CP210x USB driver. A link can be found on [Esspressif's website](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/establish-serial-connection.html).