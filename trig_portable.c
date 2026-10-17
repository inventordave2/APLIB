

double generate_half_pi();
double sine( double x );
double cosine( double x );
double tangent( double x );
double quickexp( double b, int r );
double root2( double a );
double atan_dave( double t );
double deg_to_rad( double deg );
double rad_to_deg( double radians );

double tangent( double x )    {

    double sine_ = sine(x);
    double cos_ = cosine(x);
    return ( sine_/cos_ );
}

double quickexp( double base, int radix )  {

  double v = 1.0;
  while( radix )  {

    v *= base;
    --radix;
  }

  return v;
}

double atan_dave( double t )    {

    if( t > 1.0 )    {  return (generate_half_pi() - atan_dave( 1.0/t )); }
    if( t < 0.0 )    { t = 0.0 - x; return (0.0 - atan_dave( t )); }


    int n = 1;
    int high_order = 48;
    int signed sign = -1;
 
    while( (n+=2) < high_order )    {
        
        t = t + ( sign * ( quickexp( tan_, n )/(double)n ) );
        if( sign==-1 )    sign = +1;
        else              sign = -1;
    }

    return t;
}

double sine( double x ) {

    double total = x;
    double current_term = x;
    double x_squared = x * x;

    for (int i = 1; i < 16; i++) {
        // Multiplies previous term by -x^2 / (2i * (2i + 1))
        current_term *= -x_squared / ((2 * i) * (2 * i + 1));
        total += current_term;
    }

    return total;
}

double cosine( double x )    {

    double s = sine( x );
    return root2( 1 - (s * s) );
}

double root2( double a )   {

    double x = a / 2.0;
    double prev_x = 0.0;
    double diff = 0.0;
    
    int max_iterations = 20;
    double tolerance = 0.000000001;
    
    // Iteration loop
    while( max_iterations-- )   {

        prev_x = x;
        x = 0.5 * (x + (a/x) );  // Newton iteration formula
        
        diff = x - prev_x;
        if( diff < 0.0 )
            diff = 0.0 - diff;
        
        if( diff < tolerance )  {

            break;
        }
        
    }

    return x;
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
double deg_to_rad(double degrees)   {

    // 90 degrees strictly equals half_pi radians
    double conversion_factor = generate_half_pi() / 90.0;
    return degrees * conversion_factor;
}

// Converts radians to degrees using the bootstrapped constant
double rad_to_deg(double radians)   {

    double conversion_factor = 90.0 / generate_half_pi();
    return radians * conversion_factor;
}

