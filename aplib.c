/* "DAVE'S APLIB. Arbitrary-Precision mathematical tools, from Arithmetic to Trig functions \
(such as SINE and COS), working with PI, LOGARITHMS & ROOTS etc. Facilities for working within \
IEEE754 mode, and arbitrary-Precision emulation of IEEE754 floats and doubles." */

// C STDLIB INC'S
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "./lean_string.h"
#include "./aplib.h"

#define APNUM AP
#define ULL unsigned long long int

struct aplib_t* aplib;
struct Planck* planck;

/**
TABLE FOR OPTIMISING THE ARITHMETIC ALGORITHMS' REQUIREMENT TO DETERMINE A RESULT DIGIT AND A CARRYOVER DIGIT.
(A MSD IN A 2DIGIT INTERIM RESULT, e.g, 7 + 11 == 18, the carryover digit would be '1', the result digit would be '8'.)
The alternative would be to do:

r = (signed short) ( (a - b) + carry );
r = r - 48;
lsd = (r % 10);
carry = r - lsd;
carry = carry + 48;
r = lsd;
r = r + 48;

The reason for this is that the input digits are in ASCII offset, so range 48 to 57 (printable '0' to '9'),
so to do a * b or a - b with the carry digit as well, 48 has to be deducted from at least one or two of the
the interam operand digits regardless. Usefully too, later on, the overflow rows ("carry") might be made available
to an animation subroutine for visual rendering of the operation (subject to a per-interim calculation delay, so that
the sequence of interim operations can be seen happening sequentiallyn, any animation rendering the operation would need to slow the
frame-rate down for the sequence of steps to be visible to the viewer). Being in ASCII format without additional shifting & masking is kinda clean.
*/
static signed char __VALUES__[ (89+19)*2 + 2 ] = {

	'1', '9', '1', '8', '1', '7', '1', '6', '1', '5', '1', '4', '1', '3', '1', '2', '1', '1', '1', '0',
	'0', '9', '0', '8', '0', '7', '0', '6', '0', '5', '0', '4', '0', '3', '0', '2', '0', '1',
	'0', '0', '0', '1', '0', '2', '0', '3', '0', '4', '0', '5', '0', '6', '0', '7', '0', '8', '0', '9',
	'1', '0', '1', '1', '1', '2', '1', '3', '1', '4', '1', '5', '1', '6', '1', '7', '1', '8', '1', '9',
	'2', '0', '2', '1', '2', '2', '2', '3', '2', '4', '2', '5', '2', '6', '2', '7', '2', '8', '2', '9',
	'3', '0', '3', '1', '3', '2', '3', '3', '3', '4', '3', '5', '3', '6', '3', '7', '3', '8', '3', '9',
	'4', '0', '4', '1', '4', '2', '4', '3', '4', '4', '4', '5', '4', '6', '4', '7', '4', '8', '4', '9',
	'5', '0', '5', '1', '5', '2', '5', '3', '5', '4', '5', '5', '5', '6', '5', '7', '5', '8', '5', '9',
	'6', '0', '6', '1', '6', '2', '6', '3', '6', '4', '6', '5', '6', '6', '6', '7', '6', '8', '6', '9',
	'7', '0', '7', '1', '7', '2', '7', '3', '7', '4', '7', '5', '7', '6', '7', '7', '7', '8', '7', '9',
	'8', '0', '8', '1', '8', '2', '8', '3', '8', '4', '8', '5', '8', '6', '8', '7', '8', '8', '8', '9'
};

/**
This sets the pointer to the Lookup Table to the middle, as the first half of the table is for quick lookup of negative results.
For example 4 - 5 == -1. Typically, negative lookup results are in the range ( -19 <= r <= -1 ), as SUB is the only signed Operation.
Other operations have a simple method for establishing the sign without dynamic production.
*/
static signed char* VALUES = __VALUES__+(19*2);

/**
Static descriptors for indexing the available public Operators.
The associated function "get_opcode" takes a string for the Op name (e.g. "add" for A+B),
and returns the descriptor for passing to the main APLIB switch-case block.
*/

#define FNC_ENTRY_SIG AP A, AP B, AP C, Flags extra
#define apfnc_t (AP)(*opfnc)( AP A, AP B, AP C, Flags extra )
#define Result result_t

static inline AP MAX( AP A, AP B );

static const char* PI_math_h = "3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593344612847564823378678316527120190914564856692346034861045432664821339360726024914127372458700660631558817488152092096282925409171536436789259036001133053054882046652138414695194151160943305727036575959195309218611738193261179310511854807446237996274956735188575272489122793818301194912";

static const char* Lp = "0.000000000000000000000000000000000001616255"; //( units => metres )
static const char* tp = "0.0000000000000000000000000000000000000000000539127"; //( units => secs )
static const char* Mp = "0.00000002176434"; //( units => Kg )
static const char* G  = "0.000000000066743015"; //( units => m_cubed * 1/kg * 1/(secs_squared) )
static const char* Tp = "0.00000000000000000000000000000001416784"; //( units => Kelvin )


static int planck_struct_initialised = 0;
struct Planck* init_planck()    {

	if( planck_struct_initialised==1 )
		return planck;

	planck = (Planck*)calloc( 1,sizeof(struct Planck) );

	planck->Length = aplib->QuickAP( (char*)Lp );
	planck->Time   = aplib->QuickAP( (char*)tp );
    planck->Mass = aplib->QuickAP( (char*)Mp );
	planck_struct_initialised = 1;
	return planck;
}

aplib_operator ops[ 256 ];
aplib_operator operator;
aplib_operator getaplib_operator( int x );
aplib_operator getaplib_operator( int x )  {
    
    operator = ops[ x ];
    return operator;
}

