#ifndef DAVELIB_COLOUR
#define DAVELIB_COLOUR

#include <stdlib.h>
#include <stdio.h>
#include "./lean_string.h"

#define COLOUR_MAX_CODES_COUNT 256
#define ANSI_ISO_ESC "\033"
#define PORTABLE_COLOUR_H_DIAGNOSTICS

/*
The Colour Library Interface.
Usage:
colour->fmt( char* ); // Pass in a ColourLib-formatted string, and this member function will replace the code hints with the ANSI/VT literals.
*/
typedef struct Colour	{

	const char* desc;

	char colourMode;
	const char codeAlign: 8; /* A fixed width for any of the control code strings.
								With ANSIVT100, for simple usage, a length of 8 (bytes/chars unpacked) suffices. */

	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
	unsigned char RGBA[4];
	const char** codes;

	const unsigned (*resetAnsiVtCodes)(unsigned f);

	int NUMROWS;
	int NUMCOLS;
	int** screen; /* 2d (rows and columns) screen buffer */
	int x; /* column */
	int y; /* row    */
	
	int codepage; /* Such as UTF8, UCS4, ASCII, etc. */
	char mode; /* charmode = 1, bitmapmode = 2, unverified = 0 */

	int (*print)( char* );
    int (*printf)( char*, ... );
    int (*fprintf)( FILE*, char*, ... );

	char* (*wrap)( char*, char* cc );
	char* (*fmt)( char* in );
	char* (*setcodevalues)( int c, int v, ... );
	char* (*reset)();
	void (*bg)( uint8_t cc );
	void (*fg)( uint8_t cc );
	void (*bold)();
	void (*b)();
	void (*italic)();
	void (*i)();
	void (*underline)();
	void (*ul)();

} Colour;

/*
An instance of the Colour Library Interface.
Initialised by InitColour().
*/
struct Colour* colour;

/*
The only public (non-interface-wrapped) function to the Colour Library.
Must be called before the Colour Interface can be used, as the Init function initialises the Library runtime requirements.
*/
void InitColour();
void DeInitColour();

/*
These are global copies of the strings representing the ANSI/VT control code sequences.
Populated after InitColor() has been called, and optionally accessible as globals,
they can also be accessed via the Interface colour.codes[] Array.
*/
char* FG_BLACK;
char* FG_RED;
char* FG_GREEN;
char* FG_YELLOW;
char* FG_BLUE;
char* FG_MAGENTA;
char* FG_CYAN;
char* FG_WHITE;

char* FG_BRIGHT_BLACK;
char* FG_BRIGHT_RED;
char* FG_BRIGHT_GREEN;
char* FG_BRIGHT_YELLOW;
char* FG_BRIGHT_BLUE;
char* FG_BRIGHT_MAGENTA;
char* FG_BRIGHT_CYAN;
char* FG_BRIGHT_WHITE;

char* BG_BLACK;
char* BG_RED;
char* BG_GREEN;
char* BG_YELLOW;
char* BG_BLUE;
char* BG_MAGENTA;
char* BG_CYAN;
char* BG_WHITE;

char* BG_BRIGHT_BLACK;
char* BG_BRIGHT_RED;
char* BG_BRIGHT_GREEN;
char* BG_BRIGHT_YELLOW;
char* BG_BRIGHT_BLUE;
char* BG_BRIGHT_MAGENTA;
char* BG_BRIGHT_CYAN;
char* BG_BRIGHT_WHITE;

char* NORMAL;

char* BOLD;
char* ITALIC;
char* UNDERLINE;
char* STRIKETHROUGH;

char* COLOUR; // Allows a palette of 256 colours, 0 to 255.
char* RGB_FG;
char* RGB_BG;

char* DEL;


struct Colour* colour;
void InitColour();
void DeInitColour();


static char colourlib_initialised = 0;
static void InitColourI();

