include <libparameters.scad>;
use <libcantilever.scad>;

cant_post_height = 10;
cant_errosion_margin = 2;
cant_slope_height = 6;
cant_overhang = 2;
cant_length = 1;
cant_length_end = 4;

postLength = 20;

difference()
{
    union()
    {
        translate([-handleBarDiameter/2 - handleBarMountThickness + attachAdaptorBezzelRad, attachAdaptorBezzelRad  + handleBarConnectorSeparation/2, 0]){
            minkowski()
            {
                cube([handleBarDiameter + 2*handleBarMountThickness-2*attachAdaptorBezzelRad, handleBarDiameter/2 + handleBarMountThickness - 2*attachAdaptorBezzelRad - handleBarConnectorSeparation/2 + postLength, bikeMountHeight/2]);
                cylinder(h = bikeMountHeight/2, r = attachAdaptorBezzelRad, center = false, $fn = 360);
            }
        }
        
        translate([handleBarDiameter/2,  + handleBarConnectorSeparation/2, 0])
            cube([handleBarScrewMountWidth, handleBarMatingPlateThickness, bikeMountHeight]);
        translate([-handleBarDiameter/2 - handleBarScrewMountWidth,  + handleBarConnectorSeparation/2, 0])
            cube([handleBarScrewMountWidth, handleBarMatingPlateThickness, bikeMountHeight]);
            
        // Draw left cantilever
        translate([bikeMountHeight/2 - cant_length_end, handleBarDiameter/2 + handleBarMountThickness - handleBarConnectorSeparation/2 + handleBarConnectorSeparation/2 + postLength, bikeMountHeight])rotate([0, 180, -90])cantilever (cant_post_height, cant_errosion_margin, cant_slope_height, cant_overhang, cant_length, cant_length_end, bikeMountHeight);
        
       // Draw right cantilever
       translate([-bikeMountHeight/2 + cant_length_end, handleBarDiameter/2 + handleBarMountThickness - handleBarConnectorSeparation/2 + handleBarConnectorSeparation/2 + postLength, 0])rotate([0, 0, 90])cantilever (cant_post_height, cant_errosion_margin, cant_slope_height, cant_overhang, cant_length, cant_length_end, bikeMountHeight);
    }
    
    
    
    // Draw inner hole
    translate([0, 0, -0.5])
        cylinder(h = bikeMountHeight+1, d = handleBarDiameter, center = false, $fn = 360);
    
    // Draw left Screw Hole
    translate([handleBarDiameter/2 + handleBarMountThickness + screwMargin + m3HeadDiameter/2, -0.5  + handleBarConnectorSeparation/2, bikeMountHeight/2])rotate([-90, 0, 0]) cylinder(h = handleBarMatingPlateThickness + 1, d = m3ThreadDiameter, center = false, $fn = 360);
    
    translate([handleBarDiameter/2 + handleBarMountThickness + screwMargin + m3HeadDiameter/2, handleBarMatingPlateThickness - m3NutHeight  + handleBarConnectorSeparation/2, bikeMountHeight/2])rotate([-90, 0, 0]) cylinder(h = m3NutHeight + 1, d = m3NutWidth, center = false, $fn = 6);
    
    // Draw Right Screw Hole
    translate([-handleBarDiameter/2 - handleBarMountThickness - screwMargin - m3HeadDiameter/2, -0.5  + handleBarConnectorSeparation/2, bikeMountHeight/2])rotate([-90, 0, 0]) cylinder(h = handleBarMatingPlateThickness + 1, d = m3ThreadDiameter, center = false, $fn = 360);
    
    translate([-handleBarDiameter/2 - handleBarMountThickness - screwMargin - m3HeadDiameter/2, handleBarMatingPlateThickness - m3NutHeight  + handleBarConnectorSeparation/2, bikeMountHeight/2])rotate([-90, 0, 0]) cylinder(h = m3NutHeight + 1, d = m3NutWidth, center = false, $fn = 6);
}
