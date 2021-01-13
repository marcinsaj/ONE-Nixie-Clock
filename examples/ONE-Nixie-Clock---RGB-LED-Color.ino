// LED backlight RGB test example
// ONE Nixie Clock by Marcin Saj https://nixietester.com
// https://github.com/marcinsaj/ONE-Nixie-Clock
//
// Hardware:
// Arduino Nano/Nano Every/Nano 33 IoT 
// ONE Nixie Clock Arduino Shield
// 12V Power Supply

#include <Adafruit_NeoPixel.h>
// https://github.com/adafruit/Adafruit_NeoPixel
// https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN     A3

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT  4

// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 255

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
  led.setBrightness(255);                 // Set BRIGHTNESS 0-255
}

void loop() 
{
  PixelColor();                           // Set LEDs RGB colors
  PixelColorArray();                      // Set LEDs RGB colors using color array
  
  FillColor();                            // Fill all LEDs with a color
  FillColorArray();                       // Fill all LEDs with a color using color array
  
  FadeColor();                            // Fade in/out LEDs   
  FadeColorArray();                       // Fade in/out LEDs using color array
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
    led.setPixelColor(i, led_array_color[i]);    // Set color red for LED 0 
    led.show();                                  // Update LEDs
    delay(500);    
  }
  
  led.clear();                            // Turn off LEDs
  led.show();                             // Update LEDs   
  delay(1000);  
}

void FillColor()
{
  led.setBrightness(255);                 // Set current brightness 
  
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
}

void FillColorArray()
{
  led.setBrightness(255);                 // Set current brightness 

  for(int i = 0; i < 5; i++)
  {
    led.fill(led_array_color[i]);         // Fill all LEDs with a color red
    led.show();                           // Update LEDs
    delay(1000);  
  }    
  
  led.clear();                            // Turn off LEDs
  led.show();                             // Update LEDs   
  delay(1000);
}  

void FadeColor()
{
  for (int i = 0; i <= 255; i ++) 
  {
    led.setBrightness(i);                 // Set current brightness      
    led.fill(red_color);                  // Fill all LEDs with a color
    delay(5); 
    led.show();                           // Update LEDs
  }
    
  for (int i = 255; i >= 0; i--) 
  {
    led.setBrightness(i);                 // Set current brightness      
    led.fill(red_color);                  // Fill all LEDs with a color
    delay(5); 
    led.show();                           // Update LEDs
  }         
}

void FadeColorArray()
{
  for (int count = 0; count < 5; count++)
  {
    for (int i = 0; i <= 255; i ++) 
    {
      led.setBrightness(i);               // Set current brightness      
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
     
  led.clear();                            // Turn off LEDs
  led.show();                             // Update LEDs   
  delay(1000); 
}
