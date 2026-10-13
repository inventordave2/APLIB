#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rb_core.h"

static void* (*rb_alloc_fn)(size_t) = NULL;
static void* (*rb_calloc_fn)(size_t, size_t) = NULL;
static void (*rb_free_fn)(void*) = NULL;

static void* rb_default_alloc(size_t nbytes) {
    return malloc(nbytes);
}

static void* rb_default_calloc(size_t count, size_t size) {
    return calloc(count, size);
}

static void rb_default_free(void* ptr) {
    free(ptr);
}

static void rb_runtime_init(void) {
    if( rb_alloc_fn == NULL )
        rb_alloc_fn = rb_default_alloc;
    if( rb_calloc_fn == NULL )
        rb_calloc_fn = rb_default_calloc;
    if( rb_free_fn == NULL )
        rb_free_fn = rb_default_free;
}

void rb_set_allocators(
    void* (*alloc_fn)(size_t),
    void* (*calloc_fn)(size_t, size_t),
    void (*free_fn)(void*)
) {
    rb_alloc_fn = alloc_fn ? alloc_fn : rb_default_alloc;
    rb_calloc_fn = calloc_fn ? calloc_fn : rb_default_calloc;
    rb_free_fn = free_fn ? free_fn : rb_default_free;
}

void* rb_alloc(size_t nbytes) {
    rb_runtime_init();
    return rb_alloc_fn(nbytes);
}

void* rb_calloc(size_t count, size_t size) {
    rb_runtime_init();
    return rb_calloc_fn(count, size);
}

void rb_free(void* ptr) {
    if( ptr == NULL )
        return;
    rb_runtime_init();
    rb_free_fn(ptr);
}

static char* rb_strdup_text(const char* s) {
    size_t len;
    char* out;

    if( s == NULL )
        return NULL;

    len = strlen(s);
    out = (char*)rb_alloc(len + 1);
    if( out == NULL )
        return NULL;

    memcpy(out, s, len + 1);
    return out;
}

static char* rb_strdup_range(const char* s, size_t len) {
    char* out;

    if( s == NULL )
        return NULL;

    out = (char*)rb_alloc(len + 1);
    if( out == NULL )
        return NULL;

    memcpy(out, s, len);
    out[len] = '\0';
    return out;
}

static int rb_is_digit(char c) {
    return (c >= '0' && c <= '9');
}

rb_ap_ptr rb_ap_new_empty(void) {
    rb_ap_ptr ap = (rb_ap_ptr)rb_calloc(1, sizeof(rb_ap));
    if( ap == NULL )
        return NULL;

    ap->whole = rb_strdup_text("0");
    ap->frac = rb_strdup_text("0");
    ap->sign = '+';
    ap->base = 10;
    ap->type = 0;
    ap->prec = 0;
    ap->desc = rb_strdup_text("rb_ap");

    if( ap->whole == NULL || ap->frac == NULL || ap->desc == NULL ) {
        rb_ap_free(ap);
        return NULL;
    }

    return ap;
}

static void rb_trim_leading_zeroes(char** text) {
    char* p;
    char* q;
    size_t len;

    if( text == NULL || *text == NULL )
        return;

    p = *text;
    while( *p == '0' )
        ++p;

    if( *p == '\0' ) {
        free(*text);
        *text = rb_strdup_text("0");
        return;
    }

    len = strlen(p);
    q = rb_strdup_range(p, len);
    free(*text);
    *text = q;
}

