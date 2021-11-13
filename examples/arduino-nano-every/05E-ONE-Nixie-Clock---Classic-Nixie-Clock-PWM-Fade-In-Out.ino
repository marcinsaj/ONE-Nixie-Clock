// ONE Nixie Clock by Marcin Saj https://nixietester.com
// https://github.com/marcinsaj/ONE-Nixie-Clock
//
// Classic Nixie Clock with PWM fade in/out effect
// This example demonstrates how to set new time, display (time) digits or symbols 
// fade in/out effect and fade in/out backlight color effect.
//
// Hardware:
// ONE Nixie Clock Arduino Shield - https://nixietester.com/project/one-nixie-clock
// Arduino Nano Every - https://store.arduino.cc/arduino-nano-every
//
// NOTE: For Arduino Nano Every use 5V power settings on the clock motherboard (VCC jumper)
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

#include <RTClib.h>          
// https://github.com/adafruit/RTClib

// RTC library declaration
RTC_DS3231 rtc;

// **************************************************************************
// To modify the fade in/out effect,
// you need to change the number of PWM loop steps (5-10) "i = i - steps" 
// and/or the value (16, 32, 48) of "DelayTime(value); 
// in ShowSymbol(); or/and ShowDigit();
// **************************************************************************

// Choose Time Format *******************************************************
#define hourFormat       12     // 12 Hour Clock or 24 Hour Clock
// **************************************************************************

// Cathode poisoning prevention settings*************************************
// How often to run the cathode poisoning prevention routine
#define howOftenRoutine   1     // 0 - none, 1 - everytime, 
                                // 2 - every second time and so on
// **************************************************************************

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

// Blue backlight color
uint32_t hour_color = led.Color(0, 0, 255);
// Green backlight color
uint32_t minute_color = led.Color(0, 255,0);

// Shift registers control pins
#define DIN_PIN     A0
#define EN_PIN      A1
#define CLK_PIN     A2

// Nixie Power Supply Module control pin
#define EN_NPS_PIN  13 

// PWM pin for nixie tube fade effect
#define PWM_PIN     10

// The clock has a built-in detection mechanism 
// for 15 segment nixie tubes (e.g. B-7971, B-8971)
#define DETECT_PIN  A6    

int loopCounter = 0;

int analogDetectInput = 0;

// Serial monitor state
boolean serialState = 0;

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
 
void setup() 
{  
  Serial.begin(9600);
  rtc.begin();    
  
  led.begin();                            // Initialize NeoPixel led object
  led.show();                             // Turn OFF all pixels ASAP
  led.setBrightness(255);                 // Set brightness 0-255  

  delay(5000);
 
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
      
  Serial.println("##############################################################");
  Serial.println("------------ Test Example - Classic Nixie Clock --------------");
  Serial.println("---------------- If you want to set new Time -----------------");
  Serial.println("----------- press ENTER for Arduino IDE up to 1.8 ------------"); 
  Serial.println("----------- press CTRL+ENTER for Arduino IDE 2.0 -------------"); 

  // Millis time start
  unsigned long millis_time_now = millis();
  unsigned long millis_time_now_2 = millis();
    
  // Wait 10 seconds
  while((millis() < millis_time_now + 10000))
  {         
    if (millis() - millis_time_now_2 > 160)
    {
      Serial.print("#");
      millis_time_now_2 = millis();    
    }

    // Set serialState flag if time settings have been selected 
    if(Serial.available() > 0) 
    {            
      serialState = 1;
      break;  
    }
  }

  Serial.println('\n');
    
  // Clear serial buffer
  while(Serial.available())
  Serial.read();

  if(serialState == 0)
  {
    // Turn on the nixie power module if settings have not been selected
    digitalWrite(EN_NPS_PIN, LOW); 
    // Set new PWM frequency
    NewPWMFreq();  
  }   
}

void loop() 
{
  loopCounter++;

  // Set a new time if settings have been selected
  if(serialState == 1)
  {
    SetNewTime();
    serialState = 0;
    
    // Turn ON nixie power supply module
    digitalWrite(EN_NPS_PIN, LOW);
    // Set new PWM frequency
    NewPWMFreq();             
  }    

  // Get time from RTC and display on nixie tubes
  DisplayTime();
  
  // How often to run the cathode poisoning prevention routine
  if(loopCounter == howOftenRoutine) 
  {
    CathodePoisoningPrevention();
    loopCounter = 0;
  }
}

void NewPWMFreq()
{
  // Turn off timer while we change parameters
  TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
  // Clear all CLKSEL bits
  TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_CLKSEL_gm;
  // Deafult prescaler is 64, set new prescaler 16 times more to 1024
  // Default PWM frequency 976Hz, new PWM frequency 61Hz                                    
  TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV1024_gc;
  // Re-enable timer
  TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}

void SetNewTime()
{ 
  Serial.println("--------------- Enter the TIME without spaces ----------------");
  Serial.println("--------------- in the HHMM format e.g. 0923 -----------------");
  Serial.println("------- and when you are ready to send data to the RTC -------");
  Serial.println("------------ press ENTER for Arduino IDE up to 1.8 -----------"); 
  Serial.println("------------ press CTRL+ENTER for Arduino IDE 2.0 ------------"); 
  Serial.println('\n');

  // Clear serial buffer
  while(Serial.available())
  Serial.read();
    
  // Wait for the values
  while (!Serial.available()) {}                      

  // Read time as an integer value
  int hhmm_time = Serial.parseInt();

  // Extract minutes and hours
  byte timeSecond = 0;
  byte timeMinute = (hhmm_time / 1) % 100;
  byte timeHour   = (hhmm_time / 100) % 100;
  
  rtc.adjust(DateTime(0, 0, 0, timeHour, timeMinute, 0));             
}