static char* fmt( char* in );
static char* getANSIVTSeq( char* str );
static char* reset()	{ char* str = getANSIVTSeq( "reset" ); printf( str ); return str; }
static char* wrap( char*, char* cc );

static void bg( uint8_t cc );
static void fg( uint8_t cc );
static void bold();

unsigned int _black_;
unsigned int _red_;
unsigned int _green_;
unsigned int _blue_;
unsigned int _yellow_;
unsigned int _magenta_;
unsigned int _cyan_;
unsigned int _white_;
unsigned int _bright_;
unsigned int _reset_;

static char** codes;


static char desc[] = "COLOUR_C comes to you courtesy of Inventor Dave, his primary co-hort the Emeritus Lab Assistant Mopsey Wowbagger, \
the historical ANSI specification committee, and an iron-clad personal commitment by Dave (that's me, but my mates call me \"The Dizzle\", so, y'know, \
it's only polite...) to free noobs and code-kiddies like you losers from the Atlantean shackles of technical debt. \
All of the important subsystems you could want to be wrapped, friction-free, in a plug-in architecture, for any C application project \
your puny mortal minds could possibly envisage, let alone successfully implement, across multiple target architectures (Windows, and Linux. Tbf, \
it's just like, those two, and that's it, but you've gotta keep the energy up).\n(c)\tInventor Dave\n   \t1,000,000 b.c.\n\nOh, and \
WELCOME, TO THE WORLD OF TOMORROW!!!\n";

void lean_memcpy( void* dest, void* src, int size );

static char* setCodePage( char* );
static const char* getCodePage( );
static const unsigned int resetAnsiVtCodes(unsigned f);
static char* reset();

static int print( char* );
static int colour_printf_hook( char*, ... );
static int colour_print_hook( char* str );
static const char* modifiers = "csdf\0";
static int _printf( FILE* file_h, char* args_in, int num_modifiers, char* buff_in );

char* init_colour_codes();


static void print( char* str )   {
    
    printf( str );
   	fflush( stdout );
}

static int seek_modifiers( char* arr1, char cm )    {

    while( *arr1 != '\0' )
        if( *arr1==cm )
            return 1;
        else
            ++arr1;

    return 0;
}

static int count_modifiers( char* _ ) {
 
    int escaped = 0;
    int count = 0;

    while( *_ != '\0' ) {

        if( *_=='%' && !escaped )  {
    
            if( seek_modifiers( (char*)modifiers,*_ ) )    {

                count+=1;
            }
            else if( *_=='\\' ) {
    
                if( !escaped )
                    escaped=1;
                else
                    escaped=0;
            }
        }

        ++_;
    }

    return count;
}

#define convert_arg_to_cstr( Type, v, buf ) lean_substring( buf, (char*)&v, 0, sizeof( Type ) )

static void colour_print_hook( char* buff_in )   {
    
    char* fmtd = colour->fmt( buff_in );
    printf( fmtd );
    free( fmtd );
    
    return;
}


static int _printf( FILE* file_h, char* args, int num_modifiers, char* buff_in )	{
    /*
    	va_list args;
    	memcpy( &args, args_in, sizeof(va_list) );  
    	va_start( args, num_modifiers );
	*/

	char* buff = fmt( buff_in );
	int escaped = 0;
	char* _ = (char*)0;
	char c_[2]; c_[1] = '\0';
	int x = 0;
	int y = 0;

	for( x=0, y=0; x<lean_strlen(buff); x++ )    {
 	
		if( buff[x]=='\\' ) {

			if( !escaped )	{

           		escaped = 1;
				c_[0] = '\\';
				lean_strcat( _,c_ );
				continue;
			}
       		else  {

				escaped = 0;
				c_[0] = '\\';
				lean_strcat( _,c_ );
				continue;
			}

    	}

		if( buff[x]=='%' )  {

       		if( escaped==1 )	{

				escaped = 0;
				c_[0]='%';
				lean_strcat( _,c_ );
				continue;
			}

           	x+=1;

       		switch( buff[x] )   {

           		case 'c':
						y+=1;
						c_[0] = *(char*)get_arg( args, "char");
            
                     	lean_strcat( _,c_ );
                     	break;

           		case 's':
						y+=1;
						lean_strcat( _, (char*)get_arg( args,"char*" ) );
           				break;

           		case 'd':
						y+=1;
						lean_strcat( _, lean_itoa( *(int*)get_arg(args,"int")) );
              			break;

           		case 'f':
						y+=1;
						lean_strcat( _, lean_ftoa(*(float*)get_arg(args,"float")) );
                		break;

				default:
						c_[0] = buff[x];
               			lean_strcat( _, c_ );
                		break;
       			}

		}

        fprintf( file_h, _ );
		free( _ );
		free( buff );

		return y;
	}
	
	return 0;
}


