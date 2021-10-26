/* 
  The part of the sketch generated by the Arduino IoT Cloud Thing "ONE Nixie Clock - Backlight"
  https://create.arduino.cc/cloud/things/place-here-id-number-of-your-thing
  The following variables are automatically generated and updated when changes are made to the Thing:
  bool backlight;
  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard: onBacklightChange() 
  These functions are generated with the Thing and added at the end of this sketch.
*/

// If you decide to use the Arduino Cloud Code Editor 
// then this file #include "arduino_secrets.h" should be omitted (commented). 
// Use "uncomment" only with Arduino IDE offline software.
// #include "arduino_secrets.h"

#include "thingProperties.h"

// ONE Nixie Clock by Marcin Saj https://nixietester.com
// https://github.com/marcinsaj/ONE-Nixie-Clock
//
// Basic example of led backlight control from desktop Arduino IoT Cloud Dashboard 
// or Arduino IoT Cloud Remote App:
// Android - https://bit.ly/arduino-android-app
// IOS - https://bit.ly/arduino-ios-app
// 
// IoT ONE Nixie Clock Datasheet: https://bit.ly/IoT-Datasheet
// 
// Hardware:
// ONE Nixie Clock Arduino Shield - https://nixietester.com/project/one-nixie-clock
// Arduino Nano 33 IoT - https://store.arduino.cc/arduino-nano-33-iot
//
// NOTE: For Arduino Nano 33 IoT use 3.3V power settings on the clock motherboard (VCC jumper)
//
// Nixie Tube Socket - https://bit.ly/nixie-socket & https://bit.ly/NixieSocket-Project
// Nixie Power Supply module and RTC DS3231 module
// Nixie Clock require 12V, 1.5A power supply
// Schematic ONE Nixie Clock - http://bit.ly/ONE-Nixie-Clock-Schematic
// Schematic Nixie Power Supply Module - http://bit.ly/ONE-Nixie-Clock-NPS-Module
// DS3231 RTC datasheet: https://datasheets.maximintegrated.com/en/ds/DS3231.pdf

#include <Adafruit_NeoPixel.h>
// https://github.com/adafruit/Adafruit_NeoPixel
// https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN     A3

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT    4

// Declare our NeoPixel led object:
Adafruit_NeoPixel led(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels
// Argument 2 = Arduino pin number
// Argument 3 = Pixel type flags:
// NEO_KHZ800  800 KHz bitstream for WS2812 LEDs
// NEO_GRB     Pixels are wired for GRB bitstream

uint32_t red_color = led.Color(255, 0, 0);
uint32_t green_color = led.Color(0, 255, 0);
uint32_t blue_color = led.Color(0, 0, 255);
uint32_t white_color = led.Color(255, 255, 255);
uint32_t magenta_color = led.Color(255, 0, 255);

void setup() 
{ 
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  // The following function allows you to obtain more information
  // related to the state of network and IoT Cloud connection and errors
  // the higher number the more granular information you’ll get.
  // The default is 0 (only errors).
  // Maximum is 4

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();  
   
  led.begin();                            // Initialize NeoPixel led object
  led.show();                             // Turn OFF all pixels ASAP
  led.setBrightness(255);                 // Set brightness 0-255
  led.clear();
}

void loop() 
{ 
  ArduinoCloud.update();
}

//  Since Backlight is READ_WRITE variable, onBacklightChange() is
//  executed every time a new value is received from IoT Cloud.
void onBacklightChange()  
{
  Serial.print("The Backlight is ");
  
  if (backlight) 
  {
    led.fill(magenta_color);                // Fill all LEDs with a color
    Serial.println("ON");
  } else 
  {
    led.clear();                            // Turn off LEDs
    Serial.println("OFF");
  }
  
  led.show();
  delay(100); 
}
