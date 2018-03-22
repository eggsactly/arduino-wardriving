include <parameters.scad>;

sliderLipWidth = 15;
sliderLipLength = 5;
sliderSupportWidth = 5;
SliderSupportLength = 5;
bikeMountWidth = attachAdaptorWidthActual + 2*(m3HeadDiameter + screwMargin);
sliderSupportHeight = (attachAdaptorWidthActual + bikeMountWidth)/2;

difference()
{
    // Draw the main block
    translate([-attachAdaptorWidthActual/2 - (m3HeadDiameter + screwMargin), 0, 0])
    {
        cube([bikeMountWidth, bikeMountHeight, bikeMountWidth]);
    }
    
    // Draw the attachment adaptor screw holes
    translate([0, bikeMountHeight, attachAdaptorWidthActual - screwMargin])rotate([90, 0, 0])
    {
        for (i = [-attachAdaptorScrewOffset, attachAdaptorScrewOffset])
        {
            for(j = [-attachAdaptorScrewOffset, attachAdaptorScrewOffset]){
                translate(v = [i, j, -0.5]) {
                    cylinder(h = bikeMountHeight+1, d = m3InnerThreadDiameter, center = false, $fn = 360);
                }
            }  
        } 
    }
    
    // Put in the slider support
    translate([-sliderSupportWidth/2, bikeMountHeight - SliderSupportLength - 0.5, (bikeMountWidth - sliderSupportHeight)/2])
    {
        cube([SliderSupportLength, sliderSupportWidth + 1, sliderSupportHeight + 1 + (bikeMountWidth - sliderSupportHeight)/2]);

    }
}

// Draw the mount insert
translate([0, 0, attachAdaptorWidthActual/2 + m3HeadDiameter + screwMargin])
{
    rotate([90, 0, 0])
    {
        translate(v = [-attachAdaptorWidthActual/2 + attachAdaptorBezzelRad, -attachAdaptorWidthActual/2 + attachAdaptorBezzelRad, 0])
        {
            minkowski()
            {
                cube([attachAdaptorWidthActual - 2 * attachAdaptorBezzelRad, attachAdaptorWidthActual - 2 * attachAdaptorBezzelRad, plateHeight/2 + 1], center = false);
                cylinder(h = plateHeight/2 + 1, r = attachAdaptorBezzelRad, center = false, $fn = 360);
            }
        } 
    }
}