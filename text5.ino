#include <SoftwareSerial.h>
#include <Servo.h>
#include <math.h>
#define theMax(a, b) ((a > b) ? (a) : (b))
void move_X_Y_Z();
void moveInT(Servo x, int T, int to);
int serialText();
void test_X_Y_Z();
void moveInabc(int plan);
void movecs_X_Y_Z();
void move_from_to(double x_from,double y_from,double z_from,double x_to,double y_to,double z_to);

int xMax = 180, xMin = 0, yMax = 130, yMin = 0, zMax = 180, zMin = 40, y_add_z_Min = 120;                    //限位角

  unsigned long lnow;int li=102;

  int RX=13,TX=1;
  SoftwareSerial mySerial(RX,TX);

  Servo servo_x, servo_y, servo_z, servo_get;

  unsigned long now_vrx=millis(),now_vry=millis(),now_vrz=millis();

  const int servoX = 9, servoY = 8, servoZ = 7, servoGet = 6;  //XYZ舵机对应的接口
  const int VRx1 = A0,VRy1 = A1,VRx2 = A3,VRy2 = A2;         //摇杆接口
  const int buttonPinabc = 4;                           //任务三按钮接口

  const int DEADZONE = 80,MIDZONE = 512;                //死区,中点

  int abcturn = 0;unsigned long abctime;
  unsigned long abcwite = 100;                          //防抖

  int x, y, z;                       //接收旋转角度
  int *px = &x, *py = &y, *pz = &z;  //    ^^^

  double csX,csY,csZ;                    //坐标系中xyz
  double *pcsx=&csX,*pcsy=&csY,*pcsz=&csZ;  //^^^
  double l1=8,l2=8,l3=1.45;                       //机械臂数据
  int x_error = 1,y_error = -1,z_error = 3;           //角度误差

  double x_from,y_from,z_from,x_to,y_to,z_to;                                         //直线绘制
  double *pfx=&x_from,*pfy=&y_from,*pfz=&z_from,*ptx=&x_to,*pty=&y_to,*ptz=&z_to;     //  ^^^

  int T = 18, Tx, Ty, Tz;  //转一度需要的时间
  
  int witetime = 1000;  //重置时间

  int wayabc[3][6][4] = 
    {{{110,90,90,80},                    //A路径
      {110,50,140,80},
      {110,50,140,150},
      {110,60,130,150},
      {110,20,100,80},
      {90,90,90,80}},
      
     {{100,90,90,80},                    //B路径
      {100,50,130,80},
      {100,50,130,150},
      {100,60,130,150},
      {100,20,100,80},
      {90,90,90,80}},
      
     {{80,90,90,80},                    //C路径  
      {80,45,125,80},
      {80,45,125,120},
      {80,60,130,120},
      {80,20,100,80},
      {90,90,90,80}}};

    double wayword[4][3]={
      {0,0,0},
      {0,0,0},
      {0,0,0},
      {0,0,0}
    };
  
    int way[3][101] = {0};

