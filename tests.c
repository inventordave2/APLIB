
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "./trig.h"
#include "./native.h"
#include "./aplib.h"
#include "./tools.h"
#include "./tests.h"



/*
extern double sine( double x, int terms );
extern double cosine( double x, int terms );

extern double tangent( double sine_, double cos_ );
extern double tangent_alt( double angle_in_radians, int precision );

extern double angle( double tan_ );
*/
void trig_fnc_tests( double angle_in_radians )	{

		printf( "Stdlib sin(x) generates %lf for angle %lf.\n", sin(angle_in_radians), angle_in_radians );
		printf( "Davelib sine(x) generates %lf for angle %lf.\n", sine( angle_in_radians, 20 ), angle_in_radians );
		printf( "\n" );

		printf( "Stdlib cos(x) generates %lf for angle %lf.\n", cos(angle_in_radians), angle_in_radians );
		printf( "Davelib cos(x) generates %lf for angle %lf.\n" cosine(angle_in_radians), angle_in_radians );
		printf( "\n" );

		printf( "Stdlib tan(x) generates %lf for angle %lf.\n", tan(angle_in_radians), angle_in_radians );
		printf( "Davelib tangent(x) generates %lf for angle %lf.\n", tangent(angle_in_radians), angle_in_radians );
		printf( "\n" );

		printf( "Davelib angle( tangent(x) ) generates %lf, which should match %lf.\n", angle( tangent(angle_in_radians) ), angle_in_radians );

		exit(0);
}


void tangent_test( double a, int p )  {
    
    double r = tangent( a, p );
    
    printf( "The tangent for angle %.20f is %.20f\n", a, r );
    
    return;
}

void sine_test() {

    double angle = PI / 3; // 60 degrees
    int accuracy_terms = 6;

    double result = sine(angle, accuracy_terms);
    
    printf("Taylor sine(PI/3): %f\n", result);
    return;
}

int log_test( double X ) {

    SPEEDTESTINIT();
    
    SPEEDTESTBEGIN();
    //
    struct result Y = LOGb( X, 10, 0.00001 );
    //
    SPEEDTESTEND();
    
    double time_spent_lean = (double)(end - begin) / CLOCKS_PER_SEC;
    
    SPEEDTESTBEGIN();
    //
    double Z = log10( X );
    //
    SPEEDTESTEND();
    double time_spent_stdlib = (double)(end - begin) / CLOCKS_PER_SEC;

	
	printf( "C stdlib log(%f) == %f\nThe lean approximator for log(%f) gives %f\nThe rational approximation gives %d/%d\n", X, Z, X, Y.r, Y.n_m.n, Y.n_m.m );

	printf( "\n" );
	printf( "My lean function took %f time.\nThe stdlib log10() fnc took %f time.\n", time_spent_lean, time_spent_stdlib );
	
	exit(0);

}


