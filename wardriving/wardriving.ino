#include <SPI.h>
#include <SD.h>
#include <TinyGPS++.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_FeatherOLED.h>

Adafruit_FeatherOLED lcd = Adafruit_FeatherOLED();

#define BUTTON_A 0
#define BUTTON_C 2

#define ARDUINO_USD_CS 15 // Pin D8
#define LOG_FILE_PREFIX "gpslog"
#define MAX_LOG_FILES 100
#define LOG_FILE_SUFFIX "csv" 
char logFileName[13];
#define LOG_COLUMN_COUNT 13
const String csvHeader = "WigleWifi-1.4,appRelease=2.26,model=Feather,release=0.0.0,device=myDevice,display=3fea5e7,board=esp8266,brand=Adafruit";
char * log_col_names[LOG_COLUMN_COUNT] = {
  "MAC" ,"SSID", "AuthMode", "FirstSeen", "Channel" ,"RSSI", "CurrentLatitude", "CurrentLongitude", "AltitudeMeters", "AccuracyMeters", "Type"
};

// Keeps track of the GPS unavailable time
unsigned long long gpsUnavailableTime;
// The number of time allowed between no gps input
const unsigned long long gpsUnavailableCheckInterval = 1000;
// Digital pin 16 is for detecting the GPS fix
const int fixPin = 16;
// Amount of time to sample the fix pin
const unsigned long long fixSamplePeriod = 1000; // Milliseconds
// The maximum amount of time allowed to pass over the sample period
const unsigned long long fixSamplePeriodOverShoot = 500; // Milliseconds
// Used to store the last time the fix pin was sampled
unsigned long long fixSampleTime;
// Indicates if there is a fix pin
bool hasFix;
// Stores samples of the record 
byte fixRecordArray[4];
// Holds current index of the record array
byte fixRecordIndex;
// fullSampleCycle indicates when fixRecordArray has been fully populated with values
bool fullSampleCycle;
// Indicates if the GPS is outputting values
bool isGpsAvailable = true;

#define LOG_RATE 2000
unsigned long lastLog = 0;

unsigned long long batteryCheck;
// Update battery level every 15 seconds
const unsigned long long batteryCheckPeriod = 15000;

TinyGPSPlus tinyGPS;
#define GPS_BAUD 9600 // GPS module's default baud rate

int display = 1;

#define SerialMonitor Serial
#define gpsSerial Serial

void zeroOutFixRecordArray(){
  for(fixRecordIndex = 0; fixRecordIndex < sizeof(fixRecordArray)/sizeof(byte); fixRecordIndex++){
    fixRecordArray[fixRecordIndex] = 0;
  }
  fixRecordIndex = 0;
  fullSampleCycle = false;
}

byte updateFixRecordIndex(byte fixRecordVar){
  if(fixRecordVar + 1 >= sizeof(fixRecordArray)/sizeof(byte))
  {
    fullSampleCycle = true;
    return 0;
  }
  else {
    return fixRecordVar + 1;
  }
}

void checkFix(){
  // Get and store current time
  unsigned long long currentTime = millis();
  //Serial.println(millis());
  // If one fixSamplePeriod period has transpired since our last sample and 
  if((fixSampleTime + fixSamplePeriod <= currentTime) && 
    (fixSampleTime + fixSamplePeriod + fixSamplePeriodOverShoot >= currentTime))
  {
    // Calculate the new time to sample
    fixSampleTime = currentTime + fixSamplePeriod - (currentTime - fixSampleTime - fixSamplePeriod);
    fixRecordArray[fixRecordIndex] = digitalRead(fixPin);
    //Serial.println(fixRecordArray[fixRecordIndex]);
    fixRecordIndex = updateFixRecordIndex(fixRecordIndex);
  }
  // Zero everything out if we overshot our sample time
  else if(fixSampleTime + fixSamplePeriod + fixSamplePeriodOverShoot < currentTime)
  {
    zeroOutFixRecordArray();
  }

  byte oneCount = 0;
  if(fullSampleCycle)
  {
    // Check if more than two samples are one, then we have no fix
    for(byte i = 0; i < sizeof(fixRecordArray)/sizeof(byte); i++)
    {
      if(fixRecordArray[i] == 1)
      {
        oneCount++;
      }
    }
  
    if(oneCount > 1)
    {
      hasFix = false;
    }
    else
    {
      hasFix = true;
    }
  }
}

