#ifndef I754_H
#define I754_H

extern char* IEEE_readFloat( float f );
extern void IEEE_writeFloat(float* dest, char* str);

extern char* IEEE_readDouble( double f );
extern void IEEE_writeDouble(double* dest, char* str);

extern char* IEEE_convertFloatString2BigEndian( char* str );
extern char* IEEE_convertDoubleString2BigEndian( char* str );

extern char* INCB( char* );

extern void FLIPBITS_STR( char* );
extern char* FLIPB( char* );

extern char* _2sComplement_str( char* );

#endif
