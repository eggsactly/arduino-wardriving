// Feather wig doubler dimensions pulled from here: https://learn.adafruit.com/featherwing-proto-and-doubler/downloads

// All measurements are in mm

mmPerIn = 25.4;

// Feather Wing Dimensions
featherWingDoublerWidth = 2 * mmPerIn;
featherWingDoublerLength = 1.85 * mmPerIn; 
screwHoleCenterFromEdge = 0.1 * mmPerIn;
bodyMargin = 10;
heightMargin = 5;
solderHeight = 4;

// Screw Dimensions
m3ThreadDiameter = 3.00;
m3InnerThreadDiameter = 2;
m3HeadDiameter = 6.00; 
m3HeadHeight = 3.00;
threadLength = 11;

// Intermediate calculations
plateHeight = threadLength - solderHeight + heightMargin;

// Draw the elements
difference() 
{
    union()
    {
        // Draw the main body
        cube([featherWingDoublerWidth + 2 * bodyMargin, featherWingDoublerLength + 2 * bodyMargin, plateHeight], center = true);
        // Draw the stacks
        for (i = [-(featherWingDoublerWidth/2 - screwHoleCenterFromEdge), (featherWingDoublerWidth/2 - screwHoleCenterFromEdge)])
        {
            for(j = [-(featherWingDoublerLength/2 - screwHoleCenterFromEdge), (featherWingDoublerLength/2 - screwHoleCenterFromEdge)]){
                translate(v = [i, j, plateHeight/2 + solderHeight/2]) {
                    cylinder(h = solderHeight, r = screwHoleCenterFromEdge, center = true, $fn = 360);
                }
            }  
        }
    }
    
    
    // Draw the screw holes
    for (i = [-(featherWingDoublerWidth/2 - screwHoleCenterFromEdge), (featherWingDoublerWidth/2 - screwHoleCenterFromEdge)])
    {
        for(j = [-(featherWingDoublerLength/2 - screwHoleCenterFromEdge), (featherWingDoublerLength/2 - screwHoleCenterFromEdge)]){
            translate(v = [i, j, solderHeight/2]) {
                cylinder(h = plateHeight+solderHeight+1, d = m3InnerThreadDiameter, center = true, $fn = 360);
            }
        }  
    }  
}

