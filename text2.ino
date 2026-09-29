#include <Servo.h>
void move_X_Y_Z();
void moveInT(Servo x,int T,int to);
int serialText();


Servo servo_x,servo_y,servo_z;

int servoX = 9,servoY = 10,servoZ;                                         //XYZ舵机对应的接口

int x,y,z;                                                        //接收旋转角度
int *px = &x,*py = &y,*pz = &z;                                   //    ^^^

int T = 20,Tx,Ty,Tz;                                                        //转一度需要的时间

void setup() {
  // put your setup code here, to run once:
  servo_x.attach(servoX);servo_y.attach(servoY);servo_z.attach(servoZ);          //链接接口
  servo_x.write(0);servo_y.write(0);servo_z.write(0);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()){
    switch(serialText()){
      case 0:Serial.println("Unknow Commend");Serial.println("");
      case 3:if(T>=5)T-=3;Serial.println("T = ");Serial.write(T);Serial.println("");break;
      case 4:if(T<=45)T+=3;Serial.println("T = ");Serial.write(T);Serial.println("");break;
      case 8:
        move_X_Y_Z();
        break;
    }
  }
}

void move_X_Y_Z(){               //x舵机按T旋转，剩下的按x舵机速度同步
  Tx = T;
  int movex = abs(servo_x.read() - x);int movey = abs(servo_y.read() - y);int movez = abs(servo_z.read() - z);          //转动角度
  Ty = (movey!=0)?(movex*Tx/movey):(10000);
  Tz = (movez!=0)?(movex*Tx/movez):(10000);
  unsigned int lastTime_x = millis(),lastTime_y = millis(),lastTime_z = millis();
    Serial.print("x\tfrom :");Serial.print(servo_x.read());Serial.print(" \t , to :");Serial.print(x);Serial.print("\t , T :");Serial.println(Tx);
      Serial.print("y\tfrom :");Serial.print(servo_y.read());Serial.print(" \t , to :");Serial.print(y);Serial.print("\t , T :");Serial.println(Ty);
        Serial.print("z\tfrom :");Serial.print(servo_z.read());Serial.print(" \t , to :");Serial.print(z);Serial.print("\t , T :");Serial.println(Tz);Serial.println("");
  while(servo_x.read()!=x || servo_y.read()!=y || servo_z.read()!=z){
    unsigned int now_x = millis(),now_y = millis(),now_z = millis();
    if((now_x - lastTime_x >= Tx)){
      lastTime_x = now_x;
      if(servo_x.read()<x)servo_x.write(servo_x.read()+1);
      if(servo_x.read()>x)servo_x.write(servo_x.read()-1);
    }
    if((now_y - lastTime_y >= Ty)){
      lastTime_y = now_y;
      if(servo_y.read()<y)servo_y.write(servo_y.read()+1);
      if(servo_y.read()>y)servo_y.write(servo_y.read()-1);
    }
    if((now_z - lastTime_z >= Tz)){
      lastTime_z = now_z;
      if(servo_z.read()<z)servo_z.write(servo_z.read()+1);
      if(servo_z.read()>z)servo_z.write(servo_z.read()-1);
    }
  }
}

void moveInT(Servo x,int T,int to){
  int lastTime = millis();
  int from = x.read();
  int i = from;
  Serial.print("from :");Serial.print(from);Serial.print("\t , to :");Serial.print(to);Serial.print("\t , T :");Serial.println(T);
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
      int x = Serial.parseInt();
      if(Serial.read() == 'y' && Serial.read() == ','){
        int y = Serial.parseInt();
        if(Serial.read() == 'z' && Serial.read() == ','){
          int z = Serial.parseInt();
          *px = x;*py = y;*pz = z;
           return 8;
        }else{
          while(Serial.available()) Serial.read();
         return 0;          
        }
      }else{
        while(Serial.available()) Serial.read();
        return 0;
      }
    default:
      while(Serial.available()) Serial.read();
      return 0;
  }
}