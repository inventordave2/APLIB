#ifndef RB_BUFFER_H
#define RB_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "rb_core.h"

/* Buffer lifecycle */
void rb_buffer_init(rb_buffer* b, char* data, size_t len);
void rb_buffer_reset(rb_buffer* b);
void rb_buffer_release(rb_buffer* b);

/* Convenience constructors */
rb_status rb_buffer_from_text(rb_buffer* b, const char* text);
rb_status rb_buffer_from_ap(const rb_ap_ptr a, rb_buffer* out);

/* Read/write helpers */
char rb_buffer_peek(const rb_buffer* b);
char rb_buffer_consume(rb_buffer* b);
rb_status rb_buffer_append(rb_buffer* b, const char* text);
rb_status rb_buffer_append_char(rb_buffer* b, char c);

#ifdef __cplusplus
}
#endif

#endif
