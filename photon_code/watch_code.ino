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

#define sw0 D4 //Button Pins red (low pressed)
#define sw1 D5 //green

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

int loopDelay = 10;
int capCutOffVoltage = 110;
int capOffCounter = 0;
int capOffCountLimit = 10;
int greenButton = 0;
int redButton = 0;
int redButtonCounter = 0;
int redButtonCounterLimit = 4;
float zAccel = 0;
float yAccel = 0;
float xAccel = 0;
float accelMagnitude = 0;
float accelLimit = .6; //Max is .866
int state1Timer = 0;
int state1TimerLimit = 4/(loopDelay/1000);
int state2Timer = 0;
int state2TimerLimit = 10/(loopDelay/1000);
	
void loop() {
	//Update Constants
	findAccel();
	greenButton = !sw1;
	redButton = !sw0;
	
    // Finite state machine
    switch (state) {
        case 0:
            //Not Worn
            if(cap.touchedAnalog(0) < capCutOffVoltage){
                state = 0;
            }
            else{
                state = 1;
            }
             break;
        case 1:
            //Worn Safe
			state1Timer += 1;
			if(state1Timer > state1TimerLimit){
					redButtonCounter = 0;
					state1Timer = 0;	
			}
			
			if(redButton){
				capOffCounter = 0;
				if(redButtonCounter < redButtonCounterLimit){
					redButtonCounter += 1;
				}
				else{
					redButtonCounter = 0;
					capOffCounter = 0;
					state = 2;
				}
            }
			if(accelMagnitude > accelLimit){
				redButtonCounter = 0;
				capOffCounter = 0;
				state = 2;
			}
			else if(greenButton){
				redButtonCounter = 0;
				capOffCounter = 0;
                state = 5;
            }
            else if(cap.touchedAnalog(0) < capCutOffVoltage){
				if(capOffCounter < capOffCounterLimit){
					capOffCounter += 1;
				}
				else{
					redButtonCounter = 0;
					capOffCounter = 0;
					state = 0;
				}
            }
            else{
                state = 1;
            }
            break;
        case 2:
			//Check Response

			LEDRingRed();
			buzz();
			
			state2Timer += 1;

			if(red){
				state 3;	
			}
			else if(state2Timer > state2TimerLimit){
				state2Timer = 0;
				state = 3;	
			}
			else if(green){
				state2Timer = 0;
				state = 1;	
			}
			else{
				state = 2;
			}
            break;
        case 3:
			//Alert
			
			//CODE TO SEND TEXTS
			LEDRingRed();
			buzz();
			
			state = 4;
            break;
        case 4:
			//Wait For Help
			
			LEDRingRed();
			buzz();
			
			if(green){
				state = 1;
			}
			else{
				state = 4;
			}
            break;
        case 5:
			//Show Time

			displayTime();
			state = 1;
            break;
    }       
  
	delay(loopDelay);
}


//Toggles the Buzzer on and off
void buzz() {
    tone(buzzPin,3000,0);
}

//Tests the LED Ring
void LEDRingRed() {
for (int i = 0; i<12; i++){
    strip.setPixelColor(i,10,0,0);
    strip.show();
    delay(100);
  }
	for (int i = 0; i<12; i++){
    strip.setPixelColor(i,0,0,0);
    strip.show();
  }
}

void findAccel(){
	zAccel = (float)analogRead(zPin)/4095-.5;
	yAccel = (float)analogRead(yPin)/4095-.5;
	xAccel = (float)analogRead(xPin)/4095-.5;
	accelMagnitude = sqrt(zAccel*zAccel+yAccel*yAccel+xAccel*xAccel);
}

void displayTime() {

	// Print the current Unix timestamp
	time_t unixTime = Time.now();
	int time =  (int)unixTime%31556926;
		time = time%2629743;	
		time = time%604800;
		time = time%86400;
	//Now time = num seconds into day
	int hour = 0;
	int min = 0;
	hour = time/3600;
	time = time%3600;	
	min = time/60;
	Serial.println("hour: " + hour + " min: " + min);

    strip.setPixelColor(hour,0,10,0);
    strip.show();
    delay(100);
	
	for (int i = 0; i<12; i++){
    	strip.setPixelColor(i,0,0,0);
	    strip.show();
 	}
	
	strip.setPixelColor(min,0,10,0);
    strip.show();
    delay(100);
	
	for (int i = 0; i<12; i++){
    	strip.setPixelColor(i,0,0,0);
	    strip.show();
 	}
}
