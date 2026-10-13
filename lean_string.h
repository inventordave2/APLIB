/* LEAN_STRING_H */
/* An iteration of the lean_string.h header-only library. */

#ifndef LEAN_STRING_H
#define LEAN_STRING_H

#include <stdlib.h>

/* These 3 varchar defines are not yet utilized. */
#define glyph char
#define u_string glyph*
#define next_sym( str, x )  (u_string) str [ x++ ]

/* FNC DECLARATIONS. */

/* This needs to be called if the lean_tray needs to be initialised. lean_strdup(s) requires it. */
static void init_lean_string();
static unsigned char lean_tray_initialised = 0;

static char* lean_strdup( char* s2 );
static void lean_strcat( char*, char* );
static void lean_strcpy( char* to, char* from );
static void lean_substring( char*, char*, int, int );
static char* lean_strtouppercase( char* );
static char* lean_strtolowercase( char* );
static unsigned int lean_strcmp( char*, char* );
static unsigned long long int lean_strlen( char* );

/* Helper functions. */

/* Compare 2 digit-strings for similarity. Returns one of the following: { -1, 0, +1 } */
static signed int cmpdstr( char* lhs, char* rhs );
static signed int cmpdstr( char* lhs, char* rhs ) {

    if( lhs==(char*)0 )
        lhs = "0";
        
    if( rhs==(char*)0 )
        rhs = "0";

	char lhs_sign = '+';
	char rhs_sign = '+';

	// Extract and skip signs
	if( *lhs == '+' || *lhs == '-' ) {
		lhs_sign = *lhs;
		++lhs;
	}

	if( *rhs == '+' || *rhs == '-' ) {
		rhs_sign = *rhs;
		++rhs;
	}

	// Different signs: positive > negative
	if( lhs_sign != rhs_sign ) {
		return (lhs_sign == '+') ? +1 : -1;
	}

	// Skip leading zeros
	while( *lhs == '0' )
		++lhs;

	while( *rhs == '0' )
		++rhs;

	// Find end of both strings (length scan)
	char* lhs_end = lhs;
	char* rhs_end = rhs;

	while( *lhs_end != '\0' )
		++lhs_end;

	while( *rhs_end != '\0' )
		++rhs_end;

	unsigned int lhs_len = lhs_end - lhs;
	unsigned int rhs_len = rhs_end - rhs;
	signed int len = (signed int) lhs_len - rhs_len;

	// Different lengths: longer wins (or loses if negative)
	if( len != 0 ) {

		if( len>0 )
			return +1;

		return -1;
	}

	// Same length: character-by-character comparison
	while( lhs < lhs_end ) {

		if( *lhs != *rhs ) {

			if( *lhs > *rhs )
				return +1;
			else
				return -1;

		}

		++lhs;
		++rhs;
	}

	return 0;
}


/* These 3 helper functions are for widestring buffer scanning. */
static unsigned int base2_native_shift( unsigned int size );
#define lean_strlen_ustring( str ) (unsigned long long) lean_strlen(str) >> base2_native_shift(sizeof(glyph))
static void lean_utf8_str_to_ascii( char*, char* );

/* Helper functions for converting between a digit-string and a Number type (int, float). */
static char* lean_itoa( int a );
static char* lean_ftoa( float a );
static int lean_atoi( char* a );
static float lean_atof( char* f );

/* Simple utility function for shifting off any leading-zero padding (ptr stays unchanged). */
static void shift_left_leading_zeroes( char* A );

/* ANSIVT16 COLOUR SUPPORT */
#ifndef LEAN_COLOUR_H
static char* colourlib_stub( char* str );
static char* colourlib_stub( char* str )    {
    
    return str;
}
static char* (*colour_fmt)( char* ) = colourlib_stub;
#else
static char* (*colour_fmt)( char* ) = colour->fmt;
#endif

/**/
/*
FNC DEFINITIONS.
*/
static void lean_substring( char* in, char* out, int start, int end )	{

	char ch = in[end+1];
	in[end+1] = '\0';
	in += start;

	if( out==(char*)0 )	{

		// get from static lean_heap
	}


	lean_strcpy( out, in );

	in -= start;
	in[end+1] = ch;

	return;
}

static void shift_left_leading_glyph( u_string str, glyph sym )    {

	char* _ = (char*)str;
	unsigned long long int len__ = lean_strlen( _ );
	char* glyph_buf = (char*)calloc( len__+1, sizeof(glyph) );
	char* sym_buf = (char*)calloc( 2,sizeof(glyph) );
	sym_buf[0] = sym;

	unsigned long long int x = 0;

	while( _ < (char*)(&str)+len__ )   {

		lean_substring( glyph_buf, _, x, sizeof(glyph) );

		if( cmpdstr( glyph_buf, sym_buf )==0 )  {

			//shl (char*) str by sizeof(glyph)

			char* start = _;
			unsigned long long int x = sizeof(glyph);
			char* temp = _ + x;
			unsigned long long y = 0;

			while( y<x ) {

				*start = *(temp+y);
				++start;
				++y;
			}
		}

		_ += sizeof( glyph );

	}

	return;
}

