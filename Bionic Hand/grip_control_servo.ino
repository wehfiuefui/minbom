
#include <Servo.h>
Servo b4;
Servo b5; 
Servo b3;
Servo b2;
Servo b1; 

void setup() {
  b5.attach(5);
  b4.attach(4);
  b3.attach(3);
  b2.attach(2);
  b1.attach(6);
}
int i=0;
void loop() {
  
    for (  i=0; i<=130; i+=1)   
    {
    b5.write(i);
    b4.write(i);
    delay (10);
    } 
    
    for (  i=130; i<=0; i-=1)   
    {
    b5.write(i);
    b4.write(i);
    delay (10);
    }

    for (  i=0; i<=131; i+=1)   
    {
    b5.write(i);
    b4.write(i);
    b3.write(i);
    delay (10);
    }  
    
    for (  i=131; i<=0; i-=1)   
    {
    b5.write(i);
    b4.write(i);
    b3.write(i);
    delay (10);
    } 

    } 
