# ESP8266 based Arduino wardriving


## What is this?
A simple/~~cheap~~ hardware + software setup to do wardriving trips and store as much information as we can on a SD card, using a small Arduino box. This project was [forked from reynico's arduino-wardriving project](https://github.com/reynico/arduino-wardriving) and adapted to work with the Adafruit ESP8266 HUZZAH and FeatherWings. This hasn't been tested yet and much of the content will be subject to change till the end of May 2018. The parts are on order. 
Part of the datalogging code stolen from [Sparkfun](https://learn.sparkfun.com/tutorials/gps-logger-shield-hookup-guide/example-sketch-gps-serial-passthrough) guys

## What do we need?
* [$19.95](https://www.adafruit.com/product/3213) HUZZAH ESP8266
* [$8.95](https://www.adafruit.com/product/2922) Adalogger FeatherWing - RTC + SD Add-on For All Feather Boards
* [$39.95](https://www.adafruit.com/product/3133) Adafruit Ultimate GPS FeatherWing
* [$14.95](https://www.adafruit.com/product/2900) FeatherWing OLED
* [$7.50](https://www.adafruit.com/product/2890) FeatherWing Doubler for hooking the circuit together
* [$0.95](https://www.adafruit.com/product/2886) Feather Female Header Set
* [$1.25](https://www.adafruit.com/product/2830) Feather Stacking Header Set
* [$0.95](https://www.adafruit.com/product/380) CR1220 Coin Battery for Holding GPS Ephermeris 
* [$9.95](https://www.adafruit.com/product/1781) Cylindrical Lithium Ion Battery
* [$2.95](https://www.adafruit.com/product/2671) 2mm Pitch Break-apart Male Headers
* [$0.75](https://www.adafruit.com/product/2785) 22k Ohm Resistor
* [$0.75](https://www.adafruit.com/product/2787) 100k Ohm Resistor
* [$5.50](https://www.amazon.com/Position-Panel-Switch-Solder-SS12F15G3/dp/B008DFYT0Q) SPDT 1P2T Panel Mini Slide Switch (optional)
* [$7.99](https://www.amazon.com/gp/product/B073X3BJ4D/ref=oh_aui_detailpage_o05_s01?ie=UTF8&psc=1) JST 2.0 PH cables and connector (optional)
* Access to a 3D Printer for the mount

## What does this do?
* Checks for a working SD Card
* Waits for GPS signal
* Collects all WiFi signals on 2.4Ghz on every (configurable) GPS sample
* Stores this information as a CSV file

## What does this not do?
* This doesn't capture pcap files due power limitations
* This doesn't attack any network

## What does this need?
* Fix some race conditions
* ~~A PCB design that I'm working on~~
* A better antenna for GPS and WiFi, that will help to speed up satellite lock-up and enlarge WiFi coverage

## What is different in this fork 
* Used parts from one vendor, Adafruit, for the A E S T H E T I C. This eliminates need to make a custom circuit board as well.
* Took out previously recorded check, I felt checking to see if an AP was already recorded would become inefficient over time, so it was removed. 
* Put the output CSV into Wigle CSV format.
* Added way to verify we have a fix on GPS location. 

## What do you need to configure?
Nothing, but you can:
* Setup CS pin for SD card module on *ARDUINO_USD_CS* variable
* Change log file prefix and suffix on *LOG_FILE_PREFIX* and *LOG_FILE_SUFFIX*
* Remove or add columns to CSV datalog on *log_col_names*
* Define GPS log rate time on *LOG_RATE* (milliseconds)
* Define GPS TX/RX pins on *ARDUINO_GPS_RX* and *ARDUINO_GPS_TX*

## Electric setup
The diagrams below show how to configure all the Feather Wings. Please read all instructions carefully. 
 
### Feather Wing Doubler
On the Featherwing Doubler pins a 22K resistor should be attached between the ADC pin and ground. A 100k resistor should be attached between the VBAT pin and the ADC pin. The RX pin should be mapped to the second NC from RX. Pin 4 should be mapper from the third NC pin from RX. 

![Featherwing Doubler](https://imgur.com/6ZSRHUw.png)

### Adalogger Feather Wing
The Adalogger Feather Wing requires no special configuration, solder on header pins to all pin locations.

![Adalogger](https://imgur.com/OMgErBR.png)

### OLED Display Feather Wing
All pins on the OLED Feather Wing should have 2mm headers soldered on except for pin B.

![OLED Screen](https://imgur.com/6AdT9cG.png)

### GPS Feather Wing
The default TX and RX connections should be cut with a knife on the bottom of the GPS breakout feather wing. The TX pin should be mapped to the second NC from TX. The Fix Pin should be mapped from the third NC pin from TX. All pins on the GPS logger should have header pins soldered onto them.

![GPS breakout](https://imgur.com/pvPheGV.png)

The ESP8266 and GPS module should be stacked on the top portion and the Data Logger and LCD screen should be stacked on the bottom portion. 

