#!/usr/bin/python

# Reduce removes all the duplicate entries in a raw csv file and outputs a kml
# file of the strongest detected signal of each access point. 

from argparse import ArgumentParser
import sys
from datetime import datetime

# Returns true if string is a valid UTF-8 string
def isValidUtf8(string):
    valid_utf8 = True
    try:
        string.decode('utf-8')
    except UnicodeDecodeError:
        valid_utf8 = False
    return valid_utf8

# Prints out the wifi if it SSID is a valid UTF-8 string
def printPlaceMark(f, ap, style):
    # Only print out if SSID is a valid UTF-8 string
    if isValidUtf8(ap[1]):
        epoch = datetime.utcfromtimestamp(0)
        f.write("\t<Placemark>\n")
        f.write("\t\t<name><![CDATA[")
        f.write(ap[1])
        f.write("]]></name>\n")
        f.write("<description><![CDATA[BSSID: <b>")
        f.write(ap[0])
        f.write("</b><br/>Capabilities: <b></b><br/>Frequency: <b></b><br/>Level: <b>")
        f.write(ap[5])
        f.write("</b><br/>Timestamp: <b>")
        sampleTime = datetime.strptime(ap[3], '%Y-%m-%d %H:%M:%S')
        f.write(str(int((sampleTime - epoch).total_seconds())))
        f.write("</b><br/>Date: <b>")
        f.write(sampleTime.strftime("%B %d, %Y %I:%M:%S %p"))
        f.write("</b>]]></description><styleUrl>")
        f.write(style)
        f.write("</styleUrl>\n")
        f.write("\t\t<Point>\n")
        f.write("\t\t<coordinates>")
        f.write(ap[7])
        f.write(",")
        f.write(ap[6])
        f.write(",")
        f.write(ap[8])
        f.write("</coordinates>\n")
        f.write("\t\t</Point>\n")
        f.write("\t</Placemark>\n")

parser = ArgumentParser()
parser.add_argument("-i", "--input", dest="inputFile",
                    help="Input file to process", metavar="FILE")
parser.add_argument("-o", "--output", dest="outputFile",
                    help="output file from reduce", metavar="FILE")

args = parser.parse_args()

if args.inputFile == None:
    print "Please provide an input file with -i flag."
    sys.exit()

# If no output file is provided, take the input and append .kml to it
outputfile = ""
if args.outputFile == None:
    outputfile = (args.inputFile).split('.')[0] + ".kml"
else:
    outputfile = args.outputfile

APs = []
header = ""
with open(args.inputFile, "r") as f:
    # Iterate through each line of the file and put it in if it's the strongest
    # Skip the first line because it's a header
    iterLines = iter(f)
    header = iterLines.next()
    header += iterLines.next()
    for line in iterLines:
        entry = []
        for x in line.split(','):
            entry.append(x.rstrip())

        # Search for entries MAC in AP
        foundMatch = False 
        i = 0;
        # Go through all the entries in AP
        for apInst in APs:
            # If the MAC address matches
            if len(apInst) > 0 and apInst[0] == entry[0]:
                # And if the SSID matches
                if apInst[1] == entry[1]:
                    foundMatch = True
                    # Compare their intensity
                    if int(entry[5]) > int(apInst[5]):
                        # For entries that have higher intensities replace the
                        # existing entry with it
                        APs[i] = entry
            i += 1
        # If we went through the whole loop and found no matches, append entry
        # to APs
        if foundMatch == False:
            APs.append(entry) 
        
f.closed

# Write APs to a file
f = open(outputfile, 'w')

f.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<kml xmlns=\"http://www.opengis.net/kml/2.2\"><Document><Style id=\"red\"><IconStyle><Icon><href>http://maps.google.com/mapfiles/ms/icons/red-dot.png</href></Icon></IconStyle></Style> <Style id=\"yellow\"><IconStyle><Icon><href>http://maps.google.com/mapfiles/ms/icons/yellow-dot.png</href></Icon></IconStyle></Style><Style id=\"green\"><IconStyle><Icon><href>http://maps.google.com/mapfiles/ms/icons/green-dot.png</href></Icon></IconStyle></Style>\n")

# Print out all the open wifis
f.write("<Folder><name>Open WiFis</name>\n")
for ap in APs:
    if len(ap) > 9 and ap[2] == "[ESS]":
        printPlaceMark(f, ap, "#green")
f.write("</Folder>\n")

# Print out WEP wifi
f.write("<Folder><name>WEP WiFis</name>\n")
for ap in APs:
    if len(ap) > 9 and ap[2] == "[WEP][ESS]":
        printPlaceMark(f, ap, "#yellow")
f.write("</Folder>\n")

# Print out Closed wifi
f.write("<Folder><name>Closed WiFis</name>\n")
for ap in APs:
    if len(ap) > 9 and ap[2] != "[ESS]" and ap[2] != "[WEP][ESS]":
        printPlaceMark(f, ap, "#red")
f.write("</Folder>\n")
f.write("</Document></kml>\n")

f.close()

