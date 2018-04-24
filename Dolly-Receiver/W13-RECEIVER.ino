#include <SoftwareSerial.h>
SoftwareSerial xbee(2, 3); // initialize xbee RX,TX pins

#include <Servo.h>
Servo servo;

// motor chip digital pins
int enablePin = 11; 
int in3Pin = 8; // receives logic from arduino to rotate clockwise
int in4Pin = 9; // receives logic from arduino to rotate anti

// autoloop variables
bool isRunningAutoLoop = false;
int autoLoopPhase = 0;
int autoLoopTimer = 0;
int lastAutoLoopUpdate = 0;

// main function variables
int motorSpeed = 0;
int angle = 50;
bool is_cw = true; // track motor direction

void setup()
{
    Serial.begin(9600);
    xbee.begin(9600);
    servo.attach(13); // servo logic pin attached to PWM pin
    pinMode(in3Pin, OUTPUT);
    pinMode(in4Pin, OUTPUT);
    pinMode(enablePin, OUTPUT);
}

void loop()  {
  
  if(isRunningAutoLoop) {
    autoLoopTimer += millis() - lastAutoLoopUpdate; //timer check
    lastAutoLoopUpdate = millis();
    Serial.println(autoLoopTimer);
    if(autoLoopPhase % 2 == 0) {
      //count even
      setMotor(255, true);
    } else {
      //count odd
      setMotor(255, false);
    }

    if(autoLoopTimer >= 3000) {
      autoLoopTimer = 0;
      autoLoopPhase ++;
      // when 6 clockwise and 6 anti-clockwise rotations completed, auto mode stops
      if(autoLoopPhase >= 12) { 
        autoLoopPhase = 0;
        isRunningAutoLoop = false;
        setMotor(0, true);
      }
    }
  } else {
    // when manual mode, just perform motor from remote
    setMotor(motorSpeed, is_cw);
  }
  
  servo.write(angle);

  // XBee transmission to xbee, signal conversion
  String input = "";
  while(xbee.available()){
      char x = xbee.read();
      if(x == '<') {
        input = "";
      } else if(x == '>') {
        handle(input);
        input = "";
        break;
      } else {
        input += x;
      } 
  }
}

void handle(String input){
  
  Serial.println(input);

  //claw control
  // motor is immobile when performing claw function
  if(input == "A"){
    angle = 10;
    motorSpeed = 0;
    Serial.println("Claw Opens");
  }
  
  if (input == "B"){
    angle = 50;
    motorSpeed = 0;
    Serial.println("Claw Closes");
  }

  // driver control
  if(input == "C"){
    motorSpeed = 255; // max speed
    is_cw = true;
  } else if (input == "D"){
    motorSpeed = 255;
    is_cw = false;
  }

  // driver on auto control
  if (input == "E" && !isRunningAutoLoop){
    isRunningAutoLoop = true;
    lastAutoLoopUpdate = millis();
  }

  // joystick button pressed
  // testing signal
  if (input == "F"){
   xbee.write("<S>");
   // check from XBee shield LED pin flash
  }
  
}

void setMotor(int motorSpeed, boolean reverse)
{
  //Clockwise
  if(reverse){
    digitalWrite(in3Pin, LOW);
    digitalWrite(in4Pin, HIGH);
  }

  if(!reverse){
    digitalWrite(in3Pin, HIGH);
    digitalWrite(in4Pin, LOW);
  }
  if(motorSpeed==0){
    digitalWrite(in3Pin, LOW);
    digitalWrite(in4Pin, LOW);
  }
}

