// ONE Nixie Clock by Marcin Saj https://nixietester.com
// https://github.com/marcinsaj/ONE-Nixie-Clock
//
// Test Example - Classic Nixie Clock without PWM fade in/out effect and without backlight
// Code only for testing because the Arduino Nano PWM default frequency value is too low 
// and "singing tube" audible noise may occur.
//
// This example demonstrates how to set and display time
//
// Hardware:
// ONE Nixie Clock Arduino Shield - https://nixietester.com/project/one-nixie-clock
// Arduino Nano - https://store.arduino.cc/arduino-nano
// Or Arduino Nano Every - https://store.arduino.cc/arduino-nano-every
// Or Arduino Nano IoT 33 - https://store.arduino.cc/arduino-nano-33-iot
// Nixie Tube Socket - http://bit.ly/nixie-socket & http://bit.ly/NixieSocket-Project
// Nixie Power Supply Module and RTC DS3231 Module
// Nixie clock require 12V, 1.5A power supply
// Schematic ONE Nixie Clock - http://bit.ly/ONE-Nixie-Clock-Schematic
// Schematic Nixie Power Supply Module - http://bit.ly/ONE-Nixie-Clock-NPS-Module
// DS3231 RTC datasheet: https://datasheets.maximintegrated.com/en/ds/DS3231.pdf

#include <RTClib.h>          
// https://github.com/adafruit/RTClib

// RTC library declaration
RTC_DS3231 rtc;

// Choose Time Format *******************************************************
#define hourFormat    12     // 12 Hour Clock or 24 Hour Clock
// **************************************************************************

// Shift registers control pins
#define DIN_PIN     A0
#define EN_PIN      A1
#define CLK_PIN     A2

// Nixie Power Supply Module control pin
#define EN_NPS_PIN  13 

// The clock has a built-in detection mechanism 
// for 15 segment nixie tubes (e.g. B-7971, B-8971)
#define DETECT_PIN  A6    

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
  delay(5000);
  
  pinMode(EN_NPS_PIN, OUTPUT);
  digitalWrite(EN_NPS_PIN, HIGH);         // Turn OFF nixie power supply module 

  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);
  
  pinMode(CLK_PIN, OUTPUT);
  digitalWrite(CLK_PIN, LOW);
  
  pinMode(DIN_PIN, OUTPUT);
  digitalWrite(DIN_PIN, LOW); 
      
  Serial.println("#############################################################");
  Serial.println("------------- Test Example - Classic Nixie Clock ------------");
  Serial.println("---------------- If you want to set new Time ----------------");
  Serial.println("--------------- press ENTER within 10 seconds ---------------");

  // Millis time start
  unsigned long millis_time_now = millis();
  unsigned long millis_time_now_2 = millis();
    
  // Wait 5 seconds
  while((millis() < millis_time_now + 10000))
  {    
    // Print progress bar      
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
  }    
}

void loop() 
{
  // Set a new time if settings have been selected
  if(serialState == 1)
  {
    SetNewTime();
    serialState = 0;
    
    // Turn ON nixie power supply module
    digitalWrite(EN_NPS_PIN, LOW);             
  }    
    
  // Get time from RTC and display on nixie tubes
  DisplayTime();
  delay(2500);
}

void SetNewTime()
{  
  Serial.println("--------------- Enter the TIME without spaces ----------------");
  Serial.println("--------------- in the HHMM format e.g. 0923 -----------------");
  Serial.println("- and press enter when you are ready to send data to the RTC -");
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
  // Extract individual digits
  digit  = (timeHour / 10) % 10;
  NixieDisplay(digit);
  
  digit  = (timeHour / 1)  % 10;
  NixieDisplay(digit);
    
  delay(600);
  
  digit  = (timeMinute / 10) % 10;
  NixieDisplay(digit); 
   
  digit  = (timeMinute / 1)  % 10;
  NixieDisplay(digit);    
}

// If a high state appears on the analog input, 
// it means that a multi-segment tube socket has been inserted
bool DetectNixieTube()
{
  analogDetectInput = analogRead(DETECT_PIN);
  // 0 - 1024, Detecting anything above 0 means true
  // 900 is for sure 
  if(analogDetectInput >= 900) return(true);
  else return(false);  
}

void NixieDisplay(uint16_t digit)
{
  if(DetectNixieTube() == true) ShowSymbol(digit);
  else ShowDigit(digit);
}

// PWM fade in/out effect
void ShowDigit(uint16_t digit)
{       
  ShiftOutData(digit_nixie_tube[digit]);
  delay(600);
  ClearNixieTube();
  delay(150);   
}

// PWM fade in/out effect
void ShowSymbol(uint16_t digit)
{       
  ShiftOutData(symbol_nixie_tube[digit]);
  delay(600);
  ClearNixieTube(); 
  delay(150);    
}

// Turn off nixie tube
void ClearNixieTube()
{
  ShiftOutData(0);  
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