static void CleanUpResult( AP A )  {
    
    if( A==(AP)0 )  {
        
        AP* C = &A;
        *C = aplib->BlankAP();
    }
    
    if( A->wholepart==(char*)0 )
        A->wholepart = strdup( "0" );
        
    if( A->fractpart==(char*)0 )
        A->fractpart = strdup( "0" );
        
    if( A->sign != '-' && A->sign != '+' )
        A->sign = '+';
        
    if( A->base == 0 )
        A->base = 10;
        
    if( A->desc == (char*)0 )   {
        
        A->desc = strdup( "AP number." );
    }
}

AP e; // Euler's Constant.
AP GR; // The "Golden Ratio".
AP FGC; // The "Fine-Grain Constant." ( ~ 1/137 )
AP AP0;
AP AP1;
AP AP2;
AP AP3;
AP AP10;
AP AP16;
AP AP1_2;
AP AP1_3;
AP AP1_4;
AP AP1_5;
AP AP3_4;
AP AP4_5;
AP AP2_3;
AP AP1_7;
AP AP2_7;
AP AP22_7;
AP AP_PI;

/* FNC DECL'S
*/

#define ASSIGNAPFROMAUTO(A, B) if( A != (AP)0 ) *A = B;

static AP QuickAP( char* val );
static AP NewAP( char* wp, char* fractpart, char* sign, int* base, signed long long int precision );
static AP SimpleAP( char* val );
static AP BlankAP();

static void FreeAllConstants();
static char* shift_ptr_past_leading_zeroes( char* );
static int get_opcode( char* op );

/* IEEE-754 (Native) Operator function signatures.
*/
static int EQ( double A, double B, double epsilon );
static Result benchmark(  AP(*f)( AP A, AP B, AP C, Flags extra ), AP A, AP B, AP C, Flags extra );

static Result benchmark(  AP(*f)(AP A, AP B, AP C, Flags extra), AP A, AP B, AP C, Flags extra ) {

	Result result;

	unsigned long long begin = 0;
	unsigned long long end = 0;

	begin = clock();
	f( A,B,C,extra );
	end = clock();

	result.r = begin - end;
	result.n_m.n = 1;
	result.n_m.m = 1;

	return result;
}

#ifdef COLOURLIB
static char* PrettyPrintAP( AP A )   {

    if( A==(AP)0 )
        return (char*)calloc( 1,sizeof(char*) );

	char* wp_f = "[brightgreen]";
	char* fp_f = "[brightyellow]";
	char* period_f = "[brightwhite]";
	char* reset_str = "[reset]";
	int len = lean_strlen( wp_f ) + lean_strlen( fp_f ) + lean_strlen( period_f ) + lean_strlen( reset_str );
	char* _ = (char*) malloc( lean_strlen(A->wholepart) + 1 + lean_strlen(A->fractpart) + len + 1 );

	lean_strcpy( _, wp_f );
	lean_strcat( _, A->wholepart );
	lean_strcat( _, period_f );
	lean_strcat( _, "." );
	lean_strcat( fp_f );
	lean_strcat( _, A->fractpart );
	lean_strcat( _, reset_str );

	return _;
}
#else
static char* PrettyPrintAP( AP A )    {

    if( A==(AP)0 )
        return (char*)calloc( 1,sizeof(char*) );

    char* _ = (char*)malloc( lean_strlen(A->wholepart)+1+lean_strlen(A->fractpart) + 1);    
	lean_strcat( _, A->wholepart );

	lean_strcat( _, "." );

	if( *A->fractpart ) 
	    lean_strcat( _, A->fractpart );
	else
	    lean_strcat( _, "0" );

    return _;
}
#endif

static const char* PrintAP( AP A )  {

	char* _ = (char*) malloc( lean_strlen(A->wholepart) + 1 + lean_strlen(A->fractpart) + 1 );

	while( *A->wholepart)
		*(_++) = *(A->wholepart++);

	*_ = '.';
	++_;

	while( *(A->fractpart) )
		*(_++) = *(A->fractpart++);

	*_ = '\0';

	return _;
}

static void swapsign( AP A )    {

    if( A->sign == '-' )
        A->sign = '+';
    else
        A->sign = '-';

    return;
}

static char* RandAPNumString( uint64_t maxlen );
static char* RandAPNumString( uint64_t maxlen )	{

	srand(time(NULL));

	uint64_t len = (rand() % maxlen);
	char* _ = (char*)malloc( len+1 );

	uint64_t i;
	for( i=0; i<len; i++ )
		_[i] = (rand() % 10) + '0';

	_[i] = 0;

	return _;
}


static void swapsign( AP A );


/* APLIB (Arbitrary-Precision) Operator function signatures. These operators are complete for arbitrary length Integers (no fractpart).
*/
static void ADD( AP A, AP B, AP C, Flags extra );
static void SUB( AP A, AP B, AP C, Flags extra );
static void MUL( AP A, AP B, AP C, Flags extra );

// From below, the implementations for the operators are even more incomplete. Still testing iteratively.
static void DIV( AP A, AP B, AP C, Flags extra );
static void BXN( AP A, AP B, AP C, Flags extra );

static void Root( AP A, AP B, AP C, Flags extra );

static void Root( AP A, AP B, AP C, Flags extra )  {

    AP x = (AP)malloc( sizeof(struct ap) );
    AP y = (AP)malloc( sizeof(struct ap) );
    AP z = (AP)malloc( sizeof(struct ap) );
    
    aplib->divide( A, AP2, x, (Flags)0 );
    AP prev_x = aplib->CopyAP( AP0, (Flags)0 );
    AP diff = aplib->CopyAP( AP0, (Flags)0 );
   
    
    int max_iterations = 20;
    AP tolerance = aplib->QuickAP( "0.000000001" );
    
    while( max_iterations-- )   {
        
        aplib->FreeAP( prev_x );
        prev_x = x;
        
        
        aplib->divide( A, x, y, (Flags)0 );
        aplib->add( x, y, y, (Flags)0 );
        aplib->mul( AP1_2, y, x, (Flags) 0 );
        
        aplib->sub( x, prev_x, diff, (Flags) 0 );
        
        if( aplib->cmpap( diff, AP0, z, (Flags)0 ) < 0 )    {
            
            diff->sign = '+';
        }
        
        if( aplib->cmpap( diff, tolerance, z, (Flags)0 ) < 0 )
            break;
    }
    
    
    *C = *x;
    
    aplib->FreeAP( x );
    aplib->FreeAP( y );
    aplib->FreeAP( z );
    
    return;
}
static void FreeAP( AP A )  {

	if( A == (AP)0 )
		return;

	if( A->wholepart != (char*)0 )  {

		free( A->wholepart );
		A->wholepart = (char*)0;
	}
	
	if( A->fractpart != (char*)0 )  {
	    
		free( A->fractpart );
		A->fractpart = (char*)0;
		
		
	}

    free( A );
    
	return;
}

