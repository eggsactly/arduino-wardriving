#include <SPI.h>
#include <SD.h>
#include <TinyGPS++.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_FeatherOLED.h>
#include "RTClib.h"


// Added this pound define to get around compiler error
// https://github.com/neu-rah/ArduinoMenu/issues/68 
#define typeof(x) __typeof__(x)

Adafruit_FeatherOLED lcd = Adafruit_FeatherOLED();

// Define the pin locations of the OLED buttons
#define BUTTON_A 0
#define BUTTON_C 2

// Pin D8
#define ARDUINO_USD_CS 15

#define SerialMonitor Serial
#define gpsSerial Serial

#define GPS_BAUD 9600 // GPS module's default baud rate

// RecordingSpeedStates indicates the heuristic for recording speed 
typedef enum  
{
  SLOW_RECORD = 0,
  MEDIUM_RECORD = 1,
  FAST_RECORD = 2,

  // RECORDING_SPEED_STATES_COUNT should always be last
  RECORDING_SPEED_STATES_COUNT
} RecordingSpeedStates;

// RecordingStates indicates whether the arduino should be recording or not
typedef enum 
{
  PAUSED_RECORDING = 0,
  RECORDING = 1,

  // RECORDING_STATES_COUNT should always be last 
  RECORDING_STATES_COUNT
} RecordingStates;


// New setting state to replace SettingStates and SettingMenuOptions
typedef enum
{
  MAIN_MENU,
  EXIT_SETTINGS,
  SET_TIMEZONE,
  DISPLAY_TIMEZONE,

  // SETTING_STATES_COUNT should always be last
  SETTING_STATES_COUNT 
} SettingStates;


// These three defines indicate the name of the log file 
const char LOG_FILE_PREFIX[] = "gpslog";
const uint16_t MAX_LOG_FILES = 10000;
const char LOG_FILE_SUFFIX[] = "csv";

// logFileName contains the name of the log file
// which will be in the format gpslog-YYYY-MM-DD-XXXX.csv
char logFileName[32];

// csvHeader contains the first line of text in the csv log file
const String csvHeader = "WigleWifi-1.4,appRelease=2.26,model=Feather,release=0.0.0,device=myDevice,display=3fea5e7,board=esp8266,brand=Adafruit";

// log_col_names contains the text of the first row of the log file
const char * log_col_names[] = {
  "MAC" ,
  "SSID",
  "AuthMode",
  "FirstSeen",
  "Channel",
  "RSSI",
  "CurrentLatitude",
  "CurrentLongitude",
  "AltitudeMeters",
  "AccuracyMeters",
  "Type"
};

// longPressTime defines how many milliseconds the user must press a button on 
// the OLED screen for it to be considered a long press 
const uint64_t longPressTime = 2000;

// buttonAPressTime records when button A was pressed
uint64_t buttonAPressTime;

// buttonALastSample indicates the state button A was put in when it last 
// transitioned state
byte buttonALastSample;

// buttonCPressTime records when button C was pressed
uint64_t buttonCPressTime;

// buttonCLastSample indicates the state button C was put in when it last 
// transitioned state
byte buttonCLastSample;

// recordingState keeps track of whether the arduino is recording or not
RecordingStates      recordingState;

// settingState keeps track of whether the settings menu should be displayed or
// not.
SettingStates        settingState;

// recordingSpeed indicates the current index of the recordingSpeedRecord array
RecordingSpeedStates recordingSpeed;

// This array of struct contains a record of the associations between speed 
// setting and time period between records
const uint16_t recordingSpeedRecord[RECORDING_SPEED_STATES_COUNT] = {
  5000, // SLOW_RECORD
  3000, // MEDIUM_RECORD
  2000, // FAST_RECORD
};

// Keeps track of the GPS unavailable time
uint64_t gpsUnavailableTime;

// The number of time allowed between no gps input
const uint64_t gpsUnavailableCheckInterval = 1000;

