include <libparameters.scad>;
use <libcantilever.scad>;
use <libRoundCantilever.scad>;

function rotateRoundCant(i, j) = (i == -1) ? (j == -1) ? 315 : 225 : (j == -1) ? 45 : 135;

union()
{
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
        // Draw the circular cantilevers for the featherwing
        a = 0;
        for (i = [-1, 1])
        {
            for(j = [-1, 1])
            {
                translate(v = [i * (featherWingDoublerWidth/2 - screwHoleCenterFromEdge), j* (featherWingDoublerLength/2 - screwHoleCenterFromEdge) - batteryDiameter/2 - batteryMargin, plateHeight + solderHeight]) 
                {
                    rotate([0, 0, rotateRoundCant(i, j)])circularCantilever(featherBoardThickness, featherHoleRadius, circularCantLipRad, circularCantLen);
                }
            }  
            
        } 
    }
    
    
    
    
   
    // Draw the attachment adaptor hole
    translate(v = [-(bikeMountHeight + 2 * fitMargin)/2, -(bikeMountHeight + 2 * fitMargin)/2, -0.5]) 
    {
        cube([bikeMountHeight + 2 * fitMargin, bikeMountHeight + 2 * fitMargin, plateHeight + 1], center = false);
    }
    
    // Draw the hole for the battery mount
    difference()
    {
        translate([-(batterySheathLength + 2 * fitMargin)/2, batteryPlateOffsetY + fitMargin, -0.5])cube([batterySheathLength + 2 * fitMargin, batteryPlankWidth - 2 * fitMargin, plateHeight + 1]);
        
        // Draw the battery connect block
        translate([-batterySheathLength/2 - fitMargin, batteryPlateOffsetY + (batteryPlankWidth - batteryBlockLength)/2 + fitMargin, 0])cube([batterySheathLength + 2*fitMargin, batteryBlockLength - 2*fitMargin, handle_cant_errosion_margin + handle_cant_slope_height + handle_cant_post_height]);
        
        // Draw the far cantilever fitting
        translate([0, batteryBlockLength - fitMargin + (batteryPlankWidth - batteryBlockLength)/2, 0])      
            rotate([0, 0, 0])translate([batterySheathLength/2 + fitMargin, batteryPlateOffsetY, handle_cant_errosion_margin + handle_cant_slope_height + fitMargin])
                rotate([0, -90, 0])
                    cantilever(0, 2, handle_cant_post_height - 2 - 2 * fitMargin, handle_cant_overhang - fitMargin, 0, 0, batterySheathLength + 2 * fitMargin);
        
        // Draw the near cantilever fitting
        translate([0, fitMargin + (batteryPlankWidth - batteryBlockLength)/2, 0])      
            rotate([0, 0, 0])translate([-batterySheathLength/2 - fitMargin, batteryPlateOffsetY, handle_cant_errosion_margin + handle_cant_slope_height + fitMargin])
                rotate([0, -90, 180])
                    cantilever(0, 2, handle_cant_post_height - 2 - 2 * fitMargin, handle_cant_overhang - fitMargin, 0, 0, batterySheathLength + 2 * fitMargin);
    }
        
    // Draw the little holes to put you fingers in
    for(i = [(bikeMountHeight + 2 * fitMargin)/2, -(bikeMountHeight + 2 * fitMargin)/2])translate([i, 0, plateHeight])sphere(r=fingerHoleRadius, $fn = 30);
        for(i = [(bikeMountHeight + 2 * fitMargin)/2, -(bikeMountHeight + 2 * fitMargin)/2])translate([0, i, plateHeight])sphere(r=fingerHoleRadius, $fn = 30);
    }
    
    
}