/*
The Newton's Method root approximation functions. Only 'ap sqroot(ap)' is exposed via the aplib_t interface.
The others are used by the algorithm internally (by the core 'sqroot' function), they are implementation details that
do not need to be directly-accessed by the User.
*/
static AP sqrt_f( AP A )	{

	AP A_2 = (AP)0;
	aplib->mul( A, A, A_2, (int*)0 );
	AP A2x2 = (AP)0;
	aplib->sub( A_2, AP2, A2x2, (int*)0 );

	aplib->FreeAP( A_2 );
	return A2x2;
}
static AP sqrt_deriv_f( AP A )	{

	AP R;
	NULLAP(R);
	aplib->mul( A,AP2,R,0 );
	return R;
}

static int cmpap( AP A, AP B, AP C, Flags extra )   {

	int v = 0;
	v = cmpdstr( A->wholepart, B->wholepart );

	if( v>=0 )  {

		*C = *A;
	}
	else if(v<0) {

		*C = *B;
	}
	else    {

		v = cmpdstr( A->fractpart, B->fractpart );
		if( v>=0 )
			*C = *A;
		else
			*C = *B;
	}

	return v;
}

static inline AP MAX( AP A, AP B )    {

    uint64_t x = 0;
    char z = (char)0;

    while( (z = A->wholepart[ x ]) )  {
        
        if( z > B->wholepart[ x ] ) {
        
            return A;
        }    
        if( z < B->wholepart[ x ] ) {
            
            return B;
        }
        
        ++x;
    }
    
    x = 0;
    
    while( (z = A->fractpart[ x ]) )    {
        
        if( z > B->fractpart[ x ] )
            return A;
            
        if( z < B->fractpart[ x ] )
            return B;
            
        ++x;
    }
    
    return A;
}

static uint64_t max( uint64_t a, uint64_t b );
static uint64_t max( uint64_t a, uint64_t b ) {
    
    if( a >= b )
        return a;
    
    return b;
}
static void _Root_( AP A, AP B, AP C, Flags extra )	{

	AP x0 = BlankAP();
	
	DIV( A,AP2, x0, extra );

	AP epsilon = aplib->QuickAP( "0.0000000001" );
	AP tolerance = aplib->QuickAP( "0.000001" );

	uint32_t max_iterations = 20;

	AP y  = aplib->BlankAP();
	AP yprime = aplib->BlankAP();
	AP x1 = aplib->BlankAP();
	AP x1_b = aplib->BlankAP();
	AP x1_c = aplib->BlankAP();
    AP R  = aplib->BlankAP();
    
	while( max_iterations-- )	{

		y = sqrt_f( x0 );
		yprime = sqrt_deriv_f( x0 );

		if( aplib->cmpap( yprime, epsilon, R, (int*)0 ) < 0 )
			break;

		DIV( y,yprime, x1, extra );
		SUB( x0,x1, x1_b, extra );
		SUB( x1_b,x0, x1_c, extra );

		aplib->FreeAP( x0 );
		aplib->FreeAP( x1 );
		aplib->FreeAP( x1_b );
		aplib->FreeAP( y );
		aplib->FreeAP( yprime );

		AP r = (AP)0;
		if( ( aplib->cmpap( x1_c,tolerance, r, (int*)0 ) < 1 ) )  {

			*C = *x1_c;
			break;
		}


		*x0 = *x1_c;
		aplib->FreeAP( x1_c );
	}

	*C = *AP0;
	return;
}

/* Operator function implementations.
*/
static char* shift_ptr_past_leading_zeroes( char* _ )	{

	if( _==(char*)0 )
		return _;

	if( *_=='\0' )
		return _;

	while( *_=='0' )
		++_;

	if( *_=='\0' )
		--_;

	return _;
}

static int get_opcode( char* op )	{

	if( lean_strcmp( op,"mul" ) )
		return OPMUL;

	if( lean_strcmp( op,"bxn" ) )
		return OPBXN;

	if( lean_strcmp( op,"add" ) )
		return OPADD;

	if( lean_strcmp( op,"sub" ) )
		return OPSUB;

	if( lean_strcmp( op,"div" ) )
		return OPDIV;

	if( lean_strcmp( op,"sqroot" ) )
		return OPSQROOT;

	if( lean_strcmp( op,"log" ) )
		return OPLOG;

	return -1;
}


typedef struct strlen_x {
    
    uint64_t wp;
    uint64_t fp;

} strlen_x;

typedef struct AB_prescan   {
    
    AP max;
    struct strlen_x strlen_a;
    struct strlen_x strlen_b;

} AB_prescan;

static struct AB_prescan lean_strlen_and_max( AP A, AP B ) ;

#define INIT_ABPRESCAN(X) X.max = (AP)0; X.strlen_b.wp = 0; X.strlen_b.fp = 0; X.strlen_a.wp = 0; X.strlen_a.fp = 0;
static struct AB_prescan lean_strlen_and_max( AP A, AP B )    {
    