// Digital pin 16 is for detecting the GPS fix
const int fixPin = 16;

// Amount of time to sample the fix pin
const uint64_t fixSamplePeriod = 1000; // Milliseconds

// The maximum amount of time allowed to pass over the sample period
const uint64_t fixSamplePeriodOverShoot = 500; // Milliseconds

// Used to store the last time the fix pin was sampled
uint64_t fixSampleTime;

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

// lastLog indicates the time in milliseconds of the last file log
uint64_t lastLog = 0;

// Indicates if the GPS has updated the RTC time
bool updatedDate;

uint64_t batteryCheck;
// Update battery level every 15 seconds
const uint64_t batteryCheckPeriod = 15000;

// Update rate of the LCD, 
const uint64_t lcdRefreshRate = 2000;

// Time LCD screen was last updated
uint64_t lcdLastUpdate;

// Indicates if the SD card has been found
bool hasSdCard;

// Create the gps connection 
TinyGPSPlus tinyGPS;

// Display indicates whether the lat/long or Wifi info should be printed on the
// OLED screen.
byte display = 1;

// Number of wifi networks observed
int n;

// Battery Level
int level;

// Create the realtime clock
RTC_PCF8523 rtc;

/**
 * toggleRecordingState() transitions the enum recordingState to the next state.
 */
void toggleRecordingState()
{
  switch(recordingState)
  {
    case RECORDING:
      recordingState = PAUSED_RECORDING;
      break;
    case PAUSED_RECORDING:
      recordingState = RECORDING;
      break;
    default:
      recordingState = PAUSED_RECORDING;
  }
}

/**
 * cycleRecordingSpeed() transitions the enum recordingSpeed to the next state.
 */
void cycleRecordingSpeed()
{
  switch(recordingSpeed)
  {
    case SLOW_RECORD:
      recordingSpeed = MEDIUM_RECORD;
      break;
    case MEDIUM_RECORD:
      recordingSpeed = FAST_RECORD;
      break;
    case FAST_RECORD:
      recordingSpeed = SLOW_RECORD;
      break;
    default:
      recordingSpeed = SLOW_RECORD;
  }
}

/** 
 * handleSettingStates determines the value of settingState
 * @param a indicates if button a is released
 * @param b indicates if button b is released
 * @param longA indicates  if button A was pressed for a long time
 * @param longB indicates if button B was pressed for a long time 
 */
void handleSettingStates(bool a, bool c, bool longA, bool longC)
{
  switch(settingState)
  {
    case MAIN_MENU:
      if(longA)
      {
        settingState = EXIT_SETTINGS;
      }
      else
      {
        settingState = MAIN_MENU;
      }
      break;
    case EXIT_SETTINGS:
      if(a)
      {
        settingState = SET_TIMEZONE;
      }
      else if(c)
      {
        settingState = MAIN_MENU;
      }
      else
      {
        settingState = EXIT_SETTINGS;
      }
      break;
    // When the user selects this option they will be allowed to set the time zone
    case SET_TIMEZONE:
      if(a)
      {
        settingState = EXIT_SETTINGS;
      }
      else if(c)
      {
        settingState = DISPLAY_TIMEZONE;
      }
      else
      {
        settingState = SET_TIMEZONE;
      }
      break;
    // DISPLAY_TIMEZONE lets the user select which timezone to use
    case DISPLAY_TIMEZONE:
      if(a)
      {
        settingState = SET_TIMEZONE;
      }
      else
      {
        settingState = DISPLAY_TIMEZONE;
      }
      break;
    // Unknown state, default back to the main menu
    default:
      settingState = MAIN_MENU;
  }
}

/**
 * zeroOutFixRecordArray() is used by checkFix() to zero out the fixRecordArray.
 */
void zeroOutFixRecordArray()
{
  for(fixRecordIndex = 0; fixRecordIndex < sizeof(fixRecordArray)/sizeof(byte); fixRecordIndex++)
  {
    fixRecordArray[fixRecordIndex] = 0;
  }
  fixRecordIndex = 0;
  fullSampleCycle = false;
}

