#ifndef GET_ME_READER_H
#define GET_ME_READER_H

/* Generated by flatcc 0.6.1-dev FlatBuffers schema compiler for C by dvide.com */

#ifndef FLATBUFFERS_COMMON_READER_H
#include "flatbuffers_common_reader.h"
#endif
#include "flatcc/flatcc_flatbuffers.h"
#ifndef __alignas_is_defined
#include <stdalign.h>
#endif
#include "flatcc/flatcc_prologue.h"
#ifndef flatbuffers_identifier
#define flatbuffers_identifier 0
#endif
#ifndef flatbuffers_extension
#define flatbuffers_extension ".bin"
#endif


typedef const struct TomTomBackend_Request_GetMe_table *TomTomBackend_Request_GetMe_table_t;
typedef struct TomTomBackend_Request_GetMe_table *TomTomBackend_Request_GetMe_mutable_table_t;
typedef const flatbuffers_uoffset_t *TomTomBackend_Request_GetMe_vec_t;
typedef flatbuffers_uoffset_t *TomTomBackend_Request_GetMe_mutable_vec_t;
#ifndef TomTomBackend_Request_GetMe_identifier
#define TomTomBackend_Request_GetMe_identifier flatbuffers_identifier
#endif
#define TomTomBackend_Request_GetMe_type_hash ((flatbuffers_thash_t)0x9dd8e14c)
#define TomTomBackend_Request_GetMe_type_identifier "\x4c\xe1\xd8\x9d"



struct TomTomBackend_Request_GetMe_table { uint8_t unused__; };

static inline size_t TomTomBackend_Request_GetMe_vec_len(TomTomBackend_Request_GetMe_vec_t vec)
__flatbuffers_vec_len(vec)
static inline TomTomBackend_Request_GetMe_table_t TomTomBackend_Request_GetMe_vec_at(TomTomBackend_Request_GetMe_vec_t vec, size_t i)
__flatbuffers_offset_vec_at(TomTomBackend_Request_GetMe_table_t, vec, i, 0)
__flatbuffers_table_as_root(TomTomBackend_Request_GetMe)

__flatbuffers_define_string_field(0, TomTomBackend_Request_GetMe, token, 1)


#include "flatcc/flatcc_epilogue.h"
#endif /* GET_ME_READER_H */
