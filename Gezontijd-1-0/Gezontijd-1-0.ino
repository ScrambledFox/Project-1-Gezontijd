/// THIS PROGRAM HAS BEEN CREATED BY A.J. JORIS LODEWIJKS FOR PROJECT 1 AT TU/E INDUSTRIAL DESIGN. (C) 2018
/// THIS PROGRAM CONTROLS THE MOVING CLOCK PARTS, THE TIME AND DAY/DATE INFO.

// Import libraries
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

// Time Variables         // REPLACE THIS WITH AN IC TIME CHIP.


// Loop Variables
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, 6, NEO_RGB + NEO_KHZ800);


// SunAndMoon Stepper variables
int IN1 = 8;
int IN2 = 9;
int IN3 = 10;
int IN4 = 11;

// Internal step Variables
int steps = 0;
int direction = 1; // Clock-wise
float pulseTime = 50; //21093.75 for real time.

// Background Servo variables

void setup(){

	strip.begin();
	strip.show();

	uint32_t magenta = strip.Color(100, 100, 255);
  	for(int n = 0; n < 60; n++){
    	strip.setPixelColor(n, magenta);
  	}
  	strip.show();
 
}

void loop(){



}
