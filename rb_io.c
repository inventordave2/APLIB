#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rb_io.h"
#include "rb_buffer.h"

rb_status rb_io_load_argc_argv(int argc, char** argv, rb_buffer* out) {
    int i;
    size_t total = 0;
    char* data;
    char* cursor;

    if( out == (void*)0 )
        return RB_ERR_NULL;

    rb_buffer_reset(out);

    if( argv == (void*)0 )
        return RB_ERR_BAD_INPUT;

    for( i = 0; i < argc; ++i ) {
        if( argv[i] != (void*)0 )
            total += strlen(argv[i]) + 1;
    }

    if( total == 0 ) {
        out->data = (void*)0;
        out->len = 0;
        out->used = 0;
        return RB_OK;
    }

    data = (char*)rb_alloc(total + 1);
    if( data == (void*)0 )
        return RB_ERR_ALLOC;

    cursor = data;
    for( i = 0; i < argc; ++i ) {
        size_t len;
        if( argv[i] == (void*)0 )
            continue;
        len = strlen(argv[i]);
        memcpy(cursor, argv[i], len);
        cursor += len;
        *cursor = ' ';
        ++cursor;
    }
    if( cursor > data )
        --cursor;
    *cursor = '\0';

    out->data = data;
    out->len = (size_t)(cursor - data);
    out->used = out->len;
    return RB_OK;
}

rb_status rb_io_read_file(const char* path, rb_buffer* out) {
    FILE* fp;
    long sz;
    char* data;
    size_t read_count;

    if( out == (void*)0 )
        return RB_ERR_NULL;

    rb_buffer_reset(out);
    if( path == (void*)0 )
        return RB_ERR_BAD_INPUT;

    fp = fopen(path, "rb");
    if( fp == (void*)0 )
        return RB_ERR_BAD_INPUT;

    if( fseek(fp, 0, SEEK_END) != 0 ) {
        fclose(fp);
        return RB_ERR_BAD_INPUT;
    }

    sz = ftell(fp);
    if( sz < 0 ) {
        fclose(fp);
        return RB_ERR_BAD_INPUT;
    }

    rewind(fp);
    data = (char*)rb_alloc((size_t)sz + 1);
    if( data == (void*)0 ) {
        fclose(fp);
        return RB_ERR_ALLOC;
    }

    read_count = fread(data, 1, (size_t)sz, fp);
    fclose(fp);

    if( read_count != (size_t)sz ) {
        rb_free(data);
        return RB_ERR_BAD_INPUT;
    }

    data[read_count] = '\0';
    out->data = data;
    out->len = read_count;
    out->used = read_count;
    return RB_OK;
}

rb_status rb_io_write_file(const char* path, const rb_ap_ptr a) {
    FILE* fp;
    char* text = (void*)0;
    rb_status st;

    if( path == (void*)0 || a == (void*)0 )
        return RB_ERR_NULL;

    st = rb_ap_to_text(a, &text);
    if( st != RB_OK )
        return st;

    fp = fopen(path, "wb");
    if( fp == (void*)0 ) {
        rb_free(text);
        return RB_ERR_BAD_INPUT;
    }

    if( text != (void*)0 ) {
        fwrite(text, 1, strlen(text), fp);
    }

    fclose(fp);
    rb_free(text);
    return RB_OK;
}
