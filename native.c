// LOGb( X )

/*
A Lean approximator for:

Y = LOGb( X );

When tested on onlinegdb.com, it can be argued to be technically more
accurate than the C stdlib implememted there, as my function doesn't
round the l.s.d.
*/
	
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "./lean_string.h"
#include "./tools.h"
#include "./native.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NUMERICAL_DIGITS_ASCII_OFFSET
#define NUMERICAL_DIGITS_ASCII_OFFSET
#endif

struct ap_result y_envelope;
struct ap_rational n_m;


/**
Optimisation subroutines.
Not used by default, but available for compressing APNATIVERANGE-string numbers. Bases 1,2,8,10,16.
This would then require the aplib user to unpack the packed string for use in both calculation of new LVALUES, and also for printability.
I may update the PrettyPrinter subroutines to detect packed strings, but the top 3 bits are the only assessment to make (apart from a packed-byte value 
of 0, as in '\0', not '0'), and in a packed-byte, bits 5-8 are used for the LHS digit, which could use all of it's nibble. */

char* pack( char* _ ) {
	
	APNATIVERANGE strlen__ = lean_strlen( _ );

	char* P = (char*)calloc( 1, (strlen__>>1) + 2 );
	char t;
	
	APNATIVERANGE i,j;
	for( i=0,j=0; i<strlen__-1; i+=2, j++ )	{
		
		t = _[i] - '0';
		if( t==0 )
			t=10;
		
		char nd = (_[i+1] - NUMERICAL_DIGITS_ASCII_OFFSET);
		if( nd==0 )
			nd=10;
		
		t += ( nd << 4 );
		
		P[j] = t;		
	}

	if( i==strlen__-1 )	{

	t = _[i] - NUMERICAL_DIGITS_ASCII_OFFSET;
	if( t==0 )
		t=10;
	
	P[j++] = t;
	
	}
	P[j] = '\0';
	return P;
}
char* unpack( char* _ ) {
	/** assumes a packed-digit string of 4 bits/digit. */
	
	
	APNATIVERANGE strlen__ = lean_strlen( _ );

	int BITMASK_LOWER = 15;		// 00001111 (8+4+2+1)
	int BITMASK_UPPER = 255-15; // 11110000 (128+64+32+16)
	char t;
	
	APNATIVERANGE i, j;
	char* U = (char*)calloc( 1, (2<<strlen__) + 1 );
	for( i=0, j=0; i<strlen__; i++ ) {
		
		t = _[i] & BITMASK_LOWER;
		if( t==10 )	t=0;
		
		U[j++] = t + NUMERICAL_DIGITS_ASCII_OFFSET;
		
		t = (_[i] & BITMASK_UPPER)>>4;
		if( t==0 )
			U[j++] = '\0';
		else if( t==10 )
			U[j++] = '0';
		else
			U[j++] = t + NUMERICAL_DIGITS_ASCII_OFFSET;

	}

	if( U[j-1]!='\0' )
		U[j] = '\0';

	return U;
}

unsigned int BITFIELD_ADD( unsigned int A, unsigned int B )  {
    
    unsigned int bit = 1;
    
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;
    int v = 0;
    int x = 0;
    unsigned int r = 0;
    
    Loop:
        
        a = A&bit;
        b = B&bit;
        
        a>>=x;
        b>>=x;


        v = 0;
        v |= a;
        v |= (b<<1);
        v |= (c<<2);
        
        d = 1;
        c = 0;

        if( (v==3) || (v==5) || (v==6) )    { d = 0; c = 1; }
        else if( v==0 ) { d = 0; }
        else if( v==7 ) { c = 1; }
        
        r |= (d<<x);
        if( (x & 1)==0 )    {

            x |= 1;
        }
        else    {
            
            x = BITFIELD_ADD( x, 1 );
        }
        
        if( bit < (127 << (8*(sizeof(int)-1))) )   {
            
            bit <<= 1;   
            goto Loop;
        }
        
        return r;   
}

int AP_EQ( AP A, AP B, AP epsilon, Flags info ) {

    AP _ = (AP) calloc( 1,sizeof(struct ap) );
    
	if( aplib->cmpap( A, B, _, info ) == 0 )    {
	    
	    aplib->FreeAP( _ );
	   
		return TRUE;
	}
	if( aplib->cmpap( A, B, _, info ) < 0 ) {
	    
	     aplib->sub( A, B, _, info );
	     
		if( (AP)(A-B) < epsilon ) {
		    
            aplib->FreeAP( _ );
       		return TRUE;
		}
		
		
	}
	
	if( aplib->cmpap( B, A, _, info ) > 0 )  {
	   
        aplib->sub( B, A, _, info );	 
		if( (AP)(B-A) < epsilon ) {
		    
		    aplib->FreeAP( _ );
			return TRUE;
		}
		
	}
	
	aplib->FreeAP( _ );
	
	AP C = (AP)calloc( 1,sizeof(struct ap) );
	
	*C = *AP0;
	
	return FALSE;    
}
int EQ( double A, double B, double epsilon )	{

	if( A == B )
		return TRUE;

	if( A>B )
		if( A-B < epsilon )
			return TRUE;

	if( B>A )
		if( B-A < epsilon )
			return TRUE;

	return FALSE;
}


