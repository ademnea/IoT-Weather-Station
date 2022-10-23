/*
This code Obtains Weather Data and uploads the same to a website of choice; ThingSpeak or AdEMNEA. 
You may not use, reproduce, change or violate any piece of code or the entire code in itself unless written permission is obtained from the Author.
Author: Muhairwe Arnold, ENGINEER.
engineer.arnoldmuhairwe@gmail.com
+256755230906
*/

#include <TimerOne.h>
#include <math.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Sodaq_DS3231.h>
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define Bucket_Size 0.1   
#define RainGauge_Pin (3)         
#define Anemometer_Pin (2)      
#define WindVane_Pin (A3)      
#define WindVaneOffset 0       

volatile unsigned long tipCount;    
volatile unsigned long contactTime; 
volatile bool requireMeasurement;      
volatile unsigned int  timerCount;    
volatile unsigned long rotations;    
volatile unsigned long contactBounceTime; 
volatile float WindSpeed;
volatile float totalRainfall;     
int previousWindDirection;    
int vaneValue;      
int vaneDirection;  
int WindDirection;    
int lastDirection;    
String wind_speed, wind_direction, temperature, humidity, rain_fall, pressure, rain_today;

char weekDay[][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
DateTime myDate(2022, 7, 8, 15, 23, 0, 5);
File iotWeather;
Adafruit_BME280 bmeReadings;
SoftwareSerial haqen (7, 8); //Rx, Tx

DateTime now;
int newHour = 0;
int oldHour = 0;

void setup() {

  tipCount = 0;
  totalRainfall = 0;

  lastDirection = 0;
  rotations = 0;
  requireMeasurement = false;

  timerCount = 0;

  bmeReadings.begin();
  haqen.begin(9600);
  Serial.begin(9600);

  Wire.begin();
  rtc.begin();
  rtc.setDateTime(myDate);
  while (!Serial) {
    ;
  }
  Serial.print(F("Locating SD card..."));
  if (!SD.begin(10)) {
    Serial.println(F("Process failed!"));
    while (1);
  }
  now = rtc.now();
  oldHour = now.hour();
  delay(5000);

  pinMode(RainGauge_Pin, INPUT);
  pinMode(Anemometer_Pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(RainGauge_Pin), isr_raingauge, FALLING);
  attachInterrupt(digitalPinToInterrupt(Anemometer_Pin), isr_anemometer, FALLING);

  Timer1.initialize(1000000);
  Timer1.attachInterrupt(isr_timer);

  sei();// Enable Interrupts
}

void loop() {
  Serial.print(F("Temperature: "));
  Serial.print(bmeReadings.readTemperature());
  Serial.print(F(" ºC"));
  Serial.print(F(" | "));
  Serial.print(F("Pressure: "));
  Serial.print(bmeReadings.readPressure() / 100.0F);
  Serial.print(F(" hPa "));
  Serial.print(F(" | "));
  Serial.print(F("Humidity: "));
  Serial.print(bmeReadings.readHumidity());
  Serial.print(F(" %"));
  Serial.print(F(" | "));
  if (requireMeasurement) {
    getWindDirection();
    Serial.print(F("Rainfall: ")); Serial.print(totalRainfall); Serial.print(F(" mm  ")); Serial.print(F("| "));
    Serial.print(F("Wind Speed: ")); Serial.print(WindSpeed); Serial.print(F(" m/s  ")); Serial.print(F("| "));
    Serial.print(F("Wind Direction: ")); Serial.print(WindDirection); Serial.print(F("º "));
    getHeading(WindDirection);
    requireMeasurement = false;
  }
  delay(2000);

  iotWeather = SD.open("weather.txt", FILE_WRITE);
  now = rtc.now();

  Serial.print("Writing to SD Card...");
  iotWeather.print(now.year());
  iotWeather.print('/');
  iotWeather.print(now.month());
  iotWeather.print('/');
  iotWeather.print(now.date());
  iotWeather.print(F("::"));
  iotWeather.print(now.hour());
  iotWeather.print(":");
  iotWeather.print(now.minute());
  iotWeather.print(F("  "));
  iotWeather.print(F(" | "));
  iotWeather.print(bmeReadings.readTemperature());
  iotWeather.print(F(" ºC "));
  iotWeather.print(F(" | "));
  iotWeather.print(bmeReadings.readPressure() / 100.0F);
  iotWeather.print(F(" hPa "));
  iotWeather.print(F(" | "));
  iotWeather.print(bmeReadings.readHumidity());
  iotWeather.print(F(" % "));
  iotWeather.print(F(" | "));
  iotWeather.print(totalRainfall);
  iotWeather.print(F(" mm ")); iotWeather.print(F(" | "));
  iotWeather.print(WindSpeed);
  iotWeather.print(F(" m/s ")); iotWeather.print(F(" | "));
  iotWeather.print(WindDirection);
  iotWeather.println(F("º ")); iotWeather.print(F(" |"));
  //getHeading(WindDirection);

  iotWeather.close();
  Serial.println(F("Weather Data Successfully written to SD Card."));
  delay(2000);
  now = rtc.now();
  newHour = now.hour();
  if (oldHour != newHour) {
    oldHour = newHour;
  }
  readSensors();
  Serial.println(F("Uploading Weather Data..."));
  sendData();
  if (haqen.available()) {
    Serial.write(haqen.read());
    Serial.println(F("Weather Data Uploaded Successfully."));
  }
}

void isr_timer() {

  timerCount++;

  if (timerCount == 5) {
    // convert to mp/h using the formula V=P(2.25/T)
    // V = P(2.25/2.5) = P * 0.9....CAN BE 0.75
    WindSpeed = (rotations * 0.9 * 0.447);
    rotations = 0;
    requireMeasurement = true;
    timerCount = 0;
  }
}

void isr_anemometer ()   {

  if ((millis() - contactBounceTime) > 15 ) {  
    rotations++;
    contactBounceTime = millis();
  }
}

void isr_raingauge() {

  if ((millis() - contactTime) > 15 ) { 
    tipCount++;
    totalRainfall = tipCount * Bucket_Size;
    contactTime = millis();
  }
}

void getWindDirection() {

  vaneValue = analogRead(WindVane_Pin);
  vaneDirection = map(vaneValue, 0, 1023, 0, 360);
  WindDirection = vaneDirection + WindVaneOffset;

  if (WindDirection > 360)
    WindDirection = WindDirection - 360;

  if (WindDirection < 0)
    WindDirection = WindDirection + 360;
}

void getHeading(int direction) {
  if (direction > 337 and direction <= 22)
    Serial.println(F("North"));
  else if (direction > 22 and direction <= 67)
    Serial.println(F("North East"));
  else if (direction > 67 and direction <= 112)
    Serial.println(F("East"));
  else if (direction > 112 and direction <= 157)
    Serial.println(F("South East"));
  else if (direction > 157 and direction <= 202)
    Serial.println(F("South"));
  else if (direction > 202 and direction <= 247)
    Serial.println(F("South West"));
  else if (direction > 247 and direction <= 292)
    Serial.println(F("West"));
  else if (direction > 292 and direction <= 337)
    Serial.println(F("North West"));
  else
    Serial.println(F("North"));
}

void readSensors() {
  wind_speed = String(WindSpeed);
  wind_direction = String(WindDirection);
  temperature = String(bmeReadings.readTemperature());
  rain_fall = String(totalRainfall);
  humidity = String(bmeReadings.readHumidity());
  pressure = String(bmeReadings.readPressure() / 100.0F);
  //rain_today = String(dailyRainfall, 8);
}

void sendData() {
  haqen.println(F("AT"));
  delay(2000);
  showSerialData();

  haqen.println(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""));
  delay(2000);
  showSerialData();

  haqen.println(F("AT+SAPBR=3,1,\"APN\",\"internet\""));
  delay(2000);
  showSerialData();

  haqen.println(F("AT+SAPBR=1,1"));
  delay(2000);
  showSerialData();

  haqen.println(F("AT+SAPBR=2,1"));
  delay(2000);
  showSerialData();

  haqen.println(F("AT+HTTPINIT"));
  delay(2000);
  showSerialData();

  //Upload to both ThingSpeak and Site at AdEMNEA
  haqen.println("AT+HTTPPARA=\"URL\",\"http://api.thingspeak.com/update?api_key=X2PWUZ7E85TSZD7A&field1=" + temperature + "&field2=" + humidity + "&field3=" + pressure + "&field4=" + rain_fall + "&field5=" + wind_speed + "&field6=" + wind_direction + "\"");
  delay(8000);
  showSerialData();

  /*
    haqen.println("AT+HTTPPARA=\"URL\",\"https://ademnea.net/insert.php?temperature=" + temperature + "&humidity=" + humidity + "&pressure=" + pressure + "&rainfall=" + rain_fall + "&wind_speed=" + wind_speed + "&wind_direction=" + wind_direction + "&dailyrainfall=" + rain_today + "\"");
    delay(2000);
    showSerialData();
  */
  
  haqen.println(F("AT+HTTPACTION=0")); 
  delay(6000);
  showSerialData();

  haqen.println(F("AT+HTTPTERM"));
  showSerialData();
  delay(300);

  haqen.println(F("AT+SAPBR=0,1"));
  delay(2000);
  showSerialData();

  haqen.println("");
  delay(7000);
}

void showSerialData()
{
  while (haqen.available() != 0) {
    Serial.write(char (haqen.read()));
  }
}
