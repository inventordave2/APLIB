
#include "lean_string.h"
#include "colour.h"


#define ASCII_NULL (char)0
#define ANSI_ISO_ESC (char)27

const char* FULL_STRING = ANSI_ISO_ESC "[30m" ANSI_ISO_ESC "[31m" ANSI_ISO_ESC "[32m" ANSI_ISO_ESC "[33m" ANSI_ISO_ESC "[34m" \
ANSI_ISO_ESC "[35m" ANSI_ISO_ESC "[36m" ANSI_ISO_ESC "[37m";

static char NORMAL[16];

static char* FG_BLACK = FULL_STRING+0;
static char* FG_RED = FULL_STRING+8;
static char* FG_GREEN = FULL_STRING+16;
static char* FG_YELLOW = FULL_STRING+24;
static char* FG_BLUE = FULL_STRING+32;
static char* FG_MAGENTA = FULL_STRING+40;
static char* FG_CYAN = FULL_STRING+48;
static char* FG_WHITE = FULL_STRING+56;
static char* MOSTRECENT = FG_WHITE;



static char* FG_BRIGHT_BLACK = FULL_STRING+64;
static char* FG_BRIGHT_RED = FULL_STRING+72;
static char* FG_BRIGHT_GREEN = FULL_STRING+;
static char* FG_BRIGHT_YELLOW;
static char* FG_BRIGHT_BLUE;
static char* FG_BRIGHT_MAGENTA;
static char* FG_BRIGHT_CYAN;
static char* FG_BRIGHT_WHITE;

static char* BG_BLACK;
static char* BG_RED;
static char* BG_GREEN;
static char* BG_YELLOW;
static char* BG_BLUE;
static char* BG_MAGENTA;
static char* BG_CYAN;
static char* BG_WHITE;

static char* BG_BRIGHT_BLACK = BG_WHITE+9;
static char* BG_BRIGHT_RED = BG_WHITE+18;
static char* BG_BRIGHT_GREEN = BG_WHITE+27;
static char* BG_BRIGHT_YELLOW = BG_WHITE+36;
static char* BG_BRIGHT_BLUE = BG_WHITE+45;
static char* BG_BRIGHT_MAGENTA = BG_WHITE+54;
static char* BG_BRIGHT_CYAN = BG_WHITE+63;
static char* BG_BRIGHT_WHITE = BG_WHITE+72;

static char* BOLD;
static char* ITALIC;
static char* UNDERLINE;
static char* STRIKETHROUGH;


static long int flags = 0; // Assume to be 32 bits, so 4 addressable bytes.
static char* bg_selection = (char*)0;
static char* fg_selection = (char*)0; 

#define info 1
#define warning 2
#define error 3
#define advice 4
#define fmt_filename 5
#define success 6
#define fmt_dirpath 7
#define website_url 8

static char** modes_codes[16];
modes_codes[ success ] = FG_GREEN;
modes_codes[ info ] = FG_BLUE;
modes_codes[ warning ] = FG_YELLOW;
modes_codes[ error ] = FG_RED;
modes_codes[ advice ] = FG_CYAN;
modes_codes[ fmt_filename ] = FG_YELLOW;
modes_codes[ fmt_dirpath ] = FG_YELLOW;

static char* fmt( char* in );
static char* fg( char* cn );
static char* bg( char* cn );
static char* getcolourcode( char* colourname );
static char* wrap( char* str, char* opts );


static char* getcolourcode( char* colourname )	{

	if( lean_strcmp( "white", colourname )) 
		return FG_WHITE;
	
	if( lean_strcmp( "red", colourname ) )
		return FG_RED;

	if( lean_strcmp( "green", colourname )) 
		return FG_GREEN;

	if( lean_strcmp( "yellow", colourname )) 
		return FG_YELLOW;

	if( lean_strcmp( "blue", colourname )) 
		return FG_BLUE;

	if( lean_strcmp( "magenta", colourname )) 
		return FG_MAGENTA;

	if( lean_strcmp( "cyan", colourname )) 
		return FG_WHITE;

	if( lean_strcmp( "bold", colourname ))
		return BOLD;

	if( lean_strcmp( "underline", colourname ))
		return UNDERLINE;

	if( lean_strcmp( "italic", colourname ) || lean_strcmp( "italics", colourname ))
		return ITALIC;

	if( lean_strcmp( "strikethrough", colourname ) || lean_strcmp( "score", colourname ))
		return STRIKETHROUGH;

	return NORMAL;
}