static void bg( uint8_t cc ) { return; }
static void fg( uint8_t cc ) { return; }
static void bold() { return; }

#ifdef EXPECT_CSTDLIB
// DECLS AND DEFS FOR FUNCTIONS THAT WOULD NEED THE CSTDLIB (beyond <stdio.h>) TO BE LINKED

#include <stdarg.h>
#include <time.h>

static int colour_printf_hook( char* buff_in, ... );
static void selectRandomColourString( char* buf );

static int colour_printf_hook( char* buff_in, ... ) {

	int num_modifiers = count_modifiers(buff_in);

	va_list args_;
	char* args_buf_copy = (char*)malloc( sizeof(va_list)+1 );
	memcpy( args_buf_copy, &args_, sizeof(va_list) );

	return _printf( stdin, args_buf_copy, num_modifiers, buff_in );

}

static int colour_fprintf_hook( FILE* file_h, char* buff_in, ... ) {

	int num_modifiers = count_modifiers(buff_in);

	va_list args_;
	
	char* args_buf_copy = (char*)malloc( sizeof(va_list) );
	lean_memcpy( args_buf_copy, &args_, sizeof(va_list) );

	return _printf( file_h, args_buf_copy, num_modifiers, buff_in );
}

static void selectRandomColourString( char* buf )  {
    

    srand(time(NULL));
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

#endif


static char* wrap( char* str, char* opt )	{
	
	int strlen_str = lean_strlen(str);
	int len;
	char* _;
	
	if( lean_strcmp(opt,RAINBOW) )	{
		
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
			
;
			lean_strcat( _, "[" );
			lean_strcat( _, opt );
			lean_strcat( _, "]" );
			lean_strcat( _, cstr );
			//lean_strcat( _, "[reset]" );

			free( opt );
		}
	}
	else	{
		
		len = strlen_str+lean_strlen(opt)+lean_strlen("[reset]")+2;
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


char* getColourCode( char* colour_str ) {
    
    char* _ = (char*)0;

	return _;
}

/*
Some Windows configs require a manual invocation of VT processing.
*/
#ifdef _WIN32
#include <windows.h>
static int color_win32_vt;
int win32_color()	{

	void* StdHandle = GetStdHandle( ((DWORD)-11) );
	color_win32_vt = (int)SetConsoleMode(
		StdHandle,
		0x0001 | 0x0002 | 0x0004
	);

	if(color_win32_vt == 0)
		fprintf( stderr, "ANSIVT mode could not be activated in this Win128 process.\n" );

	return color_win32_vt;	
}
#endif


static char* get_ansivt_seq( char* str )	{

	if( lean_strcmp( str,"reset" ) )
		return (char*)NORMAL;

	char prefix[4];
	prefix[0] = str[0];
	prefix[1] = str[1];
	prefix[2] = str[2];
	
	prefix[3] = '\0';
	
	short bg = 0;
	
	const char** codes = colour->codes;
	char ch;

	int n = 6;
	
	if( lean_strcmp( prefix,"fg:" ) || (bg=(lean_strcmp( prefix,"bg:" ))) )
		str += 3;

	ch = str[n];
	str[n] = '\0';
	
	if( lean_strcmp( str,"bright" ) )
		codes += _bright_, str[n] = ch, str += n;
	else
		str[n] = ch;
	
	if( bg )
		codes += 16;
	
	if( lean_strcmp( str,"white" ) )
		return (char*) codes[_white_];

	if( lean_strcmp( str,"red" ) )
		return (char*) codes[_red_];
	
	if( lean_strcmp( str,"blue" ) )
		return (char*) codes[_blue_];
	
	if( lean_strcmp( str,"green" ) )
		return (char*) codes[_green_];
	
	if( lean_strcmp( str,"yellow" ) )
		return (char*) codes[_yellow_];

	if( lean_strcmp( str,"magenta" ) )
		return (char*) codes[_magenta_];

	if( lean_strcmp( str,"cyan" ) )
		return (char*) codes[_cyan_];
	
	if( lean_strcmp( str,"black" ) )
		return (char*) codes[_black_];

	char* vt100_result = getColourCode( str );
	
	if( lean_strcmp( vt100_result,"" ) )	{
		
		int a = lean_strlen( str );
		char* errmsg = (char*)malloc( a+2+1 );
		lean_strcpy( errmsg, "[" );
		lean_strcat( errmsg, str );
		lean_strcat( errmsg, "]" );

		return errmsg;
	}
	
	return vt100_result;
}

void InitColour()	{
	
	InitColourI();
	return;
}

#define COLOURLIB_CONTROL_CODES_ITINERARY_COUNT 256

static void InitColourI()	{

	#ifdef COLOURMODE
	#if COLOURMODE==0

		colour = (struct Colour*) calloc( sizeof( struct Colour ) );
		return;

	#endif
	#endif

	codes = (char**)calloc( COLOURLIB_CONTROL_CODES_ITINERARY_COUNT, sizeof(char*) );

	FG_BLACK = codes_tray;
	FG_RED = codes_tray+16;
	FG_GREEN = codes_tray+32;
	FG_YELLOW = codes_tray+48;
	FG_BLUE = codes_tray+64;
	FG_MAGENTA = codes_tray+80;
	FG_CYAN = codes_tray+96;
	FG_WHITE = codes_tray+112;

	FG_BRIGHT_BLACK = codes_tray+128;
	FG_BRIGHT_RED = codes_tray+146;
	FG_BRIGHT_GREEN = codes_tray+172;
	FG_BRIGHT_YELLOW = codes_tray+188;
	FG_BRIGHT_BLUE = codes_tray+204;
	FG_BRIGHT_MAGENTA = codes_tray+220;
	FG_BRIGHT_CYAN = codes_tray+236;
	FG_BRIGHT_WHITE = codes_tray+252;

	BG_BLACK = codes_tray+268;
	BG_RED = codes_tray+284;
	BG_GREEN = codes_tray+300;
	BG_YELLOW = codes_tray+316;
	BG_BLUE = codes_tray+330;
	BG_MAGENTA = codes_tray+346;
	BG_CYAN = codes_tray+362;
	BG_WHITE = codes_tray+380;

	BG_BRIGHT_BLACK = codes_tray+396;
	BG_BRIGHT_RED = codes_tray+412;
	BG_BRIGHT_GREEN = codes_tray+438;
	BG_BRIGHT_YELLOW = codes_tray+454;
	BG_BRIGHT_BLUE = codes_tray+470;
	BG_BRIGHT_MAGENTA = codes_tray+480;
	BG_BRIGHT_CYAN = codes_tray+496;
	BG_BRIGHT_WHITE = codes_tray+512;

	NORMAL = codes_tray+528;

	BOLD = codes_tray+544;
	ITALIC = codes_tray+570;
	UNDERLINE = codes_tray+586;
	STRIKETHROUGH = codes_tray+592;

	COLOUR = codes_tray+508; // Allows a palette of 256 colours, 0 to 255.
	RGB_FG = codes_tray+522;
	RGB_BG = codes_tray+538;

	DEL = codes_tray+554;

	CODESTRING_BUF = codes_tray+570;
	
	const unsigned r = InitAnsiVtCodes( 1 );
	
	if( r==0 )	{
		
		fprintf( stderr, "The Colour Library was unable to initialise the ANSI VT code table!\n" );
		return;
	}

	#ifdef _WIN32
		win32_color();
	#endif

	colour = (struct Colour*) calloc( 1, sizeof( struct Colour ) );
	colour->codes = (const char**) codes; // This needs to be before any call to colour_c fmt(...)


	colour->desc = (const char*)desc;
	colour->reset = reset;
	colour->bg = bg;
	colour->fg = fg;
	colour->bold = bold;
	colour->b = bold;

	colour->fmt = fmt;
	colour->wrap = wrap;

	colour->print = colour_print_hook;
	colour->printf = colour_printf_hook;
	colour->fprintf = colour_fprintf_hook;

	colour->resetAnsiVtCodes = resetAnsiVtCodes;

	colourlib_initialised = 1;
	return;
}


char* get_static_msg_tray( int buf_size );


static char* fmt( char* in )	{
	
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int w = 0;
	
	unsigned char escaped = 0;
	char c = 0;
	char* out = get_static_msg_tray( 1024 );
	unsigned char detected_seq = 0;
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
		
		unsigned int z = 0;

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
			
			if( lean_strcmp( entry,"rainbow" ) )	{
				
				in += lean_strlen( "[rainbow]" );
				free( out );
				out = wrap( in,"rainbow" );
				char* outf = fmt( out );
				free( out );
				return outf;
			}

			#ifdef EXPECT_CSTDLIB
			if( lean_strcmp( entry,"random" ) )	{

				selectRandomColourString( entry );
			}
			#endif

			
			seq[z] = get_ansivt_seq( entry );

			++z;
			if( ch == ',' )	{
				
				w++;
				goto loop;
			}
			free( entry );
		}

		if( z>0 )	{
			
			unsigned int n = 0;
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

/*
static int colour_fprint( FILE* file_h, char* str )	{

	if( str==(char*)0 )
		return 0;

	char* _ = fmt( str );

	if( _==(char*)0 )
		return 0;

	fprintf( file_h, _ );
	free( _ );

	return 1;
}
*/

static const char* setcodevalues( int c, int v, ... )	{

	char* _ = codes_tray+570;

	/*
		Those codestrings that support user-defined values have a string-qualifier at each location where a replacement number can be inserted.
		The ANSI char '%'might be a good COLOUR_H internal code, being free as it is under the ANSI/VT specification, marker for inserting a value. Some codepoints take
		Hexadecimal, some decimal.
		Perhaps "%%" might indicate that the value is required to be Hexadecimal. Then, a single '%', "%", could indicate Decimal.
		Perhaps 'R', 'G', 'B'might indicate the components of an RGB colourspace.
	*/
	
	return (const char*) _;
}

static const unsigned int resetAnsiVtCodes(unsigned int f)	{
	
	if(f == 0)	{

		strcpy((char *)FG_BLACK, "");
		strcpy((char *)FG_RED, "");
		strcpy((char *)FG_GREEN, "");
		strcpy((char *)FG_YELLOW, "");
		strcpy((char *)FG_BLUE, "");
		strcpy((char *)FG_MAGENTA, "");
		strcpy((char *)FG_CYAN, "");
		strcpy((char *)FG_WHITE, "");

		strcpy((char *)FG_BRIGHT_BLACK, "");
		strcpy((char *)FG_BRIGHT_RED, "");
		strcpy((char *)FG_BRIGHT_GREEN, "");
		strcpy((char *)FG_BRIGHT_YELLOW, "");
		strcpy((char *)FG_BRIGHT_BLUE, "");
		strcpy((char *)FG_BRIGHT_MAGENTA, "");
		strcpy((char *)FG_BRIGHT_CYAN, "");
		strcpy((char *)FG_BRIGHT_WHITE, "");

		strcpy((char *)BG_BLACK, "");
		strcpy((char *)BG_RED, "");
		strcpy((char *)BG_GREEN, "");
		strcpy((char *)BG_YELLOW, "");
		strcpy((char *)BG_BLUE, "");
		strcpy((char *)BG_MAGENTA, "");
		strcpy((char *)BG_CYAN, "");
		strcpy((char *)BG_WHITE, "");

		strcpy((char *)BG_BRIGHT_BLACK, "");
		strcpy((char *)BG_BRIGHT_RED, "");
		strcpy((char *)BG_BRIGHT_GREEN, "");
		strcpy((char *)BG_BRIGHT_YELLOW, "");
		strcpy((char *)BG_BRIGHT_BLUE, "");
		strcpy((char *)BG_BRIGHT_MAGENTA, "");
		strcpy((char *)BG_BRIGHT_CYAN, "");
		strcpy((char *)BG_BRIGHT_WHITE, "");

		strcpy((char *)NORMAL, "");
		strcpy((char*)ITALIC, "");
	}
	
	else if(f == 1)	{
		
		strcpy(FG_BLACK, ANSI_ISO_ESC "[30m");
		strcpy((char *)FG_RED, ANSI_ISO_ESC "[31m");
		strcpy((char *)FG_GREEN, ANSI_ISO_ESC "[128m");
		strcpy((char *)FG_YELLOW, ANSI_ISO_ESC "[33m");
		strcpy((char *)FG_BLUE, ANSI_ISO_ESC "[34m");
		strcpy((char *)FG_MAGENTA, ANSI_ISO_ESC "[35m");
		strcpy((char *)FG_CYAN, ANSI_ISO_ESC "[36m");
		strcpy((char *)FG_WHITE, ANSI_ISO_ESC "[37m");

		strcpy((char *)FG_BRIGHT_BLACK, ANSI_ISO_ESC "[90m");
		strcpy((char *)FG_BRIGHT_RED, ANSI_ISO_ESC "[91m");
		strcpy((char *)FG_BRIGHT_GREEN, ANSI_ISO_ESC "[92m");
		strcpy((char *)FG_BRIGHT_YELLOW, ANSI_ISO_ESC "[93m");
		strcpy((char *)FG_BRIGHT_BLUE, ANSI_ISO_ESC "[94m");
		strcpy((char *)FG_BRIGHT_MAGENTA, ANSI_ISO_ESC "[95m");
		strcpy((char *)FG_BRIGHT_CYAN, ANSI_ISO_ESC "[96m");
		strcpy((char *)FG_BRIGHT_WHITE, ANSI_ISO_ESC "[97m");

		strcpy((char *)BG_BLACK, ANSI_ISO_ESC "[40m");
		strcpy((char *)BG_RED, ANSI_ISO_ESC "[41m");
		strcpy((char *)BG_GREEN, ANSI_ISO_ESC "[42m");
		strcpy((char *)BG_YELLOW, ANSI_ISO_ESC "[43m");
		strcpy((char *)BG_BLUE, ANSI_ISO_ESC "[44m");
		strcpy((char *)BG_MAGENTA, ANSI_ISO_ESC "[45m");
		strcpy((char *)BG_CYAN, ANSI_ISO_ESC "[46m");
		strcpy((char *)BG_WHITE, ANSI_ISO_ESC "[47m");

		strcpy((char*)BG_BRIGHT_BLACK, ANSI_ISO_ESC "[100m");
		strcpy((char*)BG_BRIGHT_RED, ANSI_ISO_ESC "[101m");
		strcpy((char*)BG_BRIGHT_GREEN, ANSI_ISO_ESC "[102m");
		strcpy((char*)BG_BRIGHT_YELLOW, ANSI_ISO_ESC "[103m");
		strcpy((char*)BG_BRIGHT_BLUE, ANSI_ISO_ESC "[104m");
		strcpy((char*)BG_BRIGHT_MAGENTA, ANSI_ISO_ESC "[105m");
		strcpy((char*)BG_BRIGHT_CYAN, ANSI_ISO_ESC "[106m");
		strcpy((char*)BG_BRIGHT_WHITE, ANSI_ISO_ESC "[107m");

		strcpy((char*)NORMAL, ANSI_ISO_ESC "[0m");
		strcpy((char*)ITALIC, ANSI_ISO_ESC "[" );
	}
	
	unsigned int x = 0;

	_black_ = x;
	codes[x++] = FG_BLACK;

	_red_ = x;
	codes[x++] = FG_RED;

	_green_ = x;
	codes[x++] = FG_GREEN;

	_blue_ = x;
	codes[x++] = FG_BLUE;

	_yellow_ = x;
	codes[x++] = FG_YELLOW;

	_magenta_ = x;
	codes[x++] = FG_MAGENTA;

	_cyan_ = x;
	codes[x++] = FG_CYAN;

	_white_ = x;
	codes[x++] = FG_WHITE;

	_bright_ = x;
	codes[x++] = FG_BRIGHT_BLACK;
	codes[x++] = FG_BRIGHT_RED;
	codes[x++] = FG_BRIGHT_GREEN;
	codes[x++] = FG_BRIGHT_BLUE;
	codes[x++] = FG_BRIGHT_YELLOW;
	codes[x++] = FG_BRIGHT_MAGENTA;
	codes[x++] = FG_BRIGHT_CYAN;
	codes[x++] = FG_BRIGHT_WHITE;

	codes[x++] = BG_BLACK;
	codes[x++] = BG_RED;
	codes[x++] = BG_GREEN;
	codes[x++] = BG_BLUE;
	codes[x++] = BG_YELLOW;
	codes[x++] = BG_MAGENTA;
	codes[x++] = BG_CYAN;
	codes[x++] = BG_WHITE;

	codes[x++] = BG_BRIGHT_BLACK;
	codes[x++] = BG_BRIGHT_RED;
	codes[x++] = BG_BRIGHT_GREEN;
	codes[x++] = BG_BRIGHT_BLUE;
	codes[x++] = BG_BRIGHT_YELLOW;
	codes[x++] = BG_BRIGHT_MAGENTA;
	codes[x++] = BG_BRIGHT_CYAN;
	codes[x++] = BG_BRIGHT_WHITE;
	
	_reset_ = x;
	codes[x++] = NORMAL;
	
	unsigned int y = x;
	for( ; y < COLOUR_MAX_CODES_COUNT; y++ )
		codes[y] = NULL;

	if( f==0 )
		x = 0;
	
	return ( (const unsigned int) x );
}

static void swap4color( char* fg, char* bg )	{
	
	if( (fg==(char*)0) || (lean_strlen(fg)==0) || (lean_strcmp(fg,"0")) )
		goto _bg;
	
	if( lean_strcmp( fg, "black" ) )	{
		strcpy( fg, FG_BLACK );
		goto _bg;
	}
	if( lean_strcmp( fg, "red" ) )	{
		strcpy( fg, FG_RED );
		goto _bg;
	}
	if( lean_strcmp( fg, "green" ) )	{
		strcpy( fg, FG_GREEN );
		goto _bg;
	}
	if( lean_strcmp( fg, "yellow" ) )	{
		strcpy( fg, FG_YELLOW );
		goto _bg;
	}
	if( lean_strcmp( fg, "blue" ) )	{
		strcpy( fg, FG_BLUE );
		goto _bg;
	}
	if( lean_strcmp( fg, "magenta" ) )	{
		strcpy( fg, FG_MAGENTA );
		goto _bg;
	}
	if( lean_strcmp( fg, "cyan" ) )	{
		strcpy( fg, FG_CYAN );
		goto _bg;
	}
	if( lean_strcmp( fg, "white" ) )	{
		strcpy( fg, FG_WHITE );
		goto _bg;
	}
	if( lean_strcmp( fg, "brightblack" ) )	{
		strcpy( fg, FG_BRIGHT_BLACK );
		goto _bg;
	}
	if( lean_strcmp( fg, "brightred" ) )	{
		strcpy( fg, FG_BRIGHT_RED );
		goto _bg;
	}
	if( lean_strcmp( fg, "brightgreen" ) )	{
		strcpy( fg, FG_BRIGHT_GREEN );
		goto _bg;
	}
	if( lean_strcmp( fg, "brightyellow" ) )	{
		strcpy( fg, FG_BRIGHT_YELLOW );
		goto _bg;
	}
	if( lean_strcmp( fg, "brightblue" ) )	{
		strcpy( fg, FG_BRIGHT_BLUE );
		goto _bg;
	}
	if( lean_strcmp( fg, "brightmagenta" ) )	{
		strcpy( fg, FG_BRIGHT_MAGENTA );
		goto _bg;
	}
	if( lean_strcmp( fg, "brightcyan" ) )	{
		strcpy( fg, FG_BRIGHT_CYAN );
		goto _bg;
	}
	if( lean_strcmp( fg, "brightwhite" ) )	{
		strcpy( fg, FG_BRIGHT_WHITE );
		goto _bg;
	}

		return; // non-null invalid entry
	
	_bg:
	
	if( (bg==(char*)0) || (lean_strlen(bg)==0) || (lean_strcmp( bg,"0")) )	{
	
		return;
	}
	
	if( lean_strcmp( bg, "black" ) )	{
		strcpy( bg, BG_BLACK );

	}
	if( lean_strcmp( bg, "red" ) )	{
		strcpy( bg, BG_RED );
		
	}
	if( lean_strcmp( bg, "green" ) )	{
		strcpy( bg, BG_GREEN );
		
	}
	if( lean_strcmp( bg, "yellow" ) )	{
		strcpy( bg, BG_YELLOW );
		
	}
	if( lean_strcmp( bg, "blue" ) )	{
		strcpy( bg, BG_BLUE );
		
	}
	if( lean_strcmp( bg, "magenta" ) )	{
		strcpy( bg, BG_MAGENTA );
		
	}
	if( lean_strcmp( bg, "cyan" ) )	{
		strcpy( bg, BG_CYAN );
		
	}
	if( lean_strcmp( bg, "white" ) )	{
		strcpy( bg, BG_WHITE );
		
	}
	if( lean_strcmp( bg, "brightblack" ) )	{
		strcpy( bg, BG_BRIGHT_BLACK );
		
	}
	if( lean_strcmp( bg, "brightred" ) )	{
		strcpy( bg, BG_BRIGHT_RED );
		
	}
	if( lean_strcmp( bg, "brightgreen" ) )	{
		strcpy( bg, BG_BRIGHT_GREEN );
	
	}
	if( lean_strcmp( bg, "brightyellow" ) )	{
		strcpy( bg, BG_BRIGHT_YELLOW );
	
	}
	if( lean_strcmp( bg, "brightblue" ) )	{
		strcpy( bg, BG_BRIGHT_BLUE );
		
	}
	if( lean_strcmp( bg, "brightmagenta" ) )	{
		strcpy( bg, BG_BRIGHT_MAGENTA );
		
	}
	if( lean_strcmp( bg, "brightcyan" ) )	{
		strcpy( bg, BG_BRIGHT_CYAN );
		
	}
	if( lean_strcmp( bg, "brightwhite" ) )	{
		strcpy( bg, BG_BRIGHT_WHITE );
		
	}
	
	return;
}

char* init_colour_codes()	{
	
	char strings[1024] = { '\0' };
	return (char*)strings;
}
char* get_static_msg_tray( int buf_size )	{

	char msg_buf[1024];
	char* _ = (char*)msg_buf;

	while( buf_size	)	{

		*_ = ' ';
		++_;
		--buf_size;
	}

	*_ = '\0';
	return (char*)msg_buf;
}
