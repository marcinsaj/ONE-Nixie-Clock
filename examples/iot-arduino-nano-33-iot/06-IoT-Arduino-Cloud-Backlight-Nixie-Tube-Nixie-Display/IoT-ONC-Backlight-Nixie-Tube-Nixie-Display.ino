/*
  The part of the sketch generated by the Arduino IoT Cloud Thing
  "ONE Nixie Clock - Backlight Nixie Tube Nixie Display"
  https://create.arduino.cc/cloud/things/place-here-id-number-of-your-thing
  The following variable is automatically generated and updated when changes are made to the Thing:
  
  CloudColoredLight backlight;
  bool nixie_Tube;
  int nixie_Display;
  
  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard: 
  onBacklightChange(), onNixieTubeChange(), onNixieDisplayChange();
  These functions are generated with the Thing and added at the end of this sketch.
*/

#include "arduino_secrets.h"
#include "thingProperties.h"

// ONE Nixie Clock by Marcin Saj https://nixietester.com
// https://github.com/marcinsaj/ONE-Nixie-Clock
//
// Example code showing the control of the nixie tube backlight, 
// turning the tube on and off, displaying selected digits,
// from desktop Arduino IoT Cloud Dashboard or Arduino IoT Cloud Remote App:
// Android - https://bit.ly/arduino-android-app
// IOS - https://bit.ly/arduino-ios-app
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

// NeoPixels LEDs pin
#define LED_PIN     A3

// Number of NeoPixels LEDs
#define LED_COUNT    4

// Declare our NeoPixel led object:
Adafruit_NeoPixel led(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels
// Argument 2 = Arduino pin number
// Argument 3 = Pixel type flags:
// NEO_KHZ800  800 KHz bitstream for WS2812 LEDs
// NEO_GRB     Pixels are wired for GRB bitstream

// Shift registers control pins
#define DIN_PIN     A0
#define EN_PIN      A1
#define CLK_PIN     A2

// PWM pin for nixie tube fade effect.
// Not used in this example but need to be declared.
// For the correct operation of shift registers, 
// the LOW state must be set
#define PWM_PIN     10

// Nixie Power Supply Module control pin
#define EN_NPS_PIN  13 

// The clock has a built-in detection mechanism 
// for 15 segment nixie tubes (e.g. B-7971, B-8971)
#define DETECT_PIN  A6    

int analogDetectInput = 0;
 
// Bit numbers 
//
//            8
//       ___________
//      |\    |    /|
//      | \   |0  / |
//    9 | 1\  |  /7 | 13
//      |   \ | /   |
//      |____\|/____| 
//      | 2  /|\  6 |
//      |   / | \   |
//   10 |  /  |4 \  | 12
//      | /3  |  5\ |
//      |/    |    \|
//       ¯¯¯¯¯¯¯¯¯¯¯
//            11
//       /¯¯¯¯¯¯¯¯¯\
//            14    
//  ______________________
// | SOCKET 20A, 24A, 26A |
//  ¯¯¯¯¯|¯¯¯¯¯¯¯¯¯¯|¯¯¯¯¯

// Bit notation of 15-segment tube symbols                                  
uint16_t symbol_nixie_tube[]={
  0b0011111110001000,   // 0 
  0b0000000000010001,   // 1
  0b0010100101001000,   // 2
  0b0001100111000000,   // 3
  0b0011001001000100,   // 4
  0b0000101100100100,   // 5
  0b0001111101000100,   // 6
  0b0000000110010000,   // 7
  0b0011111101000100,   // 8
  0b0011101101000100,   // 9 
  0b0011011101000100,   // A
  0b0011100101010001,   // B 
  0b0000111100000000,   // C 
  0b0011100100010001,   // D 
  0b0000111100000100,   // E
  0b0000011100000100,   // F
  0b0001111101000000,   // G
  0b0011011001000100,   // H
  0b0000100100010001,   // I
  0b0011110000000000,   // J
  0b0000011010100100,   // K
  0b0000111000000000,   // L
  0b0011011010000010,   // M
  0b0011011000100010,   // N 
  0b0011111100000000,   // O 
  0b0010011101000100,   // P 
  0b0011111100100000,   // Q
  0b0010011101100100,   // R
  0b0001101101000100,   // S
  0b0000000100010001,   // T
  0b0011111000000000,   // U
  0b0000011010001000,   // V
  0b0011011000101000,   // W
  0b0000000010101010,   // X
  0b0000000010001010,   // Y
  0b0000100110001000    // Z             
};

// Bit notation of 10-segment tube digits 
uint16_t digit_nixie_tube[]={
  0b0000000000000001,   // 0 
  0b0000000000000010,   // 1
  0b0000000000000100,   // 2
  0b0000000000001000,   // 3
  0b0000000000010000,   // 4
  0b0000000000100000,   // 5
  0b0000000001000000,   // 6
  0b0000000010000000,   // 7
  0b0000000100000000,   // 8
  0b0000001000000000    // 9    
};

/*
// Bit notation of 7-segment tube digits
// Socket no.29, MG-17G nixie tube 
uint16_t digit_nixie_tube[]={
  0b0000000001110111,   // 0
  0b0000000000100100,   // 1
  0b0000000001011101,   // 2
  0b0000000001101101,   // 3
  0b0000000000101110,   // 4
  0b0000000001101011,   // 5
  0b0000000001111011,   // 6
  0b0000000000100101,   // 7
  0b0000000001111111,   // 8
  0b0000000001101111    // 9    
};
*/

uint32_t backlight_color;
uint16_t backlight_brightness;

void setup() 
{  
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(3000);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  // The following function allows you to obtain more information
  // related to the state of network and IoT Cloud connection and errors
  // the higher number the more granular information you’ll get.
  // The default is 0 (only errors).
  // Maximum is 4

  setDebugMessageLevel(4);
  ArduinoCloud.printDebugInfo();  
  
  led.begin();                            // Initialize NeoPixel led object
  led.show();                             // Turn OFF all pixels ASAP
  led.setBrightness(255);                 // Set brightness 0-255  
  led.clear();                            // Clear LEDs
  
  pinMode(EN_NPS_PIN, OUTPUT);
  digitalWrite(EN_NPS_PIN, HIGH);         // Turn OFF nixie power supply module 

  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);
  
  pinMode(CLK_PIN, OUTPUT);
  digitalWrite(CLK_PIN, LOW);
  
  pinMode(DIN_PIN, OUTPUT);
  digitalWrite(DIN_PIN, LOW);

  pinMode(PWM_PIN, OUTPUT);
  digitalWrite(PWM_PIN, LOW); 
    
  digitalWrite(EN_NPS_PIN, LOW);          // Turn ON nixie power supply module

  ShiftOutData(0);                        // Clear Nixie Tube
}

