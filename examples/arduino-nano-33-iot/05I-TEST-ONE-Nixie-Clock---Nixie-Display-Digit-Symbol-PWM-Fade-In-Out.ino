// ONE Nixie Clock by Marcin Saj https://nixietester.com
// https://github.com/marcinsaj/ONE-Nixie-Clock
//
// Nixie Counter Example with PWM fade in/out effect
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

// Blue backlight color
uint32_t backlight = led.Color(0, 0, 255);

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


int16_t digit[]={
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


uint16_t symbol[]={
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
// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN     A3

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT  4

#define EN_PIN      A1
#define CLK_PIN     A2
#define DIN_PIN     A0
#define EN_NPS_PIN  13
// #define PWM_PIN     10  // Declared as PA21, find below in code

// PWM frequency can be calculated by
// freq = GCLK4_freq / (TCC0_prescaler * (1 + period))
// With value 100, we get a 75Hz
uint32_t period = 100 - 1;

void setup() 
{  
// Enable and configure generic clock generator 4
  GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC |          // Improve duty cycle
                      GCLK_GENCTRL_GENEN |        // Enable generic clock gen
                      GCLK_GENCTRL_SRC_DFLL48M |  // Select 48MHz as source
                      GCLK_GENCTRL_ID(4);         // Select GCLK4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Set clock divider of 25 to generic clock generator 4
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(25) |        // Divide 48 MHz by 25
                     GCLK_GENDIV_ID(4);           // Apply to GCLK4 4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization
  
  // Enable GCLK4 and connect it to TCC0 and TCC1
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |        // Enable generic clock
                      GCLK_CLKCTRL_GEN_GCLK4 |    // Select GCLK4
                      GCLK_CLKCTRL_ID_TCC0_TCC1;  // Feed GCLK4 to TCC0/1
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

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


  led.begin();                                    // Initialize NeoPixel led object
  led.show();                                     // Turn OFF all pixels ASAP
  led.setBrightness(255);                         // Set brightness 0-255  
 
  pinMode(EN_NPS_PIN, OUTPUT);
  digitalWrite(EN_NPS_PIN, HIGH);                 // Turn OFF nixie power supply module 

  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);
  
  pinMode(CLK_PIN, OUTPUT);
  digitalWrite(CLK_PIN, LOW);
  
  pinMode(DIN_PIN, OUTPUT);
  digitalWrite(DIN_PIN, LOW);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  
      
  digitalWrite(EN_NPS_PIN, LOW);                  // Turn ON nixie power supply module      
}

void loop() 
{
  NixieDisplay(); 
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

void StartPWM()
{
  TCC0->CTRLBSET.reg = TCC_CTRLBCLR_CMD_RETRIGGER;    // Restart the timer
  while(TCC0->SYNCBUSY.bit.CTRLB);                    // Wait for synchronization
}

void StopPWM ()
{
  TCC0->CTRLBSET.reg = TCC_CTRLBCLR_CMD_STOP;         // Stop the timer
  while(TCC0->SYNCBUSY.bit.CTRLB);                    // Wait for synchronization
}

void NixieDisplay()
{
  if(DetectNixieTube() == true) ShowSymbol();
  else ShowDigit();
}

void ShowDigit() 
{
  for(int count = 0; count <= 9; count ++)
  {
    StartPWM();
    delay(200);

    ShiftOutData(digit[count]);
          
    for(int i = 100; i > 0; i = i - 2)
    { 
      TCC0->CC[3].reg = i;
      while (TCC0->SYNCBUSY.bit.CC2);

      StopPWM();

      led.setBrightness(255 - (i *2.55));     // Set brightness 0 - 255
      led.fill(backlight);                    // Fill all LEDs with a color
      led.show();                             // Update LEDs
      
      StartPWM();
      delay(10);
    }

    delay(300);

    for(int i = 0; i <= 100 ; i = i + 2)
    { 
      TCC0->CC[3].reg = i;
      while (TCC0->SYNCBUSY.bit.CC2);

      led.setBrightness(255 - (i *2.55));     // Set brightness 255 - 0
      led.fill(backlight);                    // Fill all LEDs with a color
      led.show();                             // Update LEDs

      delay(15);
    }              
  }        
}

void ShowSymbol() 
{
  for(int count = 0; count <= 36; count ++)
  {
    StartPWM();
    delay(200);

    ShiftOutData(symbol[count]);
          
    for(int i = 100; i > 0; i = i - 2)
    { 
      TCC0->CC[3].reg = i;
      while (TCC0->SYNCBUSY.bit.CC2);

      StopPWM();

      led.setBrightness(255 - (i *2.55));     // Set brightness 0 - 255
      led.fill(backlight);                    // Fill all LEDs with a color
      led.show();                             // Update LEDs
      
      StartPWM();
      delay(10);
    }

    delay(300);

    for(int i = 0; i <= 100 ; i = i + 2)
    { 
      TCC0->CC[3].reg = i;
      while (TCC0->SYNCBUSY.bit.CC2);

      led.setBrightness(255 - (i *2.55));     // Set brightness 255 - 0
      led.fill(backlight);                    // Fill all LEDs with a color
      led.show();                             // Update LEDs

      delay(15);
    }              
  }        
}

void ShiftOutData(uint16_t character)
{ 
  uint8_t second_half = character;
  uint8_t first_half = character >> 8;       
  digitalWrite(EN_PIN, LOW);
  shiftOut(DIN_PIN, CLK_PIN, MSBFIRST, first_half);
  shiftOut(DIN_PIN, CLK_PIN, MSBFIRST, second_half);
      
  // Return the latch pin high to signal chip that it
  // no longer needs to listen for information
  digitalWrite(EN_PIN, HIGH);
}
