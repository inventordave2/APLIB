#include <math.h>
#include "./native.h"
#include "./trig.h"


double divide_and_conquer_angle( double sine_, double cos_ )    {

    double r;

    return angle( tangent( sine_,cos_ ) );
}

static inline void triangle_internals( Triangle* T );

double tangent( double sine_, double cos_ )    {

    return ( sine_/cos_ );
}

double angle( double tan_ )    {

    int max_radix = 17;
    double e = tan_;
    double x = 0.0;

    signed int sign = -1;
    int n = 3;
    int v = 0;

    do    {

        v = 2;
        while( --v )
            e = e * tan_;
    
        x += ( ((double)sign) * (e/(double)n) );

        if( sign==-1 )    sign = +1;
        else              sign = -1;
        
        n += 2;

    } while( n <= max_radix );

    return x;
}

// double x = tangent( sine_, cos_ );
double atan_dave( double tan_ )    {

    double x = tan_;

    if( x > 1.0 )    {  return (generate_half_pi() - atan_dave( 1.0/x )); }
    if( x < 0.0 )    { x = 0.0 - x; return (0.0 - atan_dave( x )); }


    int n = 1;
    int high_order = 21;
    int signed sign = -1;
 
    while( (n+=2) < high_order )    {
        
        x = x + ( sign * ( quickexp( x, n )/(double)n ) );
        if( sign==-1 )    sign = +1;
        else              sign = -1;
    }

    return x;
}


inline void triangle_internals( Triangle* T )    {

    double x = T->B.x - T->A.x;
    double y = T->B.y - T->A.y;
    
    T->hyp = root2( (x * x) + (y * y) );
    T->opp = root2( (T->B.y * T->B.y) + (T->A.y * T->A.y) );
    T->adj = root2( (T->A.x * T->A.x ) + (T->B.x * T->B.x) );

    // SOH-CAH-TOA
    T->sine_ = T->opp/T->hyp;
    T->cos_ = T->adj/T->hyp;
    T->tan_ = T->opp/T->adj;
    
    T->a = divide_and_conquer_angle( T->sine_, T->cos_ );
    T->c = generate_half_pi();
    T->b = T->c - T->a;

    return;
}

Triangle triangle_from_sides_vectors( triangle_side_t hypoteneuse, triangle_side_t opp_side, triangle_side_t adjacent_side )    {

    Triangle T;
    
    vector2d A;
    A.x = hypoteneuse.v1.x; A.y = hypoteneuse.v1.y;

    vector2d B;
    B.x = adjacent_side.v2.x; B.y = opp_side.v1.y;

    vector2d C;
    C.x = B.x; C.y = A.y;

    T.A = A;
    T.B = B;
    T.C = C;

    triangle_internals( &T );

    return T;
}

Triangle triangle_from_hyp_vector( triangle_side_t hypoteneuse )    {

    vector2d v1 = hypoteneuse.v1;
    vector2d v2 = hypoteneuse.v2;
    vector2d v3;
    v3.x = hypoteneuse.v2.x;
    v3.y = hypoteneuse.v1.y;
    
    // From the  HYPOTENEUSE (AB). We derive the rest:
    // (BC) the OPP,
    // (AC) ADJACENT. ...

    Triangle T;


    T.A = hypoteneuse.v1;
    T.B = hypoteneuse.v2;
    T.C = v3;

    triangle_internals( &T );

    return T;
}

// tangent (tan) = opposite/adjacent
double tangent_alt( double angle_in_radians, int precision )   {
    
    double opp = sine( angle_in_radians, precision);
    double hyp = 1.0;
    double adj = root2( hyp - (opp*opp) );

    return (double)opp/adj;
}

double sine( double x, int terms ) {

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

double cosine( double x, int terms )    {

    double s = sine( x, terms );
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