void DisplayTime()
{
  DateTime now = rtc.now();
 
  byte timeHour = now.hour();
  byte timeFormat = hourFormat;
    
  // Check time format and adjust
  if(timeFormat == 12 && timeHour > 12) timeHour = timeHour - 12;
  if(timeFormat == 12 && timeHour == 0) timeHour = 12; 

  byte timeMinute = now.minute();
  byte timeSecond = now.second();

  Serial.print("Time: ");
  if(timeHour < 10)   Serial.print("0");
  Serial.print(timeHour);
  Serial.print(":");
  if(timeMinute < 10) Serial.print("0");
  Serial.print(timeMinute);  
  Serial.print(":");
  if(timeSecond < 10) Serial.print("0");
  Serial.println(timeSecond);      

  int digit;

  // Underscore symbol turn on for multisegment tubes
  hourUnderscore = 1; 
  
  // Extract individual digits
  digit  = (timeHour / 10) % 10;
  NixieDisplay(digit, hour_color);
  
  digit  = (timeHour / 1)  % 10;
  NixieDisplay(digit, hour_color);
    
  DelayTime(400);

  // Underscore symbol turn off for multisegment tubes
  hourUnderscore = 0; 
  
  digit  = (timeMinute / 10) % 10;
  NixieDisplay(digit, minute_color); 
   
  digit  = (timeMinute / 1)  % 10;
  NixieDisplay(digit, minute_color);    
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

void NixieDisplay(uint16_t digit, uint32_t backlight_color)
{
  if(DetectNixieTube() == true) ShowSymbol(digit, backlight_color);
  else ShowDigit(digit, backlight_color);
}

// PWM fade in/out effect
// To modify the effect, you need to change the number of PWM loop steps "i = i - steps" 
// and/or the value (16, 32, 48) of "DelayTime(value);"
void ShowDigit(uint16_t digit, uint32_t backlight_color)
{         
  ShiftOutData(digit_nixie_tube[digit]);
    
  // fade in from min to max in decrements of 8 points
  for (int i = 255 ; i >= 0; i = i - 8) 
  {
    if(i == 7) i = 0;                       // 255/8 = 31 steps and rest 7  
    analogWrite(PWM_PIN, i);
    led.setBrightness(255 - i);             // Set brightness
    led.fill(backlight_color);              // Fill all LEDs with a color
    led.show();                             // Update LEDs
      
    // wait for 16 milliseconds to see the fade in effect
    DelayTime(16);
  }  

  DelayTime(400);

  // fade out from max to min in increments of 8 points
  for (int i = 0 ; i <= 255; i = i + 8) 
  {
    if(i == 248) i = 255;                   // 255/8 = 31 steps and rest 7 
    analogWrite(PWM_PIN, i);
    led.setBrightness(255 - i);             // Set brightness
    led.fill(backlight_color);              // Fill all LEDs with a color
    led.show();                             // Update LEDs

    // wait for 16 milliseconds to see the fade out effect
    DelayTime(16);
  } 
  
  ClearNixieTube();
  DelayTime(200);   
}

// PWM fade in/out effect
// To modify the effect, you need to change the number of PWM loop steps "i = i - steps" 
// and/or the value (16, 32, 48) of "DelayTime(value);"
void ShowSymbol(uint16_t digit, uint32_t backlight_color)
{        
  uint16_t currentDigit;

  if(hourUnderscore == 1) currentDigit = symbol_nixie_tube[digit] | hour_symbol;
  else currentDigit = symbol_nixie_tube[digit];   
   
  ShiftOutData(currentDigit);
    
  // fade in from min to max in decrements of 8 points
  for (int i = 255 ; i >= 0; i = i - 8) 
  {
    if(i == 7) i = 0;                       // 255/8 = 31 steps and rest 7  
    analogWrite(PWM_PIN, i);
    led.setBrightness(255 - i);             // Set brightness
    led.fill(backlight_color);              // Fill all LEDs with a color
    led.show();                             // Update LEDs
      
    // wait for 16 milliseconds to see the fade in effect
    DelayTime(16);
  }  

  DelayTime(400);

  // fade out from max to min in increments of 8 points
  for (int i = 0 ; i <= 255; i = i + 8) 
  {
    if(i == 248) i = 255;                   // 255/8 = 31 steps and rest 7 
    analogWrite(PWM_PIN, i);
    led.setBrightness(255 - i);             // Set brightness
    led.fill(backlight_color);              // Fill all LEDs with a color
    led.show();                             // Update LEDs

    // wait for 16 milliseconds to see the fade out effect
    DelayTime(16);
  } 
  
  ClearNixieTube();  
  DelayTime(200); 
}

// Turn off nixie tube
void ClearNixieTube()
{
  ShiftOutData(0);  
}

// Recalculated delay time
void DelayTime(uint16_t wait)
{
  // delay() affected by new TCA0 settings (PWM frequency)
  // New delay time = delay time / 16
  delay(wait/16);
}

void CathodePoisoningPrevention()
{
  DelayTime(1000);
  analogWrite(PWM_PIN, 0);

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

void ShiftOutData(uint16_t character)
{ 
  uint8_t first_half = character >> 8;  
  uint8_t second_half = character;     
  digitalWrite(EN_PIN, LOW);
  shiftOut(DIN_PIN, CLK_PIN, MSBFIRST, first_half);
  shiftOut(DIN_PIN, CLK_PIN, MSBFIRST, second_half);
      
  // Return the latch pin high to signal chip that it
  // no longer needs to listen for information
  digitalWrite(EN_PIN, HIGH);
}