void battery_level() {
 
  // read the battery level from the ESP8266 analog in pin.
  // analog read level is 10 bit 0-1023 (0V-1V).
  // our 100k & 22K voltage divider takes the max
  // lipo value of 4.2V and drops it to 0.758V max.
  // this means our min analog read value should be 580 (3.14V)
  // and the max analog read value should be 774 (4.2V).
  int level = analogRead(A0);
  float vbat;
  // convert battery level to percent
  vbat = map(level, 580, 774, 3.4f, 4.26f);
  level = map(level, 580, 774, 0, 100);

  lcd.clearDisplay();
  lcd.setBattery(level);
  lcd.renderBattery();
  lcd.display();
 
}

void setup() {
  gpsSerial.begin(GPS_BAUD);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  pinMode(fixPin, INPUT_PULLDOWN_16);
  zeroOutFixRecordArray();
  hasFix = false;
  fixSampleTime = millis();
  gpsUnavailableTime = millis();
  isGpsAvailable = false;
  
  // initialize with the I2C addr 0x3C (for the 128x32 display)
  lcd.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  lcd.init();
  lcd.clearDisplay();
  lcd.setBatteryIcon(true);
  lcd.renderBattery();
  lcd.clearMsgArea();
  lcd.display();
  lcd.setCursor(0, 0);
  batteryCheck = millis();

  // Set up the input buttons, we don't use B because it conflicts with the fix line for GPS
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  
  lcd.print("Setting up SD card.");
  lcd.display();
  SerialMonitor.println("Setting up SD card.");
  delay(200);
  lcd.clearDisplay();
  if (!SD.begin()) {
    lcd.setCursor(0, 0);
    lcd.print("Error initializing SD card.");
    lcd.display();
    SerialMonitor.println("Error initializing SD card.");
  }
  updateFileName();
  printHeader();
}

void loop() {
//  while (gpsSerial.available() > 0)
//    tinyGPS.encode(gpsSerial.read());

  checkFix();

  // Only update the battery every 15 Seconds
  if(millis() - batteryCheck > batteryCheckPeriod) 
  {
    battery_level();
    batteryCheck = millis();
  }

  // Scan Buttons
  // A quick press of button A means to start/stop recording
  // A long press means to open settings
  if (! digitalRead(BUTTON_A)){
    
    
  }

  // Log GPS data
  if ((lastLog + LOG_RATE) <= millis()) {
    if (tinyGPS.location.isUpdated() && hasFix) {
      if (logGPSData()) {
        SerialMonitor.print("GPS logged ");
        SerialMonitor.print(tinyGPS.location.lat(), 6);
        SerialMonitor.print(", ");
        SerialMonitor.println(tinyGPS.location.lng(), 6);
        SerialMonitor.print("Seen networks: ");
        SerialMonitor.println(countNetworks());
        if (display == 1) {
          lcd.clearDisplay();
          lcd.setCursor(0, 0);
          lcd.print("Lat: ");
          lcd.print(tinyGPS.location.lat(), 6);
          lcd.setCursor(0, 1);
          lcd.print("Lon: ");
          lcd.print(tinyGPS.location.lng(), 6);
          lcd.display();
          display = 0;
        } else {
          lcd.clearDisplay();
          lcd.setCursor(0, 0);
          lcd.print("Seen: ");
          lcd.print(countNetworks());
          lcd.setCursor(0, 1);
          lcd.print("networks");
          lcd.display();
          display = 1;
        }
        lastLog = millis();
      } else {
        lcd.setCursor(0, 1);
        SerialMonitor.println("Failed to log new GPS data.");
      }
    } else {
      if(hasFix == false)
      {
        lcd.clearDisplay();
        lcd.setCursor(0, 0);
        lcd.print("Acquiring GPS fix");
        lcd.display();
      }
      else
      {
        lcd.clearDisplay();
        lcd.setCursor(0, 0);
        lcd.print("No GPS data");
        lcd.setCursor(0, 1);
        lcd.print("Sats: ");
        lcd.print(tinyGPS.satellites.value());
        lcd.display();
        SerialMonitor.print("No GPS data. Sats: ");
        SerialMonitor.println(tinyGPS.satellites.value());
      }
      delay(100);
    }
  }

  // Get the GPS information
  if(gpsSerial.available() == 0 && isGpsAvailable){
    gpsUnavailableTime = millis();
    isGpsAvailable = false;
  }
  if(gpsSerial.available() > 0){
    isGpsAvailable = true;
  }
  while (gpsSerial.available() > 0){
    tinyGPS.encode(gpsSerial.read());
  }

}
int countNetworks() {
  File netFile = SD.open(logFileName);
  int networks = 0;
  if(netFile) {
    while(netFile.available()) {
      netFile.readStringUntil('\n');
      networks++;
    }
    netFile.close();
    if (networks == 0) {
      return networks;
    } else {
      return (networks-1); //Avoid header count
    }
  }
}

