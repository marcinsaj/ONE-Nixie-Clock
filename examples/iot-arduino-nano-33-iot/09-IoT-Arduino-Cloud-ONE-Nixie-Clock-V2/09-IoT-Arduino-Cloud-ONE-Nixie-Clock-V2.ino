// ONE Nixie Clock by Marcin Saj https://nixietester.com
// https://github.com/marcinsaj/ONE-Nixie-Clock
//
// IoT ONE Nixie Clock V2
// This example demonstrates how to use Arduino IoT Cloud Dashboard
// and Amazon Alexa Assistant to control ONE Nixie Clock
// Control options: 
// - ON/OFF Nixie Clock, 
// - ON/OFF Cycle - cathode poisoning prevention routine,
// - First Backlight - ON/OFF, Color, Brightness - backlight for hours,
// - Second Backlight - ON/OFF, Color, Brightness - backlight for minutes.
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
// Schematic ONE Nixie Clock - https://bit.ly/ONE-Nixie-Clock-Schematic
// Schematic Nixie Power Supply Module - https://bit.ly/ONE-Nixie-Clock-NPS-Module
// DS3231 RTC datasheet: https://datasheets.maximintegrated.com/en/ds/DS3231.pdf

#include "arduino_secrets.h"
#include "thingProperties.h"

#include <Adafruit_NeoPixel.h>
// https://github.com/adafruit/Adafruit_NeoPixel
// https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use

#include <RTCZero.h>
RTCZero main_rtc;         // RTC inside Nano 33 IoT

#include <RTClib.h>       // https://github.com/adafruit/RTClib
RTC_DS3231 ds3231_rtc;    // RTC DS3231 library declaration

uint32_t epochTime = 0;
uint8_t timeHour = 0;
uint8_t timeMinute = 0;
uint8_t timeSecond = 0;

// Choose Time Format *******************************************************
#define hourFormat        24     // 12 Hour Clock or 24 Hour Clock
// **************************************************************************

// https://en.wikipedia.org/wiki/List_of_time_zones_by_country
// Choose your Time Zone ****************************************************
#define timeZone          1
// ************************************************************************** 

// Choose your hour to synchronize the Time via WiFi ************************
// The RTC DS3231 always works in 24 hour mode so if you want to set 3:00AM 
// use "3" if you want to set 14:00 or 2:00PM use "14" etc. 
#define timeToSynchronizeTime     23     // 3:00AM              
// **************************************************************************

// Set fade in/out effect delay *********************************************
#define fadeDelay         12     // Best effect in range 5 - 20 milliseconds
// **************************************************************************

// Set PWM frequency ********************************************************
// PWM frequency can be calculated by
// Freq = 48MHz CPU / (TCC0_prescaler 256 * (1 + period 99) * pwm divider)
uint32_t period = 100 - 1;      // Do not change the period!
#define PWM_Divider       15    // 24 - 78Hz, 15 - 125Hz, 10 - 188Hz 
// **************************************************************************

// Cathode poisoning prevention settings*************************************
// How often to run the cathode poisoning prevention routine
#define routine   4             // 1 - everytime, 1 is default do not use it 
                                // 2 - every second time and so on
// **************************************************************************

// How often to run the cathode poisoning prevention routine
// The settings are handled by onCycleChange()
uint8_t howOftenCycle = routine; 

// Loop counter to run cathode poisoning prevention routine
uint8_t loopCounter = 0;

// How many times to try to synchronize the Time
uint8_t numberOfTries = 0; 
uint8_t maxTries = 10;

// NeoPixels LEDs pin
#define LED_PIN       A3

// Number of NeoPixels LEDs
#define LED_COUNT     4