/**
 * updateFixRecordIndex() is used by checkFix() as a function that updates the 
 * fixRecordVar counter.
 */
byte updateFixRecordIndex(byte fixRecordVar)
{
  if(fixRecordVar + 1 >= sizeof(fixRecordArray)/sizeof(byte))
  {
    fullSampleCycle = true;
    return 0;
  }
  else 
  {
    return fixRecordVar + 1;
  }
}

/**
 * checkFix() indicates if the GPS has a fix and updates a global variable 
 * hasFix to indicate to the rest of the program whether the GPS has a fix. 
 */
void checkFix()
{
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

/**
 * battery_level() updates the OLED screens battery level indicator 
 */
void battery_level() 
{
  // read the battery level from the ESP8266 analog in pin.
  // analog read level is 10 bit 0-1023 (0V-1V).
  // our 100k & 22K voltage divider takes the max
  // lipo value of 4.2V and drops it to 0.758V max.
  // this means our min analog read value should be 580 (3.14V)
  // and the max analog read value should be 774 (4.2V).
  level = analogRead(A0);
  float vbat;
  // convert battery level to percent
  vbat = map(level, 580, 774, 3.4f, 4.26f);
  level = map(level, 580, 774, 0, 100); 
}

/**
 * setup() is called at the beginning of the program, it is a reserved function.
 */
void setup() 
{
  recordingState = PAUSED_RECORDING;
  settingState = MAIN_MENU;
  recordingSpeed = SLOW_RECORD;
  buttonALastSample = 1;
  buttonCLastSample = 1;
  buttonAPressTime = millis();
  buttonCPressTime = millis();
  
  gpsSerial.begin(GPS_BAUD);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  pinMode(fixPin, INPUT_PULLDOWN_16);
  zeroOutFixRecordArray();
  hasFix = false;
  fixSampleTime = millis();
  gpsUnavailableTime = millis();
  isGpsAvailable = false;
  updatedDate = false;

  lcdLastUpdate = millis();
  
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
  n = 0;

  uint16_t year = 0;
  uint8_t month = 0;
  uint8_t day = 0;

  // Set up the input buttons, we don't use B because it conflicts with the fix line for GPS
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  // Set up Real Time Clock
  if (! rtc.begin()) {
    SerialMonitor.println("Couldn't find RTC");
  }

  if (rtc.initialized()) {
    DateTime now = rtc.now();
    year = now.year();
    month = now.month();
    day = now.day();
  }
  else
  {
    year = 0;
    month = 0;
    day = 0;
  }
  lcd.println(" ");
  lcd.print("Setting up SD card.");
  lcd.display();
  SerialMonitor.println("Setting up SD card.");
  delay(200);
  lcd.clearDisplay();
  hasSdCard = SD.begin();
  if (!hasSdCard) 
  {
    SerialMonitor.println("Error initializing SD card.");
  }

  // Get the battery level
  battery_level(); 
  
  updateFileName(year, month, day);

  // Print the file header 
  if(hasSdCard)
  {
    printHeader();
  }
}

/** loop gets called in an infinite loop
 * loop() is a reserved function that is called in an infinite loop and is 
 * called after setup(). loop() is the center of the program and corridinates 
 * all the interaction between hardware
 */
void loop() 
{
//  while (gpsSerial.available() > 0)
//    tinyGPS.encode(gpsSerial.read());

  bool aRelease = false;
  bool cRelease = false;
  bool aLongPress = false;

  // Call checkFix() to see if the gps lock is only giving a pulse every 15 seconds, 
  // this tells us that we have GPS lock with the global variable hasFix.
  checkFix();

  // This if statement checks to see when 15 seconds have elapsed to update the 
  // battery level indicator.
  if(millis() - batteryCheck > batteryCheckPeriod) 
  {
    battery_level();
    batteryCheck = millis();
  }

  // Scan Buttons
  // This if statement detects the button release of button A on the OLED board
  if (digitalRead(BUTTON_A) != buttonALastSample && digitalRead(BUTTON_A) != 0)
  {
    // A quick release of button A means to start/stop recording or cycle through 
    // settings options
    if(millis() <= buttonAPressTime + longPressTime) 
    {
      // Toggle recording on/off
      if(settingState == MAIN_MENU)
      {
        toggleRecordingState();
      }
      aRelease = true;
    }
  }

  // If the A button on the OLED board has been pressed longer than 
  // longPressTime then we have a long press condition, so we should go into 
  // the settings menu.
  if(millis() > buttonAPressTime + longPressTime && buttonALastSample == 0 && digitalRead(BUTTON_A) == 0)
  {
    buttonAPressTime = millis();
    aLongPress = true;
  }

  // If button C was released cycle the recording speed if we're not in settings
  // and if we are in settings use it to select the option and if we're 
  if (digitalRead(BUTTON_C) != buttonCLastSample && digitalRead(BUTTON_C) != 0)
  {
    if(settingState == MAIN_MENU)
    {
      cycleRecordingSpeed();
    }
    cRelease = true;
  }

  // Detect Button A transition to detect button state and time of change
  if(digitalRead(BUTTON_A) != buttonALastSample)
  {
    buttonALastSample = digitalRead(BUTTON_A);
    buttonAPressTime = millis();
  }

  // Detect Button C transition to detect button state and time of change 
  if(digitalRead(BUTTON_C) != buttonCLastSample)
  {
    buttonCLastSample = digitalRead(BUTTON_C);
    buttonCPressTime = millis();
  }

  // Change the setting state based on the user input from the buttons
  handleSettingStates(aRelease, cRelease, aLongPress, false);

  // Log GPS data if enough time has elapsed since the last sample and 
  // we have a GPS fix and updated GPS information
  if (recordingState == RECORDING && 
    (lastLog + recordingSpeedRecord[recordingSpeed]) <= millis()) 
  {
    if (tinyGPS.location.isUpdated() && hasFix) 
    {
      if (logGPSData()) 
      {
        uint8_t networkCount = countNetworks();

        SerialMonitor.print("GPS logged ");
        SerialMonitor.print(tinyGPS.location.lat(), 6);
        SerialMonitor.print(", ");
        SerialMonitor.println(tinyGPS.location.lng(), 6);
        SerialMonitor.print("Seen networks: ");
        SerialMonitor.println(networkCount);

        lastLog = millis();
      } 
      else 
      {
        lcd.setCursor(0, 1);
        SerialMonitor.println("Failed to log new GPS data.");
      }
    } 
  }

  // Get the GPS information
  if(gpsSerial.available() == 0 && isGpsAvailable)
  {
    gpsUnavailableTime = millis();
    isGpsAvailable = false;
  }
  if(gpsSerial.available() > 0)
  {
    isGpsAvailable = true;
  }
  while (gpsSerial.available() > 0)
  {
    tinyGPS.encode(gpsSerial.read());
  }

  // Update the screen
  if(millis() - lcdLastUpdate >= lcdRefreshRate)
  {
    lcdLastUpdate = millis();

    lcd.clearDisplay();

    lcd.setBattery(level);
    lcd.renderBattery();
    
    lcd.setCursor(0, 0);

    lcd.println(" ");

    // Print the first line
    if (!hasSdCard)
    {
      lcd.println("Couldn't find SD Card");
    }
    else if(hasFix == false)
    {
      lcd.println("Acquiring GPS fix"); 
    }
    else if (!tinyGPS.location.isUpdated())
    {
      lcd.println("Updating GPS data"); 
    }
    else if(recordingState == PAUSED_RECORDING)
    {
      lcd.println("Recording Paused"); 
    }
    else
    {
      lcd.print("Scan, period ");
      lcd.print(recordingSpeedRecord[recordingSpeed]);
      lcd.println(" ms");
    }
    
    // Print the second line
    if (!hasSdCard)
    {
      lcd.println("Check SD card");
    }
    else
    {
      lcd.print("Wifi Observed: ");
      lcd.println(n);
    }

    // Print the third line
    if (!hasSdCard)
    {
      lcd.println("Then press Reset");
    }
    else
    {
      lcd.print("pos: ");
      lcd.print(tinyGPS.location.lat(), 2);
      lcd.print(", ");
      lcd.print(tinyGPS.location.lng(), 2);
    }
    
    lcd.display();

    n = 0;
  }

  // Update the time on the RTC if it hasn't been updated since we turned on
  if(isGpsAvailable && hasFix && updatedDate == false)
  {
    rtc.adjust(DateTime(
      tinyGPS.date.year(), 
      tinyGPS.date.month(), 
      tinyGPS.date.day(), 
      tinyGPS.time.hour(), 
      tinyGPS.time.minute(), 
      tinyGPS.time.second())
    );
    // Make sure that the date doesn't get updated during the run of the program
    updatedDate = true;
  }
}

/**
 * countNetworks() returns the number of networks found.
 */
uint8_t countNetworks() 
{
  File netFile = SD.open(logFileName);
  uint8_t networks = 0;
  if(netFile) 
  {
    while(netFile.available()) 
    {
      netFile.readStringUntil('\n');
      networks++;
    }
    netFile.close();
    if (networks == 0) 
    {
      return networks;
    } 
    else 
    {
      //Avoid header count
      return (networks-1); 
    }
  }
}

/**
 * logGPSData() is called by loop() to write update sampled gps and wifi data.
 */
bool logGPSData() 
{
  n = WiFi.scanNetworks(); 
  if (n == 0) 
  {
    SerialMonitor.println("no networks found");
  } 
  else 
  {
    for (uint8_t i = 1; i <= n; ++i) 
    {
      //Avoid erroneous channels
      if ((WiFi.channel(i) > 0) && (WiFi.channel(i) < 15)) 
      { 
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
  return true;
}

/**
 * printHeader() is used in setup() to print the first line of the .csv output
 * log file.
 */
void printHeader() 
{
  File logFile = SD.open(logFileName, FILE_WRITE);
  if (logFile) 
  {
    int i = 0;
    logFile.print(csvHeader);
    for (; i < sizeof(log_col_names)/sizeof(typeof(log_col_names)); i++) 
    {
      logFile.print(log_col_names[i]);
      if (i < sizeof(log_col_names)/sizeof(typeof(log_col_names)) - 1)
      {
        logFile.print(',');
      }
      else
      {
        logFile.println();
      }
    }
    logFile.close();
  }
}

/**
 * updateFileName() tries to find a unique name to name the log file. 
 * Used in setup().
 * @param year the current year
 * @param month the current month
 * @param day the current day
 */
void updateFileName(uint16_t year, uint8_t month, uint8_t day) 
{
  uint16_t i = 0;
  for (; i < MAX_LOG_FILES; i++) 
  {
    memset(logFileName, 0, strlen(logFileName));
    sprintf(logFileName, "%s-%04u-%02u-%02u-%04d.%s", LOG_FILE_PREFIX, year, month, day, i, LOG_FILE_SUFFIX);
    if (!SD.exists(logFileName)) 
    {
      break;
    } 
    else 
    {
      SerialMonitor.print(logFileName);
      SerialMonitor.println(" exists");
    }
  }
  SerialMonitor.print("File name: ");
  SerialMonitor.println(logFileName);
}

/**
 * Given the network type ID getEncryption returns a string for the network type
 * for printouts to the logfile.
 */
String getEncryption(uint8_t network) 
{
  byte encryption = WiFi.encryptionType(network);
  switch (encryption) 
  {
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

