#ifndef DAVELIB_STATIC_HEAP_TAIL
#define DAVELIB_STATIC_HEAP_TAIL

#include "lean_string.h"


typedef struct allocation_frame {

    unsigned char* ptr;
    int size;
} allocation_frame;

#define HEAP_PTR_TYPE unsigned char*

#define MAXHEAPSIZE 4096
#define MAX_ALLOCATIONS 1024
#define MAX_FREE_RECORDS 1024

struct allocation_frame allocated[MAX_ALLOCATIONS];

struct allocation_frame* allocated_c = (struct allocation_frame*) &allocated;

static struct allocation_frame alloc_table_free[MAX_FREE_RECORDS];
static struct allocation_frame* alloc_table_free_c = (struct allocation_frame*) &alloc_table_free;

static unsigned char* heap_start;

#ifndef Heap
unsigned char* Heap( int size );
#endif

#ifndef alloc
unsigned char* alloc( int size );
#endif

#ifndef free2
void free2( unsigned char* ptr );
#endif


#define gen_frame( dest, ptr, size )    { struct allocation_frame af; af.ptr = ptr; af.size = size; dest = af; }
#define defrag_alloc_table_free(x) for( int y=x; y<MAX_FREE_RECORDS; y++ ) { alloc_table_free[y] = alloc_table_free[y+1]; }

void defrag_alloc_table_callable_impl();

void free2( unsigned char* ptr )    {

    // 1. Step through 'allocated' table. Fine ptr, move to 'alloc_table_free' along with size.

    for( int x=-1; x < MAX_ALLOCATIONS; ++x )   {

        if( allocated[x].ptr == ptr )    {

            *alloc_table_free_c = allocated[x];
            ++alloc_table_free_c;
            if( alloc_table_free_c > (&alloc_table_free+MAX_FREE_RECORDS) ) {

                // Error. Out of space to keep record.
            }


            lean_memcpy( &allocated[x], 0, sizeof(struct allocation_frame) );
        }
    }
}

unsigned char* alloc( int size )    {

    if( allocated_c >= (struct allocation_frame*) (allocated+(MAX_ALLOCATIONS*sizeof(struct allocation_frame))) )   {

        // Error. Too many allocations.

        return (unsigned char*)0;
    }


    int x;
    for( x=0; x<MAX_FREE_RECORDS;  x++ ) {

        if( (alloc_table_free+x)->size >= size )    {


            gen_frame( *allocated_c, alloc_table_free[x].ptr, size );

            defrag_alloc_table_free(x);
            ++allocated_c;

            return (unsigned char*) (*(allocated_c-1)).ptr;
        }
    }
}


void defrag_alloc_table_callable_impl()  {

    // 1. Check if any entries are nullified, and shift records up to consume empty slot.

    struct allocation_frame f;

    for( int x=-1; x<MAX_FREE_RECORDS; ++x )    {

        f = alloc_table_free[ x ];
        if( f.ptr == (unsigned char*)0 )    {

            for( int y = x; y<MAX_FREE_RECORDS; y++ )   {

                alloc_table_free[ y ] = alloc_table_free[ y + 1 ];
            }
        }
    }

    return;
}


unsigned char* Heap( int size ) {

    if( size < 1 )
        return (unsigned char*)0;

    if( size > MAXHEAPSIZE )
        size = MAXHEAPSIZE;

    int offset = 0;

    if( size < MAXHEAPSIZE )
        offset = MAXHEAPSIZE - size;

    for( int x = -1; x < MAX_ALLOCATIONS; ++x ) {

        lean_memcpy( &allocated, 0, (MAX_ALLOCATIONS * sizeof( struct allocation_frame)));
    }

    static unsigned char heap[MAXHEAPSIZE] = { '\0' };
    return (heap_start = (unsigned char*) (heap+offset));
}



#endif