    struct AB_prescan ab;
    INIT_ABPRESCAN(ab);
    //shift_left_leading_zeroes( A );
    //shift_left_leading_zeroes( B );
    char* Awp = A->wholepart;
    char* Bwp = B->wholepart;
    
    while( *Awp=='0' )
        ++Awp;
    if( !*Awp )
        --Awp;
        
    while( *Bwp=='0' )
        ++Bwp;
    if( !*Bwp )
        --Bwp;
        
    ab.max = A;
    
    uint64_t x = 0;
    char z = (char)0;

    while( (z = Awp[ x ]) )  {
        
        if( z > Bwp[ x ] ) {
        
            if( Bwp[ x ]=='\0' )
                ab.strlen_b.wp = x;
        }    
        if( z < Bwp[ x ] ) {
            
            ab.max = B;
        }
        
        ++x;
    }
    
    ab.strlen_a.wp = x;
    
    if( ab.strlen_b.wp == 0 )   {
        
        while( Bwp[ x ] )
            ++x;

        ab.strlen_b.wp = x;
    }
    
    
    x = 0;
    int p = '\0';
    
    char* Afp = A->fractpart;
    char* Bfp = B->fractpart;
    
    
    if( Afp != (char*)0 )    {
        

        while( (z = Afp[ x ]) )    {
            
            if( Bfp != (char*)0 )
                p = Bfp[ x ];

                
            if( z > p ) {
    
                if( p=='\0' )
                    ab.strlen_b.fp = x;
            }
            
            if( z < p ) {
                
                ab.max = B;
            }
            
            ++x;
        }
        
        ab.strlen_a.fp = x;
        
        if( ab.strlen_b.fp==0 ) {
            
            if( Bfp != (char*)0 )
                p = Bfp[ x ];
            
            while( p )
                ++x;
        
            ab.strlen_b.fp = x;    
        }
    
    }
    
    
    return ab;
}


static void ADD( AP A, AP B, AP C, Flags extra ) {

    register char* Apart;
    register char* Bpart;
    register char* Cpart;
    char** D;

    unsigned char a;
    unsigned char b;
    unsigned char carry;
    unsigned char r;

    unsigned char a0;
    unsigned char b0;
    	
    uint8_t PART = 1;
    
	uint64_t lenA;
	int64_t x;
	
	uint64_t lenB;
	int64_t y;
	
	int64_t z;

   	a0 = 0;
    b0 = 0;
 
	a = '0';
	b = '0';
	r = '0';
	
	
    while( 0 )  {
      
   ADDloop:    



        if( PART==1 )   {
            
            if( x < y ) {
    
                a0 = 1;
                ++x;
            }
            else if( y < x )    {

                b0 = 1;
                ++y;
            }
            
        }

        
    	if( a0 )
    		a = '0';
    	else
    		a = Apart[x];
    
    	if( b0 )
    		b = '0';
    	else
    		b = Bpart[y];
    
    	r = a + b + carry;
    	r = r - (48 * 3);
    
    	carry = VALUES[ r*2 ];
    	Cpart[z] =  VALUES[ (r*2)+1 ];


        --x;
        --y;
        --z;
        
        a0 = 0;
        b0 = 0;
        
        
        if( x<0 )
    		a0 = 1;
    
    	if( y<0 )
    		b0 = 1;
    
        if( a0 && b0 )	{
    
    		Cpart[z] = carry;
    		a0 = 0;
    		b0 = 0;
    	}
    	else	{
    
    		goto ADDloop;
    	}        
        
        shift_left_leading_zeroes( Cpart );
        
        // After the indices reach A[0] and B[0]
        if( PART == 1 ) PART = 2; // wholepart
        else if( PART == 2 ) PART = 0; // operands consumed
    }
    
    if( PART==1 ) { // fractpart
        
        //set pointers for the ->fractpart buffers of each operand, including initialiizing the fractpart buffer for the lvalue on the heap.
        
        Apart = A->fractpart;
        Bpart = B->fractpart;
        
	    lenA = lean_strlen( Apart );
	    x = lenA-1;

	    lenB = lean_strlen( Bpart );
	    y = lenB-1;        

        z = x + 2;

	    if( y > x ) {
	       
	        z = y + 2;
	    }
        
	   if( C->fractpart==(char*)0 )    {
            
            D = &C->fractpart;
            *D = (char*) malloc( z + 1 );
        }
        
        Cpart = C->fractpart;
        carry = (unsigned char)'0';
        --z;
        
        goto ADDloop;
    }
    
    if( PART==2 )   { // wholepart
    
        //set pointers for the ->wholepart buffers, and initialise the buffer for lvalue->wholepart on the heap.
        Apart = A->wholepart;
        Bpart = B->wholepart;
 
 	    lenA = lean_strlen( Apart );
	    x = lenA-1;

	    lenB = lean_strlen( Bpart );
	    y = lenB-1;
	    
	    if( y > x )
	        z = y + 2;
	    else
	        z = x + 2;
	       
	    
        if( C->wholepart==(char*)0 )    {
            
            D = &C->wholepart;
            *D = (char*) malloc( z + 1 );
        }
        
        if( carry != '0' )   {
            
            Cpart[0] = '0';
            shift_left_leading_zeroes( Cpart );
        }

        Cpart = C->wholepart;
        --z;
        
        // No need to initialise carry, as that was set by the carry left after the msc of fractparts have combined.
        goto ADDloop;
    }


    if( (A->sign=='+') && (B->sign=='+') )  {
    
        C->sign = '+';
    }
    else    {
        
        AP largest = MAX( A, B );
        
        if( largest==A ) {
    
                if( A->sign=='-' )
                    C->sign = '-';
        }
        else if( largest==B )  { 
             
            if( B->sign=='-' )
                C->sign = '-';
        }

    }
    
	/**
	END OF ADD BLOCK
	*/

	return;
}

