int red1=8;
int green1=6;
int yellow1=7;
int red2=3;
int green2=5;
int yellow2=4;
const int pingPin = 10; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 9; // Echo Pin of Ultrasonic Sensor
const int pingPin2 = 12; // Trigger Pin of Ultrasonic Sensor
const int echoPin2 = 11; // Echo Pin of Ultrasonic Sensor
void setup() {
  // put your setup code here, to run once:
pinMode(red1,OUTPUT);
pinMode(red2,OUTPUT);
pinMode(yellow1,OUTPUT);
pinMode(yellow2,OUTPUT);
pinMode(green1,OUTPUT); 
pinMode(green2,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int distance1,distance2;
  distance1=calculatedistance(pingPin , echoPin);
  distance2=calculatedistance(pingPin2 , echoPin2);
  if(distance1>=distance2){
     digitalWrite(red1,LOW);
digitalWrite(green2,LOW);
digitalWrite(red2,LOW);
digitalWrite(green1,LOW);
digitalWrite(yellow1,HIGH);
digitalWrite(yellow2,HIGH);
delay(200);
    while(distance1>distance2){
   distance1=calculatedistance(pingPin , echoPin);

   distance2=calculatedistance(pingPin2 , echoPin2);
digitalWrite(red1,HIGH);
digitalWrite(green2,HIGH);
digitalWrite(red2,LOW);
digitalWrite(green1,LOW);
digitalWrite(yellow1,LOW);
digitalWrite(yellow2,LOW);
    }  }


if(distance2>distance1){
    digitalWrite(red1,LOW);
digitalWrite(green2,LOW);
digitalWrite(red2,LOW);
digitalWrite(green1,LOW);
digitalWrite(yellow1,HIGH);
digitalWrite(yellow2,HIGH);
delay(200);
  while(distance2>distance1){
 distance1=calculatedistance(pingPin , echoPin);
  distance2=calculatedistance(pingPin2 , echoPin2);
digitalWrite(red1,LOW);
digitalWrite(green2,LOW);
digitalWrite(red2,HIGH);
digitalWrite(green1,HIGH);
digitalWrite(yellow1,LOW);
digitalWrite(yellow2,LOW);
  }


}

}
long microsecondsToCentimeters(long microseconds)
{
   return microseconds / 29 / 2;
}

int calculatedistance(int pingPin , int echoPin){
    long duration, inches, cm,meter;
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  cm = microsecondsToCentimeters(duration);
  meter = cm/100;
  return meter;
}