// https://ocw.mit.edu/courses/18-01sc-single-variable-calculus-fall-2010/242ad6a22b86b20799afc7f207cd4271_MIT18_01SCF10_Ses99c.pdf
// The above is a normative paper on sine approximation using Taylor series.

double quickexp( double base, int radix )   {
    
    double result = base;
    
    while( radix > 1 )  {
        
        result *= base;
        --radix;
    }    
    
    return result;
}

int factorial( int n )  {
    
    int R = 1;
    
    while( n>=1 )   {
        
        R *= n;
        --n;
    }
    
    return R;
}


#include <stdint.h>
void* maximumValueForRange( void* ep, unsigned size )	{
	
	if( size<+1 )
		return (void*)0;
	
	uint8_t Mask = 255;
	uint8_t* storage = (uint8_t*)ep;
	unsigned int i;
	for( i=0;i<size; i++ )	{
		
		storage[i] = Mask;
	}
	
	return (void*)storage;
}

// [int] systemEndian( [void] );
// Function to test for big-endianness or little-endianness in the local machine.
// There are even simpler methods, but I wrote this one.
// requires #include <stdio.h>, if printing diagnostics.
int systemEndian()	{
	
	int s = sizeof( int );	
	int v = 0;
	
	int i;
	for( i=0; i<s; i++ )
		v += ( '1'+(i) ) << ( i*8 ); /**generates "1234..N", where N is the byte-length of a system int. */

	for( i=0; i<s; i++ )
		// Uncomment the "printf" code line below before compiling the function and it will print a clue on invocation
		// as to how the function performs the test.

		//printf( "Byte at offset '%d' contains '%c'\n", i, ((char*)(void*)(&v))[i] );

		;

	int b = ((char*)(&v))[0] == ( NUMERICAL_DIGITS_ASCII_OFFSET + s );
	if( b )
		//printf( "System is big-endian.\n" );
		;
	else
		//printf( "System is little-endian.\n" );
		;
	return ((char*)(&v))[0] == ( '1' ); // returns 1 if little-endian. returns 0 if big-endian.
}

// sample usage:  genasciistring( '0', '9' );
char* genasciistring( char start, char end )	{
	
	char i;
	
	if( start>end )	{
	
		char t = end;
		end = start;
		start = t;
	}
	
	i=start;
	
	int k=0;
	char* _ = (char*) malloc( end-start + 2 );

	while( i<=end )	{
		
		_[k++] = i++;
	}
	
	_[k] = '\0';
	
	return _;
}

static void AP_LOGb( AP X, AP Base, AP C, Flags info )	{
	
	AP root = Base;
	AP V = aplib->CopyAP( AP1, info );
	AP V2 = aplib->CopyAP( AP0, info );	
	AP N = aplib->CopyAP( AP1, info );
	
	AP* tray = (AP*)calloc( 128, sizeof(AP) );
	AP* slider = tray;

	/*
	Stage 1 is to accumulate 2k-form n-roots (radix of n) in Base via multiplicative combination
	to as close to the scalar value of input X, a reasonable approximation.
	*/

	AP temp_root = (AP)calloc( 1, sizeof(struct ap) );
	AP temp_N = (AP)calloc( 1, sizeof(struct ap) );
	AP swap_tray = (AP)0;
	AP* swap_tray_array_type = (AP*)0;
	AP _ = (AP) calloc( 1, sizeof(struct ap) );
	AP temp_slider = (AP) calloc( 1, sizeof(struct ap) );
	
	do	{

		aplib->root( root, AP2, temp_root, info );
		SWAP(root,temp_root,swap_tray);

		aplib->mul( N, AP2, temp_N, info );
		SWAP(N,temp_N,swap_tray);

		aplib->mul( V, root, V2, info );

		if( aplib->cmpap( V2, X, _, info )  == +1 )
			continue;

		SWAP(V,V2,swap_tray);

		// Cannot use SWAP here as N must be preserved for the next loop iteration as an RVALUE first.
		*slider = N;

		aplib->add( *slider, AP1, temp_slider, info );
		SWAP(*slider,temp_slider,*swap_tray_array_type);

	} while( ! AP_EQ( V, X, AP10, info ) );

    aplib->FreeAP( temp_slider );
	aplib->FreeAP( temp_root );
	aplib->FreeAP( temp_N );

	
/* The 2nd stage (if the cakker has set the appropriate flag in 'info', is to additivily accumulate all the
n-roots 1/n radix values for the denominator 'n', and produce a decimal LOG(x) valuw. */
    
    #define BUILD_RATIONAL (128<<(8*3))
    
	if( (*info) & BUILD_RATIONAL == BUILD_RATIONAL ) 	{
		
		AP max_denom = N;
		AP rational_numerator = aplib->CopyAP( AP0, info );
		AP R = aplib->CopyAP( AP0, info );
		AP d;
	
		while( slider >= tray )	{
			
			if( (d = *slider) )	{
		
				//R += ( 1.0/d );
				aplib->divide( AP1, d, swap_tray, info );
				aplib->add( R, swap_tray, swap_tray, info );
				R = swap_tray;
	
				//rational_numerator += floor( max_denom / d );
				aplib->divide( max_denom, d, swap_tray, info );
				aplib->add( rational_numerator, swap_tray, swap_tray, info );
				
			}
	
			--slider;
		}
	
		free( tray );
	
		/*
		During the above 2nd stage, the code is also generating a rational n/m form
		structure for reference (to represent the actual rational for the radix (log).
		*/
	
		n_m.n = rational_numerator;
		n_m.m = max_denom;
	
		y_envelope.r = R;
		y_envelope.n_m = n_m;

		aplib->FreeAP( d );
	}

    aplib->FreeAP( _ );
    
	return;
}

