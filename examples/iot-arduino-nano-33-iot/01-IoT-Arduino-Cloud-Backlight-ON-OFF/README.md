# ONC IoT Step by Step Datasheet
- How to Connect ONC to the Arduino IoT Cloud
- How to Control IoT ONC Backlight - ON/OFF
- How to Control IoT ONC Backlight - ON/OFF & Color
- How to Control IoT ONC Backlight - ON/OFF & Color & Brightness - 3 Different Widgets
- How to Control IoT ONC Backlight - ON/OFF & Color & Brightness - Only 1 Widget for all
- How to Control IoT ONC Backlight & Nixie Tube
- How to Control IoT ONC Backlight & Nixie Tube Display
- IoT ONE Nixie Clock & Arduino Cloud
<br/>

<p align="center"><img src="https://github.com/marcinsaj/ONE-Nixie-Clock/blob/main/extras/one-nixie-clock-on-off.gif"></p>

## If you decide to use the Arduino Cloud Code Editor then you should use only this sketch: <br/>

- IoT-ONC-Backlight-Control.ino


## If you use Arduino IDE offline software then you should use these files: <br/>

- IoT-ONC-Backlight-Control.ino
- arduino-secrets.h
- thingProperties.h


## Copy all files into one folder "IoT-ONC-Backlight-Control" open "arduino-secrets.h" and "thingProperties.h" and edit the required informations: <br/>

- define SECRET_SSID "type here your wifi network name"
- define SECRET_PASS "type here your wifi password"
- const char THING_ID[] = "place-here-id-number-of-your-thing-from-iot-arduino-cloud";

_***ONC - ONE Nixie Clock_