void setup() {
  // put your setup code here, to run once:
  servo_x.attach(servoX);
  servo_y.attach(servoY);
  servo_z.attach(servoZ);
  servo_get.attach(servoGet);  //链接接口
  mySerial.begin(9600);
  Serial.begin(9600);
  pinMode(buttonPinabc, INPUT_PULLUP);
  abctime = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(buttonPinabc) == LOW && millis()-abctime > abcwite){
    abctime = millis();
    Serial.print("commend ");
    Serial.println(abcturn);
    Serial.println("");
    moveInabc(abcturn++);
    if(abcturn == 3) abcturn = 0;
  }
  if(Serial.available()) {
    int st1 = serialText();
    switch (st1) {
      case 0:
        Serial.println("Undefined Commend");
        Serial.println("");
        break;
      case 1:
      Serial.println("put");
      Serial.println("");
      moveInT(servo_get,5,60);
      break;
      case 2:
      Serial.println("get");
      Serial.println("");
      moveInT(servo_get,5,150);
      break;
      case 3:
        if (T > 3) {
          T -= 3;
          Serial.print("speed higher,T = ");
          Serial.println(T);
          Serial.println("");
        } else {
          Serial.println("the highest speed");
          Serial.println("");
        }
        break;
      case 4:
        if (T < 30) {
          T += 3;
          Serial.print("speed lower,T = ");
          Serial.println(T);
          Serial.println("");
        } else {
          Serial.println("the lowest speed");
          Serial.println("");
        }
        break;
      case 5:
        Serial.println("commend A");
        Serial.println("");
        moveInabc(0);
        break;
      case 6:
        Serial.println("commend B");
        Serial.println("");
        moveInabc(1);
        break;
      case 7:
        Serial.println("commend C");
        Serial.println("");
        moveInabc(2);
        break;
      case 8:
        move_X_Y_Z();
        break;
      case 12:
        movecs_X_Y_Z();
        break;
      case 13:
        move_from_to(x_from,y_from,z_from,x_to,y_to,z_to);
        break;
    }
    goto end;
  }
  if(mySerial.available()) {
    int st2 = mySerialText();
    switch (st2) {
      case 1:    
        lnow = millis();
        li = 0;
        Serial.println("start");
        break;
      case 2:
        for(int i=0;i<=100;i++){
          unsigned long now = millis();
          servo_x.write(way[0][i]);
          servo_y.write(way[1][i]);
          servo_z.write(way[2][i]);
          while(millis()-now<100);
        }
        break;
      case 3:
        Serial.println("commend A");
        Serial.println("");
        moveInabc(0);
        break;
      case 4:
        Serial.println("commend B");
        Serial.println("");
        moveInabc(1);
        break;
      case 5:
        Serial.println("commend C");
        Serial.println("");
        moveInabc(2);
        break;
      case 6:
        move_X_Y_Z();
        break;
    }
    goto end;
  }
  int VRx = analogRead(VRx1),VRy = analogRead(VRx2),VRz = analogRead(VRy1);
  if(VRx>MIDZONE+DEADZONE && millis()-now_vrx > T){servo_x.write(servo_x.read()-1);now_vrx=millis();}if(VRx<MIDZONE-DEADZONE && millis()-now_vrx > T){servo_x.write(servo_x.read()+1);now_vrx=millis();}
  if(VRy>MIDZONE+DEADZONE && millis()-now_vry > T){servo_y.write(servo_y.read()+1);now_vry=millis();}if(VRy<MIDZONE-DEADZONE && millis()-now_vry > T){servo_y.write(servo_y.read()-1);now_vry=millis();}
  if(VRz>MIDZONE+DEADZONE && millis()-now_vrz > T){servo_z.write(servo_z.read()-1);now_vrz=millis();}if(VRz<MIDZONE-DEADZONE && millis()-now_vrz > T){servo_z.write(servo_z.read()+1);now_vrz=millis();}
  if(millis()-lnow>100 && li<=101){
    lnow = millis();
    Serial.println(li);
    way[0][li]=servo_x.read();
    Serial.println(way[0][li]);
    way[1][li]=servo_y.read();
    Serial.println(way[1][li]);
    way[2][li]=servo_z.read();
    Serial.println(way[2][li]);
    Serial.println("");
    li++;
  }
  if(li==102){
    Serial.println("end");
    li = 103;
  }
  end:;
}