rb_ap_ptr rb_ap_new_from_text(const char* text) {
    const char* p;
    const char* frac_start;
    const char* end;
    char* whole;
    char* frac;
    rb_ap_ptr ap;
    size_t whole_len;
    size_t frac_len;
    int saw_digit = 0;
    int saw_dot = 0;
    int negative = 0;

    if( text == NULL )
        return NULL;

    ap = rb_ap_new_empty();
    if( ap == NULL )
        return NULL;

    p = text;
    while( *p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' )
        ++p;

    if( *p == '+' )
        ++p;
    else if( *p == '-' ) {
        negative = 1;
        ++p;
    }

    whole = NULL;
    frac = NULL;
    end = p;
    while( *end != '\0' && *end != '.' ) {
        if( rb_is_digit(*end) )
            saw_digit = 1;
        else if( *end != ' ' && *end != '\t' && *end != '\n' && *end != '\r' ) {
            rb_ap_free(ap);
            return NULL;
        }
        ++end;
    }

    if( saw_digit == 0 && *p == '\0' ) {
        rb_ap_free(ap);
        return NULL;
    }

    whole_len = (size_t)(end - p);
    if( whole_len > 0 )
        whole = rb_strdup_range(p, whole_len);
    else
        whole = rb_strdup_text("0");

    if( whole == NULL ) {
        rb_ap_free(ap);
        return NULL;
    }

    rb_trim_leading_zeroes(&whole);

    if( *end == '.' ) {
        saw_dot = 1;
        ++end;
        frac_start = end;
        while( *end != '\0' ) {
            if( rb_is_digit(*end) )
                saw_digit = 1;
            else if( *end != ' ' && *end != '\t' && *end != '\n' && *end != '\r' ) {
                rb_free(whole);
                rb_ap_free(ap);
                return NULL;
            }
            ++end;
        }

        frac_len = (size_t)(end - frac_start);
        if( frac_len > 0 )
            frac = rb_strdup_range(frac_start, frac_len);
        else
            frac = rb_strdup_text("0");

        if( frac == NULL ) {
            rb_free(whole);
            rb_ap_free(ap);
            return NULL;
        }

        while( *frac == '0' ) {
            char* q = frac;
            ++q;
            if( *q == '\0' ) {
                rb_free(frac);
                frac = rb_strdup_text("0");
                break;
            }
            if( frac[0] == '0' ) {
                memmove(frac, frac + 1, strlen(frac));
            }
        }
    } else {
        frac = rb_strdup_text("0");
    }

    if( frac == NULL ) {
        rb_free(whole);
        rb_ap_free(ap);
        return NULL;
    }

    if( negative )
        ap->sign = '-';
    else
        ap->sign = '+';

    rb_free(ap->whole);
    rb_free(ap->frac);
    ap->whole = whole;
    ap->frac = frac;
    ap->base = 10;
    ap->type = 0;
    ap->prec = (rb_prec_t)strlen(frac);
    ap->desc = rb_strdup_text("rb_ap");

    if( ap->desc == NULL ) {
        rb_ap_free(ap);
        return NULL;
    }

    return ap;
}

rb_ap_ptr rb_ap_copy(const rb_ap_ptr a) {
    rb_ap_ptr out;

    if( a == NULL )
        return NULL;

    out = rb_ap_new_empty();
    if( out == NULL )
        return NULL;

    rb_free(out->whole);
    rb_free(out->frac);
    out->whole = rb_strdup_text(a->whole ? a->whole : "0");
    out->frac = rb_strdup_text(a->frac ? a->frac : "0");
    out->sign = a->sign;
    out->base = a->base;
    out->type = a->type;
    out->prec = a->prec;
    rb_free(out->desc);
    out->desc = rb_strdup_text(a->desc ? a->desc : "rb_ap");

    return out;
}

void rb_ap_free(rb_ap_ptr a) {
    if( a == NULL )
        return;

    rb_free(a->whole);
    rb_free(a->frac);
    rb_free(a->desc);
    rb_free(a);
}

void rb_ap_set_base(rb_ap_ptr a, int base) {
    if( a == NULL )
        return;
    a->base = base;
}

void rb_ap_set_prec(rb_ap_ptr a, rb_prec_t prec) {
    if( a == NULL )
        return;
    a->prec = prec;
}

void rb_ap_set_sign(rb_ap_ptr a, char sign) {
    if( a == NULL )
        return;
    if( sign == '-' || sign == '+' )
        a->sign = sign;
}

rb_status rb_parse_text(const char* text, rb_ap_ptr* out) {
    rb_ap_ptr ap;

    if( out == NULL )
        return RB_ERR_NULL;

    *out = NULL;
    if( text == NULL )
        return RB_ERR_BAD_INPUT;

    ap = rb_ap_new_from_text(text);
    if( ap == NULL )
        return RB_ERR_BAD_INPUT;

    *out = ap;
    return RB_OK;
}

rb_status rb_parse_buffer(rb_buffer* in, rb_ap_ptr* out) {
    char* tmp;
    size_t len;
    rb_status status;

    if( in == NULL || out == NULL )
        return RB_ERR_NULL;

    *out = NULL;
    if( in->data == NULL || in->len == 0 )
        return RB_ERR_BAD_INPUT;

    len = in->len;
    tmp = (char*)rb_alloc(len + 1);
    if( tmp == NULL )
        return RB_ERR_ALLOC;

    memcpy(tmp, in->data, len);
    tmp[len] = '\0';

    status = rb_parse_text(tmp, out);
    rb_free(tmp);
    return status;
}

rb_status rb_ap_to_text(const rb_ap_ptr a, char** out_text) {
    size_t len;
    char* out;

    if( out_text == NULL )
        return RB_ERR_NULL;

    *out_text = NULL;
    if( a == NULL )
        return RB_ERR_NULL;

    len = strlen(a->whole ? a->whole : "0") + strlen(a->frac ? a->frac : "0") + 4;
    out = (char*)rb_alloc(len + 1);
    if( out == NULL )
        return RB_ERR_ALLOC;

    if( a->frac != NULL && strcmp(a->frac, "0") != 0 ) {
        snprintf(out, len + 1, "%c%s.%s", a->sign, a->whole ? a->whole : "0", a->frac ? a->frac : "0");
    } else {
        snprintf(out, len + 1, "%c%s", a->sign, a->whole ? a->whole : "0");
    }

    *out_text = out;
    return RB_OK;
}

rb_status rb_ap_to_buffer(const rb_ap_ptr a, rb_buffer* out) {
    char* text = NULL;
    rb_status st;

    if( out == NULL )
        return RB_ERR_NULL;

    out->data = NULL;
    out->len = 0;
    out->used = 0;

    st = rb_ap_to_text(a, &text);
    if( st != RB_OK )
        return st;

    out->data = text;
    out->len = strlen(text);
    out->used = out->len;
    return RB_OK;
}

int rb_ap_cmp(const rb_ap_ptr a, const rb_ap_ptr b) {
    const char* a_whole;
    const char* b_whole;
    const char* a_frac;
    const char* b_frac;
    int cmp;

    if( a == NULL && b == NULL )
        return 0;
    if( a == NULL )
        return -1;
    if( b == NULL )
        return 1;

    a_whole = a->whole ? a->whole : "0";
    b_whole = b->whole ? b->whole : "0";
    a_frac = a->frac ? a->frac : "0";
    b_frac = b->frac ? b->frac : "0";

    cmp = strcmp(a_whole, b_whole);
    if( cmp != 0 )
        return cmp;

    return strcmp(a_frac, b_frac);
}

int rb_ap_eq(const rb_ap_ptr a, const rb_ap_ptr b) {
    return rb_ap_cmp(a, b) == 0;
}

rb_status rb_add(const rb_ap_ptr a, const rb_ap_ptr b, rb_ap_ptr* out) {
    (void)a;
    (void)b;
    (void)out;
    return RB_ERR_BAD_INPUT;
}

rb_status rb_sub(const rb_ap_ptr a, const rb_ap_ptr b, rb_ap_ptr* out) {
    (void)a;
    (void)b;
    (void)out;
    return RB_ERR_BAD_INPUT;
}

rb_status rb_mul(const rb_ap_ptr a, const rb_ap_ptr b, rb_ap_ptr* out) {
    (void)a;
    (void)b;
    (void)out;
    return RB_ERR_BAD_INPUT;
}

rb_status rb_div(const rb_ap_ptr a, const rb_ap_ptr b, rb_ap_ptr* out) {
    (void)a;
    (void)b;
    (void)out;
    return RB_ERR_BAD_INPUT;
}

rb_status rb_pow(const rb_ap_ptr a, const rb_ap_ptr b, rb_ap_ptr* out) {
    (void)a;
    (void)b;
    (void)out;
    return RB_ERR_BAD_INPUT;
}

rb_status rb_root(const rb_ap_ptr a, const rb_ap_ptr b, rb_ap_ptr* out) {
    (void)a;
    (void)b;
    (void)out;
    return RB_ERR_BAD_INPUT;
}

rb_status rb_sin_approx(const rb_ap_ptr angle, rb_ap_ptr* out) {
    (void)angle;
    (void)out;
    return RB_ERR_BAD_INPUT;
}

rb_status rb_cos_approx(const rb_ap_ptr angle, rb_ap_ptr* out) {
    (void)angle;
    (void)out;
    return RB_ERR_BAD_INPUT;
}

rb_status rb_tan_approx(const rb_ap_ptr angle, rb_ap_ptr* out) {
    (void)angle;
    (void)out;
    return RB_ERR_BAD_INPUT;
}

rb_status rb_log_approx(const rb_ap_ptr x, rb_ap_ptr* out) {
    (void)x;
    (void)out;
    return RB_ERR_BAD_INPUT;
}
