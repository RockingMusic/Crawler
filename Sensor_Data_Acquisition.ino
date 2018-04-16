////////////////////////////////////////////////////////////////////////
//intialize variables
////////////////////////////////////////////////////////////////////////
//Sensors
#include <DHT.h>//Library for DHT22
#include <Wire.h>//Library for DHT22 communication
#include <SoftwareSerial.h>//Library for communication to data logging system.
#include "MS5837.h"//Library for pressure sensor

SoftwareSerial data(10, 11);  //Initialize Software Serial communication with data logging system. Only need TX (pin 11)

//Sensors
#define DHTPIN 3//Pin for communicating with DHT
#define DHTTYPE DHT22//DHT sensor type
#define SOSPIN 6//Leak sensor pin

MS5837 sensor;//Initialize pressure sensor as sensor

DHT dht(DHTPIN, DHTTYPE);//Initialize DHT with pin to read on and DHT type

////////////////////////////////////////////////////////////////////////
//void setup()
////////////////////////////////////////////////////////////////////////
void setup() {
  // put your setup code here, to run once:
  ////////////////////////////////////////////////////////////////////////
  //initialize sensors
  ////////////////////////////////////////////////////////////////////////
  pinMode(SOSPIN, INPUT);//Sets up pin to read logic high from leak sensor
  Serial.begin(9600);           //Start Serial for output
  Serial.println("Starting!");
  data.begin(2400);//Data transmission rate to data logging system
  dht.begin();//Starts the DHT
  Wire.begin();//Starts Wire communication with DHT                                                                                                                                                                                                                                                                                                                                 

  while(!sensor.init()){//Makes sure the pressure sensor is connected before continuing to collect data.
    Serial.println("Pressure sensor initilization failed!");
    Serial.println("White wire = SDA, Green wire = SCL");
    delay(5000);
  }
  sensor.setModel(MS5837::MS5837_30BA);//Sets the pressure sensor to the 30 Bar model.
  sensor.setFluidDensity(997);// kg/m^3 (997 freshwater, 1029 for seawater)//Sets the fluid density for the sensor readings.
}

////////////////////////////////////////////////////////////////////////
//void loop()
////////////////////////////////////////////////////////////////////////
void loop() {
  // put your main code here, to run repeatedly:

  ////////////////////////////////////////////////////////////////////////
  //get DHT22 sensor readings and output readings
  ////////////////////////////////////////////////////////////////////////
  float humidity = dht.readHumidity();//Reads humidity from DHT as float value.
  float tempin = dht.readTemperature(); //Default read DHT temp as Celsius as float value( true for Fahrenheit)
  if(isnan(humidity)||isnan(tempin)){//isnan checks to see if value is not a number. Indicating the values were not read.
    Serial.println("Failed to read from DHT Sensor!");
  }

  float heatC = dht.computeHeatIndex(tempin, humidity, false);//Computes heat in Celsius using temp and humidity
  Serial.println("\n****DHT****");
  Serial.println("Humidity: "+String(humidity));
  data.print(String(humidity) + ",");//Sends humidity to the data logging system as comma parsed string
  Serial.println("Heat: "+String(heatC));
  data.print(String(heatC) + ",");//Sends heat to the data logging system as a comma parsed string
  Serial.println("Temperature inside: "+String(tempin));
  data.print(String(tempin) + ",");//Sends inside temperature to the data logging system as a comma parsed string
  ////////////////////////////////////////////////////////////////////////
  //get internal and external temperature sensor readings and output readings
  ////////////////////////////////////////////////////////////////////////

  sensor.read();//Read information from the pressure sensor
  
  Serial.print("External Temperature: "); 
  Serial.print(sensor.temperature()); 
  Serial.println(" deg C");
  data.print(String(sensor.temperature())+ ',');//Sends the pressure sensor temperature(outside temperature) to the data logging system as a comma parsed string

  Serial.print("Pressure: ");
  Serial.print(sensor.pressure());
  Serial.println("mbar");
  data.print(String(sensor.pressure()) + ",");//Sends the pressure sensor pressure to the data logging system as a comma parsed string
  
  Serial.print("Depth: "); 
  Serial.print(sensor.depth()); 
  Serial.println(" m");
  data.print(String(sensor.depth()) + ",");//Sends the pressure sensor depth to the data logging system as a comma parsed string
  
  Serial.print("Altitude: "); 
  Serial.print(sensor.altitude());//Pressure sensor can also read altitude above sea level.
  Serial.println(" m above mean sea level");
  
  int leak = digitalRead(SOSPIN);//Check the leak sensor pin for a logic high, indicating a leak
  while(leak == HIGH){//While there is a leak.
    Serial.println("LEAK!!!!");
    data.print("LEAK!!!,");
    leak = digitalRead(SOSPIN);
  }
  delay(10000);
}

