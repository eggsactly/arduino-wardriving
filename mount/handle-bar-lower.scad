include <parameters.scad>;

handleBarMountThickness = 5;
m3NutWidth = sqrt(3) * 5.47 / 2 + 0.25;
m3NutHeight = 2.29;
handleBarScrewMountWidth = m3HeadDiameter + 2 * screwMargin + handleBarMountThickness;
handleBarMatingPlateThickness = max(2*m3NutHeight, 2*m3HeadHeight);

difference()
{
    union()
    {
        cylinder(h = bikeMountHeight, d = handleBarDiameter + 2*handleBarMountThickness, center = false, $fn = 360);
        translate([handleBarDiameter/2, 0, 0])
            cube([handleBarScrewMountWidth, handleBarMatingPlateThickness, bikeMountHeight]);
        translate([-handleBarDiameter/2 - handleBarScrewMountWidth, 0, 0])
            cube([handleBarScrewMountWidth, handleBarMatingPlateThickness, bikeMountHeight]);
    }
    
    // Draw inner hole
    translate([0, 0, -0.5])
        cylinder(h = bikeMountHeight+1, d = handleBarDiameter, center = false, $fn = 360);
    
    // Make cylinder a crecent 
    translate([-handleBarDiameter/2-handleBarMountThickness, -handleBarDiameter/2-handleBarMountThickness - 1, -0.5])
        cube([handleBarDiameter + 2*handleBarMountThickness, handleBarDiameter/2 + handleBarMountThickness+1, bikeMountHeight + 1]);
    
    
    // Draw left Screw Hole
    translate([handleBarDiameter/2 + handleBarMountThickness + screwMargin + m3HeadDiameter/2, -0.5, bikeMountHeight/2])rotate([-90, 0, 0]) cylinder(h = handleBarMatingPlateThickness + 1, d = m3ThreadDiameter, center = false, $fn = 360);
    
    translate([handleBarDiameter/2 + handleBarMountThickness + screwMargin + m3HeadDiameter/2, handleBarMatingPlateThickness - m3HeadHeight, bikeMountHeight/2])rotate([-90, 0, 0]) cylinder(h = m3HeadHeight + 1, d = m3HeadDiameter, center = false, $fn = 360);
    
    // Draw Right Screw Hole
    translate([-handleBarDiameter/2 - handleBarMountThickness - screwMargin - m3HeadDiameter/2, -0.5, bikeMountHeight/2])rotate([-90, 0, 0]) cylinder(h = handleBarMatingPlateThickness + 1, d = m3ThreadDiameter, center = false, $fn = 360);
    
    translate([-handleBarDiameter/2 - handleBarMountThickness - screwMargin - m3HeadDiameter/2, handleBarMatingPlateThickness - m3HeadHeight, bikeMountHeight/2])rotate([-90, 0, 0]) cylinder(h = m3HeadHeight + 1, d = m3HeadDiameter, center = false, $fn = 360);
}
