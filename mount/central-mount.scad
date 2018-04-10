include <libparameters.scad>;

// Draw the elements
difference() 
{
    union()
    {
        // Draw the main body
        translate(v = [-plateWidth/2, -featherWingDoublerWidth/2 -minBodyMargin - screwHoleCenterFromEdge, 0])
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
    translate(v = [-(attachAdaptorWidthActual - 2 * attachAdaptorBezzelRad)/2, -(attachAdaptorWidthActual - 2 * attachAdaptorBezzelRad)/2, -0.5]) 
    {
        minkowski()
        {
            cube([attachAdaptorWidthActual - 2 * attachAdaptorBezzelRad, attachAdaptorWidthActual - 2 * attachAdaptorBezzelRad, plateHeight/2 + 1], center = false);
            cylinder(h = plateHeight/2 + 1, r = attachAdaptorBezzelRad, center = false, $fn = 360);
        } 
    }
    
    // Draw the battery attachment holes
    for(i = [batteryPlateOffsetY + m3HeadDiameter/2 + screwMargin, batteryPlateOffsetY + batterySheathLength - m3HeadDiameter/2 - screwMargin])
        for(j = [batteryPlankWidth/2 - m3HeadDiameter/2 - screwMargin, - batteryPlankWidth/2 + m3HeadDiameter/2 + screwMargin])
        {
            translate(v = [j, i, -0.5]) cylinder(h = plateHeight+1, d = m3InnerThreadDiameter, center = false, $fn = 360);
        }
}
