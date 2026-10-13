#ifndef FS_OPTS_H
#define FS_OPTS_H

static char* msg = (char*)0;

static signed int rotate( int* argc, char** argv );
static signed int getOptions(int* argc, char** argv);


static signed int getOptions(int* argc, char** argv)	{

	char buff[200];
	int rc = 0;
	
	char i = 0, j = 0, invalid = '\0';
	char suffix[2];
	suffix[0] = '\0';
	suffix[1] = '\0';
	
	filename[0] = '\0';

	FLAGS |= OTS;

	for(;;)	{

		rotate( argc, argv ); // consume last cmd-line component, first time around, that's the fs exe name.

		if( *argc==0 )		
			// all cmd-line args consumed.
			break;
		
		printf( "Processing args...\n" );

		if(cmp(argv[0], "-r"))	{
			
			FLAGS |= RECURSE;
			continue;
		}

		if(cmp(argv[0], "-o"))	{

			strcpy(outputFile, argv[1]);
			FLAGS |= OTF;
			
			rotate( argc, argv );
			// 2 components,
			// -f then "filename"
			// delete 1st, 2nd will be shifted
			// on loop-around
			
			continue;
		}
		
		if(cmp(argv[0], "-c"))	{
			
			if(cmp(argv[1], "off") || cmp(argv[1], "0"))	{
					
				resetAnsiVtCodes(0);
				printf( "Colour output DISABLED.\n" );
				rotate( argc, argv );
			}
			else	{

				resetAnsiVtCodes(1);
				msg = colour->wrap( "Colour output ENABLED.", "brightgreen" );
				printf( msg );
				free( msg );

				if(cmp(argv[1], "on") || cmp(argv[1], "1"))
					rotate(argc, argv);
			}	
			
			continue;
		}
		
		if( cmp(argv[0], "-dir") || cmp(argv[0], "-d") )	{
			
			if( argv[1][0] != '-' )	{ // next arg is directory name to find (but can do -f filename to set the dirname.
			
				strcpy(filename, argv[1]);
				rotate( argc, argv ); // shunts off "-d", next loop-around, the dirname will be automatically shunted off the arg list.
			}
			
			FLAGS |= DIRECTORY;
			continue;
		}
		
		if( cmp(argv[0], "-filecontents") || cmp(argv[0], "-fc") )	{
			
			strcpy(search_string, argv[1]);
			FLAGS |= FILE_CONTENTS;
			
			rotate( argc, argv );
			// 2 components,
			// -filecontents then "search string (max 1023 octets/ANSI)"
			// delete 1st, 2nd will be shifted
			// on loop-around			
			continue;
		}
		
		if( cmp( argv[0], "-regexp" ) || cmp( argv[0], "-re" ) )	{
			
			regExp = 1;
			continue;
		}
		
		if( cmp(argv[0], "-html") )	{
			
			FLAGS |= HTML;
			continue;
		}
		
		if( cmp(argv[0], "-ci") )	{
			
			FLAGS |= CASE_INSENSITIVE;
			continue;
		}
		
		if( cmp(argv[0], "-i") )	{
			
			if( argv[1][0] == '-' )	{
				
				printf( "Used '-i' switch, but passed no DIR_BLOCK list! Assuming default list.\n" );
			}
			else	{
				
				strcpy( ignoreList, argv[1] );
				rotate( argc, argv );
			}
			
			continue;
		}
		
		if( cmp(argv[0], "+i") )	{
			
			if( (argv[1][0] == '-') || (argv[1][0] == '+') )	{
				
				print( "Used '+i' switch, but passed no DIR_WHITELIST list! Assuming default list.\n" );
				strcpy(whiteList, defaultWhiteList);
			}
			else	{
				
				strcpy(whiteList, argv[1]);
				rotate( argc, argv );
			}
			
			continue;
		}
		
		if( cmp(argv[0], "-f") || cmp(argv[0], "-filename") )	{ // optional switch, can just pass filename pattern as a stand-alone arg.

			if(argv[1][0]!='-')	{ // next arg is filename to find (but can do -d dirname to set the dirname).

				strcpy( filename, argv[1] );
				rotate( argc, argv ); // shunts off "-d", next loop-around, the dirname will be automatically shunted off the arg list.
			}

			continue;
		}
		
		strcpy( filename, argv[0] );
	}
	
	if( cmp(filename, "") )
		strcpy(filename, "*");
	
	int t = 0;
	//char temp[100];
	
	char* fn = filename;

	msg = colour->fmt( "[brightred,bg:black]Invalid non-printable char ( [yellow]ASCII [white]%d [brightblue]@ [brightred]offset[brightblue]([white]%d[brightblue])[brightred].\n" );

	while((*fn) != '\0')
		if((*(fn++)) < 32)	{
			
			invalid = (*fn);
    		
    		printf( msg, invalid, (fn - filename) );
		}

	free( msg );
	
	fn = filename;
	if((*fn) == ':')	{
		
		regExp = 1;
		++filename;
	}
	if(regExp == 1)
		;
	else
		while((*fn) != '\0')	{
			
			
			
			/*
			If the app user feeds in a filepath URI, e.g. /temp/tmpfile.txt
			FS should split it up into path and filename.
			*/
			
			
			
			switch(*fn)	{

				case 92: // \ backslash
					
					break;
				case 34: // " double-quote
				//case 42: // * star, we allow this for wc-search
				//case 47: // / forward-slash
				//case 58: // : colon
				case 60: // < lt
				case 62: // > gt
				case 63: // ? question-mark			
				case 124:// | pipe
				
					invalid = *fn;
				  	msg = colour->fmt( "[bg:black,fg:brightred]Invalid char ('[brightblue]%c[brightred]') ([brightblue]%d[brightred]).\n" );

       				fprintf( stderr, msg, invalid, invalid );
       				free( msg );

					break;
				
				default:
					//printf( "Char ok: '%c'\n", (*fn) );
					break;	
			}
			
			++fn;
		}
	;
}

signed int rotate( int* argc, char** argv )	{

	if( *argc == 0 )
		return 0;

	if( argv == (char**)NULL )
		return -1;

	if( argv[0] == (char*)NULL )
		return -1;
	
	int x;
	for( x = 0; x < (*argc-1); x++ )
		argv[x] = argv[x+1];

	argv[*argc-1] = (char*)NULL;

	*argc = *argc - 1;

	return *argc;
}

#endif
