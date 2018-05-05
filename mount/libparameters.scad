// Feather wig doubler dimensions pulled from here: https://learn.adafruit.com/featherwing-proto-and-doubler/downloads

// All measurements are in mm

mmPerIn = 25.4;
fitMargin = 0.2;

// Feather Wing Dimensions
featherWingDoublerWidth = 2 * mmPerIn;
featherWingDoublerLength = 1.85 * mmPerIn; 
screwHoleCenterFromEdge = 0.1 * mmPerIn;
featherBoardThickness=1.5;
featherHoleRadius=2.5/2;

circularCantLipRad = 3/2 * featherHoleRadius;
circularCantLen = 2.5/2;

// Base Dimensions
riserBase = 2 * screwHoleCenterFromEdge;
bodyMargin = 10;
heightMargin = 5;
solderHeight = 4;

// Battery Dimensions
batteryDiameter = 18;
batteryLength = 69;
batteryMargin = 0;
batterySheathRadius = 3;
batterySheathLength = batteryLength/2;
batteryBlockLength = 20;

// Screw Dimensions
m3ThreadDiameter = 4.00;
m3InnerThreadDiameter = 3.00;
m3HeadDiameter = 6.00; 
m3HeadHeight = 3.00;
threadLength = 10;
screwMargin = 2; // Min distance between screws
m3NutWidth = 2 * 5.47 / sqrt(3) + 1;
m3NutHeight = 2.29;

// Attachment Adaptor Dimensions
attachAdaptorWidth = 20;
attachAdaptorBezzelRad = 2;

// Handlebar Dimensions
handleBarDiameter = 25;
bikeMountHeight = 20;
mountFitHeight = 5;
mountPostLength = 20;

sliderLipWidth = 12;
sliderLipLength = 3;
sliderSupportWidth = 5;
SliderSupportLength = 2.5;
sliderSupportHeight = 20;

handleBarMountThickness = 5;

handleBarConnectorSeparation = 8;

// Cantiliver settings for 
handle_cant_post_height = 10;
handle_cant_errosion_margin = 2;
handle_cant_slope_height = 6;
handle_cant_overhang = 2;
handle_cant_length = 1.5;
handle_cant_length_end = 3;

fingerHoleRadius = 6;

// The hardcoded value is the actual value, the min max provide the range
// no larger than the handle bar cantiliver, but no smaller than a 45 dedree
// slope
central_cant_slope_height = min(max(4, handle_cant_overhang), handle_cant_slope_height);
central_cant_errosion_margin = min(2, handle_cant_post_height - central_cant_slope_height - fitMargin);

// Intermediate calculations 
// NOTE: These should not need to be changed if you're just adjusting the parameters above
plateHeight = fitMargin + handle_cant_slope_height + handle_cant_errosion_margin + handle_cant_post_height + mountFitHeight;
plateWidth = max(batterySheathLength, featherWingDoublerWidth);
minBodyMargin = max(bodyMargin, riserBase/2);
attachAdaptorWidthActual = min(featherWingDoublerWidth - 3 * riserBase - 2 * screwMargin, attachAdaptorWidth);
attachAdaptorScrewOffset = attachAdaptorWidthActual/2 + m3HeadDiameter/2;
batteryPlankWidth = batteryBlockLength + 2 * handle_cant_overhang +  2 * fitMargin + 2 * handle_cant_length_end;
batteryPlankThickness = 2 * m3HeadHeight;

batteryPlateOffsetY = bikeMountHeight/2 + fingerHoleRadius + batteryMargin + bodyMargin/2;

plateLength = featherWingDoublerLength + batteryPlateOffsetY + batteryPlankWidth - 2 * minBodyMargin;


bikeMountWidth = attachAdaptorWidthActual + 2*(m3HeadDiameter + screwMargin);
sliderSupportHeightPlusSlideLength = sliderSupportHeight + (bikeMountWidth - sliderSupportHeight)/2;
SliderScrewHolePos = bikeMountWidth/2 + sliderSupportHeight/2 + m3HeadDiameter/2;

handleBarScrewMountWidth = m3HeadDiameter + 2 * screwMargin + handleBarMountThickness;
handleBarMatingPlateThickness = max(2*m3NutHeight, 2*m3HeadHeight);