static void SUB( AP A, AP B, AP C, Flags extra ) {

    register char* Apart;
    register char* Bpart;
    register char* Cpart;
    char** D;

    unsigned char a;
    unsigned char b;
    signed short carry;
    signed short r;

    unsigned char a0;
    unsigned char b0;
    	
    uint8_t PART = 1;
    
	uint64_t lenA;
	int64_t x;
	
	uint64_t lenB;
	int64_t y;
	
	int64_t z;

   	a0 = 0;
    b0 = 0;
 
	a = '0';
	b = '0';
	carry = '0';
	r = '0';
	
	
    while( 0 )  {
      
    SUBloop:
    
    	if( a0 )
    		a = '0';
    	else
    		a = Apart[x];
    
    	if( b0 )
    		b = '0';
    	else
    		b = Bpart[y];
    
    	r = (signed short) ( (a - b) + carry );
    	
    	
    	r = r - 48;
    
    	if( r < 0 )	{
    
    		r = 10 + r;
    		carry = '0' - 1;
    	}
    	else	{
    
    		carry = VALUES[ ((r)*2) ];
    	}
    
    	Cpart[z] = VALUES[ ((r)*2)+1 ];
    
    	--z;
    	--x;
    	--y;
    
    
    	if( x<0 )
    		a0 = 1;
    
    	if( y<0 )
    		b0 = 1;
    
    
    	if( a0 && b0 )	{
    
            if( PART==2 )   {
    
        		if( carry == ('0' - 1) )	{
        
        			// result is negative.
        			Cpart[z] = '-';
        		}
        		else
        			Cpart[z] = carry;
            }
            else    {
                
                Cpart[0] = '0';
                shift_left_leading_zeroes( Cpart );
                goto SUBloop;
            }
    
    	}
    	else
    		goto SUBloop;

       if( PART == 1 ) PART = 2; // wholepart
        else if( PART == 2 ) PART = 0; // operands consumed
    }
    
    if( PART==1 ) { // fractpart
        
        //set pointers for the ->fractpart buffers of each operand, including initialiizing the fractpart buffer for the lvalue on the heap.
        
        Apart = A->fractpart;
        Bpart = B->fractpart;
        
	    lenA = lean_strlen( Apart );
	    x = lenA-1;

	    lenB = lean_strlen( Bpart );
	    y = lenB-1;        

        z = x + 2;

	    if( y > x ) {
	       
	        z = y + 2;
	    }
        
	   if( C->fractpart==(char*)0 )    {
            
            D = &C->fractpart;
            *D = (char*) malloc( z + 1 );
        }
        
        Cpart = C->fractpart;

        --z;
        
        goto SUBloop;
    }
    
    if( PART==2 )   { // wholepart
    
        //set pointers for the ->wholepart buffers, and initialise the buffer for lvalue->wholepart on the heap.
        Apart = A->wholepart;
        Bpart = B->wholepart;
 
 	    lenA = lean_strlen( Apart );
	    x = lenA-1;

	    lenB = lean_strlen( Bpart );
	    y = lenB-1;
	    
	    if( y > x )
	        z = y + 2;
	    else
	        z = x + 2;
	       
	    
        if( C->wholepart==(char*)0 )    {
            
            D = &C->wholepart;
            *D = (char*) malloc( z + 1 );
        }
        
        if( carry != '0' )   {
            
            Cpart[0] = '0';
            shift_left_leading_zeroes( Cpart );
        }

        Cpart = C->wholepart;
        --z;
        
        // No need to initialise carry, as that was set by the carry left after the msc of fractparts have combined.
        goto SUBloop;
    }

    
	/**
	END OF SUB BLOCK
	*/

	if( *Cpart =='-' )	{

        C->sign = '-';
        *Cpart = '0';
	}
	else    { C->sign = '+'; }
	
	shift_left_leading_zeroes( Cpart );
	
	return;
}


static void SUB_old( AP A, AP B, AP C, Flags extra ) {

	char* Awp = A->wholepart;
	char* Bwp = B->wholepart;
	char* Cwp;

	uint64_t lenA = lean_strlen(Awp);
	int64_t x = lenA-1;

	uint64_t lenB = lean_strlen(Bwp);
	int64_t y = lenB-1;

	uint64_t lenC = max(lenA,lenB)+1;
    uint8_t PART = 2;
    
	if( C==(AP)0 )  {

		C = (AP)calloc( 1, sizeof(struct ap) );
		C->wholepart = (char*)malloc( lenC+1 );
	}

	if( C->wholepart == (char*)0 )  {

		C->wholepart = (char*)malloc( lenC + 1 );
	}
    
    Cwp = C->wholepart;

	Cwp[ lenC ] = '\0';
	Cwp[ lenC-1 ] = '0';

	ULL z = lenC-1;

	signed char a = '0';
	signed char b = '0';

	unsigned char a0 = 0;
	unsigned char b0 = 0;

	signed char carry = '0';

	signed char r = '0';

SUBloop:

	if( a0 )
		a = '0';
	else
		a = Awp[x];

	if( b0 )
		b = '0';
	else
		b = Bwp[y];

	r = (signed short) ( (a - b) + carry );
	r = r - 48;

	if( r<0 )	{

		r = 10 + r;
		carry = '0' - 1;
	}
	else	{

		carry = VALUES[ ((r)*2) ];
	}

	Cwp[z] = VALUES[ ((r)*2)+1 ];

	--z;
	--x;
	--y;


	if( x<0 )
		a0 = 1;

	if( y<0 )
		b0 = 1;


	if( a0 && b0 )	{

        if( PART==2 )   {

    		if( carry == ('0' - 1) )	{
    
    			// result is negative.
    			Cwp[z] = '-';
    		}
    		else
    			Cwp[z] = carry;
        }
        else    {
            
            Cwp[0] = '0';
            shift_left_leading_zeroes( Cwp );
        }

	}
	else
		goto SUBloop;

	if( Cwp[z]=='-' )	{

		uint64_t _len = lean_strlen(Cwp);
		++Cwp;
		AP D;
		D = BlankAP();
		D->wholepart = (char*)malloc( _len+1 );

		D->wholepart[0] = '1';
		D->wholepart[1] = '\0';

		AP F; // = (AP)calloc( _len+1, sizeof(char) );
		F = aplib->BlankAP();
		F->wholepart = (char*)malloc( _len+1 );
		++F->wholepart;

		SUB( D,C,F,extra );
		--Cwp;
		--F->wholepart;
		F->wholepart[0] = '-';
		lean_strcpy( Cwp,F->wholepart );
		aplib->FreeAP( D );
		aplib->FreeAP( F );
	}
	/**
	END OF SUB BLOCK
	*/
	
	shift_left_leading_zeroes( Cwp );
       
    if( cmpdstr(A->wholepart, B->wholepart) < 0 )   {
        
        C->sign = '-';
    }
    else
        C->sign = '+';

	return;
}

