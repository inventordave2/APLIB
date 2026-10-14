/* APLIB_H */

#ifndef APLIBI_H
#define APLIBI_H

extern void InitAPLIB();
extern void DeInitAPLIB();


typedef signed long long Precision;
typedef int* Flags;

typedef struct ap	{

	char* wholepart;
	char* fractpart;
	char* desc;
	Precision precision;

	int type; // Integer, Real, Radians, Degrees, etc...
	int base;

	char sign;

} ap;
typedef struct ap ap_obj;
typedef ap_obj* AP;

typedef void (*aplib_operator)( AP A, AP B, AP* C, Flags extra );
extern aplib_operator ops[ 256 ];
extern aplib_operator operator;

typedef struct aplib_t {

	Precision precision;
	int default_base;

	AP (*SimpleAP)( char* val );
	AP (*QuickAP)( char* str );
	AP (*NewAP)( char* wholepart, char* fractpart, char* sign, int* Base, signed long long int Precision );
    AP (*CopyAP)( AP A, Flags extra );
	void (*FreeAP)( AP A );
	AP (*BlankAP)();
	char*(*PrintAP)( AP A );

	int (*getBase)(  AP A );
	void (*setBase)(  AP A, int Base );
	char (*getSign)(  AP A );
	void (*setSign)(  AP A, char* s );
    void (*swapsign)( AP A );
    
	Precision (*getPrecision)(  AP A );
	void (*setPrecision)(  AP A, Precision P );

	int (*get_opcode)( char* );

	void (*add)( AP A, AP B, AP* C, Flags extra );
	void (*sub)( AP A, AP B, AP* C, Flags extra );
	void (*mul)( AP A, AP B, AP* C, Flags extra );
	void (*divide)( AP A, AP B, AP* C, Flags extra );
	void (*bxn)( AP A, AP B, AP* C, Flags extra );
	void (*divby2)( AP A, AP* C, Flags extra );
	void (*reciprocal)( AP A, AP B, AP* C, Flags extra );

	void (*power)( AP A, AP B, AP* C, Flags extra );
	void (*root)( AP A, AP B, AP* C, Flags extra );

	void (*log10)( AP A, AP* C, Flags extra );
	void (*log2)( AP A, AP* C, Flags extra );
	void (*log16)( AP A, AP* C, Flags extra );
	void (*nlog)( AP A, AP* C, Flags extra );
	void (*logb)( AP A, AP B, AP* C, Flags extra );

	signed short (*cmpdigitstr)( char* dstr1, char* dstr2 );
	signed short (*cmpap)( AP A, AP B, AP* C, Flags extra );
	void (*diff)( AP A, AP B, AP C, Flags extra );

    void (*floor)( AP A, AP* C, Flags extra );

	void (*cos)( AP R, AP* C, Flags extra );
	void (*sine)( AP R, AP* C, Flags extra );
	void (*tan)( AP R, AP C, Flags extra );
	//void (*PI)( Precision P, Flags extra ); // if Precision P==0, the Precision is based on the current setting for aplib->Precision


} aplib_t;
extern struct aplib_t* aplib;

#define OPMUL 1
#define OPADD 2
#define OPSUB 3
#define OPBXN 4
#define OPDIV 5
#define OPSQROOT 6
#define OPLOG 7
#define OPSINE 8 
#define OPCOS 9 
#define OPTAN 10 


#ifndef real
#define real double
#endif
#ifndef PIE
#define PIE PI
#endif
#define ap_bool unsigned char

#define NULLAP(A) (A = (AP)0)

typedef struct rational {

	int n;
	int m;

} rational;
typedef struct ap_rational  {
    
    AP n;
    AP m;
} ap_rational;

typedef struct result {

	double r;
	struct rational n_m;

} result;
typedef struct ap_result    {
    
    AP r;
    struct ap_rational n_m;
} ap_result;

// Extra helper objects.
#include "./aplib_constants.h"

/* 'nostdlib' (0-dependency) heap allocate/free function signatures. */
extern void* (*malloc_fnc)( int size );
extern void* (*calloc_fnc)( int numentries, int size );
extern void (*heap_free_alloc_fnc)( void* ep );

extern void* register_heap_allocator( void*(*f)(int) );
extern void* register_heap_callocator( void*(f)(int, int) );
extern void* register_heap_alloc_free( void(*_free)(void*) );

#endif
