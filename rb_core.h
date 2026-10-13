#ifndef RB_CORE_H
#define RB_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/* ===== Core numeric model ===== */

typedef signed long long rb_prec_t;

typedef struct rb_ap {
    char* whole;
    char* frac;
    char sign;      /* '+' or '-' */
    int base;       /* 2, 8, 10, 16, etc. */
    int type;       /* integer, real, angle, etc. */
    rb_prec_t prec;
    char* desc;
} rb_ap;

typedef rb_ap* rb_ap_ptr;

typedef struct rb_buffer {
    char* data;
    size_t len;
    size_t used;
} rb_buffer;

typedef enum {
    RB_OK = 0,
    RB_ERR_NULL,
    RB_ERR_BAD_INPUT,
    RB_ERR_OVERFLOW,
    RB_ERR_DIV_ZERO,
    RB_ERR_BAD_BASE,
    RB_ERR_ALLOC,
    RB_ERR_EOF
} rb_status;

typedef enum {
    RB_OP_ADD = 1,
    RB_OP_SUB,
    RB_OP_MUL,
    RB_OP_DIV,
    RB_OP_POW,
    RB_OP_ROOT,
    RB_OP_LOG,
    RB_OP_SIN,
    RB_OP_COS,
    RB_OP_TAN
} rb_op;

/* ===== Lifecycle ===== */

rb_ap_ptr rb_ap_new_empty(void);
rb_ap_ptr rb_ap_new_from_text(const char* text);
rb_ap_ptr rb_ap_copy(const rb_ap_ptr a);
void rb_ap_free(rb_ap_ptr a);

void rb_ap_set_base(rb_ap_ptr a, int base);
void rb_ap_set_prec(rb_ap_ptr a, rb_prec_t prec);
void rb_ap_set_sign(rb_ap_ptr a, char sign);

/* ===== Parsing / serialization ===== */

rb_status rb_parse_text(const char* text, rb_ap_ptr* out);
rb_status rb_parse_buffer(rb_buffer* in, rb_ap_ptr* out);

rb_status rb_ap_to_text(const rb_ap_ptr a, char** out_text);
rb_status rb_ap_to_buffer(const rb_ap_ptr a, rb_buffer* out);

/* ===== Comparison ===== */

int rb_ap_cmp(const rb_ap_ptr a, const rb_ap_ptr b);
int rb_ap_eq(const rb_ap_ptr a, const rb_ap_ptr b);

/* ===== Arithmetic ===== */

rb_status rb_add(const rb_ap_ptr a, const rb_ap_ptr b, rb_ap_ptr* out);
rb_status rb_sub(const rb_ap_ptr a, const rb_ap_ptr b, rb_ap_ptr* out);
rb_status rb_mul(const rb_ap_ptr a, const rb_ap_ptr b, rb_ap_ptr* out);
rb_status rb_div(const rb_ap_ptr a, const rb_ap_ptr b, rb_ap_ptr* out);
rb_status rb_pow(const rb_ap_ptr a, const rb_ap_ptr b, rb_ap_ptr* out);
rb_status rb_root(const rb_ap_ptr a, const rb_ap_ptr b, rb_ap_ptr* out);

/* ===== Primitive numeric routines ===== */

rb_status rb_sin_approx(const rb_ap_ptr angle, rb_ap_ptr* out);
rb_status rb_cos_approx(const rb_ap_ptr angle, rb_ap_ptr* out);
rb_status rb_tan_approx(const rb_ap_ptr angle, rb_ap_ptr* out);
rb_status rb_log_approx(const rb_ap_ptr x, rb_ap_ptr* out);

/* ===== Memory contract ===== */

void* rb_alloc(size_t nbytes);
void* rb_calloc(size_t count, size_t size);
void rb_free(void* ptr);

void rb_set_allocators(
    void* (*alloc_fn)(size_t),
    void* (*calloc_fn)(size_t, size_t),
    void (*free_fn)(void*)
);

#ifdef __cplusplus
}
#endif

#endif
