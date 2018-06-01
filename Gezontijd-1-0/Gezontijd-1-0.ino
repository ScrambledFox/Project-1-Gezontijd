/// THIS PROGRAM HAS BEEN CREATED BY A.J. JORIS LODEWIJKS FOR PROJECT 1 AT TU/E INDUSTRIAL DESIGN. (C) 2018
/// THIS PROGRAM CONTROLS THE MOVING CLOCK PARTS, THE TIME AND DAY/DATE INFO.

// Import libraries
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

// Time Variables         // REPLACE THIS WITH AN IC TIME CHIP.
bool STATIC_MODE = false;

float timeScale = 1.0; // Scale time, is used to match sun and moon speed.
float dayProgress;

float dayTimeOffset;
bool dayTime = true; // Daytime or not.

int currentMonth = 0;
int currentDay = 0;

unsigned long currentMillis; // Counter for the global time.
unsigned long lastTime; // Saves the last pulse time.

// LED Strip Variables
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, 6, NEO_RGB + NEO_KHZ800);
uint32_t dayColour = strip.Color(216, 216, 255);
uint32_t nightColour = strip.Color(100, 100, 255);

// SunAndMoon Stepper variables
int IN1 = 8;
int IN2 = 9;
int IN3 = 10;
int IN4 = 11;

// Internal step Variables
int steps = 0; // Stepper sequence
int direction = 1; // Clock-wise
float pulseTime = 50; //21093.75 for real time.

// Background Servo variables
Servo backgroundServo;
int backgroundServoPin = A1;

float backgroundServoSpeed = 0.02; // Speed of the background servo, weird bug here.
int backgroundDayValue = 13; 	// This value shows day.
int backgroundNightValue = 258; // This value shows night.
int backgroundServoTargetValue; // The current target for the background.


void setup(){

	// Serial Communication
	Serial.begin(9600);
	Serial.setTimeout(10);

	// Background servo attachment
	backgroundServo.attach(backgroundServoPin);

	// Stepper pin modes
	pinMode(IN1, OUTPUT); 
  	pinMode(IN2, OUTPUT); 
  	pinMode(IN3, OUTPUT); 
  	pinMode(IN4, OUTPUT); 

}

void loop(){

	//ReadCommands();

	if(STATIC_MODE){
		// Do stuff
		return;
	}

	dayProgress = ((currentMillis / pulseTime) / 2048) / dayLength + dayTimeOffset;
	dayProgress -= currentDay;
	if (dayProgress >= 1.00) {
		currentDay++;
	}

	DoSunAndMoon(); // Do Sun and Moon Rotation.

	DoBackground(); // Do Background disk logic.



}

void ReadCommands(){
	String serialData = Serial.readString();
  	String command = "";
  	String parameter = "";

	serialData.toLowerCase();
  	serialData.replace(" ", "");
  	command = serialData;
  	int indexOfEquals = serialData.indexOf('=');
  	if(indexOfEquals != -1){
    	command = serialData.substring(0, indexOfEquals);
    	parameter = serialData.substring(indexOfEquals + 1);
  	}

  	if(command == ""){
    	// No command
  	} else if(command == "day"){
    	Serial.println("Changing background to day.");
    	ToDay();
  } else if(command == "night"){
    	Serial.println("Changing background to night.");
    	ToNight();
  } else if(command == "servo"){
    	backgroundServo.write(parameter.toInt());
  }
}

void DoSunAndMoon(){

	if(steps_left > 0){
    	currentMillis = millis(); // Save current time.
    	if(currentMillis - lastTime >= pulseTime){ // Wait for time difference of pulsetime
      		DoStep(1); // Do a step of the stepper motor.
      		currentStep += 1; // Add a step to the counter.
      		lastTime = millis(); // Save the time the last step was made.
    	}
  	}

}

// Does the background disk logic, checks whether the time is correct for day change.
// Also does the background writing.
void DoBackground(){

	if (dayProgress >= 0.5) {
		if (dayTime) ToNight();
	} else if (dayProgress < 0.5){
		if (!dayTime) ToDay();
	}

	backgroundServo.write(Lerp(back.read(), backgroundServoTargetValue, backgroundServoSpeed));

}

// Changes the system to day, w/o regard to the sun and moon.
void ToDay(){
	backgroundServoTargetValue = backgroundDayValue;
	ToColour(dayColour);
	dayTime = true;
}

// Changes the system to night, w/o regard to the sun and moon.
void ToNight(){
	backgroundServoTargetValue = backgroundNightValue;
	ToColour(nightColour);
	dayTime = false;
}

// Changes the colour of the LED strip
void ToColour(uint32_t colour){
	strip.begin();
	strip.show();

	strip.setBrightness(100);

	for(int n = 0; n < 60; n++){
    	strip.setPixelColor(n, colour);
  	}
  	
  	strip.show();
}

// Lerp function, gives back a linear interpolation result.
float Lerp(float a, float b, float t){
	return (a + t * (b-a));
}


// Stepper functions
void DoStep(int xw){
  for (int x=0;x<xw;x++){
    switch(Steps){
       case 0:
         digitalWrite(IN1, LOW); 
         digitalWrite(IN2, LOW);
         digitalWrite(IN3, LOW);
         digitalWrite(IN4, HIGH);
       break; 
       case 1:
         digitalWrite(IN1, LOW); 
         digitalWrite(IN2, LOW);
         digitalWrite(IN3, HIGH);
         digitalWrite(IN4, HIGH);
       break; 
       case 2:
         digitalWrite(IN1, LOW); 
         digitalWrite(IN2, LOW);
         digitalWrite(IN3, HIGH);
         digitalWrite(IN4, LOW);
       break; 
       case 3:
         digitalWrite(IN1, LOW); 
         digitalWrite(IN2, HIGH);
         digitalWrite(IN3, HIGH);
         digitalWrite(IN4, LOW);
       break; 
       case 4:
         digitalWrite(IN1, LOW); 
         digitalWrite(IN2, HIGH);
         digitalWrite(IN3, LOW);
         digitalWrite(IN4, LOW);
       break; 
       case 5:
         digitalWrite(IN1, HIGH); 
         digitalWrite(IN2, HIGH);
         digitalWrite(IN3, LOW);
         digitalWrite(IN4, LOW);
       break; 
         case 6:
         digitalWrite(IN1, HIGH); 
         digitalWrite(IN2, LOW);
         digitalWrite(IN3, LOW);
         digitalWrite(IN4, LOW);
       break; 
       case 7:
         digitalWrite(IN1, HIGH); 
         digitalWrite(IN2, LOW);
         digitalWrite(IN3, LOW);
         digitalWrite(IN4, HIGH);
       break; 
       default:
         digitalWrite(IN1, LOW); 
         digitalWrite(IN2, LOW);
         digitalWrite(IN3, LOW);
         digitalWrite(IN4, LOW);
       break; 
    }
    SetDirection();
  }
}
 
void SetDirection(){
  if(Direction==1){ Steps++;}
  if(Direction==0){ Steps--; }
  if(Steps>7){Steps=0;}
  if(Steps<0){Steps=7; }
}