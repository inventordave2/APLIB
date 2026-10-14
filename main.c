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

static char* safe_argv( int argc, char** argv, int index ) {

    if( argv == (char**)0 )
        return (char*)0;

    if( index < 0 || index >= argc )
        return (char*)0;

    if( argv[index] == (char*)0 )
        return (char*)0;

    return argv[index];
}

int main( int argc, char** argv )   {

    char* opt = (char*)0;
    char* operand1 = (char*)0;
    char* operand2 = (char*)0;

	if( argc > 1 )
    opt = safe_argv( argc, argv, 1 );
		
	if( argc > 2 )
    operand1 = safe_argv( argc, argv, 2 );

	if( argc > 3 )
    operand2 = safe_argv( argc, argv, 3 );
	
    if( (opt != (char*)0) && lean_strcmp( opt, "ieee_754" ) )    {
        
        double f;
        if( operand1 != (char*)0 )
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
        
        if( operand1 != (char*)0 )
            a = atof( operand1 );
        
        if( operand2 != (char*)0 )
            p = atoi( operand2 );
        

        trig_fnc_tests( a );
    
        return 0;
    }
    
    
    if( lean_strcmp( opt, "calc_pi" ) )    {
        

        double r = generate_half_pi();
        
        printf( "Approx value for (1/2) * PI = %.20f\n", r );
        
        exit(0);
    }
    
    if( lean_strcmp( opt, "bitfield_add" ) )    {
    
        if( argc >= 4 )  {
            
            char* arg_a = safe_argv( argc, argv, 2 );
            char* arg_b = safe_argv( argc, argv, 3 );
            if( arg_a == (char*)0 || arg_b == (char*)0 )    {
                fprintf( stderr, "bitfield_add requires two integer arguments.\n" );
                return 1;
            }

            unsigned int A = (unsigned int) atoi( arg_a );
            unsigned int B = (unsigned int) atoi( arg_b );
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

    if( lean_strcmp( opt, "sine" ) )    { 
        
        double a = 1.0;
        
        char* angle_arg = safe_argv( argc, argv, 2 );
        if( angle_arg != (char*)0 )
            a = atof( angle_arg );
        
        printf( "My version of sine calculates sine(%.10f) to be: %.10f\nThe cstdlib calcs %.10f", a, sine( a,6 ), sin(a) );
        
        exit(0);
    }
    if( lean_strcmp( opt, "newtons_root") )  {
        
        char* input = "100.0";
        
        char* x_arg = safe_argv( argc, argv, 2 );
        if( x_arg != (char*)0 )
            input = x_arg;
            
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
    if( lean_strcmp( opt, "stringify_float" ) )    {
        
        // char* IEEE_readDouble( double f )
        char* input;
        
        if( operand1 )
            input = operand1;
        else
			 input = "65535.0";
	
        float x = (float)atof( input );

        char* result = IEEE_readFloat( x );
        
        printf( "Inputted '%.20f'. Binary string is '%s'\n", x, result );
        
        float y = 0.0;
        
        //void IEEE_writeDouble(double* dest, char* str)
        IEEE_writeFloat( &y, result );

        printf( "writing back to float silo indicates value %.20f .\n", y );
        
        
        return 0;
    }
    if( lean_strcmp(opt, "native_log") )    {
        
        char* input = "5.0";
        char* log_arg = safe_argv( argc, argv, 2 );
        if( log_arg != (char*)0 )
            input = log_arg;
            
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

		char* lhs = safe_argv( argc, argv, 2 );
		char* rhs = safe_argv( argc, argv, 3 );
		if( lhs == (char*)0 || rhs == (char*)0 )    {
			fprintf( stderr, "AP arithmetic requires OPERATION LHS RHS.\n" );
			return 1;
		}

		arg1 = strdup( opt );
		arg2 = strdup( lhs );
		arg3 = strdup( rhs );
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
    
    printf( "Result of operator '%s' calculated in %.30f clocks.\n", arg1, timespent );
 
	// EMIT OPERATION RESULT.
	if( emit_result )	{
        
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
	free( extra );

	// EXIT.
	return 0;
}