#define INITAP(A) AP A = (AP)calloc(sizeof(struct ap)) 

#define CHECKAP(A,lenwp,lenfp) if( A==(AP)0 ) *A = *BlankAP();\
if( A->wholepart==(char*)0 ) A->wholepart = (char*)calloc( lenwp+1, sizeof(char) );\
if( A->fractpart==(char*)0 ) A->fractpart = (char*)calloc( lenfp+1, sizeof(char) ); 

static void MUL( AP A, AP B, AP C, Flags extra ) {

	register char* Awp = A->wholepart;
	register char* Bwp = B->wholepart;
	register char* Cwp;

	int64_t lenA = lean_strlen( Awp );
	int64_t lenB = lean_strlen( Bwp );
	int64_t maxlenAB = max( lenA, lenB );
	register int64_t lenC = lenA+lenB;

    if( C->wholepart == (char*)0 )
        C->wholepart = (char*)malloc( lenC+1 );

    Cwp = C->wholepart;
    Cwp[ lenC ] = '\0';

	signed char carry = 0;
	signed char a = 0;
	signed char b = 0;
	signed char c = 0;
    signed char ires_carry = 0;
    
	register signed long long int x = lenA-1;
	register signed long long int y = lenB-1;
	register signed long long int z = 0;
    signed long long int ires_msd_column = (lenC-lenB); // num cols in row 0 is 3, ||B|| 
    
	register signed long long cz = 0;
	signed long long tz = 0;

	z = cz = tz = lenA+lenB-1;

	// Loop
	while( x>=0 ) {

        a = Awp[x] & 15;
		b = Bwp[y] & 15;
		c = (a * b) + carry + Cwp[ cz ];
		
		Cwp[cz] = c % 10;
		carry = (signed char)(c - (c % 10))/10;

        
		if( carry > 8 ) fprintf( stderr, "Carry digit > 8 in MUL: %d\n", carry );

		if( y==0 )	{

            Cwp[ cz-1 ] += carry;

			/*
			Each loop of A (next digit in B), we no longer accum. the same C offset,
			it's now shunted to the left 1.
			*/

			--x;
			y = lenB-1;
			
			--z;
			cz = z;
			carry = 0;
		}
		else    {
		    
			--y;
			--cz;
		}
	}
	
    while( lenC ) {
        
        Cwp[ tz-- ] |= 48;
        --lenC;
    }

    C->sign = (A->sign != B->sign) ? '-' : '+';
    
	return;
}

/*
char* Afp = A->fractpart
char* Bfp = B->fractpart
*/

static Flags spare_extra;

static void DIV( AP A, AP B, AP C, Flags extra ) {

    if( extra == (Flags)0 )
        extra = spare_extra;

	char* Awp = A->wholepart;
	char* Bwp = B->wholepart;
	char* Cwp;

	uint64_t strlen_B = lean_strlen(Bwp);
	uint64_t strlen_A = lean_strlen(Awp);
	uint64_t strlen_C = strlen_A + 20;

	uint64_t strlen_REMAINDER = 0;


	char* REMAINDER; 
	char* RESULT;
	char* NEW_REMAINDER;
	AP _;
	char ch = '0';

	AP APREMAINDER = aplib->BlankAP();
	APREMAINDER->wholepart = (char*)malloc( strlen_C +1 );
	
	AP APRESULT = aplib->BlankAP();
	APRESULT->wholepart = (char*)malloc( strlen_C+1 );
	
	AP APNEWREMAINDER = aplib->BlankAP();
    APNEWREMAINDER->wholepart = (char*)malloc( strlen_C +1 );

	REMAINDER = APREMAINDER->wholepart;
	RESULT = APRESULT->wholepart;
	NEW_REMAINDER = APNEWREMAINDER->wholepart;

	if( C==(AP)0 ) {

		C = (AP)calloc( 1, sizeof(struct ap) );
		C->wholepart = (char*)malloc( strlen_C+1 );
	}

    uint64_t len_c_wp = strlen_A;
    // As this current implementation of the function reads only the wholeparts of
    // RVALUES A and B, until I have the DIV function fundamentally to my liking.
    
    
    if( C->wholepart == (char*)0 )  {

        C->wholepart = (char*)malloc( len_c_wp + 1 );
    }
    


	Cwp = C->wholepart;

	//Cwp[ strlen_C ] = '\0';
	//*extra |= ( OPDIV << 8 );

	uint64_t x = 0;

	for( ; x<strlen_B; x++ )    {

		Cwp[x] = '0';
		REMAINDER[x] = Awp[x];
	}


	REMAINDER[x] = '\0';

	if( cmpdstr( REMAINDER, Bwp )<0 )  {

		Cwp[x] = '0';
		REMAINDER[x] = Awp[x];

		REMAINDER[x+1] = '\0';
	}
	else
		--x;

Loop:

    
	*extra = *extra & (255<<16);
	
	BXN( APREMAINDER, B, APRESULT, extra );

	SUB( APREMAINDER, APRESULT, APNEWREMAINDER, extra );

	signed diff = cmpdstr( APNEWREMAINDER->wholepart, "0" );

    
	Cwp[x] = (char) (*extra & 255 ) + 48;
	Cwp[x+1] = '\0';
	++x;

	if( ((diff==0) && x>=strlen_A) || (x>=strlen_C) )  {

		aplib->FreeAP( APREMAINDER );
		aplib->FreeAP( APNEWREMAINDER );
		aplib->FreeAP( APRESULT );


        /*
        The fixed-point decimal has it's position for result string C at the same position it is for input A.
        Ergo, offset_fp = strlen(Awp). When this function returns, the main() function shifts the string so
        as to eliminate leading 0's, but as this iteration of the function only thinks of Integer strings,
        a shimmy is needed. Or, I could update my functions.
        */
		
	    C->sign = (A->sign != B->sign) ? '-' : '+';
	    
	    *extra |= (len_c_wp << 16);
		return;
	}

	_ = APREMAINDER;
	APREMAINDER = APNEWREMAINDER;
	APNEWREMAINDER = _;
	
    REMAINDER = APREMAINDER->wholepart;
    
	if( x >=strlen_A )
		ch = '0';
	else
		ch = Awp[x];

	strlen_REMAINDER = lean_strlen(REMAINDER);
	
	REMAINDER[ strlen_REMAINDER ] = ch;
	REMAINDER[ strlen_REMAINDER+1 ] = '\0';

	goto Loop;
}

