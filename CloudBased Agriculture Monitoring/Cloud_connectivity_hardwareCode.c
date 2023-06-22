#include "DHTesp.h"
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <OneWire.h>
#include "ThingSpeak.h"

#define DHT_PIN 15
#define DALLAS_TEMP_PIN 4
#define SOIL_MOISTURE_PIN 5
#define PUSH_BUTTON 2
#define SOIL_MOIST_PIN 33
#define WATER_PUMP 12
#define BUZZER 13

OneWire oneWire(DALLAS_TEMP_PIN);      // Setup a oneWire instance to communicate with dallas temperature sensor

DallasTemperature dallasTemp(&oneWire);

LiquidCrystal_I2C LCD = LiquidCrystal_I2C(0x27, 16, 2);  // Set up I2C LCD

DHTesp dhtSensor;    // set up DHT22 sensor

WiFiClient client;

// Sensor reading variables
float dht_temp;
float dht_humidity;
float soil_temp;

//ThingSpeak channelID and Write API key
unsigned long myChannelNumber = /*Your 7 digits channel ID*/;  
const char * myWriteAPIKey = "/*Your channel write API*/";

//Time variables
unsigned long cloud_lastTime = 0;
unsigned long cloudDelay = 15000;      //seconds intervals to update ThingSpeak cloud

unsigned long screen_lastTime = 0;
unsigned long screenDelay = 2000;      //seconds intervals to update the serial monitor and LCD

int LCD_view = 0;                     // a variable to switch what's being displayed on the LCD
bool cloudUpdate = false;             // A flag to track if the last cloud update was successful 

// Variables to caliberate the soil Moisture sensor
const int dryValue = 590;             //you need to replace this value with Value_1
const int wetValue = 300;          //you need to replace this value with Value_2
int soil_moistValue;                 // The analogue reading of the sensor
int soil_moistPercent;
const int moistThreshold = 60;       // Water pump is activated ones soil moisture falls bellow this level
bool soil_isDry = false;
String myStatus = "";

//WiFi Credentials
// const char* ssid = "Redmi Note 9S";   // your network SSID (name) 
// const char* password = "yinkus4ever";   // your network password


void setup() {
  Serial.begin(115200);

  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  pinMode(WATER_PUMP, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  LCD.init();
  LCD.backlight();

  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);

  dallasTemp.begin();       // Start the DS18B20 sensor

  // WiFi.begin(ssid, password);
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
    LCD.setCursor(0, 0);
    LCD.print("Connecting WiFi");
  }

 Serial.println("");
 Serial.println("WiFi connected");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());
 LCD.clear();
 LCD.setCursor(0, 0);
 LCD.print("Connected to:");
 LCD.setCursor(0, 1);
 //LCD.print(ssid);
 delay(500);

 ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  // Reading from DHT22 Sensor
  TempAndHumidity  dht = dhtSensor.getTempAndHumidity();
  dht_temp = dht.temperature;
  dht_humidity = dht.humidity;

  // Reading from Dallas Temperature sensor
   dallasTemp.requestTemperatures();
   soil_temp = dallasTemp.getTempCByIndex(0);

   // Reading from the Soil moisture sensor
   soil_moistValue = analogRead(SOIL_MOIST_PIN);
   soil_moistPercent = map(soil_moistValue, dryValue, wetValue, 0, 100);
   if (soil_moistPercent > 100)
      soil_moistPercent = 100;
   if (soil_moistPercent < 0)
      soil_moistPercent = 0;

   // Activate Water Pump and Alarm if moisture level falls below threshold
  if (soil_moistPercent < moistThreshold) {
      digitalWrite(WATER_PUMP, HIGH);
      digitalWrite(BUZZER, HIGH);
      soil_isDry = true;
    }
  else {
      digitalWrite(WATER_PUMP, LOW);
      digitalWrite(BUZZER, LOW);
      soil_isDry = false;
    }


   // Update sensor readngs on displays every "screenDelay" seconds
  if ((millis() - screen_lastTime) >= screenDelay) {
      update_Display(LCD_view);
      screen_lastTime = millis();
    }

  // Also update Screen if button is pushed
  if (digitalRead(PUSH_BUTTON) == LOW) {
     LCD_view++;
      if (LCD_view > 3) {
         LCD_view = 0;
        }
     update_Display(LCD_view);
  }
    

   // Update Sensor readings on cloud every "cloudDelay" seconds
  if ((millis() - cloud_lastTime) >= cloudDelay) {
      update_ThingSpeak();
      cloud_lastTime = millis();
    }
}



void update_Display(int viewOption) {
  Serial.println("Air Temp: " + String(dht_temp, 2) + "°C");
  Serial.println("Humidity: " + String(dht_humidity, 1) + "%");
  Serial.println("Soil Temp: " + String(soil_temp, 2) + "°C");
  Serial.println("soil Moisture: " + String(soil_moistPercent) + "%");
  Serial.println("---");

    
  switch (viewOption) {
     case 0:         //LCD screen zero also Notifies when Soil is dry
         if (!soil_isDry) {
         LCD.clear();
         LCD.setCursor(0, 0);
         LCD.print("Temp: " + String(dht_temp, 2) + char(223) + "C");
         LCD.setCursor(0, 1);
         LCD.print("Humidity: " + String(dht_humidity, 1) + "%");
        }
        else {
          LCD.clear();
          LCD.setCursor(5, 0);
          LCD.print("ALERT!");
          LCD.setCursor(0, 1);
          LCD.print("Soil is Dry");
        }
      break;
  
      case 1:    //LCD Screen one also flashes if Soil moisture is too low
         LCD.clear();
         LCD.setCursor(0, 0);
         LCD.print("Soil Temp: " + String(soil_temp, 2) + "C");
         LCD.setCursor(0, 1);
         LCD.print("Soil Moist: " + String(soil_moistPercent) + "%");
      break;

      case 2: //Gives the current status of the soil moisture and the water pump
         LCD.clear();
         LCD.setCursor(0, 0);
         LCD.print("Moisture is ");
         LCD.setCursor(0, 1);
         LCD.print("Pump is ");
         if (soil_isDry) {
            LCD.setCursor(10, 0);
            LCD.print("LOW");
            LCD.setCursor(8, 1);
            LCD.print("ON");
            }
          else {
            LCD.setCursor(12, 0);
            LCD.print("OKAY");
            LCD.setCursor(8, 1);
            LCD.print("OFF");
          }
      break;

      // LCD Screen three
      case 3:
         LCD.clear();
         LCD.setCursor(0, 0);
         LCD.print("Last CloudUpdate");
         LCD.setCursor(0, 1);
         if (cloudUpdate)
             LCD.print("Successful");
        else
             LCD.print("Failed !");
         LCD.setCursor(13, 1);
         LCD.print(String((millis() - cloud_lastTime)/1000) + "s");
      break;
  }

}

void update_ThingSpeak() {
   // set the fields with the values on thingspeak

  ThingSpeak.setField(1, dht_humidity);
  ThingSpeak.setField(2, dht_temp);
 // ThingSpeak.setField(3, soil_temp);
  ThingSpeak.setField(4, soil_moistPercent);

  if (soil_moistPercent < moistThreshold)
     myStatus = String("Soil Moisture level is OKAY");
  else
     myStatus = String("Soil Moisture level TOO LOW !");

     ThingSpeak.setStatus(myStatus);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
    cloudUpdate = true;
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
    cloudUpdate = false;
    // Notify the Update error in LCD
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("Cloud Update");
    LCD.setCursor(0, 1);
    LCD.print("Error " + String(x));
    delay(1000);
    LCD.clear();
  }
} 
