include <libparameters.scad>;
use <libcantilever.scad>;

union(){
// Draw the central cantilever mount
// For some reason this does not draw inside the union
for(i=[0 : 90 : 270])rotate([0, 0, i])translate([-(bikeMountHeight + 2 * fitMargin)/2, 0, plateHeight - handle_cant_slope_height - handle_cant_errosion_margin - fitMargin])translate([0, (bikeMountHeight + 2 * fitMargin)/2, 0])rotate([0, 90, -90])cantilever(0, central_cant_errosion_margin, central_cant_slope_height, handle_cant_overhang, 0, 0, bikeMountHeight + 2 * fitMargin);

// Draw the elements
difference() 
{
    union()
    {   
        // Draw the main body
        translate(v = [-plateWidth/2, -featherWingDoublerWidth/2 - screwHoleCenterFromEdge, 0])
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
    translate(v = [-(bikeMountHeight + 2 * fitMargin)/2, -(bikeMountHeight + 2 * fitMargin)/2, -0.5]) 
    {
        cube([bikeMountHeight + 2 * fitMargin, bikeMountHeight + 2 * fitMargin, plateHeight + 1], center = false);
    }
    
    // Draw the hole foe the battery mount
    difference()
    {
        translate([-(batterySheathLength + fitMargin)/2, batteryPlateOffsetY, -0.5])cube([batterySheathLength + fitMargin, batteryPlankWidth + fitMargin, plateHeight + 1]);
        
        // Draw the battery connect block
        translate([-batterySheathLength/2 - fitMargin, batteryPlateOffsetY + (batteryPlankWidth - batteryBlockLength)/2 + fitMargin, 0])cube([batterySheathLength + 2*fitMargin, batteryBlockLength - 2*fitMargin, handle_cant_errosion_margin + handle_cant_slope_height + fitMargin]);
    }
        
    // Draw the little holes to put you fingers in
    for(i = [(bikeMountHeight + 2 * fitMargin)/2, -(bikeMountHeight + 2 * fitMargin)/2])translate([i, 0, plateHeight])sphere(r=fingerHoleRadius, $fn = 30);
        for(i = [(bikeMountHeight + 2 * fitMargin)/2, -(bikeMountHeight + 2 * fitMargin)/2])translate([0, i, plateHeight])sphere(r=fingerHoleRadius, $fn = 30);
    }
    
    
}