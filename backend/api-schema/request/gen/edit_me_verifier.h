#ifndef EDIT_ME_VERIFIER_H
#define EDIT_ME_VERIFIER_H

/* Generated by flatcc 0.6.1-dev FlatBuffers schema compiler for C by dvide.com */

#ifndef EDIT_ME_READER_H
#include "edit_me_reader.h"
#endif
#include "flatcc/flatcc_verifier.h"
#include "flatcc/flatcc_prologue.h"

static int TomTomBackend_Request_EditMe_verify_table(flatcc_table_verifier_descriptor_t *td);

static int TomTomBackend_Request_EditMe_verify_table(flatcc_table_verifier_descriptor_t *td)
{
    int ret;
    if ((ret = flatcc_verify_string_field(td, 0, 0) /* username */)) return ret;
    if ((ret = flatcc_verify_string_field(td, 1, 0) /* password */)) return ret;
    if ((ret = flatcc_verify_string_field(td, 2, 0) /* firstName */)) return ret;
    if ((ret = flatcc_verify_string_field(td, 3, 0) /* lastName */)) return ret;
    if ((ret = flatcc_verify_field(td, 4, 1, 1) /* admin */)) return ret;
    if ((ret = flatcc_verify_string_field(td, 5, 1) /* token */)) return ret;
    return flatcc_verify_ok;
}

static inline int TomTomBackend_Request_EditMe_verify_as_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, TomTomBackend_Request_EditMe_identifier, &TomTomBackend_Request_EditMe_verify_table);
}

static inline int TomTomBackend_Request_EditMe_verify_as_typed_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, TomTomBackend_Request_EditMe_type_identifier, &TomTomBackend_Request_EditMe_verify_table);
}

static inline int TomTomBackend_Request_EditMe_verify_as_root_with_identifier(const void *buf, size_t bufsiz, const char *fid)
{
    return flatcc_verify_table_as_root(buf, bufsiz, fid, &TomTomBackend_Request_EditMe_verify_table);
}

static inline int TomTomBackend_Request_EditMe_verify_as_root_with_type_hash(const void *buf, size_t bufsiz, flatbuffers_thash_t thash)
{
    return flatcc_verify_table_as_typed_root(buf, bufsiz, thash, &TomTomBackend_Request_EditMe_verify_table);
}

#include "flatcc/flatcc_epilogue.h"
#endif /* EDIT_ME_VERIFIER_H */
