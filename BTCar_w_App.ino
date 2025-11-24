#include "Arduino.h"
#include <SoftwareSerial.h>

struct MOTOR{
  String speed;
  String direction;
};

const byte LMOTOR1=8;
const byte LMOTOR2=9;
const byte RMOTOR1=12;
const byte RMOTOR2=13;
int RMOTORPWM = 11;
int LMOTORPWM = 10;

void setMotors(String message);
MOTOR getNextNumber(String text, int cursor);

const byte rxPin = 5;
const byte txPin = 4;
SoftwareSerial BTSerial(rxPin, txPin); // RX TX

void setup() {
  // define pin modes for tx, rx:
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  BTSerial.begin(9600);
  Serial.begin(9600);
}

// message tokens
const char START_TOKEN = '?';
const char END_TOKEN = ';';
const char DELIMIT_TOKEN = '&';
const int CHAR_TIMEOUT = 20;

bool waitingForStartToken = true;
String messageBuffer = "";

// long lastRun = millis();
bool outputValue = false;

void loop() {

  // handle Bluetooth link
  char nextData;
  if (BTSerial.available()) { 

    // check for start of message
    if (waitingForStartToken) {
      do {
        nextData = BTSerial.read(); 
      } while((nextData != START_TOKEN) && BTSerial.available()); //BT
      if (nextData == START_TOKEN) {
        Serial.println("message start");
        waitingForStartToken = false;
      }
    }

    // read command
    if (!waitingForStartToken && BTSerial.available()){ //BT
      do {
        nextData = BTSerial.read(); //BT
        Serial.println(nextData);
        messageBuffer += nextData;
      } while((nextData != END_TOKEN) && BTSerial.available()); //BT

      // check if message complete
      if (nextData == END_TOKEN) {
        // remove last character
        messageBuffer = messageBuffer.substring(0, messageBuffer.length() - 1);
        Serial.println("message complete - " + messageBuffer);
        setMotors(messageBuffer);
        messageBuffer = "";
        waitingForStartToken = true;
      }

      // check for char timeout
      if (messageBuffer.length() > CHAR_TIMEOUT) {
        Serial.println("message data timeout - " + messageBuffer);
        messageBuffer = "";
        waitingForStartToken = true;
      }
    }
    
  }

  // if ((millis() - lastRun) > 1000) {
  //   lastRun = millis();
  //   outputValue = !outputValue;
  //   //Serial.print(outputValue);
  //   if (outputValue) {
  //     BTSerial.write("1");
  //     digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on
  //   }
  //   else {
  //     BTSerial.write("0");
  //     digitalWrite(LED_BUILTIN, LOW);   // turn the LED off
  //   }
  // }
  
}

void setMotors(String message){
  int textCursor = 0;
  bool msgOk = true;
  MOTOR right;
  MOTOR left;

  /*
   * message should be in the format
   * r=xxxy&l=xxxy
   */
  
  if (message.startsWith("r=")){
    // correct starting message
    textCursor = 2;
    right = getNextNumber(message, textCursor);
    textCursor += right.speed.length() + 2;
    message=message.substring(textCursor);
  }
  else {
    msgOk = false;
  }
  if (message.startsWith("l=")){
    // correct starting message
    textCursor = 2;
    left = getNextNumber(message, textCursor);
    textCursor += left.speed.length() + 2;
    message=message.substring(textCursor);
  }
  else {
    msgOk = false;
  }
  if (msgOk) {
    Serial.print("Right Speed = " + right.speed);
    Serial.println("  Right Direction: " + right.direction);
    Serial.print("Left Speed = " + left.speed);
    Serial.println("  Left Direction: " + left.direction);
    if(!right.direction.compareTo("b")){
      digitalWrite(RMOTOR1,HIGH);
      digitalWrite(RMOTOR2,LOW);
      analogWrite(RMOTORPWM,right.speed.toInt());
    }
    if(!right.direction.compareTo("f")){
      digitalWrite(RMOTOR1,LOW);
      digitalWrite(RMOTOR2,HIGH);
      analogWrite(RMOTORPWM,right.speed.toInt());
    }
    if(!left.direction.compareTo("f")){
      digitalWrite(LMOTOR1,HIGH);
      digitalWrite(LMOTOR2,LOW);
      analogWrite(LMOTORPWM,left.speed.toInt());
    }
    if(!left.direction.compareTo("b")){
      digitalWrite(LMOTOR1,LOW);
      digitalWrite(LMOTOR2,HIGH);
      analogWrite(LMOTORPWM,left.speed.toInt());
    }
  }
}

MOTOR getNextNumber(String text, int textCursor){
  String number = "";
  MOTOR mtr;
  while((text[textCursor] >= '0') && (text[textCursor] <= '9') && (textCursor < text.length())){
    number += text[textCursor];
    textCursor ++;
  }
  mtr.direction=text[textCursor];
  mtr.speed=number;
  return mtr;
}