void move_X_Y_Z() {  //最远舵机按T旋转，剩下的按其速度同步
  test_X_Y_Z();      //检查角度是否合法
  int movex = abs(servo_x.read() - x);
  int movey = abs(servo_y.read() - y);
  int movez = abs(servo_z.read() - z);  //转动角度
  if (theMax(movex, theMax(movey, movez)) == movex) {
    Tx = T;
    Ty = (movey != 0) ? (movex * Tx / movey) : (10000);
    Tz = (movez != 0) ? (movex * Tx / movez) : (10000);
  } else if (theMax(movex, theMax(movey, movez)) == movey) {
    Ty = T;
    Tx = (movex != 0) ? (movey * Ty / movex) : (10000);
    Tz = (movez != 0) ? (movey * Ty / movez) : (10000);
  } else {
    Tz = T;
    Tx = (movex != 0) ? (movez * Tz / movex) : (10000);
    Ty = (movey != 0) ? (movez * Tz / movey) : (10000);
  }
  unsigned long lastTime_x = millis(), lastTime_y = millis(), lastTime_z = millis();
  Serial.print("x\tfrom :");
    Serial.print(servo_x.read());
    Serial.print(" \t , to :");
    Serial.print(x);
    Serial.print("\t , T :");
    Serial.print(Tx);
    Serial.print(" \tPWM占空比：");
    Serial.print((double)x / 180 * 10 + 2.5);
    Serial.println("%");
    Serial.print("y\tfrom :");
    Serial.print(servo_y.read());
    Serial.print(" \t , to :");
    Serial.print(y);
    Serial.print("\t , T :");
    Serial.print(Ty);
    Serial.print(" \tPWM占空比：");
    Serial.print((double)y / 180 * 10 + 2.5);
    Serial.println("%");
    Serial.print("z\tfrom :");
    Serial.print(servo_z.read());
    Serial.print(" \t , to :");
    Serial.print(z);
    Serial.print("\t , T :");
    Serial.print(Tz);
    Serial.print(" \tPWM占空比：");
    Serial.print((double)z / 180 * 10 + 2.5);
    Serial.println("%");
    Serial.println("");
  while (servo_x.read() != x || servo_y.read() != y || servo_z.read() != z) {
    unsigned long now_x = millis(), now_y = millis(), now_z = millis();
    if ((now_x - lastTime_x >= Tx)) {
      lastTime_x = now_x;
      if (servo_x.read() < x) servo_x.write(servo_x.read() + 1);
      if (servo_x.read() > x) servo_x.write(servo_x.read() - 1);
    }
    if ((now_y - lastTime_y >= Ty)) {
      lastTime_y = now_y;
      if (servo_y.read() < y) servo_y.write(servo_y.read() + 1);
      if (servo_y.read() > y) servo_y.write(servo_y.read() - 1);
    }
    if ((now_z - lastTime_z >= Tz)) {
      lastTime_z = now_z;
      if (servo_z.read() < z) servo_z.write(servo_z.read() + 1);
      if (servo_z.read() > z) servo_z.write(servo_z.read() - 1);
    }
  }
}

void moveInT(Servo x, int T, int to) {
  unsigned long lastTime = millis();
  int from = x.read();
  int i = from;
  Serial.print("from :");
    Serial.print(from);
    Serial.print("\tto :");
    Serial.print(to);
    Serial.print("\tT :");
    Serial.println(T);
    Serial.println("");
  while (i < to) {
    unsigned long now = millis();
    if (now - lastTime >= T) {
      lastTime = now;
      i++;
      x.write(i);
    }
  }
  while (i > to) {
    unsigned long now = millis();
    if (now - lastTime >= T) {
      lastTime = now;
      i--;
      x.write(i);
    }
  }
}

int mySerialText(){
  switch (mySerial.read()) {
    case 'q': return 1;
    case 'a': return 2;
    case 'A': return 3;
    case 'B': return 4;
    case 'C': return 5;
    case 'R': 
      *px = 90;
      *py = 90;
      *pz = 90;
      Serial.println(x);
      Serial.println(y);
      Serial.println(z);
      return 6;
    default: return 0;
  }
}

int serialText() {  //串口指令检测
  switch (Serial.read()) {

    case 'M': {
      unsigned long now = millis();
      while (!(Serial.available() || millis() - now >= witetime));
      *pcsx=Serial.parseFloat();
      now = millis();
      while (!(Serial.available() || millis() - now >= witetime));
      *pcsy=Serial.parseFloat();
      now = millis();
      while (!(Serial.available() || millis() - now >= witetime));
      *pcsz=Serial.parseFloat();
      return 12;}
    case 'm':{
      unsigned long now = millis();
      while (!(Serial.available() || millis() - now >= witetime));
      *pfx=Serial.parseFloat();
      now = millis();
      while (!(Serial.available() || millis() - now >= witetime));
      *pfy=Serial.parseFloat();
      now = millis();
      while (!(Serial.available() || millis() - now >= witetime));
      *pfz=Serial.parseFloat();
      now = millis();
      while (!(Serial.available() || millis() - now >= witetime));
      *ptx=Serial.parseFloat();
      now = millis();
      while (!(Serial.available() || millis() - now >= witetime));
      *pty=Serial.parseFloat();
      now = millis();
      while (!(Serial.available() || millis() - now >= witetime));
      *ptz=Serial.parseFloat();
      return 13;}
    case 'O': return 1;
    case 'S': return 2;
    case 'H': return 3;
    case 'L': return 4;
    case 'A': return 5;
    case 'B': return 6;
    case 'C': return 7;
    case 'R':
      *px = 90;
      *py = 90;
      *pz = 90;
      Serial.println(x);
      Serial.println(y);
      Serial.println(z);
      return 8;
    case 'x':{
      int x = Serial.parseInt();
      Serial.println("x_finish");
      unsigned long now = millis();
      while (!(Serial.available() || millis() - now >= witetime));
      if (Serial.read() == ',') {
        now = millis();
        while (!(Serial.available() || millis() - now >= witetime));
        if (Serial.read() == 'y') {
          int y = Serial.parseInt();
          Serial.println("y_finish");
          now = millis();
          while (!(Serial.available() || millis() - now >= witetime));
          if (Serial.read() == ',') {
            now = millis();
            while (!(Serial.available() || millis() - now >= witetime));
            if (Serial.read() == 'z') {
              int z = Serial.parseInt();
              Serial.println("z_finish");
              *px = x;
              *py = y;
              *pz = z;
              return 8;
            }
          }
        }
      }}
      while(Serial.available()) Serial.read();
      return 0;
    default:
          while (Serial.available()) Serial.read();
          return 0;
  }
}

