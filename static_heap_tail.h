#ifndef DAVELIB_STATIC_HEAP_TAIL
#define DAVELIB_STATIC_HEAP_TAIL

#define MAXHEAPSIZE 4096

static unsigned char* heap_start;

unsigned char* Heap( int size );
unsigned char* alloc( int size );

unsigned char* alloc( int size )    {

    int x;
    for( x=-1; x<alloc_table; ++x ) {

        if( alloc_table_free[x][1] >= size )    {


        }
    }
}

unsigned char* Heap( int size ) {

    if( size < 1 )
        return (unsigned char*)0;

    if( size > MAXHEAPSIZE )
        size = MAXHEAPSIZE;

    int offset = 0;

    if( size < MAXHEAPSIZE )
        offset = MAXHEAPSIZE - size;

    unsigned char heap[MAXHEAPSIZE];
    return (heap_start = (unsigned char*) (heap+offset));
}



#endif
