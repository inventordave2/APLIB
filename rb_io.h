#ifndef RB_IO_H
#define RB_IO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rb_core.h"

/* Host-layer adapter API. The core never owns raw OS handles. */
rb_status rb_io_load_argc_argv(int argc, char** argv, rb_buffer* out);
rb_status rb_io_read_file(const char* path, rb_buffer* out);
rb_status rb_io_write_file(const char* path, const rb_ap_ptr a);

#ifdef __cplusplus
}
#endif

#endif
