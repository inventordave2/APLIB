#ifndef DAVELIB_COLOUR_H
#define DAVELIB_COLOUR_H

typedef struct colour_t	{

	char* (*fg)(char* cn);
	char* (*bg)(char* cn);
	char* (*fmt)(char* in);
	char* (*wrap)(char* str, char* opt);
	char* (*getcolourcode)(char* colourname);

} colour_t;

#endif

