
#ifndef LOG_NATIVE_H
#define LOG_NATIVE_H

#include <time.h>



#define SWAP(A,B,T) T = A, A = B, B = T 

#include "./aplib.h"

static void AP_Root_NewtonsMethod( AP A, AP B, AP C, Flags info );
static void AP_LOGb( AP X, AP Base, AP C, Flags info );

unsigned int BITFIELD_ADD( unsigned int A, unsigned int B );

struct result LOGb( double X, int Base, double epsilon );
int EQ( double A, double B, double epsilon );


double Root_NewtonsMethod( double a );
double quick_trig( double angle_in_radians, int max_taylor_exponent );


double quickexp( double base, int radix );
int factorial( int n );

#define APNATIVERANGE signed long long
#define NUMERICAL_DIGITS_ASCII_OFFSET 48

#endif