// Declare our NeoPixel led object:
Adafruit_NeoPixel led(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels
// Argument 2 = Arduino pin number
// Argument 3 = Pixel type flags:
// NEO_KHZ800  800 KHz bitstream for WS2812 LEDs
// NEO_GRB     Pixels are wired for GRB bitstream

// To distinguish the colors of the backlight hours and minutes
boolean hour_color = 1;
boolean minute_color = 0;

// Shift registers control pins
#define DIN_PIN     A0
#define EN_PIN      A1
#define CLK_PIN     A2

// Nixie Power Supply Module control pin
#define EN_NPS_PIN  13 

// Declared as PA21, find below in code
// #define PWM_PIN    10

// The clock has a built-in detection mechanism 
// for 15 segment nixie tubes (e.g. B-7971, B-8971)
#define DETECT_PIN  A6    

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

uint16_t animation[]={
  0b0000000000000010,
  0b0000000000000001,
  0b0000000110000000,
  0b0010000101000000,
  0b0011000100100000,  
  0b0111000100010000,
  0b0111100100001000,
  0b0111110100000100,  
  0b0111111100000010,
  0b0111111100000001,
  0b0111111110000000,
  0b0111111101000000,
  0b0111111100100000,
  0b0111111100010000,
  0b0111111100001000,
  0b0111111100000100,
  0b0111111100000010,
  0b0111111100000001,
  0b0111111010000000,
  0b0101111001000000,
  0b0100111000100000,
  0b0000111000010000,
  0b0000011000001000,
  0b0000001000000100,
  0b0000000000000010, 
  0b0000000000000000     
};

uint16_t transition[]={
  0b0000000000000001,
  0b0000000010000000,
  0b0000000001000000,
  0b0000000000100000,
  0b0000000000010000,
  0b0000000000001000,
  0b0000000000000100,
  0b0000000000000010  
};

uint16_t transition_mask[]={  
  0b0000000000000000,
  0b0000000100000001,
  0b0010000010000000,
  0b0001000001000000,
  0b0000000000100000,
  0b0000100000010000,
  0b0000010000001000,
  0b0000001000000100 
};

// Nixie tube cathode no.14 (underscore symbol)
uint16_t hour_symbol = 0b0100000000000000;

// Underscore symbol flag, hour display distinguishing feature
// for multisegment tubes
// 0 - turn off, 1 - turn on
boolean hourUnderscore = 0;

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

// Millis time start
uint32_t millis_start = 0;

uint16_t current_hours_hue_Value = 0;
uint8_t current_hours_sat_Value = 0;
uint8_t current_hours_bri_Value = 0;
uint16_t current_minutes_hue_Value = 0;
uint8_t current_minutes_sat_Value = 0;
uint8_t current_minutes_bri_Value = 0;

boolean status_cloud_sync = false;
boolean status_nixie_clock = false;
boolean status_backlight_hours = false;
boolean status_backlight_minutes = false;

  
void setup() 
{ 
  pinMode(EN_NPS_PIN, OUTPUT);
  digitalWrite(EN_NPS_PIN, HIGH);                 // Turn OFF nixie power supply module 
  
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);
  
  pinMode(CLK_PIN, OUTPUT);
  digitalWrite(CLK_PIN, LOW);
  
  pinMode(DIN_PIN, OUTPUT);
  digitalWrite(DIN_PIN, LOW);   

// Enable and configure generic clock generator 4
  GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC |            // Improve duty cycle
                      GCLK_GENCTRL_GENEN |          // Enable generic clock gen
                      GCLK_GENCTRL_SRC_DFLL48M |    // Select 48MHz as source
                      GCLK_GENCTRL_ID(4);           // Select GCLK4
  while (GCLK->STATUS.bit.SYNCBUSY);                // Wait for synchronization

  // Set clock divider to generic clock generator 4
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(PWM_Divider) | // Divide 48 MHz
                     GCLK_GENDIV_ID(4);             // Apply to GCLK4 4
  while (GCLK->STATUS.bit.SYNCBUSY);                // Wait for synchronization
  
  // Enable GCLK4 and connect it to TCC0 and TCC1
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |          // Enable generic clock
                      GCLK_CLKCTRL_GEN_GCLK4 |      // Select GCLK4
                      GCLK_CLKCTRL_ID_TCC0_TCC1;    // Feed GCLK4 to TCC0/1
  while (GCLK->STATUS.bit.SYNCBUSY);                // Wait for synchronization

  // Divide counter by 256
  TCC0->CTRLA.reg |= TCC_CTRLA_PRESCALER(TCC_CTRLA_PRESCALER_DIV256_Val);
  while (TCC0->SYNCBUSY.bit.WAVE);                // Wait for synchronization

  // Use "Normal PWM" (single-slope PWM): count up to PER, match on CC[n]
  TCC0->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM;         // Select NPWM as waveform
  while (TCC0->SYNCBUSY.bit.WAVE);                // Wait for synchronization

  // Set the period (the number to count to (TOP) before resetting timer)
  TCC0->PER.reg = period;
  while (TCC0->SYNCBUSY.bit.PER);

  // Set PWM signal to output
  TCC0->CC[3].reg = period / 1;
  while (TCC0->SYNCBUSY.bit.CC2);

  // Configure PA21 (D10 on Arduino Nano 33 IoT) to be output
  PORT->Group[PORTA].DIRSET.reg = PORT_PA21;      // Set pin as output
  PORT->Group[PORTA].OUTCLR.reg = PORT_PA21;      // Set pin to low

  // Enable the port multiplexer for PA21
  PORT->Group[PORTA].PINCFG[21].reg |= PORT_PINCFG_PMUXEN;

  // Connect TCC0 timer to PA21. Function F is TCC0/WO[2] for PA21.
  // Odd pin num (2*n + 1): use PMUXO
  // Even pin num (2*n): use PMUXE
  PORT->Group[PORTA].PMUX[10].reg = PORT_PMUX_PMUXO_F;

  // Enable output (start PWM)
  TCC0->CTRLA.reg |= (TCC_CTRLA_ENABLE);
  while (TCC0->SYNCBUSY.bit.ENABLE);              // Wait for synchronization

  // Initialize serial and wait 5 seconds for port to open
  Serial.begin(9600);
  delay(5000);
  
  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  delay(1000);
  
  // Invoking `addCallback` on the ArduinoCloud object allows you to subscribe
  // to any of the available events and decide which functions to call when they are fired.
  // doThisOnSync() will be called when the Clock syncs data with the cloud
  // doThisOnConnect() will be called when the Clock connects to the cloud
  // doThisOnDisconnect() will be called when the Clock disconnects from the cloud
  
  ArduinoCloud.addCallback(ArduinoIoTCloudEvent::DISCONNECT, doThisOnDisconnect);
  ArduinoCloud.addCallback(ArduinoIoTCloudEvent::CONNECT, doThisOnConnect);
  ArduinoCloud.addCallback(ArduinoIoTCloudEvent::SYNC, doThisOnSync);

  // The following function allows you to obtain more information
  // related to the state of network and IoT Cloud connection and errors
  // the higher number the more granular information you’ll get.
  // The default is 0 (only errors).
  // Maximum is 4
  
  setDebugMessageLevel(4);
  ArduinoCloud.printDebugInfo(); 
  
  ds3231_rtc.begin();  
   
  led.begin();                                    // Initialize NeoPixel led object
  led.setBrightness(255);                         // Set full brightness  
  led.show();                                     // Turn OFF all pixels ASAP   

  delay(1000);

  Serial.println(" ");
  Serial.println("#############################################################");
  Serial.println("-------------------- IoT ONE Nixie Clock --------------------");
  Serial.println("#############################################################");    
  Serial.println('\n');

  delay(1000);
    
  Serial.println("Trying to connect with Arduino IoT Cloud...");
  
  // Wait for connection with Arduino IoT Cloud
  while (ArduinoCloud.connected() == 0) 
  {
    ArduinoCloud.update();
  }
}

