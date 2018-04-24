#include "SoftwareSerial.h"
SoftwareSerial XBee(2, 3);       // initializing XBee RX, TX Pins

int joyPinX = 4;                 // slider variable connecetd to analog pin 4; control zipline movement
int joyPinY = 5;                 // slider variable connecetd to analog pin 5; control claw
int buttonPin = 12;               // joystick button connected to digital pin 12; button

int valueA = 0;                  // variable to read the valueY from the analog pin 5
int valueB = 0;                  // variable to read the valueX from the analog pin 4
int buttonState = 0;

bool clawIsOpen = false;         // track claw state
bool motorIsFwd = false;         // track motor direction
bool autoison = false;           // track auto mode

int treatValue(int data) {        // joystick value conversion
  return (data * 9 / 1024) + 48;
}

void setup()
{
    Serial.begin(9600);
    XBee.begin(9600);
    pinMode(buttonPin, INPUT_PULLUP);
}

// to read debug transmission
void loop2() { 
  if(XBee.available()) {
    char a = XBee.read();
    Serial.println(a);
    // check for LED blink on shield
  }
}


void loop()
{
  valueA = analogRead(joyPinY); 
  valueB = analogRead(joyPinX); 
  buttonState = digitalRead(buttonPin); 

  if(valueA < 100 && clawIsOpen){ 
    //when pushing up
    Serial.println(valueA);
    Serial.println("Open Claw");
    XBee.write("<A>");
    clawIsOpen = false;
  }
  else if (valueA > 300 && valueA < 520 && !clawIsOpen){
  // when button at neutral
    Serial.println(valueA);
    Serial.println("Close Claw");
    XBee.write("<B>");
    clawIsOpen = true;
  }
  delay(20);
  
  if(valueB < 100 && motorIsFwd){
    Serial.println(valueB);
    Serial.println("Move Direction A");
    XBee.write("<C>");
    motorIsFwd = false;
  } 
  else if (valueB > 700 && !motorIsFwd){
    Serial.println(valueB);
    Serial.println("Move Direction B");
    XBee.write("<D>");
    motorIsFwd = true;
  }
  delay(20);

  // turning auto zipline on
  if (valueA > 700 && !autoison){
    Serial.println(valueA);
    Serial.println("Auto On");
    XBee.write("<E>");
    autoison = true;
  }
  else if(valueA < 700){
    autoison = false;
  }
  delay(20); 

  if (buttonState == LOW){
    XBee.write("<F>");
    Serial.println("Testing");
  }

}