byte logGPSData() {
  int n = WiFi.scanNetworks(); 
  if (n == 0) {
    SerialMonitor.println("no networks found");
  } else {
    for (uint8_t i = 1; i <= n; ++i) {
      if ((WiFi.channel(i) > 0) && (WiFi.channel(i) < 15)) { //Avoid erroneous channels
        File logFile = SD.open(logFileName, FILE_WRITE);
        SerialMonitor.println("New network found");
        logFile.print(WiFi.BSSIDstr(i));
        logFile.print(',');
        logFile.print(WiFi.SSID(i));
        logFile.print(',');
        logFile.print(getEncryption(i));
        logFile.print(',');
        logFile.print(tinyGPS.date.year());
        logFile.print('-');
        logFile.print(tinyGPS.date.month());
        logFile.print('-');
        logFile.print(tinyGPS.date.day());
        logFile.print(' ');
        logFile.print(tinyGPS.time.hour());
        logFile.print(':');
        logFile.print(tinyGPS.time.minute());
        logFile.print(':');
        logFile.print(tinyGPS.time.second());
        logFile.print(',');
        logFile.print(WiFi.channel(i));
        logFile.print(',');
        logFile.print(WiFi.RSSI(i));
        logFile.print(',');
        logFile.print(tinyGPS.location.lat(), 6);
        logFile.print(',');
        logFile.print(tinyGPS.location.lng(), 6);
        logFile.print(',');
        logFile.print(tinyGPS.altitude.meters(), 1);
        logFile.print(',');
        logFile.println(max(tinyGPS.hdop.value(), 1));
        logFile.print(',');
        logFile.print("WIFI");
        logFile.println();
        logFile.close();
      }
    }
  }
}

void printHeader() {
  File logFile = SD.open(logFileName, FILE_WRITE);
  if (logFile) {
    int i = 0;
    logFile.print(csvHeader);
    for (; i < LOG_COLUMN_COUNT; i++) {
      logFile.print(log_col_names[i]);
      if (i < LOG_COLUMN_COUNT - 1)
        logFile.print(',');
      else
        logFile.println();
    }
    logFile.close();
  }
}

void updateFileName() {
  int i = 0;
  for (; i < MAX_LOG_FILES; i++) {
    memset(logFileName, 0, strlen(logFileName));
    sprintf(logFileName, "%s%d.%s", LOG_FILE_PREFIX, i, LOG_FILE_SUFFIX);
    if (!SD.exists(logFileName)) {
      break;
    } else {
      SerialMonitor.print(logFileName);
      SerialMonitor.println(" exists");
    }
  }
  SerialMonitor.print("File name: ");
  SerialMonitor.println(logFileName);
}

String getEncryption(uint8_t network) {
  byte encryption = WiFi.encryptionType(network);
  switch (encryption) {
    case 2:
      return "[WPA-PSK-CCMP+TKIP][ESS]";
    case 5:
      return "[WEP][ESS]";
    case 4:
      return "[WPA2-PSK-CCMP+TKIP][ESS]";
    case 7:
      return "[ESS]";
    case 8:
      return "[WPA-PSK-CCMP+TKIP][WPA2-PSK-CCMP+TKIP][ESS]";
  }
}
