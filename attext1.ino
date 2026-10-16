#include<SoftwareSerial.h>

  int RX=0,TX=A5;
  SoftwareSerial mySerial(RX,TX);

  const int buttonPin1=9,buttonPin2=10,buttonPin3=11,buttonPin4=12;                           //按钮接口

  unsigned long time1,time2,time3,time4;
  unsigned long witetime = 1000;

  int abcturn=0;


void setup() {
  // put your setup code here, to run once:
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  pinMode(buttonPin4, INPUT_PULLUP);
  time1 = millis();
  time2 = millis();
  time3 = millis();
  time4 = millis();
  mySerial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(buttonPin1) == LOW && millis()-time1 > witetime){
    time1 = millis();
    switch(abcturn++){
      case 0:
        mySerial.write("A");
        break;
      case 1:
        mySerial.write("B");
        break;
      case 2:
        mySerial.write("C");
        break;
    }
    if(abcturn == 3) abcturn = 0;
  }
  if(digitalRead(buttonPin2) == LOW && millis()-time2 > witetime){
    time2 = millis();    
    mySerial.write("q");
  }
  if(digitalRead(buttonPin3) == LOW && millis()-time3 > witetime){
    time3 = millis();
    mySerial.write("a");
  }
  if(digitalRead(buttonPin4) == LOW && millis()-time4 > witetime){
    time4 = millis();
    mySerial.write("R");
  }
}
