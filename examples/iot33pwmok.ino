// Number to count to with PWM (TOP value). Frequency can be calculated by
// freq = GCLK4_freq / (TCC0_prescaler * (1 + TOP_value))
// With TOP of 47, we get a 1 MHz square wave in this example
uint32_t period = 100 - 1;


uint16_t digits[]={
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
uint16_t digit[]={
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

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN     A3

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT  4

#define EN_PIN      A1
#define CLK_PIN     A2
#define DIN_PIN     A0
#define EN_NPS_PIN  13
#define PWM_PIN     10

 
void setup() {
  Serial.begin(9600);

  pinMode(EN_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(DIN_PIN, OUTPUT);
 // delay(5000);

  pinMode(EN_NPS_PIN, OUTPUT);
  digitalWrite(EN_NPS_PIN, LOW);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  

  pinMode(PWM_PIN, OUTPUT);
//  digitalWrite(PWM_PIN, HIGH); 


  // Because we are using TCC0, limit period to 24 bits
  period = ( period < 0x00ffffff ) ? period : 0x00ffffff;

  // Enable and configure generic clock generator 4
  GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC |          // Improve duty cycle
                      GCLK_GENCTRL_GENEN |        // Enable generic clock gen
                      GCLK_GENCTRL_SRC_DFLL48M |  // Select 48MHz as source
                      GCLK_GENCTRL_ID(4);         // Select GCLK4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Set clock divider of 1 to generic clock generator 4
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(24) |         // Divide 48 MHz by 1
                     GCLK_GENDIV_ID(4);           // Apply to GCLK4 4
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization
  
  // Enable GCLK4 and connect it to TCC0 and TCC1
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |        // Enable generic clock
                      GCLK_CLKCTRL_GEN_GCLK4 |    // Select GCLK4
                      GCLK_CLKCTRL_ID_TCC0_TCC1;  // Feed GCLK4 to TCC0/1
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Divide counter by 1 giving 48 MHz (20.83 ns) on each TCC0 tick
  TCC0->CTRLA.reg |= TCC_CTRLA_PRESCALER(TCC_CTRLA_PRESCALER_DIV1_Val);

  // Use "Normal PWM" (single-slope PWM): count up to PER, match on CC[n]
  TCC0->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM;         // Select NPWM as waveform
  while (TCC0->SYNCBUSY.bit.WAVE);                // Wait for synchronization

  // Set the period (the number to count to (TOP) before resetting timer)
  TCC0->PER.reg = period;
  while (TCC0->SYNCBUSY.bit.PER);

  // Set PWM signal to output 50% duty cycle
  // n for CC[n] is determined by n = x % 4 where x is from WO[x]
  TCC0->CC[3].reg = period / 1;
  while (TCC0->SYNCBUSY.bit.CC2);

  // Configure PA18 (D10 on Arduino Zero) to be output
  PORT->Group[PORTA].DIRSET.reg = PORT_PA21;      // Set pin as output
  PORT->Group[PORTA].OUTCLR.reg = PORT_PA21;      // Set pin to low

  // Enable the port multiplexer for PA18
  PORT->Group[PORTA].PINCFG[21].reg |= PORT_PINCFG_PMUXEN;

  // Connect TCC0 timer to PA18. Function F is TCC0/WO[2] for PA18.
  // Odd pin num (2*n + 1): use PMUXO
  // Even pin num (2*n): use PMUXE
  PORT->Group[PORTA].PMUX[10].reg = PORT_PMUX_PMUXO_F;

  // Enable output (start PWM)
  TCC0->CTRLA.reg |= (TCC_CTRLA_ENABLE);
  while (TCC0->SYNCBUSY.bit.ENABLE);              // Wait for synchronization

  
}

void loop() 
{

for(int count = 0; count <= 9; count ++)
{

          StartPWM();
          delay(200);

          
         NixieDisplay(digits[count]);
          
for(int i = 100; i > 40; i--)
{ 
   TCC0->CC[3].reg = i;
    while (TCC0->SYNCBUSY.bit.CC2);

    Serial.println(i);
    delay(10);
}

//StopPWM();
digitalWrite(PWM_PIN, LOW);
delay(100);
StartPWM();

for(int i = 90; i <= 100 ; i++)
{ 
   TCC0->CC[3].reg = i;
    while (TCC0->SYNCBUSY.bit.CC2);

    Serial.println(i);
    delay(40);
}
                 
}        
}

void StartPWM()
{
  TCC0->CTRLBSET.reg = TCC_CTRLBCLR_CMD_RETRIGGER;  // Restart a stopped timer or reset a stopped one
while(TCC0->SYNCBUSY.bit.CTRLB);                  // Wait for synchronization
}

void StopPWM ()
{
  TCC0->CTRLBSET.reg = TCC_CTRLBCLR_CMD_STOP;       // Stop the timer
while(TCC0->SYNCBUSY.bit.CTRLB);                  // Wait for synchronization
}

void NixieDisplay(uint16_t character)
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