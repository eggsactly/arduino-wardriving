module circularCantilever(featherBoardThickness, featherHoleRadius, circularCantLipRad, circularCantLen)
{
    intersection(featherBoardThickness, featherHoleRadius, circularCantLipRad, circularCantLen)
    {
        difference()
        {
            union()
            {
                cylinder(h = featherBoardThickness, r = featherHoleRadius, center = false, $fn=360);
                translate([0, 0, featherBoardThickness])resize([2*circularCantLen, 2*circularCantLipRad, 1])cylinder(h = circularCantLen, r1 = circularCantLipRad, r2 = featherHoleRadius, center = false, $fn=360);
            }
            translate([-circularCantLipRad, 0, -0.5])cube([2*circularCantLipRad, circularCantLipRad, featherBoardThickness+circularCantLen + 1]);
        }
    }
}