void loop() 
{
  ArduinoCloud.update();
  DisplayTime();
}

void DisplayTime()
{   
  DateTime now = ds3231_rtc.now();

  timeHour = now.hour();
  timeMinute = now.minute();
  timeSecond = now.second();

  // In such a written configuration of time display, there is no simple way 
  // to check the exact second when the time should be synchronized, 
  // so the time synchronization is set to 3:00 AM 
  // and to avoid multiple time synchronization at this time, 
  // after time synchronization, the next time synchronization condition check 
  // will be possible after 60 seconds
    
  if(timeHour == timeToSynchronizeTime && timeMinute == 20)
  {
    if(millis() - millis_start > 60000)
    {
      SynchronizeTime();
      millis_start = millis();
    }
  }

  if(status_nixie_clock == true)
  {
    loopCounter++;
    
    uint8_t timeFormat = hourFormat;
    
    // Check time format and adjust
    if(timeFormat == 12 && timeHour > 12) timeHour = timeHour - 12;
    if(timeFormat == 12 && timeHour == 0) timeHour = 12; 

    Serial.print("Time: ");
    if(timeHour < 10)   Serial.print("0");
    Serial.print(timeHour);
    Serial.print(":");
    if(timeMinute < 10) Serial.print("0");
    Serial.print(timeMinute);  
    Serial.print(":");
    if(timeSecond < 10) Serial.print("0");
    Serial.println(timeSecond);      

    uint8_t digit_1;
    uint8_t digit_2;

    // Underscore symbol turn on for multisegment tubes
    hourUnderscore = 1; 
  
    // Extract individual digits
    digit_1  = (timeHour / 10) % 10; 
    digit_2  = (timeHour / 1)  % 10;
    NixieDisplay(digit_1, digit_2, hour_color);
    
    DelayTime(400);

    // Underscore symbol turn off for multisegment tubes
    hourUnderscore = 0; 
  
    digit_1  = (timeMinute / 10) % 10;
    digit_2  = (timeMinute / 1)  % 10;
    NixieDisplay(digit_1, digit_2, minute_color);

    DelayTime(2000);

    // How often to run the cathode poisoning prevention routine
    if(loopCounter >= howOftenCycle) 
    {
      CathodePoisoningPrevention();
      loopCounter = 0;
    }
  }     
}

