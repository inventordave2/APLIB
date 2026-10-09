#ifndef DAVELIB_TRIG_H
#define DAVELIB_TRIG_H

double generate_half_pi(void);
double deg_to_rad(double degrees, double half_pi);
double rad_to_deg(double radians, double half_pi);

double tangent( double angle_in_radians, int precision );
double sine( double x, int terms );


#endif
