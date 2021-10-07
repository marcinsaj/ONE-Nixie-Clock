// ONE Nixie Clock by Marcin Saj https://nixietester.com
// https://github.com/marcinsaj/ONE-Nixie-Clock
//
// LED backlight RGB test example
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

uint32_t led_array_color[5] = 
{
  led.Color(255, 0, 0),                   // Red color
  led.Color(0, 255, 0),                   // Green color
  led.Color(0, 0, 255),                   // Blue color
  led.Color(255, 255, 255),               // White color
  led.Color(255, 0, 255)                  // Magenta color
};    


void setup() 
{  
  led.begin();                            // Initialize NeoPixel led object
  led.show();                             // Turn OFF all pixels ASAP
  led.setBrightness(255);                 // Set brightness 0-255
}

void loop() 
{ 
  PixelColor();                           // Set LEDs RGB colors
  PixelColorArray();                      // Set LEDs RGB colors using color array
  
  FillColor();                            // Fill all LEDs with a color
  FillColorArray();                       // Fill all LEDs with a color using color array
  
  FadeColor();                            // Fade in/out LEDs   
  FadeColorArray();                       // Fade in/out LEDs using color array

  CrossfadeColor();                       // Crossfade colors
}

void PixelColor()
{
  led.setBrightness(255);                 // Set brightness 

  led.setPixelColor(0, red_color);        // Set color red for LED 0 
  led.show();                             // Update LEDs
  delay(500);
  
  led.setPixelColor(1, green_color);      // Set color green for LED 1 
  led.show();                             // Update LEDs
  delay(500);
  
  led.setPixelColor(2, blue_color);       // Set color blue for LED 2 
  led.show();                             // Update LEDs
  delay(500);
  
  led.setPixelColor(3, white_color);      // Set color white for LED 3 
  led.show();                             // Update LEDs
  delay(500);

  led.clear();                            // Turn off LEDs
  led.show();                             // Update LEDs   
  delay(1000); 
}

void PixelColorArray()
{
  led.setBrightness(255);                 // Set brightness

  for(int i = 0; i < 4; i ++)
  {
    led.setPixelColor(i, led_array_color[i]);    // Set LEDs colors  
    led.show();                                  // Update LEDs
    delay(500);    
  }
  
  led.clear();                            // Turn off LEDs
  led.show();                             // Update LEDs   
  delay(1000);  
}

void FillColor()
{
  led.setBrightness(255);                 // Set brightness 
  
  // led.fill(red_color);
  led.fill(led.Color(255, 0, 0));         // Fill all LEDs with a color red
  led.show();                             // Update LEDs
  delay(1000);

  // led.fill(green_color);
  led.fill(led.Color(0, 255, 0));         // Fill all LEDs with a color green
  led.show();                             // Update LEDs
  delay(1000);
  
  // led.fill(blue_color);
  led.fill(led.Color(0, 0, 255));         // Fill all LEDs with a color blue
  led.show();                             // Update LEDs  
  delay(1000);

  // led.fill(white_color);
  led.fill(led.Color(255, 255, 255));     // Fill all LEDs with a color white
  led.show();                             // Update LEDs  
  delay(1000);

  // led.fill(magenta_color);
  led.fill(led.Color(255, 0, 255));       // Fill all LEDs with a color magenta
  led.show();                             // Update LEDs  
  delay(1000);

  led.clear();                            // Turn off LEDs
  led.show();                             // Update LEDs   
  delay(1000);
}

void FillColorArray()
{
  led.setBrightness(255);                 // Set brightness 

  for(int i = 0; i < 5; i++)
  {
    led.fill(led_array_color[i]);         // Fill all LEDs with a color
    led.show();                           // Update LEDs
    delay(1000);  
  }    
  
  led.clear();                            // Turn off LEDs
  led.show();                             // Update LEDs   
  delay(1000);
}  

void FadeColor()
{
  for (int i = 0; i <= 255; i ++)         // Fade in to red
  {
    led.setBrightness(i);                 // Set brightness      
    led.fill(red_color);                  // Fill all LEDs with a color
    delay(5); 
    led.show();                           // Update LEDs
  }
    
  for (int i = 255; i >= 0; i--)          // Fade out 
  {
    led.setBrightness(i);                 // Set current brightness      
    led.fill(red_color);                  // Fill all LEDs with a color
    delay(5); 
    led.show();                           // Update LEDs
  }   

  delay(1000);
}

void FadeColorArray()
{
  for (int count = 0; count < 5; count++)
  {
    for (int i = 0; i <= 255; i ++) 
    {
      led.setBrightness(i);               // Set brightness      
      led.fill(led_array_color[count]);   // Fill all LEDs with a color
      delay(5); 
      led.show();                         // Update LEDs
    }
    
    for (int i = 255; i >= 0; i--) 
    {
      led.setBrightness(i);               // Set current brightness      
      led.fill(led_array_color[count]);   // Fill all LEDs with a color
      delay(5); 
      led.show();                         // Update LEDs
    }         
  }
  
  delay(1000); 
}

void CrossfadeColor()
{
  led.setBrightness(255);                 // Set brightness 
  
  for(int i = 0; i <= 255; i++)           // Fade in to red
  {
    led.fill(led.Color(i, 0, 0)); 
    delay(5);   
    led.show();
  }                            

  for(int i = 0; i <= 255; i++)           // Crossfade to magenta
  {
    led.fill(led.Color(255, 0, i)); 
    delay(5);
    led.show();   
  }

  for(int i = 0; i <= 255; i++)           // Crossfade to blue
  {
    led.fill(led.Color(255 - i, 0, 255)); 
    delay(5);
    led.show();   
  }

  for(int i = 0; i <= 255; i++)           // Crossfade to green
  {
    led.fill(led.Color(0, i, 255 - i)); 
    delay(5);      
    led.show(); 
  } 

  for(int i = 0; i <= 255; i++)           // Crossfade to red
  {
    led.fill(led.Color(i, 255 - i, 0)); 
    delay(5);     
    led.show(); 
  } 

  for(int i = 0; i <= 255; i++)           // Fade out
  {
    led.fill(led.Color(255 - i, 0, 0)); 
    delay(5);  
    led.show();   
  } 
 
  delay(1000);    
}
