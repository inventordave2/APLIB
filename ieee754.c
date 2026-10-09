
#include <stdlib.h>
#include <string.h>
#include "lean_string.h"

char* IEEE_readFloat( float f )	{
	
	float * tc = malloc( sizeof(float) ) ;
	
	*tc = f;
	
	char * offset = (char *) tc;
	char * str = malloc( (sizeof(float)*8) + 3 + 1 ); // 3 for the in-between spaces, 1 for the null-terminator of the string.
	// float := 64 + 7 + 1
	
	int ptr = 0;

	for(int i=0; i<sizeof(float); i+=1 )	{ // iterate through each of the 4 bytes of the float. (change to 8 when parsing a float.)
		
		for(int k=7; k>=0; k-=1 )	{ // iterate from the MSB (bit 7), to the LSB (bit 0) of an individual byte of the memory.
			
			int r = *(offset+i) & (1 << k); // is bit 'k' of offset pointer set to 1, or not? In other words, is the &-exp TRUE, or FALSE?

			if( r==0 )
				str[ptr++] = '0';
			else
				str[ptr++] = '1';
		}
		
	}
		
	str[ptr] = '\0';
	
	return str;

}
void IEEE_writeFloat(float* dest, char* str)	{
	
#include <assert.h>

	assert( strlen(str)==(sizeof(float)*8) );
	
	char* mem = (char *)dest;
	unsigned char bitmask;
	int offset = 0;
	
	for( int i=0; i<sizeof(float); i+=1 )	{

		bitmask = 0;

		for( int k=0; k<8; k++ )
			bitmask += (str[offset+k] - '0') << (7 - k);

		(mem[i]) = bitmask;
        offset += 8;

	}
}

char* IEEE_readDouble( double f )	{
	
	double * tc = malloc( sizeof(double) ) ;
	
	*tc = f;
	
	char * offset = (char *) tc;
	char * str = malloc((sizeof(double)*8) + 1); // 3 for the in-between spaces, 1 for the null-terminator of the string.
	// float := 64 + 7 + 1
	
	int ptr = 0;

	for(int i=0; i<sizeof(double); i+=1 )	{ // iterate through each of the 4 bytes of the float. (change to 8 when parsing a float.)
		
		for(int k=7; k>=0; k-=1 )	{ // iterate from the MSB (bit 7), to the LSB (bit 0) of an individual byte of the memory.
			
			int r = *(offset+i) & (1 << k); // is bit 'k' of offset pointer set to 1, or not? In other words, is the &-exp TRUE, or FALSE?

			if( r==0 )
				str[ptr++] = '0';
			else
				str[ptr++] = '1';
		}
		
	}
		
	str[ptr] = '\0';
	
	return str;
}
void IEEE_writeDouble(double* dest, char* str)	{
	
#include <assert.h>

	assert( strlen(str)==(sizeof(double)*8) );
	
	char* mem = (char *)dest;
	unsigned char bitmask;
	int offset = 0;
	
	for( int i=0; i<sizeof(double); i+=1 )	{

		bitmask = 0;

		for( int k=0; k<8; k++ )
			bitmask += (str[offset+k] - '0') << (7 - k);

		(mem[i]) = bitmask;
        offset += 8;

	}
}

// BITWISE FNCS //
// THE FUNCTION BELOW ADDS 1 TO A BINARY STRING
char* INCB( char* bstr ){

	int i = lean_strlen( bstr );
	
	//char align = i % 8;
	// if 0, the bit-string is byte-aligned.
	// otherwise, align contains the 8-p misalignment.
	// (8-align) leading 0's would need to be padded.
	
	char* _ = (char*) malloc( i+1 );
	_[0] = '0';

	char* _copy = _;

	for( int k=0; k<i; k++ )	{

		++_copy;
		*_copy = bstr[k];
	}


	// if odd (bit 0 = 0), flip bit to add 1.
	if( *_copy=='0' )	{

		*_copy = '1';
		++_copy;
		*_copy = (char)0;

		return _;	
	}
	
	while( *_copy=='1' )	{
		
		*_copy = '0';
		--_copy;			
	}
	*_copy = '1';

	return _;
}
// THE FUNCTION BELOW FLIPS (INVERTS) A BIT-STRING
char* FLIPB( char* bstr ){

	unsigned long long dcount = lean_strlen(bstr);
	char* _ = (char*) malloc( dcount+1 );
	
	// invert inbound bitstring
	unsigned long long i;
	for( i=0; i<dcount; i++ )
		if( bstr[i]=='1' )
			_[i] = '0';
		else
			_[i] = '1';
	
	_[i] = '\0';
	
	return _;
}
void FLIPBITS_STR( char* bstr )	{

	while( *bstr )	{

		if( *bstr=='0' )
			*bstr='1';
		else if( *bstr=='1')
			*bstr='0';

		++bstr;
	}

	return;
}
// THIS FUNCTION CALCULATES THE 2'S COMPLEMENT OF A BIT-STRING.
// DEP: FLIPB( char* ), INCB( char* )
char* _2sComplement_str( char* bstr )	{

	char* swap = (char*)NULL;
	
	char* _ = (char*)malloc( lean_strlen(bstr)+1 );
	lean_strcpy( _, bstr );

	FLIPBITS_STR( _ );

	swap = _;
	_ = INCB( swap );

	free( swap );
	
	#ifdef DIAGNOSTICS
		printf( "2's-Complement of %s is: %s\n", bstr, _ );
	#endif

	return _;
}

char* IEEE_convertDoubleString2BigEndian( char* str )	{
	
	char * be_string = malloc(64 + 1) ;
	
	int offset = 0;
	
	int i;

	for( i=63; i<63+8; i++ )
		be_string[offset++] = str[i];
	
	//be_string[offset++] = ' ';

	for( i=54; i<54+8; i++ )
		be_string[offset++] = str[i];
	
	//be_string[offset++] = ' ';
	
	for( i=45; i<45+8; i++ )
		be_string[offset++] = str[i];
	
	//be_string[offset++] = ' ';

	for( i=36; i<36+8; i++ )
		be_string[offset++] = str[i];
	
	//be_string[offset++] = ' ';
	
	for( i=27; i<27+8; i++ )
		be_string[offset++] = str[i];
	
	//be_string[offset++] = ' ';
	
	for( i=18; i<18+8; i++ )
		be_string[offset++] = str[i];
	
	//be_string[offset++] = ' ';
	
	for( i=9; i<9+8; i++ )
		be_string[offset++] = str[i];
	
	//be_string[offset++] = ' ';
	
	for( i=0; i<8; i++ )
		be_string[offset++] = str[i];
	
	//be_string[offset] = '\0';
	
	return be_string;
}
char* IEEE_convertFloatString2BigEndian( char* str )	{
	
	char * be_string = (char*)malloc(32 + 1);
	
	int offset = 0;
	
	int i;
	
	for( i=24; i<32; i++ )
		be_string[offset++] = str[i];
	
	//be_string[offset++] = ' ';
	
	for( i=16; i<24; i++ )
		be_string[offset++] = str[i];
	
	//be_string[offset++] = ' ';
	
	for( i=8; i<16; i++ )
		be_string[offset++] = str[i];
	
	//be_string[offset++] = ' ';
	
	for( i=0; i<8; i++ )
		be_string[offset++] = str[i];
	
	be_string[offset] = '\0';
	
	return be_string;
}
