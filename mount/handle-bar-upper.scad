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
        translate([-handleBarDiameter/2 - handleBarMountThickness + attachAdaptorBezzelRad, attachAdaptorBezzelRad, 0]){
            minkowski()
            {
                cube([handleBarDiameter + 2*handleBarMountThickness-2*attachAdaptorBezzelRad, handleBarDiameter/2 + handleBarMountThickness - 2*attachAdaptorBezzelRad, bikeMountHeight/2]);
                cylinder(h = bikeMountHeight/2, r = attachAdaptorBezzelRad, center = false, $fn = 360);
            }
        }
        
        translate([handleBarDiameter/2, 0, 0])
            cube([handleBarScrewMountWidth, handleBarMatingPlateThickness, bikeMountHeight]);
        translate([-handleBarDiameter/2 - handleBarScrewMountWidth, 0, 0])
            cube([handleBarScrewMountWidth, handleBarMatingPlateThickness, bikeMountHeight]);
        
        // Draw slider support
        translate([-sliderSupportWidth/2, handleBarDiameter/2 + handleBarMountThickness, 0]) cube([sliderSupportWidth, SliderSupportLength + 0.5, sliderSupportHeight]);
        
        // Draw slider lip
        translate([-sliderLipWidth/2, handleBarDiameter/2+handleBarMountThickness + sliderLipLength, 0])cube([sliderLipWidth, sliderLipLength, sliderSupportHeight]);

    }
    
    // Draw inner hole
    translate([0, 0, -0.5])
        cylinder(h = bikeMountHeight+1, d = handleBarDiameter, center = false, $fn = 360);
    
    // Draw left Screw Hole
    translate([handleBarDiameter/2 + handleBarMountThickness + screwMargin + m3HeadDiameter/2, -0.5, bikeMountHeight/2])rotate([-90, 0, 0]) cylinder(h = handleBarMatingPlateThickness + 1, d = m3ThreadDiameter, center = false, $fn = 360);
    
    translate([handleBarDiameter/2 + handleBarMountThickness + screwMargin + m3HeadDiameter/2, handleBarMatingPlateThickness - m3NutHeight, bikeMountHeight/2])rotate([-90, 0, 0]) cylinder(h = m3NutHeight + 1, d = m3NutWidth, center = false, $fn = 6);
    
    // Draw Right Screw Hole
    translate([-handleBarDiameter/2 - handleBarMountThickness - screwMargin - m3HeadDiameter/2, -0.5, bikeMountHeight/2])rotate([-90, 0, 0]) cylinder(h = handleBarMatingPlateThickness + 1, d = m3ThreadDiameter, center = false, $fn = 360);
    
    translate([-handleBarDiameter/2 - handleBarMountThickness - screwMargin - m3HeadDiameter/2, handleBarMatingPlateThickness - m3NutHeight, bikeMountHeight/2])rotate([-90, 0, 0]) cylinder(h = m3NutHeight + 1, d = m3NutWidth, center = false, $fn = 6);
}
