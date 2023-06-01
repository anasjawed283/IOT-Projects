  int ldr_pin = A5;
  int ldr_value;
  
  int light = 3;
  
  void setup()
  {
    pinMode(light, OUTPUT); pinMode(ldr_pin, INPUT);
  }
  void loop()
  {
    ldr_value = analogRead(ldr_pin);
    if (ldr_value > 512)
      digitalWrite(light, 0);
    else
      digitalWrite(light, 1);
  
  }