// Helper function for widestring buffer scanning.
static unsigned int base2_native_shift( unsigned int size )  {

	if( size==0 || size==1 )
		return 0;

	unsigned int shift = 0;
	unsigned int delta = 2;
	unsigned int iteration = 1;

	while( size>iteration )   {

		if( iteration^1 )   { // Only increase delta (2kform) when iteration is Even ( iteration % 2 == 0)

			if( delta==iteration )   {

				shift += 1;
				delta *= 2;
			}
		}

		++iteration;
	}

	return shift;
}

static int lean_atoi( char* a )    {

	if( a==(char*)0 )
		return 0;

	short len_a = lean_strlen( a );

	if( len_a > sizeof(int) )
		return 2147483647;

	if( len_a == sizeof(int) )  {

		if( cmpdstr(a, "2147483647")>0 )
			return 2147483647;
	}


	char* a_copy_ref = a;

	int result = 0;
	int d = 1;


	a += (len_a-1);

	while( ! (a < a_copy_ref) ) {

		result += ((*a)-'0') * d;
		d *= 10;
		--a;
	}

	return result;
}
static char* lean_itoa( int a )    {

	int strlen_max32int = lean_strlen("2147483647");
	char* r = (char*)malloc( strlen_max32int+1 );

	if( a > 2147483647 )    {

		r[0] = '0';
		r[1] = 0;
		return r;
	}

	char* _ = r+strlen_max32int-1;

	while( (unsigned)a )  {

		*_ = (a % 10);
		a = (int)a/10;

		--_;
	}

	while( _ > r )  {

		*_ = '0';
		--_;
	}

	shift_left_leading_zeroes( r );

	return r;
}
static char* lean_ftoa( float a )    {

	char* fstr = "3402823470000000000000000000000000000";
	int strlen_max32float = lean_strlen( fstr );
	char* r = (char*)malloc( strlen_max32float+1 );

	if( a > (340282347.0) )    {

		r[0] = '0';
		r[1] = 0;
		return r;
	}

	char* _ = r+strlen_max32float-1;

	while( a>0 )  {

		*_ = ((int)a % 10);
		a = (a/10);

		--_;
	}

	while( _ > r )  {

		*_ = '0';
		--_;
	}

	shift_left_leading_zeroes( r );

	return r;
}

static char* lean_clipstring( char* str1, unsigned long long len ) {

	if( str1 == (char*)0 )
		return str1;


	char* _ = (char*)malloc( len+1 );
	char* backup = _;

	unsigned long long int strlen_str1 = lean_strlen(str1);
	unsigned long long int strlen_str1_bkp = strlen_str1;

	if( strlen_str1 < len )   {

		unsigned long long int new_len = len - strlen_str1;

		while( strlen_str1 )   {

			*_ = '0';
			--strlen_str1;
			++_;
		}

		len = new_len;
	}

	while( len )  {

		*_ = *str1;
		++_;
		++str1;
		--len;
	}

	*_ = '\0';
	return backup;
}
static void ASCII_DIGIT_ZERO( char* dstr, unsigned long long len ) {

	do  {

		--len;
		dstr[len] = '0';

	} while( len );

	return;
}
static void shift_left_leading_zeroes( char* A )	{

	if( A == (char*)0 )
		return;

	char* copy_ptr = A;

	if( *A=='\0' )
		return;

	while( *A=='0' )
		++A;

	if( *A=='\0' )
		--A;

	if( A == copy_ptr )
		return;

	do	{

		*copy_ptr = *A;
		++copy_ptr;
		++A;
	} while( *A != '\0' );

	*copy_ptr = '\0';

	return;
}

/**
Below are the current implementations of the lean_string_h library functions.
*/
static char* lean_strtolowercase( char* in ) {

	char* out = in;
	char ch = '\0';

	static char delta = 'a'-'A';

	while( (ch=*in) )    {

		if( ch >= 'A' && ch <= 'Z' )    {

			*in += delta;
		}

		++in;
	}

	return out;
}
static char* lean_strtouppercase( char* in ) {

	char* out = in;
	char ch = '\0';

	static char delta = 'a'-'A';

	while( (ch=*in) )    {

		if( ch >= 'a' && ch <= 'z' )    {

			*in -= delta;
		}

		++in;
	}

	return out;
}
static unsigned long long lean_strlen( char* str )	{

	if( str==(char*)0 )
		return (unsigned long long) 0;

	unsigned long long x = 0;

	while( *str )	{

		++x;
		++str;
	}

	return x;
}
static unsigned lean_strcmp( char* s1, char* s2)	{

	if( s1==(char*)0 || s2==(char*)0 )
		return 0;

	while( *s1 || *s2 )	{

		if( *s2!=*s1 )
			return 0;

		++s1;
		++s2;
	}

	return 1;
}
static void lean_strcpy( char* in, char* from )	{

	if( in==0 )
		return;

	if( from==0 )	{

		*in = '\0';
		return;
	}

	while( *from )	{

		*in = *from;
		++in;
		++from;
	}

	*in = '\0';

	return;
}
static void lean_strcat( char* str1, char* str2 )	{

	if( str1==(char*)0 || str2==(char*)0 )
		return;


	unsigned long long strlen_str1 = lean_strlen( str1 );

	char* str1_offset = (char*)(str1 + strlen_str1);

	lean_strcpy( str1_offset, str2 );

	return;
}

#endif