void loop() 
{
  ArduinoCloud.update();
}

// Sending data to shift registers
void ShiftOutData(uint16_t character)
{ 
  // For unknown reasons, communication with the Arduino Cloud
  // changes the purpose of the DIN pin and therefore the DIN declaration as output 
  // must be repeated before each data send to the shift registers
  pinMode(DIN_PIN, OUTPUT);
  
  uint8_t first_half = character >> 8;  
  uint8_t second_half = character;     
  digitalWrite(EN_PIN, LOW);
  shiftOut(DIN_PIN, CLK_PIN, MSBFIRST, first_half);
  shiftOut(DIN_PIN, CLK_PIN, MSBFIRST, second_half);
      
  // Return the latch pin high to signal chip that it
  // no longer needs to listen for information
  digitalWrite(EN_PIN, HIGH);
}

// If a high state appears on the analog input, 
// it means that a multi-segment tube socket has been inserted
bool DetectNixieTube()
{
  analogDetectInput = analogRead(DETECT_PIN);
  // 0 - 1024, Detecting anything above 0 means true
  // 950 is for sure 
  if(analogDetectInput >= 950) return(true);
  else return(false);  
}

void onNixieDisplayChange()
{
  Serial.print("Nixie Display: "); 
  Serial.println(nixie_Display);
  
  if(DetectNixieTube() == true)
  {
    ShiftOutData(symbol_nixie_tube[nixie_Display]);  
  }
  else 
  {
    ShiftOutData(digit_nixie_tube[nixie_Display]);   
  }
}

// Executed every time a new value is received from IoT Cloud.
void onNixieTubeChange()  
{
  Serial.print("Nixie Tube: ");
  
  if(nixie_Tube == true) 
  {
    Serial.println("ON");
    digitalWrite(EN_NPS_PIN, LOW);          // Turn ON nixie power supply module
  }
  else 
  {
    Serial.println("OFF");
    digitalWrite(EN_NPS_PIN, HIGH);         // Turn OFF nixie power supply module
  }
}

// Executed every time a new value is received from IoT Cloud.
void onBacklightChange()
{
  if (backlight.getSwitch() == true)
  {
    // Convert HSB to RGB
    // Declare a variable of the Color data type and define it using the HSB values of the color variable
    Color currentColor = Color(backlight.getValue().hue, backlight.getValue().sat, backlight.getValue().bri);

    // Declare the variables to store the RGB values
    byte RValue;
    byte GValue;
    byte BValue;

    // The variables will contain the RGB values after the function returns
    currentColor.getRGB(RValue, GValue, BValue);

    backlight_color = led.Color(RValue, GValue, BValue);

    Serial.print("Backlight Color:");
    Serial.print(" R-");
    Serial.print(RValue);
    Serial.print(" G-");
    Serial.print(GValue);
    Serial.print(" B-");
    Serial.println(BValue);

    backlight_brightness = 2.55 * (backlight.getValue().bri);
    
    Serial.print("Brightness: ");
    Serial.println(backlight_brightness);

    led.setBrightness(backlight_brightness);
    led.fill(backlight_color);  
    led.show();
    delay(10);
  }
  else
  {
    backlight_brightness = 0;
    led.clear();
    led.show();
    delay(10);
  }
}
