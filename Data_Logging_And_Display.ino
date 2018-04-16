////////////////////////////////////////////////////////////////////////
//Initialize variables
////////////////////////////////////////////////////////////////////////
//SD
#include <SPI.h>//SD card communication library
#include <SD.h>//SD card library

//Serial Communication
#include <SoftwareSerial.h>//Software serial communication library for communicating with the data acquisition system

//LCD Display
#include <LiquidCrystal_I2C.h>//LCD display I2C communication library
#include <LCD.h>//LCD display library
#include <Wire.h>//Communication with LCD display using wire

//Display Pins are predefined for I2C communication
#define I2C_ADDR 0x27 //Address
#define Rs_pin 0
#define Rw_pin 1
#define En_pin 2
#define BACKLIGHT_PIN 3
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7
//SD card Pins predefined for selecting SD card via pin 4 and SD disconnect via pin 8
#define chipSelect 4
#define SDCON 8

LiquidCrystal_I2C lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);//Initialize I2C communication with LCD display

SoftwareSerial mega(10, 11);//Initialize software serial as mega, receiving on pin 10, transmitting on pin 11. Pin 11 not used.

File myData;//Initialize a save file for logging data
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
  lcd.begin(20, 4); //Initialize lcd using I2C as a 20x4 display                                                              
  Serial.begin(9600);
  mega.begin(2400);//Initialize data transmission and receive rate from data acquisition system
  pinMode(SDCON, INPUT);//Initialize the SD card disconnect pin
  
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);//Turn lcd backlight on
  lcd.setBacklight(HIGH);//Turn lcd backlight to high brightness
  lcd.home();//Places lcd cursor at the first place
  lcd.print("Starting");
  delay(1000);

  ////////////////////////////////////////////////////////////////////////
  //Initialize SD Card
  ////////////////////////////////////////////////////////////////////////
  if(!SD.begin(chipSelect)){//Initialize and check to see if SD card is connected
    Serial.println("Card failed, make sure a card is connected.");
    lcd.clear();//Clear lcd screen
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
    if(SD.exists("Data.txt")){//If t6he data file "Data.txt" exists, delete the data file and create a new one.
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
      myData = SD.open("Data.txt", FILE_WRITE);//Open data file to write.
      myData.println("Humidity, Heat, TempIN, TempOUT, Pressure, Depth");//Sets the column names in the data file, using commas to parse data
      myData.close();//Close data file. Important! If file is not closed, can lead to data corruption!
      delay(2500);
      lcd.clear();
      lcd.home();
      lcd.print("New File Created.");
    }
    else{//If the data file does not exist, create it and initialize the file with the column names.
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
  while(!mega.available());//Checks to make sure the data logging system is getting information form the the data acquisition system
  Serial.println("Mega on");
  for(int i = 0; i <= 5; i++){//Collects the string data from the data acquisition system.
    sData[i] = mega.readStringUntil(',');//Logs the string data being sent to the data logging system, in an array.
    while(!mega.available());//Checks to make sure data is still being sent to the data logging system
  }
  if (sData[0] == String("LEAK!!!!"))//If LEAK!!!! is sent up in the first string. Display LEAK!!!! on the LCD display
  {
    lcd.clear();
    lcd.home();
    lcd.print("LEAK!!!!");
    lcd.setCursor(0,1);
    lcd.print("LEAK!!!!");
    lcd.setCursor(0,2);
    lcd.print("LEAK!!!!");
    lcd.setCursor(0,3);
    lcd.print("LEAK!!!!");
    while(1);//Locks the device until restarted.
  }
  Serial.println("Humidity: " + sData[0]);//Displays all the received data in console window
  Serial.println("Heat:     " + sData[1]);
  Serial.println("TempIN:   " + sData[2]);
  Serial.println("TempOUT:  " + sData[3]);
  Serial.println("Pressure: " + sData[4]);
  Serial.println("Depth: " + sData[5] + "\n");
  lcd.clear();//Clears the LCD display  
  lcd.home();
  lcd.print("Inside Temp: " + sData[2]);//Display the inside temperature on the LCD display, on the first line
  lcd.setCursor(0, 1);
  lcd.print("Outside Temp: " + sData[3]);//Display the outside temperature on the LCD display, on the second line
  lcd.setCursor(0, 2);
  lcd.print("Pressure: " + sData[4]);//Display the pressure on the LCD display, on the third line
  lcd.setCursor(0, 3);
  lcd.print("Depth: " + sData[5]);//Display the depth on the LCD display, on the fourth line
    

  ////////////////////////////////////////////////////////////////////////
  //log sensor data
  ////////////////////////////////////////////////////////////////////////
  wData1 = sData[0] + "," + sData[1] + "," + sData[2] + "," + sData[3] + "," + sData[4] + ",";//Format the collected string data into a single string
  wData2 = sData[5];//Format the remaining collected string data into a second string, due to arduino limitations
  DClog = digitalRead(SDCON);//Read the SD connection pin, to make sure the SD card is not set to be disconnected(HIGH).
  if(DClog == HIGH){//Disconnect SD card.//If SD connect pin is HIGH, do not read data.
    while(1){
      lcd.clear();
      lcd.home();
      lcd.print("SD safe to remove.");
      lcd.setCursor(0,1);
      lcd.print("Insert SD when done.");
      lcd.setCursor(0,2);
      lcd.print("Restart device");
      lcd.setCursor(0,3);
      lcd.print("Inside Temp: " + sData[2]);//Displays Inside temperature or leak in the event of a leak
      for(int i = 0; i <= 5; i++){//Collects data from data acquisiton system to display a leak if a leak occurs
        sData[i] = mega.readStringUntil(',');//Read data acquisition data into an array, parsing data with a comma
        while(!mega.available());//Checks to make sure data is being received from the data acquisition system
      }
  }
  }
  else{
      saveData(myData, wData1, wData2);//If the SD connect pin is not a logic high, save the data to the SD card
  }
}
//SD logging function.
////////////////////////////////////////////////////////////////////////
//log data to SD card
////////////////////////////////////////////////////////////////////////
void saveData(File FData, String Data1, String Data2){//SD card save data function
  FData = SD.open("Data.txt", FILE_WRITE);//Open data file to append data to the file
  if(FData){//if the file opens
    FData.print(Data1);//Print the first data string to file
    FData.println(Data2);//print the second data string to file
    FData.close();//Close file to save the data
  }
  else{//If the file does not open
    Serial.println("SD Card not connected. Reconnect the SD to log data.");
    lcd.clear();
    lcd.home();
    lcd.print("SD card DC.");
    lcd.setCursor(0, 1);
    lcd.print("Data not logged.");
  }
}

