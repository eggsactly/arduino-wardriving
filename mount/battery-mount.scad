include <libparameters.scad>;
use <libcantilever.scad>;


union(){
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
            
}

// Draw left cantilever
translate([0, -batteryPlankWidth/2, batterySheathLength/2])rotate([0, 0, 0])translate([batteryDiameter/2 + batteryPlankThickness, 0, -batterySheathLength/2])cantilever(handle_cant_post_height, handle_cant_errosion_margin, handle_cant_slope_height, handle_cant_overhang, handle_cant_length, handle_cant_length_end, batterySheathLength);

// Draw right cantilever
translate([0, batteryPlankWidth/2, batterySheathLength/2])rotate([180, 0, 0])translate([batteryDiameter/2 + batteryPlankThickness, 0, -batterySheathLength/2])cantilever(handle_cant_post_height, handle_cant_errosion_margin, handle_cant_slope_height, handle_cant_overhang, handle_cant_length, handle_cant_length_end, batterySheathLength);
}
