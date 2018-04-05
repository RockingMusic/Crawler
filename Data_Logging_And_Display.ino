////////////////////////////////////////////////////////////////////////
//Initialize variables
////////////////////////////////////////////////////////////////////////
//SD
#include <SPI.h>
#include <SD.h>

//Serial Communication
#include <SoftwareSerial.h>

//LCD Display
#include <LiquidCrystal_I2C.h>
#include <LCD.h>
#include <Wire.h>

//Display
#define I2C_ADDR 0x27 //Address
#define Rs_pin 0
#define Rw_pin 1
#define En_pin 2
#define BACKLIGHT_PIN 3
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7
//SD
#define chipSelect 4
#define SDCON 8
LiquidCrystal_I2C lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);

SoftwareSerial mega(10, 11);// (Rx, Tx)

File myData;
String fDone, sData[5], wData1, wData2;
int DClog;

////////////////////////////////////////////////////////////////////////
//void setup()
////////////////////////////////////////////////////////////////////////
void setup() {
  // put your setup code here, to run once:
  ////////////////////////////////////////////////////////////////////////
  //Initialize LCD display and software serial communications.
  ////////////////////////////////////////////////////////////////////////
  lcd.begin(20, 4); //20x4 LCD                                                              
  Serial.begin(9600);
  mega.begin(2400);
  pinMode(SDCON, INPUT);
  
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.home();
  lcd.print("Starting");
  delay(1000);

  ////////////////////////////////////////////////////////////////////////
  //Initialize SD Card
  ////////////////////////////////////////////////////////////////////////
  if(!SD.begin(chipSelect)){
    Serial.println("Card failed, make sure a card is connected.");
    lcd.clear();
    lcd.home();
    lcd.print("Card failed");
  }
  else{
    Serial.println("Card Initialized");
    lcd.clear();
    lcd.home();
    lcd.print("Card Initialized");
    delay(1000);
    
    ////////////////////////////////////////////////////////////////////////
    //Initialize SD file
    ////////////////////////////////////////////////////////////////////////
    if(SD.exists("Data.txt")){
      Serial.println("Data exists.");
      lcd.clear();
      lcd.home();
      lcd.print("Data exists");
      delay(5000);
      lcd.clear();
      lcd.home();
      lcd.print("Deleting Data!");
      Serial.println("Deleting Data!");
      SD.remove("Data.txt");
      myData = SD.open("Data.txt", FILE_WRITE);
      myData.println("Humidity, Heat, TempIN, TempOUT, Pressure, Depth");
      myData.close();
      delay(2500);
      lcd.clear();
      lcd.home();
      lcd.print("New File Created.");
    }
    else{
      myData = SD.open("Data.txt", FILE_WRITE);
      lcd.clear();
      lcd.home();
      lcd.print("File Created.");
      myData.println("Humidity, Heat, TempIN, TempOUT, Pressure, Depth");
      myData.close();
    } 
  }
}
////////////////////////////////////////////////////////////////////////
//void loop()
////////////////////////////////////////////////////////////////////////
void loop() {
  // put your main code here, to run repeatedly:
  ////////////////////////////////////////////////////////////////////////
  //collect data from sensor ardiuino and display
  ////////////////////////////////////////////////////////////////////////
  Serial.println("Loop Start");
  while(!mega.available());
  Serial.println("Mega on");
  for(int i = 0; i <= 5; i++){
    sData[i] = mega.readStringUntil(',');
    while(!mega.available());
  }
  Serial.println("Humidity: " + sData[0]);
  Serial.println("Heat:     " + sData[1]);
  Serial.println("TempIN:   " + sData[2]);
  Serial.println("TempOUT:  " + sData[3]);
  Serial.println("Pressure: " + sData[4]);
  Serial.println("Depth: " + sData[5] + "\n");
  lcd.clear();  
  lcd.home();
  lcd.print("Inside Temp: " + sData[2]);
  lcd.setCursor(0, 1);
  lcd.print("Outside Temp: " + sData[3]);
  lcd.setCursor(0, 2);
  lcd.print("Pressure: " + sData[4]);
  lcd.setCursor(0, 3);
  lcd.print("Depth: " + sData[5]);
    

  ////////////////////////////////////////////////////////////////////////
  //log sensor data
  ////////////////////////////////////////////////////////////////////////
  wData1 = sData[0] + "," + sData[1] + "," + sData[2] + "," + sData[3] + "," + sData[4] + ",";
  wData2 = sData[5];
  DClog = digitalRead(SDCON);
  if(DClog == HIGH){//Disconnect SD card.
    while(1){
      lcd.clear();
      lcd.home();
      lcd.print("SD safe to remove.");
      lcd.setCursor(0,1);
      lcd.print("Insert SD when done.");
      lcd.setCursor(0,2);
      lcd.print("Restart device");
      lcd.setCursor(0,3);
      lcd.print("Inside Temp: " + sData[2]);
      for(int i = 0; i <= 5; i++){
        sData[i] = mega.readStringUntil(',');
        while(!mega.available());
      }
  }
  }
  else{
      saveData(myData, wData1, wData2);
  }
}
//SD logging function.
////////////////////////////////////////////////////////////////////////
//log data to SD card
////////////////////////////////////////////////////////////////////////
void saveData(File FData, String Data1, String Data2){ 
  FData = SD.open("Data.txt", FILE_WRITE);
  if(FData){
    FData.print(Data1);
    FData.println(Data2);
    FData.close();
  }
  else{
    Serial.println("SD Card not connected. Reconnect the SD to log data.");
    lcd.clear();
    lcd.home();
    lcd.print("SD card DC.");
    lcd.setCursor(0, 1);
    lcd.print("Data not logged.");
  }
}

