#include <Servo.h>
Servo servo1; 
int trigpin = 10;
int echopin = 11 ; 
int distance; 
int duration; 
float cm; 
void setup() {
 servo1.attach(9);
 pinMode(trigpin, OUTPUT);  pinMode(echopin, INPUT);
}
 void loop() { 
 digitalWrite(trigpin, LOW);  
  delay(2); 
 digitalWrite(trigpin, HIGH);  
  delayMicroseconds(10);  
  digitalWrite(trigpin, LOW);  
  duration = pulseIn(echopin, HIGH);  
  cm = (duration/58.82);
 distance = cm;
 
 if(distance<100)
 {  servo1.write(90); 
 }  else{  servo1.write(0);  delay(50);
 }
}
