#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "./lean_string.h"
#include "./tools.h"
#include "./aplib.h"
#include "./native.h"
#include "./ieee754.h"
#include "./trig.h"
#include "./tests.h"


int main( int argc, char** argv )   {

    char* opt = argv[1];
    char* operand1 = argv[2];
    char* operand2 = argv[3];
    
    if( lean_strcmp( opt, "ieee_754" ) )    {
        
        double f;
        if( argc>2 )
            f = atof( operand1 );
        else
            f = 200.0;
        
        char* _ = IEEE_readFloat( f );
        
        printf( "Internal float bitfield in ieee754 format for float value '%.5f' is:\n\n%s\n", f, _ );

        return 0;
    }
    
    
    if( lean_strcmp( opt, "tests" ) )   {
    
            double a = 0.5;
            int p = 20;
            
            if( argc > 1 )
                if( operand1 != (char*)0 )
                    a = atof( operand1 );
            if( argc > 2 )
                if( operand2 != (char*)0 )
                    p = atoi( operand2 );
                    
                    
            tangent_test( a, p );
            
            
            
            
            return 0;
    }
    
    
    if( lean_strcmp( argv[1], "calc_pi" ) )    {
        

        double r = generate_half_pi();
        
        printf( "Approx value for (1/2) * PI = %.20f\n", r );
        
        exit(0);
    }
    
    if( lean_strcmp( argv[1], "bitfield_add" ) )    {
    
        if( argc > 2 )  {
            
            unsigned int A = atoi( argv[2] );
            unsigned int B = atoi( argv[3] );
            unsigned int C;
            
            SPEEDTESTINIT();
            
            SPEEDTESTBEGIN();
            C = BITFIELD_ADD( A, B );
            SPEEDTESTEND();
            
            CALCSPEED();
            
        
            printf( "Result of bitfield addition for '%d' + '%d' = '%d'.\n", A, B, C );
            printf( "Speed of function was %.20f secs.\n", timespent );
            
            
            SPEEDTESTBEGIN();
            C = A + B;
            SPEEDTESTEND();
            CALCSPEED();
            
            printf( "Result of native + operator for '%d' + '%d' = '%d'.\n", A, B, C );
            printf( "Speed of native instruction was %.20f secs.\n", timespent );
            
            return 0;
        }
        
    }

    if( lean_strcmp( argv[1], "sine" ) )    { 
        
        double a = 1.0;
        
        if( argc > 2 )
            a = atof( argv[2] );
        
        printf( "My version of sine calculates sine(%.10f) to be: %.10f\nThe cstdlib calcs %.10f", a, sine( a,6 ), sin(a) );
        
        exit(0);
    }
    if( lean_strcmp( argv[1], "newtons_root") )  {
        
        char* input = "100.0";
        
        if( argc>2 )
            input = argv[2];
            
        double x = (double)atof( input );
        
        SPEEDTESTINIT();
        
        double custom_method = 0.0;
        double stdlib_method = 0.0;

        SPEEDTESTBEGIN();
        double result = Root_NewtonsMethod( x );
        SPEEDTESTEND();
        custom_method = (double) (end - begin) / CLOCKS_PER_SEC;
        
        SPEEDTESTBEGIN();
        double stdlib_result = sqrt( x );
        SPEEDTESTEND();
        stdlib_method = (double) (end - begin) / CLOCKS_PER_SEC;
        
        printf( "stdlib sqrt(%f) = %.10f\nMy method sqrt(%f) = %.10f\n", x, stdlib_result, x, result );
        printf( "stdlib version took: %f\nmy custom method took: %f\n", stdlib_method, custom_method );
        exit(0);
    }
    if( lean_strcmp( argv[1], "stringify_float" ) )    {
        
        // char* IEEE_readDouble( double f )
        char* input = "65535.0";
        
        if( argc>2 )
            input = argv[2];
            
        float x = (float)atof( input );

        char* result = IEEE_readFloat( x );
        
        printf( "Inputted '%.20f'. Binary string is '%s'\n", x, result );
        
        float y = 0.0;
        
        //void IEEE_writeDouble(double* dest, char* str)
        IEEE_writeFloat( &y, result );

        printf( "writing back to float silo indicates value %.20f .\n", y );
        
        
        return 0;
    }
    if( lean_strcmp(argv[1], "native_log") )    {
        
        char* input = "5.0";
        if( argc>2 )
            input = argv[2];
            
        double A = (double) atof( input );
        
        log_test( A );

        return 0;

    }

	InitAPLIB();

	char* arg1;
	char* arg2;
	char* arg3;

	if( argc < 4 )	{

		printf( "Please invoke the test app with 3 args: OPERATION LHS(A) RHS(B). Generating a default input.\n" );

		arg1 = strdup( "add" );
		arg2 = strdup( "1211" );
		arg3 = strdup( "123" );
	}
	else	{

		arg1 = strdup( argv[1] );
		arg2 = strdup( argv[2] );
		arg3 = strdup( argv[3] );
	}

	AP A = aplib->QuickAP( arg2 );
	AP B = aplib->QuickAP( arg3 );
	AP C = aplib->BlankAP();

	Flags extra = (Flags) calloc( 1, sizeof(int) );

	char opcode = aplib->get_opcode( arg1 );
	int emit_result = 1;

    operator = ops[ opcode ];
    
    SPEEDTESTINIT();
    SPEEDTESTBEGIN();
    operator( A, B, C, extra );
    SPEEDTESTEND();
    CALCSPEED();
    
    fflush( stdout );
    
    printf( "Result of operator '%s' calculated in %.30f clocks.\n", timespent, arg1 );
 
	// EMIT OPERATION RESULT.
	if( emit_result )	{
		
		/*
		char* A_WP;
		char* A_FP;
		char* B_WP;
		char* B_FP;

        char* Z	= "0";	
		char* C_WP;
		char* C_FP; 
		
		
		if( cmpdstr(A->wholepart, "0")>0 )
		    A_WP = A->wholepart;
		else
		    A_WP = Z;

        if( cmpdstr(A->fractpart, "0")>0 )
            A_FP = A->fractpart;
        else
            A_FP = Z;
            
        if( cmpdstr(B->wholepart, "0")>0 )
            B_WP = B->wholepart;
        else
            B_WP = Z;
            
        if( cmpdstr(B->fractpart, "0")>0 )
            B_FP = B->wholepart;
        else
            B_FP = Z;
        
		if( cmpdstr(C->wholepart, "0")>0 )
		    C_WP = C->wholepart;
		else
		    C_WP = Z;
		    
		if( cmpdstr(C->fractpart, "0")>0 )
		    C_FP = C->fractpart;
		else
		    C_FP = Z;
		
        */
        
		printf( "Result of '%s.%s' '%s' '%s.%s' = '%s.%s'.\n", \
		A->wholepart, A->fractpart, arg1, B->wholepart, B->fractpart, C->wholepart, C->fractpart );

	}


	aplib->FreeAP( A );
	aplib->FreeAP( B );
	aplib->FreeAP( C );

    DeInitAPLIB();
    
	free( arg1 );
	free( arg2 );
	free( arg3 );

	// EXIT.
	return 0;
}
