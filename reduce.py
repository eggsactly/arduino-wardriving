#!/usr/bin/python

# Reduce removes all the duplicate entries in a raw csv file and outputs a csv
# file of the strongest detected signal of each access point. 

from argparse import ArgumentParser
import sys

# Returns true if string is a valid UTF-8 string
def isValidUtf8(string):
    valid_utf8 = True
    try:
        string.decode('utf-8')
    except UnicodeDecodeError:
        valid_utf8 = False
    return valid_utf8

parser = ArgumentParser()
parser.add_argument("-i", "--input", dest="inputFile",
                    help="Input file to process", metavar="FILE")
parser.add_argument("-o", "--output", dest="outputFile",
                    help="output file from reduce", metavar="FILE")

args = parser.parse_args()

if args.inputFile == None:
    print "Please provide an input file with -i flag."
    sys.exit()

if args.outputFile == None:
    print "Please provide an output file with -o flag."
    sys.exit()

APs = []
header = ""
with open(args.inputFile, "r") as f:
    # Iterate through each line of the file and put it in if it's the strongest
    # Skip the first two lines because they're headers
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
                    if len(entry) > 5 and len(apInst) > 5 and int(entry[5]) > int(apInst[5]):
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
f = open(args.outputFile, 'w')

f.write(header.rstrip())
f.write('\n')

for ap in APs:
    if len(ap) > 1 and isValidUtf8(ap[0]) and isValidUtf8(ap[1]):
        outputStr = ""
        for elm in ap:
            outputStr += elm
            outputStr += ','

        outputStr = outputStr.rstrip()
        outputStr = outputStr.rstrip(',')
        f.write(outputStr)
        f.write('\n')

f.close()

