# ONC IoT Step by Step Datasheet
1. Use this guide <a target="_blank" href="https://www.hackster.io/MarcinSaj/how-to-connect-one-nixie-clock-to-the-arduino-iot-cloud-e85081">"How to Connect ONC to the Arduino IoT Cloud"</a> and connect your clock to the Arduino IoT Cloud
2. Install Arduino app on your smartphone
   - Android - https://bit.ly/arduino-android-app
   - iOS - https://bit.ly/arduino-ios-app
3. Open this guide <a target="_blank" href="https://www.hackster.io/MarcinSaj/iot-one-nixie-clock-arduino-cloud-backlight-nixie-tube-fe4626">"HHow to Control IoT ONE Nixie Clock - Backlight & Nixie Tube"</a> and follow step by step.
4. If you decide to use the Arduino Cloud Code Editor then use only this code "IoT-ONC-Backlight-Control.ino" <br/>
If you use Arduino IDE offline software then you should download folowing files into one folder "IoT-ONC-Backlight-Control":
   - IoT-ONC-Backlight-Control.ino
   - arduino-secrets.h
   - thingProperties.h
5. (offline) Open "arduino-secrets.h" and "thingProperties.h" and edit the required informations:
   - define SECRET_SSID "type here your wifi network name"
   - define SECRET_PASS "type here your wifi password"
   - const char THING_ID[] = "place-here-id-number-of-your-thing-from-iot-arduino-cloud";
6. Upload sketch.
<br/><br/>***ONC - ONE Nixie Clock
<p align="center"><img src="https://github.com/marcinsaj/ONE-Nixie-Clock/blob/main/extras/one-nixie-clock-backlight-nixie-tube.gif"></p>

### ARDUINO IOT CLOUD - TUTORIALS
- <a target="_blank" href="https://www.hackster.io/MarcinSaj/one-nixie-clock-assembly-and-start-up-instructions-33c2d7">ONE Nixie Clock - Assembly and Start-up Instructions</a>
- <a target="_blank" href="https://www.hackster.io/MarcinSaj/how-to-connect-one-nixie-clock-to-the-arduino-iot-cloud-e85081">How to open Arduino IoT account and connect ONE Nixie Clock to the Cloud</a>
- <a target="_blank" href="https://www.hackster.io/MarcinSaj/iot-one-nixie-clock-arduino-cloud-backlight-on-off-efd9e9">How to Control IoT ONE Nixie Clock - Backlight ON/OFF</a>
- <a target="_blank" href="https://www.hackster.io/MarcinSaj/iot-one-nixie-clock-arduino-cloud-backlight-color-33a5be">How to Control IoT ONC - Backlight ON/OFF & Color</a>
- <a target="_blank" href="https://www.hackster.io/MarcinSaj/iot-one-nixie-clock-arduino-cloud-backlight-brightness-790852">How to Control IoT ONC - Backlight ON/OFF & Color & Brightness</a>
- <a target="_blank" href="https://www.hackster.io/MarcinSaj/iot-one-nixie-clock-arduino-cloud-backlight-control-7992b2">How to Control IoT ONC - Backlight Full Control - One Widget</a>
- <a target="_blank" href="https://www.hackster.io/MarcinSaj/iot-one-nixie-clock-arduino-cloud-backlight-nixie-tube-fe4626">How to Control IoT ONC - Backlight Full Control & Nixie Tube ON/OFF</a>
- <a target="_blank" href="https://www.hackster.io/MarcinSaj/iot-one-nixie-clock-arduino-cloud-backlight-nixie-tube-81f4b5">How to Control IoT ONC - Backlight Full Control & Nixie Tube Display</a>
- <a target="_blank" href="https://www.hackster.io/MarcinSaj/iot-one-nixie-clock-arduino-iot-cloud-alexa-control-85be50">How to Control IoT ONE Nixie Clock - Arduino IoT Cloud or Alexa Voice Control</a>

*ONC - ONE Nixie Clock
