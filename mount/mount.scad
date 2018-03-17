include <parameters.scad>;

// Draw the elements
difference() 
{
    union()
    {
        // Draw the main body
        translate(v = [-plateWidth/2, -plateLength/2, 0])
        {
            minkowski()
            {
                cube([plateWidth, plateLength, plateHeight/2], center = false);
                cylinder(h = plateHeight/2, r = minBodyMargin, center = false, $fn = 360);
            }
        }
        
        // Draw the risers
        for (i = [-(featherWingDoublerWidth/2 - screwHoleCenterFromEdge), (featherWingDoublerWidth/2 - screwHoleCenterFromEdge)])
        {
            for(j = [-(featherWingDoublerLength/2 - screwHoleCenterFromEdge), (featherWingDoublerLength/2 - screwHoleCenterFromEdge)]){
                translate(v = [i, j - batteryDiameter/2 - batteryMargin, plateHeight]) {
                    cylinder(h = solderHeight, r1 = riserBase, r2 = screwHoleCenterFromEdge, center = false, $fn = 360);
                }
            }  
        }
    }
    
    
    // Draw the screw holes
    for (i = [-(featherWingDoublerWidth/2 - screwHoleCenterFromEdge), (featherWingDoublerWidth/2 - screwHoleCenterFromEdge)])
    {
        for(j = [-(featherWingDoublerLength/2 - screwHoleCenterFromEdge), (featherWingDoublerLength/2 - screwHoleCenterFromEdge)]){
            translate(v = [i, j - batteryDiameter/2 - batteryMargin, -0.5]) {
                cylinder(h = plateHeight+solderHeight+1, d = m3InnerThreadDiameter, center = false, $fn = 360);
            }
        }  
    } 
   
    // Draw the attachment adaptor hole
    translate(v = [-(attachAdaptorWidthActual - attachAdaptorBezzelRad)/2, -(attachAdaptorWidthActual - attachAdaptorBezzelRad)/2, -0.5]) 
    {
        minkowski()
        {
            cube([attachAdaptorWidthActual - attachAdaptorBezzelRad, attachAdaptorWidthActual - attachAdaptorBezzelRad, plateHeight/2 + 1], center = false);
            cylinder(h = plateHeight/2 + 1, r = attachAdaptorBezzelRad, center = false, $fn = 360);
        } 
    }
    
    // Draw the attachment adaptor screw holes
    for (i = [-attachAdaptorScrewOffset, attachAdaptorScrewOffset])
    {
        for(j = [-attachAdaptorScrewOffset, attachAdaptorScrewOffset]){
            translate(v = [i, j, -0.5]) {
                cylinder(h = plateHeight+solderHeight+1, d = m3ThreadDiameter, center = false, $fn = 360);
            }
            translate(v = [i, j, plateHeight - m3HeadHeight]) {
                cylinder(h = plateHeight+solderHeight+1, d = m3HeadDiameter, center = false, $fn = 360);
            }
        }  
    } 
}