// If a high state appears it means that 
// a multi-segment tube socket has been inserted
boolean DetectNixieTube()
{
  uint16_t detectInput = analogRead(DETECT_PIN);
  
  if(detectInput > 950) return(true);
  else return(false);
}

void StartPWM ()
{
  TCC0->CTRLBSET.reg = TCC_CTRLBCLR_CMD_RETRIGGER;    // Restart the timer
  while(TCC0->SYNCBUSY.bit.CTRLB);                    // Wait for synchronization
}

void UpdatePWM(uint16_t pwmValue)
{
  TCC0->CC[3].reg = pwmValue;
  while (TCC0->SYNCBUSY.bit.CC2);
  StartPWM();
}

void StopPWM ()
{
  TCC0->CTRLBSET.reg = TCC_CTRLBCLR_CMD_STOP;         // Stop the timer
  while(TCC0->SYNCBUSY.bit.CTRLB);                    // Wait for synchronization
}

void NixieDisplay(uint16_t digit_1, uint16_t digit_2, boolean backlight_Color)
{
  if(DetectNixieTube() == true) ShowSymbol(digit_1, digit_2, backlight_Color);
  else ShowDigit(digit_1, digit_2, backlight_Color);
}

// PWM fade in/out effect
void ShowDigit(uint16_t digit_1, uint16_t digit_2, boolean backlight_Color)
{         
  // 
  for(int digits = 0 ; digits <= 1; digits++)
  { 
    if(digits == 0) ShiftOutData(digit_nixie_tube[digit_1]);
    else if(digits == 1) ShiftOutData(digit_nixie_tube[digit_2]);
          
    for(int i = 100; i > 0; i = i - 2)
    { 
      UpdatePWM(i);
      SetBacklight(i, backlight_Color);
      delay(fadeDelay);
    }

    DelayTime(500);

    for(int i = 0; i <= 100 ; i = i + 2)
    { 
      UpdatePWM(i);
      SetBacklight(i, backlight_Color);
      delay(fadeDelay);
    }     
  }

  ClearNixieTube();  
}

