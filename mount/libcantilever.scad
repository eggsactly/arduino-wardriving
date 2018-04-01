module cantilever (cant_post_height, cant_errosion_margin, cant_slope_height, cant_overhang, cant_length, cant_length_end, cant_width)
{
    linear_extrude(height = cant_width, center = false, convexity = 10, twist = 0, slices = 2, scale = 1.0) 
    {
        polygon(
            points=[
                [0, cant_length_end], 
                [cant_post_height, + cant_length_end], 
                [cant_post_height, cant_overhang + cant_length_end], 
                [cant_post_height + cant_errosion_margin, cant_overhang + cant_length_end], 
                [cant_post_height + cant_errosion_margin + cant_slope_height, + cant_length_end], 
                [cant_post_height + cant_errosion_margin + cant_slope_height, -cant_length + cant_length_end], 
                [0, 0]
            ]
        );
    }
}