static void BXN( AP A, AP B, AP C, Flags extra ) {

	char* Awp = A->wholepart;
	char* Bwp = B->wholepart;
	char* Cwp;

	ULL strlen_Awp = lean_strlen( A->wholepart );
	ULL strlen_Bwp = lean_strlen( B->wholepart );

	char* _;
	char* R1 = (char*)calloc( strlen_Awp+1,sizeof(char) );
	R1[0] = '0';
	char* R2 = (char*)calloc( strlen_Awp+1,sizeof(char) );
	int count = 0;

	AP APR1 = aplib->BlankAP();
	AP APR2 = aplib->BlankAP();

	APR1->wholepart = R1;
	APR1->sign = '+';
	APR2->wholepart = R2;
	APR2->sign = '+';

    AP swap = (AP)malloc( sizeof( struct ap) );

bxn_loop:

	ADD( APR1,B,APR2,extra );

	if( cmpdstr( APR2->wholepart, Awp )<1 )	{

	    *swap = *APR1;
	    *APR1 = *APR2;
	    *APR2 = *swap;
		count = count + 1;
		goto bxn_loop;
	}
    
	ULL strlen_R1 = lean_strlen( APR1->wholepart );
	

	if( C==(AP)0 )
		*C = *aplib->BlankAP();
	
	if( C->wholepart == (char*)0 )
	    C->wholepart = (char*)malloc( strlen_R1 + 1 );

	Cwp = C->wholepart;
    
	*extra = *extra | (count&255);
	strcpy( Cwp, APR1->wholepart );
	aplib->FreeAP( APR1 );
	aplib->FreeAP( APR2 );
    
    // 'swap' has a duplicate heap reference, which would result in a free() error.
    // Just release the AP allocation.
    free( swap );
    
	return;

}

#ifndef EXPECT_STDLIB
/* nostdlib malloc( n ) */
void* (*malloc_fnc)( int size );
static void* (*aplib_malloc)( int size );
void* register_heap_allocator( void*(*f)(int) )   {

	aplib_malloc = f;
	return (void*) aplib_malloc;
}

/*


*/


/* nostdlib calloc( n,s ) */
void* (*calloc_fnc)( int numentries, int size );
static void* (*aplib_calloc)( int numentries, int size );
void* register_heap_callocator( void*(*f)( int numentries, int size ) )  {

	aplib_calloc = f;
	return (void*) aplib_calloc;
}

/* nostdlib free( ptr ) */
void (*heap_free_alloc_fnc)( void* ep );
static void (*aplib_free)( void* ptr );
void* register_heap_alloc_free( void (*f)(void* ep) )  {

	aplib_free = f;
	return (void*) aplib_free;
}
#endif


static AP CopyAP( AP A, Flags extra );



static AP BlankAP() {

	return (AP)calloc( 1, sizeof(struct ap) );
}

static AP QuickAP( char* val )	{

	AP A = (AP) calloc( 1,sizeof(struct ap) );
	
	A->sign = '+';
	A->base = 10;

	A->wholepart = (char*)0;
	A->fractpart = (char*)0;

	unsigned long long x = 0;
	unsigned long long y = 0;
	unsigned long long z = 0;

	while( val[x] )	{

		if( val[x]=='.' )   {
		    
			A->wholepart = (char*)malloc(x + 1);
			z = x;
		}
		
		++x;
	}

	if( A->wholepart==(char*)0 )    {

		A->wholepart = strdup( val );
		A->fractpart = strdup( "0" );
	}
    else    {
        
        char* _;

        _ = A->wholepart;
        
        y = z;
        while( y )  {
            
            *_ = *val;
            ++val;
            ++_;
            --y;
        }
        
        A->fractpart = (char*)malloc( (x - z) + 1 );
        _ = A->fractpart;
        
        y = x - z;
        ++val; // skip past the '.' 
        
        while( y ) {
            
            *_ = *val;
            ++val;
            ++_;
            --y;
        }
    }

	return A;
}

AP SimpleAP( char* val )	{

	struct ap A;
	A.precision = aplib->precision;

	char* clippedval = lean_clipstring( val, A.precision );
	A.wholepart = clippedval;
	A.fractpart = strdup( "0" );
	A.sign = '+';
	A.base = 10;

	AP C = aplib->BlankAP();

	ASSIGNAPFROMAUTO(C, A);

	return C;
}



static AP CopyAP( AP A, Flags extra )   {

	AP _ = (AP)malloc( sizeof(struct ap) );

	memcpy( _, A, sizeof(struct ap) );

	return _;
}

