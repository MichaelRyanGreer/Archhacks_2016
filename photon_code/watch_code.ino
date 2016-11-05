include "Adafruit-CAP1188.h"

// This #include statement was automatically added by the Particle IDE.
#include "neopixel/neopixel.h"
#include "application.h"

SYSTEM_MODE(AUTOMATIC);

#define PIXEL_PIN D2
#define PIXEL_COUNT 12
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

Adafruit_CAP1188 cap = Adafruit_CAP1188();

#define buzzPin D3 //Buzzer Pin

#define zPin A0 //Accelerometer Pins
#define yPin A1
#define xPin A2

#define sw0 D4 //Button Pins
#define sw1 D5

bool buzz_on = false;

float accel_threshhold = 0.65;

int state = 0;

void setup() {
    //Initializes pinmodes
    pinMode(buzzPin, OUTPUT);
    pinMode(zPin, INPUT);
    pinMode(yPin, INPUT);
    pinMode(xPin, INPUT);
    pinMode(sw0, INPUT_PULLUP);
    pinMode(sw1, INPUT_PULLUP);
    
    //Sets up the Neopixel ring
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    
    //Starts debug connection
    //Serial.begin(19200);
    
    //Starts capacitor
    cap.begin();

    //Sets sensitivity from 1 to 7
    cap.setSensitivity(2);
}


int capOffCounter = 0;
int capOffLimit = 10;

void loop() {
    
    // Finite state machine
    switch (state) {
        case 0:
            //Not Worn
            if(cap.touchedAnalog(0) < capCutoff){
                state = 0;
            }
            else{
                state = 1;
            }
             break;
        case 1:
            //Worn Safe
            if((cap.touchedAnalog(0) < capCutoff) && (capOffCounter < capOffLimit)){
                capOffCounter += 1;
            }
            else if((cap.touchedAnalog(0) < capCutoff) && (capOffCounter > capOffLimit)){
                capOffCounter = 0;
                state = 0;
            }
            else if(greenButtonPressed){
                states = 5;
            }
            else if((redButtonMashed) || (accelHigh)){
                states = 2;
            }
            else{
                state = 1;
            }
            break;
        case 2:

            break;
        case 3:

            break;
        case 4:
            break;
        case 5:
            break;
    }       
  
}


//Toggles the Buzzer on and off
void buzz() {
    tone(buzzPin,3000,0);
}

//Tests the LED Ring
void testRing() {
for (int i = 0; i<12; i++)
  {
    strip.setPixelColor(i,10,0,0);
    strip.show();
    delay(100);
  }
  for (int i = 0; i<12; i++)
  {
    strip.setPixelColor(i,0,0,0);
    strip.show();
    delay(100);
  }
  for (int i = 0; i<12; i++)
  {
    strip.setPixelColor(i,0,10,0);
    strip.show();
    delay(100);
  }
  for (int i = 0; i<12; i++)
  {
    strip.setPixelColor(i,0,0,0);
    strip.show();
    delay(100);
  }
  for (int i = 0; i<12; i++)
  {
    strip.setPixelColor(i,0,0,10);
    strip.show();
    delay(100);
  }
  for (int i = 0; i<12; i++)
  {
    strip.setPixelColor(i,0,0,0);
    strip.show();
    delay(100);
  }
}

void testAccel(){
    Serial.print("z: ");
    Serial.print((float)analogRead(zPin)/4095);
    Serial.print(" y: ");
    Serial.print((float)analogRead(yPin)/4095);
    Serial.print(" x: ");
    Serial.println((float)analogRead(xPin)/4095);
    delay(100);
}
