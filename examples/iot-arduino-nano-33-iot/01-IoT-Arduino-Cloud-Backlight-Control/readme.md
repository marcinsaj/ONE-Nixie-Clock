ONC IoT Step by Step Datasheet
- How to Connect ONC to the Arduino IoT Cloud
- How to Control ONC Backlight - ON/OFF

If you decide to use the Arduino Cloud Code Editor then you should use only this sketch: 
IoT-ONC-Backlight-Control.ino 

If you use Arduino IDE offline software then you should use these files: 
- IoT-ONC-Backlight-Control.ino
- arduino-secrets.h
- thingProperties.h

Copy all files into one folder "IoT-ONC-Backlight-Control" 
open "arduino-secrets.h" and "thingProperties.h" and edit the required informations:

- #define SECRET_SSID "type here your wifi network name"
- #define SECRET_PASS "type here your wifi password"
- const char THING_ID[] = "place-here-id-number-of-your-thing-from-iot-arduino-cloud";

_***ONC - ONE Nixie Clock_
