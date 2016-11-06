#include "Adafruit-CAP1188.h"
#include "Particle.h"

// This #include statement was automatically added by the Particle IDE.
#include "neopixel/neopixel.h"
#include "application.h"


#include "math.h"

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

IPAddress ip;
String ipStr;

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
    
    //Sets current ip
    ip = WiFi.localIP();
    ipStr = String(ip[0])+"."+String(ip[1])+"."+String(ip[2])+"."+String(ip[3]);
}

float loopDelay = 10;
int capCutOffVoltage = 90;
int capOffCounter = 0;
int capOffCounterLimit = 10;
int greenButton = 0;
int redButton = 0;
int redButtonCounter = 0;
int redButtonCounterLimit = 10;
float zAccel = 0;
float yAccel = 0;
float xAccel = 0;
float accelMagnitude = 0;
float accelLimit = .23; //Max is .866
int state1Timer = 0;
float state1TimerLimit = 4/(loopDelay/1000);
int state2Timer = 0;
float state2TimerLimit = 10/(loopDelay/1000);
	
void loop() {
    //Serial.println(state);
	//Update Constants
	findAccel();
	greenButton = !digitalRead(sw1);
	redButton = !digitalRead(sw0);
	
    // Finite state machine
    switch (state) {
        case 0:
            // Not Worn
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
			
			if(redButton == 1){
				capOffCounter = 0;
				if(redButtonCounter < redButtonCounterLimit){
					redButtonCounter += 1;
				}
				else{
					redButtonCounter = 0;
					capOffCounter = 0;
					state = 2;
					break;
				}
            }
			if(accelMagnitude > accelLimit){
				redButtonCounter = 0;
				capOffCounter = 0;
				state = 2;
			}
			else if(greenButton == 1){
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
            
            if(state2Timer == 0){
			    LEDRingRedOn();
            }
				   		
			buzz(true);

			state2Timer += 1;

			if((redButton == 1) && (state2Timer > 50)){
			    buzz(true);
			   	state2Timer = 0;
				state = 3;	
			}
			else if(state2Timer > state2TimerLimit){
				state2Timer = 0;
				buzz(true);
				state = 3;	
			}
			else if(greenButton == 1){
				state2Timer = 0;
				buzz(false);
			    LEDRingRedOff();
				state = 1;
				delay(150);
			}
			else{
				state = 2;
			}
            break;
        case 3:
            // Send info to server
			Particle.publish("emergency",ipStr); // Sends patient info as a POST to server

			state = 4;
            break;
        case 4:
			// Wait For Help
			if(greenButton == 1){
			    buzz(false);
			    LEDRingRedOff();
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
void buzz(bool on) {
    if(on == true){
        tone(buzzPin,3000,0);
    }
    if(on == false){
        noTone(buzzPin);
    }
}

//Tests the LED Ring
void LEDRingRedOn() {
    for (int i = 0; i<12; i++){
        strip.setPixelColor(i,10,0,0);
        strip.show();
        delay(100);
    }
}

// Turns ring off
void LEDRingRedOff() {
    for (int i = 0; i<12; i++){
        strip.setPixelColor(i,0,0,0);
        strip.show();
    }
}

// Finds the accelerometer vector
void findAccel(){
	zAccel = (float)analogRead(zPin)/4095-.5;
	yAccel = (float)analogRead(yPin)/4095-.5;
	xAccel = (float)analogRead(xPin)/4095-.5;
	accelMagnitude = sqrt(zAccel*zAccel+yAccel*yAccel+xAccel*xAccel);
}

// Displays the current time
void displayTime() {

	// Print the current Unix timestamp
	int unixTime = Time.now();
	unixTime =  unixTime%31556926;
	unixTime = unixTime%2629743;	
	unixTime = unixTime%604800;
	unixTime = unixTime%86400;
	//Now time = num seconds into day
	int hour = 0;
	int minute = 0;
	hour = unixTime/3600 + 7;
	unixTime = unixTime%3600;	
	minute = unixTime/60;


			
	hour = hour%12-1;
	minute = ((minute+14)/5)%12;
	
    Serial.print("hour: ");
	Serial.println(hour);
	Serial.print("min: ");
	Serial.println(minute);
	
    strip.setPixelColor(hour,0,10,0);
    strip.show();
    delay(1000);
	strip.setPixelColor(hour,0,0,0);
    strip.show();

	
	strip.setPixelColor(minute,0,0,10);
    strip.show();
    delay(1000);
  	strip.setPixelColor(minute,0,0,0);
    strip.show();
 	
}
