/* 
 * Project Flower_pot
 * Author: Joseph Maes  
 * Date: 11/12/2024
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */
#include "Particle.h"
#include "Adafruit_BME280.h"
#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h"
#include "Air_Quality_Sensor.h"
#include "IoTTimer.h"
#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "credentials.h"

AirQualitySensor aqSensor(A2);
const int BUTTON=D16;//publish
const int moistSensor =A1;
const int OLED_RESET=-1;
TCPClient TheClient;//pub
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); //pub
Adafruit_MQTT_Subscribe hotPfeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/button-onoff"); 
Adafruit_MQTT_Publish  moisture = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/moisture");
Adafruit_MQTT_Publish airquality = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/airquality");
Adafruit_MQTT_Publish Humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Humidity");
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");

unsigned int last,lastTime;

float hotPValue,ranValue;

void MQTT_connect();
bool MQTT_ping();

float tempc;
float tempf;
float humidity;
bool status;
bool webonState;//pub
Adafruit_BME280 bme;

Adafruit_SSD1306 display(OLED_RESET);
int inputValue;
IoTTimer relayTimer;
int pumpPin = D16;
int moistSensorValue;
int quality;
SYSTEM_MODE(AUTOMATIC);

void setup() {
Serial.begin(9600);
waitFor(Serial.isConnected,10000);
 
pinMode(D16,OUTPUT);//Publish code
  
//WiFi.on();
//WiFi.clearCredentials();
//WiFi.setCredentials("IoTNetwork");
  
//WiFi.connect();
//while(WiFi.connecting()) needs swigliy braket
//{
//Serial.printf(".");
//  }
mqtt.subscribe(&hotPfeed);//pub
Serial.println ("\n\n");
  
pinMode(moistSensor,INPUT);
pinMode(pumpPin, OUTPUT);

waitFor(Serial.isConnected,10000);
status = bme.begin(0x76);
 
if(!status)
Serial.printf("BME280 at address 0x%02x failed to start\n",0x76);
  //while(1);

Serial.println("Ready to GO");

display.begin(SSD1306_SWITCHCAPVCC,0X3C); 
display.setTextColor(WHITE);
display.display();
display.clearDisplay();
display.display();
}

void loop() {
MQTT_connect();
MQTT_ping();
display.clearDisplay();
display.setCursor(0,0);

quality = aqSensor.getValue();

Serial.printf("Sensor value: %i\n",quality);

moistSensorValue = analogRead(moistSensor);
if (moistSensorValue > 3560) { 
  digitalWrite(pumpPin, HIGH); 
  relayTimer.startTimer(1000); 
  Serial.println("Pump ON, moisture high");
} 
  
else{
  digitalWrite(pumpPin, LOW);
  Serial.println("Pump OFF, moisture low");
  }


if (relayTimer.isTimerReady()){
digitalWrite(pumpPin, LOW);
tempc = bme.readTemperature();
humidity = bme.readHumidity(); 
tempf = (tempc*9.0/5)+32;

display.printf("TEMPERATURE %0.1f\nHUMIDITY%0.1f\nMOISTURE %i",tempf,humidity,moistSensor);
display.display();
  
Serial.print("Sensor value: ");
Serial.println(aqSensor.getValue());

if (quality == AirQualitySensor::FORCE_SIGNAL) {
Serial.println("High pollution! Force signal active.");
  
  } else if (quality == AirQualitySensor::HIGH_POLLUTION) {

Serial.println("High pollution!");
display.printf("hight pollution");
display.display();

  }else if (quality == AirQualitySensor::LOW_POLLUTION) {

Serial.println("Low pollution!");
display.printf("low pollution");
display.display();

} else if (quality == AirQualitySensor::FRESH_AIR) {

Serial.println("Fresh air.");
display.printf("fresh air");
display.display(); 
}

  }  

  if((millis()-lastTime > 3000)) {
  if(mqtt.Update()) {
    
  moisture.publish(moistSensorValue);
    Serial.printf("Publishing %0.2f \n",ranValue); 
  }  
lastTime = millis();  
}
  
 
}

void MQTT_connect() {
int8_t ret;

  // Return if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");

  while ((subscription = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.printf("Error Code %s\n",mqtt.connectErrorString(ret));
    Serial.printf("Retrying MQTT connection in 5 seconds...\n");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds and try again
  }
  Serial.printf("MQTT Connected!\n");
}


bool MQTT_ping() {
  static unsigned int last;
  bool pingStatus;

  if ((millis()-last)>120000) {
    Serial.printf("Pinging MQTT \n");
    pingStatus = mqtt.ping();
  if(!pingStatus) {
    Serial.printf("Disconnecting \n");
    mqtt.disconnect();
    }
  last = millis();
    
  }
  return pingStatus;
  }
