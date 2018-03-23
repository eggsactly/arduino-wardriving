include <parameters.scad>;

sliderLipWidth = 12;
sliderLipLength = 3;
sliderSupportWidth = 5;
SliderSupportLength = 2.5;


bikeMountWidth = attachAdaptorWidthActual + 2*(m3HeadDiameter + screwMargin);
sliderSupportHeight = 20;
sliderSupportHeightPlusSlideLength = sliderSupportHeight + (bikeMountWidth - sliderSupportHeight)/2;
SliderScrewHolePos = bikeMountWidth/2 + sliderSupportHeight/2 + m3HeadDiameter/2;

difference()
{
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
    
    // Draw slider support
    translate([-sliderSupportWidth/2, bikeMountHeight - SliderSupportLength - 0.5, bikeMountWidth - sliderSupportHeightPlusSlideLength + 0.5])
    {
        cube([sliderSupportWidth, SliderSupportLength + 1, sliderSupportHeightPlusSlideLength + 1]);
    }
    
    // Draw slider lip
    translate([-sliderLipWidth/2, bikeMountHeight - sliderLipLength - SliderSupportLength, bikeMountWidth - sliderSupportHeightPlusSlideLength + 0.5])
    {
        cube([sliderLipWidth, sliderLipLength, sliderSupportHeightPlusSlideLength]);
    }
    
    #translate([0, bikeMountHeight - threadLength - sliderLipLength - SliderSupportLength - 0.5, SliderScrewHolePos])
        rotate([-90, 0, 0])
            cylinder(h = threadLength + 1, d = m3InnerThreadDiameter, center = false, $fn = 360);
}



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