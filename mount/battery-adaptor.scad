include <parameters.scad>;



// Draw the cylinder
difference()
{
    cylinder(h = batterySheathLength, r = batteryDiameter/2 + batterySheathRadius, center = false, $fn = 360);
    translate(v=[0, 0, -0.5])
    {
        cylinder(h = batterySheathLength + 1, r = batteryDiameter/2, center = false, $fn = 360);
    }
    translate(v=[-(batteryDiameter/2 + batterySheathRadius), -1, -0.5])
    {
        cube([batteryDiameter/2 + batterySheathRadius, 2, batterySheathLength + 1], center = false);
    }
    
    
    
}

// Create the attachment block
difference()
{
    // Draw the actual block
    translate(v=[batteryDiameter/2, -batteryPlankWidth/2, 0])
    {
        cube([batteryPlankThickness, batteryPlankWidth, batterySheathLength], center = false);
    }

    // Draw the cap holes
    for(i = [m3HeadDiameter/2 + screwMargin, batterySheathLength - m3HeadDiameter/2 - screwMargin])
        for(j = [batteryPlankWidth/2 - m3HeadDiameter/2 - screwMargin, - batteryPlankWidth/2 + m3HeadDiameter/2 + screwMargin])
        {
            translate(v = [batteryDiameter/2-1, j, i]) rotate([0, 90, 0]) cylinder(h = m3HeadHeight+1, d = m3HeadDiameter, center = false, $fn = 360);
            translate(v = [batteryDiameter/2+0.5, j, i]) rotate([0, 90, 0]) cylinder(h = batteryPlankThickness+1, d = m3ThreadDiameter, center = false, $fn = 360);
        }
        
    
}