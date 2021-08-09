//@date August 2021

// SoftwareSerial to communicate with the ESP8266 module
#include <SoftwareSerial.h>
#include <SparkFunESP8266WiFi.h>
#include <Wire.h>
//Click here to get the library: http://librarymanager/All#SparkFun_SCD30
#include "SparkFun_SCD30_Arduino_Library.h"
#include "SparkFun_MS5637_Arduino_Library.h"

//////////////////////////////
// WiFi Network Definitions //
//////////////////////////////
// Replace these two character strings with the name and
// password of your WiFi network.
const char mySSID[] = "WeeFi";
const char myPSK[] = "salamander";

/////////////////////
// Server Constants //
/////////////////////
// Destination server:
const String phantServer = "data.sparkfun.com";
// Public key:
const String publicKey = "DJjNowwjgxFR9ogvr45Q";
// Private key:
const String privateKey = "P4eKwGGek5tJVz9Ar84n";
String httpHeader = "POST /input/" + publicKey + ".txt HTTP/1.1\n" +
                    "Host: " + phantServer + "\n" +
                    "Phant-Private-Key: " + privateKey + "\n" +
                    "Connection: close\n" + 
                    "Content-Type: application/x-www-form-urlencoded\n";

/////////////////////
// Sensor Constants /
/////////////////////
SCD30 airSensor;
MS5637 barometricSensor;


void setup() {
  int status;
  Serial.begin(9600);
  Wire.begin();
  
  // To turn the WiFi shield on, and verify communication
  // always begin a sketch by calling cell.begin().
  status = esp8266.begin();
  if (status <= 0) {
    Serial.println(F("Unable to communicate with shield. Looping"));
    while(1);
  }
  
  esp8266.setMode(ESP8266_MODE_STA); // Set WiFi mode to station
  if (esp8266.status() <= 0) { // If we're not already connected
    if (esp8266.connect(mySSID, myPSK) < 0) {
      Serial.println(F("Error connecting"));
      while (1) ;
    }    
  }
  
  // Get our assigned IP address and print it:
  Serial.print(F("My IP address is: "));
  Serial.println(esp8266.localIP());

  
  // Startup air sensor
  if (airSensor.begin() == false) {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1);
  }
  //Change number of seconds between measurements: 2 to 1800 (30 minutes)
  airSensor.setMeasurementInterval(4); 
  
  // Startup pressure sensor
  if (barometricSensor.begin() == false) {
    Serial.println("MS5637 sensor did not respond. Please check wiring.");
    while(1);
  }

}

void loop() {
  // check for new data
  if (airSensor.dataAvailable()) {
    // Set to current ambient pressure reading
    airSensor.setAmbientPressure(barometricSensor.getPressure());
    airSensor.setAltitudeCompensation(34); // Winter Hill 02145
    
    Serial.print("co2(ppm):");
    Serial.print(airSensor.getCO2());

    Serial.print(" temp(C):");
    Serial.print(airSensor.getTemperature(), 1);

    Serial.print(" humidity(%):");
    Serial.print(airSensor.getHumidity(), 1);

    Serial.println();

    //Pressure Sensor
    float temperature = barometricSensor.getTemperature();
    float pressure = barometricSensor.getPressure();

    Serial.print("Temperature=");
    Serial.print(temperature, 1);
    Serial.print("(C)");

    Serial.print(" Pressure=");
    Serial.print(pressure, 3);
    Serial.print("(hPa or mbar)");

    Serial.println();
  }
  else {
    //Serial.println("Waiting for new data");
    delay(2000);
  }
  // format data into http request
  // POST to server

}