// PWM fade in/out effect
void ShowSymbol(uint16_t digit_1, uint16_t digit_2, boolean backlight_Color)
{        
  uint16_t currentDigit = symbol_nixie_tube[digit_1];    
  if(hourUnderscore == 1) currentDigit = currentDigit | 0b0100000000000000;
  ShiftOutData(currentDigit);  
          
  for(int i = 100; i > 0; i = i - 2)
  { 
    UpdatePWM(i);
    SetBacklight(i, backlight_Color);     
    delay(fadeDelay);
  }

  DelayTime(800);

  for(int i = 0; i < 8; i++)
  {
    currentDigit = currentDigit | transition[i];
    currentDigit = currentDigit & ~transition_mask[i];
    if(hourUnderscore == 1) currentDigit = currentDigit | 0b0100000000000000;

    ShiftOutData(currentDigit);
    currentDigit = currentDigit & ~transition[i];
    DelayTime(80);
  }  
  
  currentDigit = symbol_nixie_tube[digit_2];  
  if(hourUnderscore == 1) currentDigit = currentDigit | 0b0100000000000000; 

  uint16_t originalDigit = currentDigit;
  uint16_t newDigit = 0;

   for(int i = 0; i < 8; i++)
  {
    currentDigit = originalDigit & transition_mask[i];    
    currentDigit = newDigit | currentDigit | transition[i];
    if(hourUnderscore == 1) currentDigit = currentDigit | 0b0100000000000000;
    
    ShiftOutData(currentDigit); 
    newDigit = currentDigit & ~transition[i];
    DelayTime(80);
  }  

  ShiftOutData(originalDigit);
  DelayTime(800);

  for(int i = 0; i <= 100 ; i = i + 2)
  { 
    UpdatePWM(i);
    SetBacklight(i, backlight_Color);
    delay(fadeDelay);
  }        
  
  ClearNixieTube(); 
}

// Turn off nixie tube
void ClearNixieTube()
{
  ShiftOutData(0);  
}

void CathodePoisoningPrevention()
{ 
  if(status_nixie_clock == true)
  {
    DelayTime(500);
    StopPWM();

    // 15 cathodes nixie tube 
    if(DetectNixieTube() == true)
    {
      for(int i = 0; i < 26; i++)
      {
        ShiftOutData(animation[i]); 
        DelayTime(80);
      }
    }
    else  // 10 cathodes nixie tube
    {
      for(int i = 0; i <= 3; i++)
      {
        for(int j = 0; j < 10; j++)
        {
          ShiftOutData(digit_nixie_tube[j]); 
          DelayTime(80);
        }
      }  
    }
  
  ClearNixieTube();
  DelayTime(1000);
  
  }
}

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

void SynchronizeTime()
{
  do 
  {
    epochTime = WiFi.getTime();
    delay(100);
    
    numberOfTries++;
  }
  while ((epochTime == 0) && (numberOfTries < maxTries));

  Serial.println(" ");
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  
  main_rtc.setEpoch(epochTime);

  timeHour = main_rtc.getHours() + timeZone;
  if(timeHour < 0) timeHour = 24 + timeHour;  
  if(timeHour > 23) timeHour = timeHour - 24;  

  timeMinute = main_rtc.getMinutes(); 
  timeSecond = main_rtc.getSeconds();

  // Update RTC DS3231 module
  ds3231_rtc.adjust(DateTime(0, 0, 0, timeHour, timeMinute, timeSecond));

  Serial.println(" ");
  Serial.println("#############################################################");
  Serial.println("--------------- Time has been Synchronized ------------------");
  Serial.println("#############################################################");
  Serial.println(" ");
}

void SetBacklight(uint16_t base_Brightness, boolean backlight_Color)
{
  if(status_nixie_clock == true)
  {
    uint16_t new_hue_Value = 0;
    uint8_t new_sat_Value = 0;
    uint8_t new_bri_Value = 0;
  
    if(backlight_Color == hour_color && status_backlight_hours == true)    // backlight_Color - true = hours
    {
      new_hue_Value = current_hours_hue_Value;
      new_sat_Value = current_hours_sat_Value;
      new_bri_Value = current_hours_bri_Value;
      new_bri_Value = new_bri_Value * base_Brightness * 0.01;
      new_bri_Value = current_hours_bri_Value - new_bri_Value;  
    }
  
    if(backlight_Color == minute_color && status_backlight_minutes == true)   // backlight_Color - false = minutes
    {
      new_hue_Value = current_minutes_hue_Value;
      new_sat_Value = current_minutes_sat_Value;
      new_bri_Value = current_minutes_bri_Value;
      new_bri_Value = new_bri_Value * base_Brightness * 0.01;
      new_bri_Value = current_minutes_bri_Value - new_bri_Value;  
    }

    // Convert HSB to RGB
    // Declare a variable of the Color data type and define it using the HSB values of the color variable
    Color currentColor = Color(new_hue_Value, new_sat_Value, new_bri_Value);

    // Declare the variables to store the RGB values
    uint8_t RValue;
    uint8_t GValue;
    uint8_t BValue;

    // The variables will contain the RGB values after the function returns
    currentColor.getRGB(RValue, GValue, BValue);

    uint32_t new_backlight_Color = led.Color(RValue, GValue, BValue);
 
    // Fill all LEDs with a color
    led.fill(new_backlight_Color);
    led.show();
  }
}

