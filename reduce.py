#!/usr/bin/python

# Reduce removes all the duplicate entries in a raw csv file and outputs a csv
# file of the strongest detected signal of each access point. 

from argparse import ArgumentParser
import sys

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

APs = [[]]
header = ""
with open(args.inputFile, "r") as f:
    # Iterate through each line of the file and put it in if it's the strongest
    # Skip the first line because it's a header
    iterLines = iter(f)
    header = iterLines.next()
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
f = open(args.outputFile, 'w')

f.write(header.rstrip())

for ap in APs:
    for elm in ap:
        f.write(elm)
        f.write(',')
    f.write('\n')

f.close()

