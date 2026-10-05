#include <Servo.h>
#define theMax(a, b) ((a > b) ? (a) : (b))
void move_X_Y_Z();
void moveInT(Servo x, int T, int to);
int serialText();
void test_X_Y_Z();

int xMax = 180, xMin = 0, yMax = 180, yMin = 0, zMax = 180, zMin = 0;  //*pym = y;                    //限位角

  int mode = 2, *pMode = &mode;  //模式，1串口，2摇杆

  Servo servo_x, servo_y, servo_z, servo_get;

  const int servoX, servoY = 10, servoZ = 9, servoGet;  //XYZ舵机对应的接口
  const int VRx1 = A0,VRy1 = A1,VRx2,VRy2 = A2;                        //摇杆接口
  const int DEADZONE = 80,MIDZONE = 512;                //死区,中点

  int x, y, z;                       //接收旋转角度
  int *px = &x, *py = &y, *pz = &z;  //    ^^^

  int T = 18, Tx, Ty, Tz;  //转一度需要的时间

void setup() {
  // put your setup code here, to run once:
  servo_x.attach(servoX);
  servo_y.attach(servoY);
  servo_z.attach(servoZ);  //链接接口
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()) {
    int st = serialText();
    if(mode == 1){
      switch (st) {
        case 0:
          Serial.println("Unknow Commend");
          Serial.println("");
          break;
        case 3:
          if (T > 3) {
            T -= 3;
            Serial.print("speed higher,T = ");
            Serial.println(T);
            Serial.println("");
          } else {
            Serial.print("the highest speed");
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
            Serial.print("the lowest speed");
            Serial.println("");
          }
          break;
        case 8:
          move_X_Y_Z();
          break;
      }
    }else while(Serial.available()) Serial.read();
  }
  if(mode == 2){
    int x = analogRead(VRx1),y = analogRead(VRy1),z = analogRead(VRy2),t = analogRead(VRx2);
    x = map((abs(x-MIDZONE)>DEADZONE)?(x):(MIDZONE),0,1023,0,180);
    y = map((abs(y-MIDZONE)>DEADZONE)?(y):(MIDZONE),0,1023,0,180);
    z = map((abs(z-MIDZONE)>DEADZONE)?(z):(MIDZONE),0,1023,0,180);
    t = map((abs(t-MIDZONE)>DEADZONE)?(t):(MIDZONE),0,1023,0,180);
    servo_x.write(x);servo_y.write(y);servo_z.write(z);servo_get.write(t);
  }
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
  int lastTime = millis();
  int from = x.read();
  int i = from;
  Serial.print("from :");
    Serial.print(from);
    Serial.print("\tto :");
    Serial.print(to);
    Serial.print("\tT :");
    Serial.println(T);
  while (i < to) {
    int now = millis();
    if (now - lastTime >= T) {
      lastTime = now;
      i++;
      x.write(i);
    }
  }
  while (i > to) {
    int now = millis();
    if (now - lastTime >= T) {
      lastTime = now;
      i--;
      x.write(i);
    }
  }
}

int serialText() {  //串口指令检测
  switch (Serial.read()) {
    case 'T':
      switch(mode){
        case 1:
          *pMode = 2;
          Serial.println("mode change,mode = 2");Serial.println("");break;
        case 2:
          *pMode = 1;
          Serial.println("mode change,mode = 1");Serial.println("");break;
      }
      return 9;
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
    case 'x':
      int x = Serial.parseInt();
      int witetime = 1000;  //重置时间
      Serial.println("x_finish");
      unsigned long now = millis();
      while (!(Serial.available() || millis() - now >= witetime))
        ;
      if (Serial.read() == ',') {
        now = millis();
        while (!(Serial.available() || millis() - now >= witetime))
          ;
        if (Serial.read() == 'y') {
          int y = Serial.parseInt();
          Serial.println("y_finish");
          now = millis();
          while (!(Serial.available() || millis() - now >= witetime))
            ;
          if (Serial.read() == ',') {
            now = millis();
            while (!(Serial.available() || millis() - now >= witetime))
              ;
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
      }
      while(Serial.available()) Serial.read();
      return 0;
    default:
          while (Serial.available()) Serial.read();
          return 0;
      }
  }

  void test_X_Y_Z() {
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