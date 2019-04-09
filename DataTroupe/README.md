Data Troupe: Source Code Samples
===============================

[http://www.xenoformlabs.com](http://www.xenoformlabs.com)

Created by Scott Kildall and Michael Ang
Part of the Xenoform Labs Residency, April 2018


## Enviromment + Platform
Visual Code Studio: true C++ environment using the Arduino Libraries

**Lolin D32 Pro** is out preferred ESP chip, but others such as the Adafruit Huzzah ESP32 also work fine

## Network Credentials
### In the source code
private_config.g contains the wifi credentials information, with just these two lines. Obviously, you should re-define your own


`#define WIFI_SSID "Network Name"`
`#define WIFI_PASSWORD "password"`


## Other Hardware

### MAX98357 Amp by Adafruit
[Can find them here](https://www.adafruit.com/product/3006)

**Pinouts from ESP to MAX98357**

ESP32 USB –> MAX98357A Vin (gives us 5V power)

ESP32 GND –> MAX98357A GND

ESP32 GPIO26 –> MAX98357A BLCK

ESP32 GPIO22 –> MAX98357A DIN

ESP32 GPIO25 –> MAX98357A LRC


Note: GPIO22 can be changed in the audio playback libraries so that it doesn't interfere with the I2C 

## Copyright and License

Copyright (c) 2017-2018, Scott Kildall and Michael Ang

Licensed under Creative Commons: Attribution-NonCommercial-ShareAlike

CC BY-NC-SA

