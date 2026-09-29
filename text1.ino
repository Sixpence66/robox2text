#include <Servo.h>
void move_X_Y_Z();
void moveInT(Servo x,int T,int to);
int serialText();


Servo servo_x,servo_y,servo_z;

int servoX,servoY,servoZ;                                         //XYZ舵机对应的接口

int x,y,z;                                                        //接收旋转角度
int *px = &x,*py = &y,*pz = &z;                                   //    ^^^

int T = 10,Tx,Ty,Tz;                                                        //转一度需要的时间

void setup() {
  // put your setup code here, to run once:
  servo_x.attach(servoX);servo_y.attach(servoY);servo_z.attach(servoZ);          //链接接口
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()){
    switch(serialText()){
      case 8:
        move_X_Y_Z();
        break;
    }
  }
}

void move_X_Y_Z(){               //x舵机按T旋转，剩下的按x舵机速度同步
  Tx = T;
  int movex = abs(servo_x.read() - x);int movey = abs(servo_y.read() - y);int movez = abs(servo_z.read() - z);          //转动角度
  Ty = movex*Tx/Ty;
  Tz = movex*Tx/Tz;
  moveInT(servo_x,Tx,x);
  moveInT(servo_y,Ty,y);
  moveInT(servo_z,Tz,z);
}

void moveInT(Servo x,int T,int to){
  int lastTime = millis();
  int from = x.read();
  int i = from;
  while(i < to){
    int now = millis();
    if(now - lastTime >= T){
      lastTime = now;
      i++;
      x.write(i);
    }
  }
  while(i > to){
    int now = millis();
    if(now - lastTime >= T){
      lastTime = now;
      i--;
      x.write(i);
    }
  }
}

int serialText(){                //串口指令检测
  switch(Serial.read()){
    case 'O':return 1;
    case 'S':return 2;
    case 'H':return 3;
    case 'L':return 4;
    case 'A':return 5;
    case 'B':return 6;
    case 'C':return 7;
    case 'x':
      int x = Serial.parseInt(SKIP_ALL,'y');
      int y = Serial.parseInt(SKIP_ALL,'z');
      int z = Serial.parseInt();
      *px = x;*py = y;*pz = z;
      return 8;
    case 'm':
    default:
      while(Serial.available()) Serial.read();
      return 0;
  }
}