static void AP_Root_NewtonsMethod( AP A, AP B, AP C, Flags info )   {

	AP X = (AP)calloc( 1,sizeof(struct ap) );
	AP NEXT_B = aplib->CopyAP( B, info );
	AP swap_tray;

	AP PREV_X = (AP)0;
    AP diff = (AP)0;

	AP TEMP_X = aplib->CopyAP( X, info );
	
    AP max_iterations = aplib->SimpleAP( "20" );
    AP tolerance = aplib->QuickAP( "0.000000001" );

	AP* results_tray = (AP*) calloc( 128, sizeof(AP) );
	AP stray = (AP) calloc( 1,sizeof(struct ap) );
	 
	while( 1 )	{

		
		// X = A / 2.0;
		aplib->divide( A, AP2, X, info );

	    // Iteration loop
	    while( aplib->cmpap( max_iterations, AP0, stray, info ) == +1 )   {

		
	        SWAP(PREV_X,X,swap_tray);
	
			//x = 0.5 * (x + (a/x) );  // Newton iteration formula    
			aplib->divide( A, X, TEMP_X, info );
			aplib->add( X, TEMP_X, TEMP_X, info );	
			aplib->mul( AP1_2, TEMP_X, TEMP_X, info );
	
	
			aplib->sub( X, PREV_X, TEMP_X, info );
	        
			// Do not use SWAP for the following semantic, as diff is an unassigned pointer.
			diff = TEMP_X;
	
	        if( aplib->cmpap( diff, AP0, stray, info ) < 0 )
	            aplib->swapsign( diff );
	        
	       
	        
	        if( aplib->cmpap( diff, tolerance, stray, info ) < 0 )  {
	
	            break;
	        }
	
			aplib->sub( max_iterations, AP1, max_iterations, info );
	    }
	
		if( aplib->cmpap( B, AP2, stray, info ) <= 0 )
			break;
	
		aplib->sub( B, AP2, NEXT_B, info );
		
		if( aplib->cmpap( B, AP0, stray, info ) <= 0 )
			break;

		A = X;
		SWAP(B,NEXT_B,swap_tray);

	}

	aplib->FreeAP( NEXT_B );
	aplib->FreeAP( TEMP_X );
	aplib->FreeAP( PREV_X );
	aplib->FreeAP( diff );
	aplib->FreeAP( max_iterations );
	aplib->FreeAP( tolerance );

	return;
}

struct result LOGb( double X, int Base, double epsilon )	{
	
	double root = (double)Base;
	double V = 1.0;
	double V2 = 0.0;
	
	int N = 1;

	int* tray = (int*)calloc( 128, sizeof(int) );
	int* slider = tray;


	/*
	Stage 1 is to accumulate 2k-form n-roots of Base via multiplicative combination
	to as close to the scalar value of input X, a reasonable approximation.
	*/

	do	{
		
		root = sqrt( root );
		N *= 2;
		
		V2 = V * root;			

		if( V2 > X )
			continue;
	
		V = V2;

		*slider = N;
		++slider;
		
	} while( ! EQ(V, X, epsilon) );


	/*
	The 2nd stage is to additivily accumulate all the n-roots 1/n radix vales
	for the denominator 'n', and produce a decimal LOG(x) valuw.
	*/

	int max_denom = N;
	int rational_numerator = 0;
	double R = 0.0;
	int d = 0;
	
	while( slider>=tray )	{
		
		if( (d = *slider) )	{
	
			R += ( 1.0/(double)d );
			rational_numerator += floor( max_denom / d );
		}

		--slider;
	}

	free( tray );

	
	/*
	During the above 2nd stage, the code is also generating a rational n/m form
	structure for reference (to represent the actual rational for the radix (log).
	*/
	struct result y_envelope;
	struct rational n_m;

	n_m.n = rational_numerator;
	n_m.m = max_denom;

	y_envelope.r = R;
	y_envelope.n_m = n_m;

	
	return y_envelope;
}

/* Newton's Method for SQROOT approximation. */

/* The core Newtons Method loop. */

double Root_NewtonsMethod( double a )   {
    
    
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
