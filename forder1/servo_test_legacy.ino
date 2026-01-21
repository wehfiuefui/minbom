#include <Servo.h>
int a;
Servo b4;
Servo b5; 
Servo b3;
Servo b2;
Servo b1; 

void setup() {
  Serial.begin(9600);
  b5.attach(5);
  b4.attach(4);
  b3.attach(3);
  b2.attach(2);
  b1.attach(6);
}

void loop() {  
  a = 0;
    for (int i=0; i<=75; i+=1)   
    {
    if (a==0)
    {
    b1.write(i);
    delay (10);
    }
    }         
    for (int i=75; i>=0; i-=1)
    {
    if (a==0)
    {
    b1.write(i);
    delay (10);
    }
    }
}
