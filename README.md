# ESP8266 based Arduino ~~wardriving~~ warcycling

![Wardriving Device on Bike](https://i.imgur.com/iSobdp6.jpg)

## What is this?
A simple/~~cheap~~ hardware + software setup to do warcycling trips and store as much information as we can on a SD card, using a small Arduino box. This project was [forked from reynico's arduino-wardriving project](https://github.com/reynico/arduino-wardriving) and adapted to work with the Adafruit ESP8266 HUZZAH and FeatherWings. 

Use this hardware to bike around your neighborhood to find where Wifi Hotspots are located, find the encryption of each hotspot, plot the data on popular map apps and upload the data to [wigle.net](https://wigle.net).

![Plot of Wifi hotspots in Tempe](https://i.imgur.com/CTdY4qT.png)

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
* [$5.50](https://www.amazon.com/Position-Panel-Switch-Solder-SS12F15G3/dp/B008DFYT0Q) SPDT 1P2T Panel Mini Slide Switch (optional) for on-off switch
* [$7.99](https://www.amazon.com/gp/product/B073X3BJ4D/) JST 2.0 PH cables and connector (optional) for on-off switch 
* [$9.38](https://www.amazon.com/uxcell-M3x20mm-Socket-Knurled-Screws/dp/B015A30R1I) two m3 20mm socket cap screws and nuts (optional) for attaching mount to a bike.
* Access to a 3D Printer for the mount

## What does this do?
* Checks for a working SD Card
* Waits for GPS signal
* Collects all WiFi signals on 2.4Ghz on every (configurable) GPS sample
* Stores this information as a CSV file

## What does this not do?
* This doesn't capture pcap files due power limitations
* This doesn't attack any network

## What is different in this fork 
* Used parts from one vendor: Adafruit.
* Used a boilerplate breadboard, eliminating need for custom circuit board.
* Took out previously-recorded check on Arduino logging code because checking to see if an AP was already recorded would become inefficient over time. 
* Put the output CSV into Wigle CSV format.
* Added FIX input from GPS to verify GPS location is up to date. 

## Electric setup
The diagrams below show how to configure all the Feather Wings. Please read all instructions carefully. 
 
### Feather Wing Doubler
On the Featherwing Doubler pins a 22K resistor should be attached between the ADC pin and ground. A 100k resistor should be attached between the VBAT pin and the ADC pin. The RX pin should be mapped to the second NC from RX. Pin 4 should be mapped from the third NC pin from RX. 

![Featherwing Doubler](https://imgur.com/6ZSRHUw.png)

### Adalogger Feather Wing
The Adalogger Feather Wing requires no special configuration, solder on Feather Stacking Headers to all pin locations.

![Adalogger](https://imgur.com/OMgErBR.png)

### OLED Display Feather Wing
All pins on the OLED Feather Wing should have the 2mm Pitch Break-apart Male Headers soldered on except for pin B because this pin is used to sample the FIX pin from the GPS. 

![OLED Screen](https://imgur.com/6AdT9cG.png)

### GPS Feather Wing
The default TX and RX connections should be cut with a knife on the bottom of the GPS breakout Feather Wing. The TX pin should be mapped to the second NC from TX. The Fix Pin should be mapped from the third NC pin from TX. All pins on the GPS logger should have Feather Stacking Headers soldered onto them.

![GPS breakout](https://imgur.com/pvPheGV.png)

The ESP8266 and GPS module should be stacked on the top portion and the Data Logger and LCD screen should be stacked on the bottom portion. 

Caution should be exercised when programming the ESP8266. Because the programming serial interface and the GPS module share the same pins, the GPS module shall be removed before programming takes place.

## Mount
The mount source files are stored in the mount/ directory as .scad files. They can be converted to .stl files to be 3D printed by running make in the mount directory. The compiled stl files [can be downloaded off thingiverse](https://www.thingiverse.com/thing:2967852). If you want to build the stl files from scratch OpenSCAD is required for the build process. The directory contains four design files. The mounts were designed to have cantilevers so they can snap together without screws. The exception being the connection between the handle bar mounts. 

### Central Mount
Mount for the feather Wing circuit card and has attachments for the battery adaptor and handle bar mount.

![Central Mount](https://i.imgur.com/L5JnCZK.png)

### Battery Mount
Mount to attach the battery to the Central Mount.

![Battery Mount](https://i.imgur.com/ReC9O2d.png)

### Handle Bar Mount Up
Mount to attach the Central Mount to the Handle Bar Mount Low.

![Handle Bar Mount Up](https://i.imgur.com/DJoreRa.png)

### Handle Bar Mount Low
Mount to attach to the Handle Bar Mount Up to clamp around bicylce handle bars. 

![Handle Bar Mount Low](https://i.imgur.com/o8YOm9J.png)

## Python Scripts
The content written to the .CSV on the SD card is large and contains redundant information. This project contains two python scripts for compressing and converting the data. 
* *reduce.py* takes an input .CSV file and picks out the strongest signal of each non-unique entry and puts it in an output CSV file. Example usage `python reduce.py -i input.csv -o output.csv`.
* *kml-convert.py* takes an input .CSV file, picks out the strongest signal of each non-unique entry and writes it to an output .kml file of the same name, which can be opened with Google Earth or Gnome Maps. Example usage `python kml-convery.py -i input.csv` outputs input.kml.

