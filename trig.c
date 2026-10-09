
#include <math.h>
#include "./native.h"
#include "./trig.h"

// tangent (tan) = opposite/adjacent
double tangent( double angle_in_radians, int precision )   {
    
    double opp = sine( angle_in_radians, precision);
    double hyp = 1.0;
    double adj = sqrt( hyp - (opp*opp) );

    return (double)opp/adj;
}

double sine( double x, int terms ) {


    // 3. Iteratively calculate the series
    double total = x;
    double current_term = x;
    double x_squared = x * x;

    for (int i = 1; i < terms; i++) {
        // Multiplies previous term by -x^2 / (2i * (2i + 1))
        current_term *= -x_squared / ((2 * i) * (2 * i + 1));
        total += current_term;
    }
    

    return total;
}


double generate_half_pi(void)   {

    double x = 1.0;     
    double y = 0.0;     
    double A = 0.00000001; 
    double num_radians = 0.0;

    // The exact moment X crosses 0, the isosceles has rotated 90 degrees
    while (x > 0.0)
    {

        double new_y = y + (x * A);
        double new_x = x - (new_y * A);
        
        y = new_y;
        x = new_x;
        
        num_radians += A;
    }
    
    return num_radians;
}

// Converts degrees to radians using the bootstrapped constant
double deg_to_rad(double degrees, double half_pi)   {

    // 90 degrees strictly equals half_pi radians
    double conversion_factor = half_pi / 90.0;
    return degrees * conversion_factor;
}

// Converts radians to degrees using the bootstrapped constant
double rad_to_deg(double radians, double half_pi)   {

    double conversion_factor = 90.0 / half_pi;
    return radians * conversion_factor;
}

