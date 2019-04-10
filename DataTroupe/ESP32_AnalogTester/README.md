ESP32_AnalogTester
===============================

[http://www.xenoformlabs.com](http://www.xenoformlabs.com)

Created by Scott Kildall and Michael Ang
Part of the Xenoform Labs Residency, April 2018


## Tests analog inputs to terminal window


## Network Credentials
private_config.g contains the wifi credentials information, with just these two lines. Obviously, you should re-define your own


`#define WIFI_SSID "Network Name"`
`#define WIFI_PASSWORD "password"`



## Hardware Connections for MAX98357 
ESP32 USB –> MAX98357A Vin (gives us 5V power)

ESP32 GND –> MAX98357A GND

ESP32 GPIO26 –> MAX98357A BLCK

ESP32 GPIO22 –> MAX98357A DIN

ESP32 GPIO25 –> MAX98357A LRC

## Copyright and License

Copyright (c) 2017-2018, Scott Kildall and Michael Ang

Licensed under Creative Commons: Attribution-NonCommercial-ShareAlike

CC BY-NC-SA

