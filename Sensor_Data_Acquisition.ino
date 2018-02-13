////////////////////////////////////////////////////////////////////////
//intialize variables
////////////////////////////////////////////////////////////////////////
//Sensors
#include <DHT.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include "MS5837.h"

SoftwareSerial data(10, 11);  //Only need TX which is second number

//Sensors
#define DHTPIN 3
#define DHTTYPE DHT22
#define SOSPIN 6

MS5837 sensor;

DHT dht(DHTPIN, DHTTYPE);

////////////////////////////////////////////////////////////////////////
//void setup()
////////////////////////////////////////////////////////////////////////
void setup() {
  // put your setup code here, to run once:
  ////////////////////////////////////////////////////////////////////////
  //initialize sensors
  ////////////////////////////////////////////////////////////////////////
  pinMode(SOSPIN, INPUT);
  Serial.begin(9600);           //Start Serial for output
  Serial.println("Starting!");
  data.begin(4800);
  dht.begin();
  Wire.begin();

  while(!sensor.init()){
    Serial.println("Pressure sensor initilization failed!");
    Serial.println("White wire = SDA, Green wire = SCL");
    delay(5000);
  }
  sensor.setModel(MS5837::MS5837_30BA);
  sensor.setFluidDensity(997);// kg/m^3 (997 freshwater, 1029 for seawater)
}

////////////////////////////////////////////////////////////////////////
//void loop()
////////////////////////////////////////////////////////////////////////
void loop() {
  // put your main code here, to run repeatedly:
  int leak = digitalRead(SOSPIN);
  if(leak == HIGH){
    Serial.println("LEAK!!!!");
  }
  else if(leak == LOW){
    Serial.println("Dry");
  }
  ////////////////////////////////////////////////////////////////////////
  //get DHT22 sensor readings and output readings
  ////////////////////////////////////////////////////////////////////////
  float humidity = dht.readHumidity();
  float tempin = dht.readTemperature(); //Default read temp as Celsius ( true for Fahrenheit)
  float tempout = 0.00;
  if(isnan(humidity)||isnan(tempin)){//isnan checks to see if value is not a number.
    Serial.println("Failed to read from DHT Sensor!");//Checks to see if values were read.
  }

  float heatC = dht.computeHeatIndex(tempin, humidity, false);
  Serial.println("\n****DHT****");
  Serial.println("Humidity: "+String(humidity));
  data.print(String(humidity) + ",");
  Serial.println("Heat: "+String(heatC));
  data.print(String(heatC) + ",");
  Serial.println("Temperature inside: "+String(tempin));
  data.print(String(tempin) + ",");
  ////////////////////////////////////////////////////////////////////////
  //get internal and external temperature sensor readings and output readings
  ////////////////////////////////////////////////////////////////////////

  sensor.read();
  
  Serial.print("External Temperature: "); 
  Serial.print(sensor.temperature()); 
  Serial.println(" deg C");
  data.print(String(sensor.temperature())+ ',');

  Serial.print("Pressure: ");
  Serial.print(sensor.pressure());
  Serial.println("mbar");
  data.print(String(sensor.pressure()) + ",");
  
  Serial.print("Depth: "); 
  Serial.print(sensor.depth()); 
  Serial.println(" m");
  data.print(String(sensor.depth()) + ",");
  
  Serial.print("Altitude: "); 
  Serial.print(sensor.altitude()); 
  Serial.println(" m above mean sea level");
  
  delay(10000);
}

