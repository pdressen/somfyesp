# SomfyESP
**ESP8266/ESP32 based Home Assistant WiFi Gateway for Somfy RTS motors**

<div>
<img height="400px" src="https://user-images.githubusercontent.com/22083742/213578358-5f8181f7-5085-4315-ad66-7fcaf4177df0.jpg"/>
<img height="400px" src="https://user-images.githubusercontent.com/22083742/213578066-1a911110-aa45-48e8-86bd-6bc60cdb4484.jpg"/>
</div>

## Getting started

### Hardware needed
- Wemos D1 mini (or equivalent)
- CC1101 RF module
- some kind of housing (eg. wiring installation box)
- a suitable power supply (eg. USB, 3.3V or 5V)

### Preparing the Hardware
Connect the required pins of the Wemos D1 mini and the CC1101 module according to the following scheme:
| Wemos D1 mini | CC1101 RF module  |
|---            |---                |
|3.3V           |1,2 VDD            |
|MOSI/D7        |3 SI               |
|SCK/D5         |4 SCK              |
|MISO/D6        |5 SO               |
|GPIO 5/D1 (TX) |6 GDO2             |
|SS/D8          |7 CSn              |
|GPIO 4/D2      |8 GD01             |
|GND            |9,10 GND           |

After testing, its recommended to pack the construction in a fancy housing (see pictures above).

### Adapting the code
Some configuration needs to be done before the code can be built and flashed onto the microcontroller.
Most of the values have descriptive names or else they are described by a comment.
- adapt the `secrets.yaml` file for WiFi connection and device credentials
- adapt the `somfy_config.h` file to suit your needs (remote id, number of channels, ...)
- compile the project by running `esphome compile somfy.yaml` in the repository root
- connect the microcontroller via USB and run `esphome run somfy.yaml` to flash it and stream its logs

## Usage
After connecting to your WiFi, your Home Assistant instance should automatically detect it as a new device.
You'll need the encryption key and device password from `secrets.yaml` to connect to it.
Besides that, the device offers a web-interface at http://<device-ip> protected by username (`somfyesp`)
and the password defined in `secrets.yaml`.

By default the device offers 16 individual virtual RTS remotes (channels) with a cover component and a PROG
button each. The buttons (almost) behave like the respective buttons on a regular Somfy RTS remote
(Up, Stop/My, Down, PROG). Long presses however are not possible at the moment.
You can program and then name the corresponding entities in Home Assistant to give them a descriptive name.

<img height="600px" src="https://user-images.githubusercontent.com/22083742/213583189-c30793d3-8c3b-4415-9a54-3550b2c01e75.jpg"/>


## Credits
This project is heavily inspired by this project by [Evgeni Golov](https://github.com/evgeni):\
https://www.die-welt.net/2021/06/controlling-somfy-roller-shutters-using-an-esp32-and-esphome/

The [original code](https://github.com/evgeni/esphome-configs/blob/devel/somfy.yaml) can be found on his GitHub.

Also show some love to the [Somfy_Remote_Lib](https://github.com/Legion2/Somfy_Remote_Lib) library authors,
who have done the heavy lifting of reverse engineering and implementing the Somfy RTS protocol.
