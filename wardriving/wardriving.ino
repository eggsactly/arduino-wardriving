#include <SPI.h>
#include <SD.h>
#include <TinyGPS++.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 lcd = Adafruit_SSD1306();

#define ARDUINO_USD_CS 15 // Pin D8
#define LOG_FILE_PREFIX "gpslog"
#define MAX_LOG_FILES 100
#define LOG_FILE_SUFFIX "csv" 
char logFileName[13];
#define LOG_COLUMN_COUNT 13
const String csvHeader = "WigleWifi-1.4,appRelease=2.26,model=SCH-I545,release=5.0.1,device=jfltevzw,display=LRX22C.I545VRSGPL1,board=MSM8960,brand=Verizon";
char * log_col_names[LOG_COLUMN_COUNT] = {
  "MAC" ,"SSID", "AuthMode", "FirstSeen", "Channel" ,"RSSI", "CurrentLatitude", "CurrentLongitude", "AltitudeMeters", "AccuracyMeters", "Type"
};

#define LOG_RATE 2000
unsigned long lastLog = 0;

TinyGPSPlus tinyGPS;
#define GPS_BAUD 9600 // GPS module's default baud rate

int display = 1;

#define SerialMonitor Serial

void setup() {
  Serial.begin(GPS_BAUD);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  // initialize with the I2C addr 0x3C (for the 128x32)
  lcd.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  lcd.clearDisplay();
  lcd.display();
  lcd.setCursor(0, 0);
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
//  while (Serial.available() > 0)
//    tinyGPS.encode(Serial.read());
    
  if ((lastLog + LOG_RATE) <= millis()) {
    if (tinyGPS.location.isUpdated()) {
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
      lcd.clearDisplay();
      lcd.setCursor(0, 0);
      lcd.print("No GPS data");
      lcd.setCursor(0, 1);
      lcd.print("Sats: ");
      lcd.print(tinyGPS.satellites.value());
      lcd.display();
      SerialMonitor.print("No GPS data. Sats: ");
      SerialMonitor.println(tinyGPS.satellites.value());
      delay(100);
    }
  }
  while (Serial.available())
    tinyGPS.encode(Serial.read());
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
