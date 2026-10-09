
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "./trig.h"
#include "./native.h"
#include "./aplib.h"
#include "./tools.h"
#include "./tests.h"

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