struct colour_t colour_api;

struct colour_t* get_colour_api()	{

	colour_api.fg = fg;
	colour_api.bg = bg;
	colour_api.fmt = fmt;
	colour_api.wrap = wrap;
	colour_api.getcolourcode = getcolourcode;
	
	return (struct colour_t*) colour_api;
}


static char* fmt( char* in )	{
	
	unsigned x = 0;
	unsigned y = 0;
	unsigned w = 0;
	
	short escaped = 0;
	char c = 0;
	char* out = (char*)calloc( 1024, 1 );
	short detected_seq = 0;
	char tbuf[2];
	tbuf[1] = '\0';
	char _[200] = { 0 };

	while( (c = in[x]) )	{
		
		++x;
		
		if( c=='\\' && !escaped )	{
			
			escaped = 1;
			continue;
		}
		if( c=='\\' && escaped )	{
			
			escaped = 0;
			out[x-1] = '\\';
			out[x] = '\\';
			continue;
		}
		if( c=='\e' )	{
			
			out[ x-1 ] = c;
			escaped = 1;
			continue;
		}
		
		char** seq;
		
		unsigned z = 0;

		if( escaped )	{
			
			char t[2];
			t[0] = c;
			t[1] = '\0';
			strcat( out, (char*)t );
			escaped = 0;
			continue;
		}
		else if( c=='[' )	{

			detected_seq = 1;
			
			seq = (char**)malloc( 10 * sizeof(char*) );
			z=0;

			char ch;
			y=0;

			while( (ch=in[x]) != ']' && ch!='\0' )	{
				
				_[y] = ch;
				++x;
				++y;
			}
			
			if( ch==']' )
				++x;
			
			_[y] = '\0';
			
			char* entry = (char*)calloc( 128, 1 );
			char* entry_copy;
			
			w = 0;
			
			loop: 
			
			entry_copy = entry;
			while( (ch=_[w]) != ',' && ch != '\0' )	{

				*entry_copy = ch;
				++w;
				++entry_copy;
			}

			*entry_copy = '\0';
				
			seq[z] = getcolourcode( entry );

			++z;
			if( ch == ',' )	{
				
				w++;
				goto loop;
			}
			free( entry );
		}

		if( z>0 )	{
			
			unsigned n = 0;
			// attach the seq[] array of char* control codes to the 'char* out' buffer.
			while( n < z )
				lean_strcat( out, seq[n++] );						
		}
		else	{

			tbuf[0] = c;
			lean_strcat( out, tbuf );
		}
				
		if( detected_seq )	{
			
			free( seq );
			detected_seq = 0;
		}

	}
	
	return out;
}

static char* wrap( char* str, char* opt )	{
	
	int strlen_str = strlen(str);
	int len;
	char* _;
	
	#include <string.h>
	if( !strcmp(opt,RAINBOW) )	{
		
		// Rainbow mode.
		len = strlen_str + (16*strlen_str ) + 1;
		_ = (char*)malloc( len+1 );
		_[0] = 0;
		char* cstr = (char*)calloc( 2,1 );

		int x = 0;
		while( x<strlen_str )	{
			
			opt = (char*)calloc( 128, 1 );
			selectRandomColourString( opt );

			cstr[0] = str[x++];
			
			strcat( _, "[" );
			strcat( _, opt );
			strcat( _, "]" );
			strcat( _, cstr );
			//strcat( _, "[reset]" );

			free( opt );
		}
		
	}
	else	{
		
		len = strlen_str+strlen(opt)+strlen("[reset]")+2;
		_ = (char*)malloc( len+1 );
		_[0] = 0;

		strcat( _, "[" );
		strcat( _, opt );
		strcat( _, "]" );
		strcat( _, str );
		strcat( _, "[reset]" );

	}

	return _;
}