void test_X_Y_Z() {
  if (y+z < y_add_z_Min){
    Serial.print("y_add_z_Min = ");
    Serial.println(y_add_z_Min);
    int temp = (y_add_z_Min-y-z)/2;
    *py += temp;
    *pz += temp;
  }
  if (x > xMax) {
    Serial.print("xMax = ");
    Serial.println(xMax);
    *px = xMax;
  }
  if (x < xMin) {
    Serial.print("xMin = ");
    Serial.println(xMin);
    *px = xMin;
  }
  if (y > yMax) {
    Serial.print("yMax = ");
    Serial.println(yMax);
    *py = yMax;
  }
  if (y < yMin) {
    Serial.print("yMin = ");
    Serial.println(yMin);
    *py = yMin;
  }
  if (z > zMax) {
    Serial.print("zMax = ");
    Serial.println(zMax);
    *pz = zMax;
  }
  if (z < zMin) {
    Serial.print("zMin = ");
    Serial.println(zMin);
    *pz = zMin;
  }
}

void moveInabc(int plan){
  for(int i=0;i<6;i++){
    *px = wayabc[plan][i][0];
    *py = wayabc[plan][i][1];
    *pz = wayabc[plan][i][2];
    move_X_Y_Z();
    servo_get.write(wayabc[plan][i][3]);
    delay(1000);
  }
}

void movecs_X_Y_Z(){                      //坐标轴移动
  double x,y,z,xmixy,d2,d;
  xmixy = sqrt(csX*csX+csY*csY);
  x=atan2(csY, csX);
  d2=(xmixy-l3)*(xmixy-l3)+csZ*csZ;
  d=sqrt(d2);
  z=atan2(csZ, xmixy - l3)+acos((l1*l1+d2-l2*l2)/(2*l1*d));
  y=acos((l1*l1+l2*l2-d2)/(2*l1*l2))+z-M_PI/2;
  *px = (int)(x/M_PI*180)+x_error;
  *py = (int)(y/M_PI*180)+y_error;
  *pz = (int)(z/M_PI*180)+z_error;
  double csz = l1*sin(z)+l2*sin(y-M_PI/2);
  double csxmisy = l1*cos(z)+l2*cos(y-M_PI/2)+l3;
  double csx = csxmisy*cos(x);
  double csy = csxmisy*sin(x);
  Serial.print("csx = ");
  Serial.println(csX);
  Serial.print("ture csx = ");
  Serial.println(csx);
  Serial.print("csy = ");
  Serial.println(csY);
  Serial.print("ture csy = ");
  Serial.println(csy);
  Serial.print("csz = ");
  Serial.println(csZ);
  Serial.print("ture csz = ");
  Serial.println(csz);
  move_X_Y_Z();
}

void move_from_to(double x_from,double y_from,double z_from,double x_to,double y_to,double z_to){
  double t=0;
  double A_1=x_to-x_from,B_1=x_from,A_2=y_to-y_from,B_2=y_from,A_3=z_to-z_from,B_3=z_from;
  for(t=0;t<=1;t+=0.01){
    *pcsx=A_1*t+B_1;
    *pcsy=A_2*t+B_2;
    *pcsz=A_3*t+B_3;
    movecs_X_Y_Z();
  }
}

void writeWord(){
  for(int i=0;i<3;i++){
    move_from_to(wayword[i][0],wayword[i][1],wayword[i][2],wayword[i+1][0],wayword[i+1][1],wayword[i+1][1]);
  }
}