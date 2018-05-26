#include <SPI.h>
#include <SD.h>
#include <TinyGPS++.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_FeatherOLED.h>

// Added this pound define to get around compiler error
// https://github.com/neu-rah/ArduinoMenu/issues/68 
#define typeof(x) __typeof__(x)

Adafruit_FeatherOLED lcd = Adafruit_FeatherOLED();

// Define the pin locations of the OLED buttons
#define BUTTON_A 0
#define BUTTON_C 2

// Pin D8
#define ARDUINO_USD_CS 15 

// These three defines indicate the name of the log file 
#define LOG_FILE_PREFIX "gpslog"
#define MAX_LOG_FILES 10000
#define LOG_FILE_SUFFIX "csv" 

// logFileName contains the name of the log file 
char logFileName[15];

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

// RecordingSpeedStates indicates the heuristic for recording speed 
typedef enum  
{
  SLOW_RECORD = 0,
  MEDIUM_RECORD = 1,
  FAST_RECORD = 2,
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

// SettingStates enum indicates whether the OLED display should show the user is
// in a menu option 
typedef enum 
{
  NOT_IN_SETTINGS = 0,
  IN_SETTINGS_MENU = 1,

  // SETTING_STATES_COUNT should always be last
  SETTING_STATES_COUNT 
} SettingStates;

// SettingMenuOptions enum indicates what the menu option the user is in
typedef enum 
{
  EXIT_SETTINGS = 0,
  SET_TIMEZONE = 1,
  SET_TIMEZONE_ACTIVE = 2,

  // SETTING_MENU_OPTIONS_COUNT should always be last
  SETTING_MENU_OPTIONS_COUNT 
} SettingMenuOptions;

// RecordingSpeedStruct holds associations between the speed settings and 
// the time periods between recording of the speed 
typedef struct 
{
  // speedSetting is an enum representing the heuristic speed setting
  RecordingSpeedStates speedSetting;
  // timePeriod indicates the number of milliseconds for what the heuristic 
  // actually means 
  unsigned int timePeriod;
} RecordingSpeedStruct;

// recordingState keeps track of whether the arduino is recording or not
RecordingStates      recordingState;

// settingState keeps track of whether the settings menu should be displayed or
// not.
SettingStates        settingState;

// settingMenuState indicates what setting menu the user is in if they are in
// the setting menu indicated by settingState.
SettingMenuOptions   settingMenuState;

// recordingSpeed indicates the current index of the recordingSpeedRecord array
RecordingSpeedStates recordingSpeed;

// This array of struct contains a record of the associations between speed 
// setting and time period between records
const RecordingSpeedStruct recordingSpeedRecord[] = {
  {SLOW_RECORD,   5000}, 
  {MEDIUM_RECORD, 3000}, 
  {FAST_RECORD,   2000}
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

// lastLog indicates the time in milliseconds of the last file log
uint64_t lastLog = 0;

unsigned long long batteryCheck;
// Update battery level every 15 seconds
const unsigned long long batteryCheckPeriod = 15000;

TinyGPSPlus tinyGPS;
#define GPS_BAUD 9600 // GPS module's default baud rate

// Display indicates whether the lat/long or Wifi info should be printed on the
// OLED screen.
byte display = 1;

#define SerialMonitor Serial
#define gpsSerial Serial

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
 * cycleThroughSettings() transitions the enum settingMenuState to the next 
 * state.
 */
void cycleThroughSettings()
{
  switch(settingMenuState)
  {
    case EXIT_SETTINGS:
      settingMenuState = SET_TIMEZONE;
      break;
    case SET_TIMEZONE:
      settingMenuState = EXIT_SETTINGS;
      break;
    default:
      settingMenuState = EXIT_SETTINGS;
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
 * checkFix() is indicates if the GPS has a fix and updates a global variable 
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

/**
 * setup() is called at the beginning of the program, it is a reserved function.
 */
void setup() 
{
  recordingState = PAUSED_RECORDING;
  settingState = NOT_IN_SETTINGS;
  settingMenuState = EXIT_SETTINGS;
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
  if (!SD.begin()) 
  {
    lcd.setCursor(0, 0);
    lcd.print("Error initializing SD card.");
    lcd.display();
    SerialMonitor.println("Error initializing SD card.");
  }
  updateFileName();
  printHeader();
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

  // Call checkFix() to see if the 1pps is only giving a pulse ever second, 
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
    // A quick press of button A means to start/stop recording or cycle through 
    // settings options
    if(millis() <= buttonAPressTime + longPressTime) 
    {
      // Toggle recording on/off
      if(settingState == NOT_IN_SETTINGS)
      {
        toggleRecordingState();
      }
      // Cycle to next setting option
      else
      {
        cycleThroughSettings();
      }
    }
  }

  // If the A button on the OLED board has been pressed longer than 
  // longPressTime then we have a long press condition, so we should go into 
  // the settings menu.
  if(millis() > buttonAPressTime + longPressTime && buttonALastSample == 0 && digitalRead(BUTTON_A) == 0)
  {
    settingState = IN_SETTINGS_MENU;
    settingMenuState = EXIT_SETTINGS;
    buttonAPressTime = millis();
  }

  // Detect if button C on the OLED board was released.
  if (digitalRead(BUTTON_A) != buttonALastSample && digitalRead(BUTTON_A) != 0)
  {
    cycleRecordingSpeed();
    cycleThroughSettings();
    // Reset recording time
    lastLog = millis();
  }

  // Detect Button A transition to detect button state and time of change
  if(digitalRead(BUTTON_A) != buttonALastSample)
  {
    buttonALastSample = digitalRead(BUTTON_A);
    buttonAPressTime = millis();
  }

  // Detect Button C transition to detext button state and time of change 
  if(digitalRead(BUTTON_C) != buttonCLastSample)
  {
    buttonCLastSample = digitalRead(BUTTON_C);
    buttonCPressTime = millis();
  }

  // Log GPS data if enough time has elapsed since the last sample and 
  // we have a GPS fix and updated GPS information
  if (recordingState == PAUSED_RECORDING && 
    (lastLog + recordingSpeedRecord[recordingSpeed].timePeriod) <= millis()) 
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

        // Alternate displaying current lat, long and number of networks seen
        if (display == 1) 
        {
          lcd.clearDisplay();
          lcd.setCursor(0, 0);
          lcd.print("Lat: ");
          lcd.print(tinyGPS.location.lat(), 6);
          lcd.setCursor(0, 1);
          lcd.print("Lon: ");
          lcd.print(tinyGPS.location.lng(), 6);
          lcd.display();
          display = 0;
        } 
        else 
        {
          lcd.clearDisplay();
          lcd.setCursor(0, 0);
          lcd.print("Seen: ");
          lcd.print(networkCount);
          lcd.setCursor(0, 1);
          lcd.print("networks");
          lcd.display();
          display = 1;
        }
        lastLog = millis();
      } 
      else 
      {
        lcd.setCursor(0, 1);
        SerialMonitor.println("Failed to log new GPS data.");
      }
    } 
    else 
    {
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
byte logGPSData() 
{
  int n = WiFi.scanNetworks(); 
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
 */
void updateFileName() 
{
  uint16_t i = 0;
  for (; i < MAX_LOG_FILES; i++) 
  {
    memset(logFileName, 0, strlen(logFileName));
    sprintf(logFileName, "%s%04d.%s", LOG_FILE_PREFIX, i, LOG_FILE_SUFFIX);
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