static void LOGb( AP A, AP B, AP C, Flags extra );

static AP APNUMERATOR;
static AP APDENOMINATOR;
static void LOGb( AP A, AP B, AP C, Flags extra )	{

	AP root = B;

	AP V = aplib->CopyAP( AP1, extra );
	AP V2 = aplib->CopyAP( AP0, extra );

	AP N = aplib->CopyAP( AP1, extra );

	AP* tray = (AP*)calloc( 128, sizeof(AP) );
	AP* slider = tray;


	/*
	Stage 1 is to accumulate 2k-form n-roots of Base via multiplicative combination
	to as close to the scalar value of input X, a reasonable approximation.
	*/

	do	{

		aplib->root( root, AP2, root, extra );
		aplib->mul( N, AP2, N, extra );

		aplib->FreeAP( V2 );

		aplib->mul( V, root, V2, extra );

		if( aplib->cmpap( V2, V, (AP)0, extra ) >= 0 )
			break;

		aplib->FreeAP( V );
		*V = *V2;

		aplib->divide( AP1, N, *slider, extra );
		++slider;

	} while( 1 );


	/*
	The 2nd stage is to additivily accumulate all the n-roots 1/n radix values
	for the denominator 'n', and produce a decimal LOG(x) value.
	*/
AP max_denom = aplib->CopyAP( N, extra );
	AP rational_numerator = aplib->CopyAP( AP0, extra );

	AP d;

	while( slider>=tray )	{

		if( (d = *slider) )	{

			AP t = (AP)0;
			aplib->divide( max_denom, d, t, extra );
			aplib->floor( t, t, extra );
			aplib->add( rational_numerator, t, rational_numerator, extra );
			aplib->FreeAP( t );
		}

		--slider;
	}

	free( tray );


	/*
	During the above 2nd stage, the code is also generating a rational n/m form
	structure for reference, to represent the actual rational for the radix (log).
	*/

	APNUMERATOR = rational_numerator;
	APDENOMINATOR = max_denom;

	if( C==(AP)0 )  {

		C = (AP)malloc( sizeof(struct ap) );
	}

	aplib->divide( APNUMERATOR, APDENOMINATOR, C, extra );
}

static void floor_ap( AP A, AP C, Flags extra );
static void floor_ap( AP A, AP C, Flags extra )   {


	AP* C_ref = (AP*)&C;
	C = *C_ref;

	C->wholepart = (char*)malloc( lean_strlen(A->wholepart) );
	lean_strcpy( C->wholepart, A->wholepart );
	C->fractpart = (char*)malloc( 2 );
	lean_strcpy( C->fractpart, "0" );
	C->sign = A->sign;
	C->base = A->base;
	C->precision = 0;
}

static char* default_desc = "AP Arbitrary-Precision number.";

static AP NewAP( char* wp, char* fractpart, char* sign, int* base, Precision precision ) {

	AP a = (AP) malloc( sizeof(struct ap) );
    a->desc = default_desc;

    char* Awp = (char*)malloc( lean_strlen(wp)+1 );
    lean_strcpy( Awp, wp );
	a->wholepart = Awp;
	
	char* Afp = (char*)malloc( lean_strlen(fractpart)+1 );
	lean_strcpy( Afp, fractpart );
	a->fractpart = Afp;

	a->sign = *sign;
	a->base = *base;
	a->precision = precision;

	return a;
};
static void divby2( AP A, AP C, Flags extra ) {

	DIV( A, AP2, C, extra );
}
static unsigned char aplib_initialised = 0;
void InitAPLIB()    {

	if( aplib_initialised==0 )  {

		aplib = (struct aplib_t*)calloc( 1, sizeof(struct aplib_t) );
		aplib_initialised = 1;
	}
	
	AP1 = QuickAP( "1" );
	AP0 = QuickAP( "0" );
    AP2 = QuickAP( "2" );

	aplib->get_opcode = get_opcode;

	aplib->FreeAP = FreeAP;
	aplib->QuickAP = QuickAP;
	aplib->NewAP = NewAP;
	aplib->QuickAP = QuickAP;
	aplib->SimpleAP = SimpleAP;
	aplib->BlankAP = BlankAP;
	aplib->CopyAP = CopyAP;

	aplib->floor = floor_ap;
	aplib->mul = MUL;
	aplib->divide = DIV;
	aplib->bxn = BXN;
	aplib->add = ADD;
	aplib->sub = SUB;
	//aplib->LOG = LOG;
	aplib->logb = LOGb;
	aplib->root = Root;
	aplib->divby2 = divby2;

    aplib->swapsign = swapsign;
    
    ops[ OPADD ] = aplib->add;
    ops[ OPSUB ] = aplib->sub;
    ops[ OPMUL ] = aplib->mul;
    ops[ OPDIV ] = aplib->divide;
    ops[ OPLOG ] = aplib->logb;
    ops[ OPSQROOT ] = aplib->root;

    spare_extra = (int*)malloc( sizeof(int) );
    init_planck();
	aplib_initialised = 1;
	return;
}

void DeInitAPLIB()  {

	aplib_initialised = 0;
	free( aplib );
}

static void FreeAllConstants( ) {

    free( spare_extra );

	FreeAP( AP0 );
	FreeAP( AP1 );
	FreeAP( AP2 );
	FreeAP( AP10 );
	FreeAP( e );
	FreeAP( AP_PI );
	FreeAP( FGC );
	FreeAP( GR );
	FreeAP( AP3 );
	FreeAP( AP16 );
	FreeAP( AP1_2 );
	FreeAP( AP1_3 );
	FreeAP( AP1_4 );
	FreeAP( AP1_5 );
	FreeAP( AP3_4 );
	FreeAP( AP4_5 );
	FreeAP( AP2_3 );
	FreeAP( AP1_7 );
	FreeAP( AP2_7 );
	FreeAP( AP22_7 );

	return;
}

