#include <Adafruit_NeoPixel.h>
#include <Servo.h>
#include <Stepper.h>

// Time Data
int currentMonth;
int currentDay;
int season = 0;
float dayLength = 2;

unsigned long currentMillis;
unsigned long lastTime;
long time;

// Tree Servo
Servo treeServo;
int treeServoPin = A0;

// Background Servo
Servo backgroundServo;
int backgroundServoPin = A1;

// Stepper
//Stepper stepper(1028, 8, 9, 10, 11);
int Steps = 0;
int steps_left = 4096;
int currentStep = 0;
int Direction = 1;
int IN1 = 8;
int IN2 = 9;
int IN3 = 10;
int IN4 = 11;
float pulseTime = 100; //21093.75;

// The tree values for the servo, because the servo isn't that precise.
int treeValue0 = 19;
int treeValue1 = 50;
int treeValue2 = 86;
int treeValue3 = 122;

// The target values for the background servo;
float backgroundServoSpeed = 0.02;
int backgroundDayValue =  13; //10;
int backgroundNightValue = 258; //160;
int backgroundServoTargetValue;
int dayCounter = 0;
float dayProgress = 0;
float dayTimeOffset = 0;
bool dayPart = false;

// LED strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, 6, NEO_RGB + NEO_KHZ800);
uint32_t dayColour = strip.Color(216, 216, 255);
uint32_t nightColour = strip.Color(100, 100, 255);

void setup() {

  // Attaching the tree servo
  //treeServo.attach(treeServoPin);
  backgroundServo.attach(backgroundServoPin);

  // Beginning Serial Communication
  Serial.begin(9600);
  Serial.setTimeout(100);

  pinMode(IN1, OUTPUT); 
  pinMode(IN2, OUTPUT); 
  pinMode(IN3, OUTPUT); 
  pinMode(IN4, OUTPUT); 

  // Time setup for a specific date
  //ChangeSeason(SeasonOnDate(23, 3));
  //ToDay();
}

void readCommands(){
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
  } else if(command == "getseason"){
    Serial.println("The current showing season is " + GetCurrentSeason() + ".");
  } else if(command == "season"){
    if(parameter == ""){
      Serial.println("The current showing season is " + GetCurrentSeason() + ".");
    } else{
      ChangeSeason(parameter);
    }
  } else if(command == "seasonfromdate"){
    if(parameter == ""){
      ChangeSeason(SeasonOnDate(currentDay, currentMonth));
    } else{
      int dayparam = parameter.substring(0, 2).toInt();
      int monthparam = parameter.substring(2).toInt();
      Serial.println((String)dayparam + ", " + (String)monthparam);
      ChangeSeason(SeasonOnDate(dayparam, monthparam));
    }
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

void loop() {

  readCommands();

  // Sun and moon stepper.
  if(steps_left > 0){
    currentMillis = millis();
    if(currentMillis - lastTime >= pulseTime){
      DoStep(1);
      //steps_left -= 1;
      currentStep += 1;
      lastTime = millis();
      time += millis()-lastTime;
    }
  }

  // background servo
  dayProgress = ((currentMillis / pulseTime) / 2048) / dayLength + dayTimeOffset;

  dayProgress -= dayCounter;
  if(dayProgress >= 1.00){
    dayCounter++;
  }

  Serial.println(dayProgress);
  
  if(dayProgress >= 0.5){
    if(dayPart) ToNight();
  } else if(dayProgress < 0.5){
    if(!dayPart) ToDay();
  }
  backgroundServo.write(Lerp(backgroundServo.read(), backgroundServoTargetValue, 0.01));
  
  Serial.setTimeout(10);
  //Serial.println(Serial.readString());
  
}

String SeasonOnDate(int day, int month){
  if((month >= 3) && (month <= 6)){
    if((month == 3 && day >= 21) || month == 6 && day <= 20){
      return "spring";
    } else{
      return "spring";
    }
  }
  if((month >= 6) && (month <= 9)){
    if((month == 6 && day >= 21) || month == 9 && day <= 20){
      return "summer";
    } else{
      return "summer";
    }
  }
  if((month >= 9) && (month <= 12)){
    if((month == 9 && day >= 21) || month == 12 && day <= 20){
      return "autumn";
    } else{
      return "autumn";
    }
  }
  if((month = 12) || (month <= 3)){
    if((month == 12 && day >= 21) || month == 3 && day <= 20){
      return "winter";
    } else{
      return "winter";
    }
  }

  return "unknown";
  
}

// this method can be sent "summer", "autumn", "winter", "spring" or "next"
void ChangeSeason(String seasonString){ 
  Serial.println("Changing season to " + seasonString + ".");
  if(seasonString == "summer" || seasonString == "0"){
    season = 0;
  } else if(seasonString == "autumn" || seasonString == "1"){
    season = 1;
  } else if(seasonString == "winter" || seasonString == "2"){
    season = 2;
  } else if(seasonString == "spring" || seasonString == "3"){
    season = 3;
  } else if(seasonString == "next" || seasonString == "4"){
    season++;
  } else{
    season = -1;
  }

  if(season > 3) season = 0;
  
  switch(season){
    default:
      treeServo.write(0);
      break;
    case 0:
      treeServo.write(treeValue0);
      break;
    case 1:
      treeServo.write(treeValue1);
      break;
    case 2:
      treeServo.write(treeValue2);
      break;
    case 3:
      treeServo.write(treeValue3);
      break;
  }
}

String GetCurrentSeason(){

  switch(season){
    default:
      return "unknown";
      break;
    case 0:
      return "summer";
      break;
    case 1:
      return "autumn";
      break;
    case 2:
      return "winter";
      break;
    case 3:
      return "spring";
      break;
  }

}

void ToDay(){
//  SmoothServo(backgroundServo, backgroundNightValue, backgroundDayValue, backgroundServoSpeed);
  backgroundServoTargetValue = backgroundDayValue;
  ToColour(dayColour);
  dayPart = true;
}

void ToNight(){
  //SmoothServo(backgroundServo, backgroundDayValue, backgroundNightValue, backgroundServoSpeed);
  backgroundServoTargetValue = backgroundNightValue;
  ToColour(nightColour);
  dayPart = false;
}

void ToColour(uint32_t colour){
  strip.begin();
  strip.show();

  strip.setBrightness(100);

  for(int n = 0; n < 60; n++){
    strip.setPixelColor(n, colour);
  }
  strip.show();
}

void SmoothServo(Servo servo, float start, float target, float servoSpeed){
  float current = start;

  if(start > target){
    servoSpeed *= -1;
  }
  
  while(target - current < servoSpeed){
    current += servoSpeed;
    servo.write(current);
  }

  servo.write(target);
}

float Lerp(float a, float b, float t){
  return (a + t * (b - a));
}

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



