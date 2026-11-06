
#include "lean_string.h"
#include "colour.h"


#define ASCII_NULL (char)0
#define ANSI_ISO_ESC "\033"

static char* FULL_STRING = ANSI_ISO_ESC "[30m" ANSI_ISO_ESC "[31m" ANSI_ISO_ESC "[32m" ANSI_ISO_ESC "[33m" ANSI_ISO_ESC "[34m" \
ANSI_ISO_ESC "[35m" ANSI_ISO_ESC "[36m" ANSI_ISO_ESC "[37m";

static char NORMAL[8];

static char* FG_BLACK;
static char* FG_RED;
static char* FG_GREEN;
static char* FG_YELLOW;
static char* FG_BLUE;
static char* FG_MAGENTA;
static char* FG_CYAN;
static char* FG_WHITE;
static char* MOSTRECENT;


static char* FG_BRIGHT_BLACK;
static char* FG_BRIGHT_RED;
static const char* FG_BRIGHT_GREEN;
static const char* FG_BRIGHT_YELLOW;
static const char* FG_BRIGHT_BLUE;
static const char* FG_BRIGHT_MAGENTA;
static const char* FG_BRIGHT_CYAN;
static const char* FG_BRIGHT_WHITE;

static const char* BG_BLACK;
static const char* BG_RED;
static const char* BG_GREEN;
static const char* BG_YELLOW;
static const char* BG_BLUE;
static const char* BG_MAGENTA;
static const char* BG_CYAN;
static const char* BG_WHITE;

static char* BG_BRIGHT_BLACK;
static char* BG_BRIGHT_RED;
static char* BG_BRIGHT_GREEN;
static char* BG_BRIGHT_YELLOW;
static char* BG_BRIGHT_BLUE;
static char* BG_BRIGHT_MAGENTA;
static char* BG_BRIGHT_CYAN;
static char* BG_BRIGHT_WHITE;


static char* EXTRA_COLOUR_TAGS;
static char* RAINBOW;

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

char* modes_codes[64];


static char* fmt( char* in );
static char* fg( char* cn );
static char* bg( char* cn );
static char* getcolourcode( char* colourname );
static char* getcolourcode_bg( char* colourname );
static char* wrap( char* str, char* opts );


static char* fg( char* cn )	{

	if( cn != (char*)0 )
		fg_selection = getcolourcode( cn );

	return fg_selection;
}

static char* getcolourcode_bg( char* cn );

static char* bg( char* cn )	{

	if( cn != (char*)0 )
		bg_selection = getcolourcode_bg( cn );

	return bg_selection;
}


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
		return FG_CYAN;

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

static char* getcolourcode_bg( char* colourname )	{

	if( lean_strcmp( "white", colourname )) 
		return BG_WHITE;
	
	if( lean_strcmp( "red", colourname ) )
		return BG_RED;

	if( lean_strcmp( "green", colourname )) 
		return BG_GREEN;

	if( lean_strcmp( "yellow", colourname )) 
		return BG_YELLOW;

	if( lean_strcmp( "blue", colourname )) 
		return BG_BLUE;

	if( lean_strcmp( "magenta", colourname )) 
		return BG_MAGENTA;

	if( lean_strcmp( "cyan", colourname )) 
		return BG_CYAN;

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
	
	modes_codes[ success ] = FG_GREEN;
	modes_codes[ info ] = FG_BLUE;
	modes_codes[ warning ] = FG_YELLOW;
	modes_codes[ error ] = FG_RED;
	modes_codes[ advice ] = FG_CYAN;
	modes_codes[ fmt_filename ] = FG_YELLOW;
	modes_codes[ fmt_dirpath ] = FG_YELLOW;

	return (struct colour_t*) &colour_api;
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
			lean_strcat( out, (char*)t );
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


#ifndef srand
void srand( unsigned int seed )	{

	while( seed )	{

		rand();
		seed -= (seed>>1);
	}

	return;
}
#endif

#ifndef rand
int rand()	{

	/* A canonical implementation returns a value between [0,RANDMAX]. This is a toy impl. for easy drop-in. */ 
	static int x = 1;
	static int y = 0;

	x *= ( x << 1 ) - y;
	y += ( x >> 2);

	return x;
}
#endif 

static void selectRandomColourString( char* buf )  {
    

    srand(0);
    short rnd_num = (short)(rand() % 8);
    char* selection = (char*)0;
    
    switch( rnd_num )   {
        
        case 0:
            selection = "brightblack";
            break;
            
        case 1:
            selection = "brightred";
            break;
            
        case 2:
            selection = "brightgreen";
            break;
            
        case 3:
            selection = "brightyellow";
            break;
            
        case 4:
            selection = "brightblue";
            break;
            
        case 5:
            selection = "brightmagenta";
            break;
            
        case 6:
            selection = "brightcyan";
            break;
            
        case 7:
            selection = "brightwhite";
            break;
            
        
        default:
            selection = "brightblack";
            break;

    }
    
    lean_strcpy( buf, selection );
    
    return;
}

#ifndef size_t
#define size_t long unsigned int 
#endif 

extern void* malloc( size_t num );
extern void* calloc( size_t num, size_t size );

static char* wrap( char* str, char* opt )	{
	
	int strlen_str = lean_strlen(str);
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
		
		len = strlen_str+lean_strlen(opt)+lean_strlen("[reset]")+2;
		_ = (char*)malloc( len+1 );
		_[0] = 0;

		lean_strcat( _, "[" );
		lean_strcat( _, opt );
		lean_strcat( _, "]" );
		lean_strcat( _, str );
		lean_strcat( _, "[reset]" );

	}

	return _;
}


