
/* 
 * Project L03_01_button
 * Author:Joseph maes
 * Date: 10/10/2024
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
const int sensor =A1;
int inputValue;



// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);



// setup() runs once, when the device is first turned on
void setup() {
  pinMode(sensor,INPUT);
  Serial.begin(9600);
  waitFor(Serial.isConnected,10000);
  
  Serial.println (" Ready to Go");

  // Put initialization like pinMode and begin functions here
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  inputValue=analogRead(sensor);
  Serial.printf("sensor=%i\n",inputValue);

}
