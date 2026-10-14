#include <stdlib.h>
#include <string.h>

#include "rb_buffer.h"
#include "rb_core.h"

void rb_buffer_init(rb_buffer* b, char* data, size_t len) {
    if( b == (void*)0 )
        return;

    b->data = data;
    b->len = len;
    b->used = len;
}

void rb_buffer_reset(rb_buffer* b) {
    if( b == (void*)0 )
        return;

    b->data = (void*)0;
    b->len = 0;
    b->used = 0;
}

void rb_buffer_release(rb_buffer* b) {
    if( b == (void*)0 )
        return;

    if( b->data != (void*)0 ) {
        rb_free(b->data);
        b->data = (void*)0;
    }

    b->len = 0;
    b->used = 0;
}

rb_status rb_buffer_from_text(rb_buffer* b, const char* text) {
    size_t len;
    char* copy;

    if( b == (void*)0 )
        return RB_ERR_NULL;

    rb_buffer_reset(b);
    if( text == (void*)0 )
        return RB_ERR_BAD_INPUT;

    len = strlen(text);
    copy = (char*)rb_alloc(len + 1);
    if( copy == (void*)0 )
        return RB_ERR_ALLOC;

    memcpy(copy, text, len + 1);
    b->data = copy;
    b->len = len;
    b->used = len;
    return RB_OK;
}

rb_status rb_buffer_from_ap(const rb_ap_ptr a, rb_buffer* out) {
    char* text = (void*)0;
    rb_status st;

    if( out == (void*)0 )
        return RB_ERR_NULL;

    rb_buffer_reset(out);
    st = rb_ap_to_text(a, &text);
    if( st != RB_OK )
        return st;

    out->data = text;
    out->len = strlen(text);
    out->used = out->len;
    return RB_OK;
}

char rb_buffer_peek(const rb_buffer* b) {
    if( b == (void*)0 || b->data == (void*)0 || b->used == 0 )
        return '\0';

    return b->data[0];
}

char rb_buffer_consume(rb_buffer* b) {
    char c = '\0';

    if( b == (void*)0 || b->data == (void*)0 || b->used == 0 )
        return '\0';

    c = b->data[0];
    if( b->used > 1 ) {
        memmove(b->data, b->data + 1, b->used - 1);
    }
    b->used -= 1;
    b->data[b->used] = '\0';
    return c;
}

rb_status rb_buffer_append(rb_buffer* b, const char* text) {
    size_t text_len;
    size_t new_len;
    char* new_data;

    if( b == (void*)0 || text == (void*)0 )
        return RB_ERR_NULL;

    text_len = strlen(text);
    if( text_len == 0 )
        return RB_OK;

    if( b->data == (void*)0 ) {
        new_data = (char*)rb_alloc(text_len + 1);
        if( new_data == (void*)0 )
            return RB_ERR_ALLOC;

        memcpy(new_data, text, text_len + 1);
        b->data = new_data;
        b->len = text_len;
        b->used = text_len;
        return RB_OK;
    }

    new_len = b->used + text_len;
    new_data = (char*)rb_alloc(new_len + 1);
    if( new_data == (void*)0 )
        return RB_ERR_ALLOC;

    memcpy(new_data, b->data, b->used);
    memcpy(new_data + b->used, text, text_len + 1);
    rb_free(b->data);
    b->data = new_data;
    b->len = new_len;
    b->used = new_len;
    return RB_OK;
}

rb_status rb_buffer_append_char(rb_buffer* b, char c) {
    char s[2];

    if( b == (void*)0 )
        return RB_ERR_NULL;

    s[0] = c;
    s[1] = '\0';
    return rb_buffer_append(b, s);
}