// To minimize communication delays with the Arduino IoT Cloud 
// most of the delay() functions have been replaced by DelayTime()
// with the ArduinoCloud.update() function call inside.
// The time needed to update the data from the Arduino IoT Cloud is 
// about 5-20ms, so the update function hidden inside the delay routine
// will not interfere with the program's operation.
void DelayTime(uint32_t wait)
{
  uint32_t millis_time_now = millis();
  while(millis() - millis_time_now < wait)
  {         
    ArduinoCloud.update();
  }  
}

// Executed every time a new value is received from IoT Cloud.
void onFirstBacklightChange()
{
  Serial.println("Inside: onFirstBacklightChange");
  Serial.println("Setup backlight for hours");

  if(status_cloud_sync == true)
  {  
    current_hours_hue_Value = firstBacklight.getValue().hue;
    current_hours_sat_Value = firstBacklight.getValue().sat;
    current_hours_bri_Value = firstBacklight.getValue().bri;
    status_backlight_hours = firstBacklight.getSwitch();
  }
}

// Executed every time a new value is received from IoT Cloud.
void onSecondBacklightChange()
{
  Serial.println("Inside: onSecondBacklightChange");
  Serial.println("Setup backlight for minutes");
  
  if(status_cloud_sync == true)
  {
    current_minutes_hue_Value = secondBacklight.getValue().hue;
    current_minutes_sat_Value = secondBacklight.getValue().sat;
    current_minutes_bri_Value = secondBacklight.getValue().bri;
    status_backlight_minutes = secondBacklight.getSwitch();
  } 
}

void onNixieClockChange()
{
  Serial.println("Inside: onNixieClockChange");
  
  if(status_cloud_sync == true)
  {
    status_nixie_clock = nixieClock;
  
    if(status_nixie_clock == true)
    {
      // Turn ON Nixie Power Supply Module
      digitalWrite(EN_NPS_PIN, LOW);
            
      Serial.println("ONE Nixie Clock Turn ON");
    }
    else
    {
      ClearNixieTube();      
    
      // Turn OFF Nixie Power Supply Module
      digitalWrite(EN_NPS_PIN, HIGH);
    
      Serial.println("ONE Nixie Clock Turn OFF");

      led.clear();
      led.show(); 
      delay(4000);
    }
  }
}

// Cathode poisoning prevention routine
// How often to run the cathode poisoning prevention routine
void onCycleChange()
{
  if(status_cloud_sync == true)
  {
    if(cycle == true) howOftenCycle = 1;  // Everytime
    else howOftenCycle = routine;         // Every fourth time  
  }
}

void doThisOnSync()
{ 
  Serial.println("Inside: doThisOnSync");
  Serial.println("Satisfactory data synchronization");

  status_cloud_sync = true;
  onCycleChange();
  onNixieClockChange();
  onFirstBacklightChange();
  onSecondBacklightChange();
}

void doThisOnConnect()
{
  Serial.println("Inside: doThisOnConnect");
  Serial.println("Connected to Arduino IoT Cloud");

  Serial.println(" ");
  SynchronizeTime();
}

void doThisOnDisconnect()
{  
  Serial.println("Inside: doThisOnDisconnect");
  Serial.println("No Time to Die");
  Serial.println("Check your System");
  status_cloud_sync = false;    
}
