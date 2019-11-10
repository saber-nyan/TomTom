#ifndef TOMTOM_BACKEND_UTILS_H
#define TOMTOM_BACKEND_UTILS_H

#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

static const unsigned char base64_table[65]
		= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

unsigned char* b64_encode(const unsigned char* src, size_t len, size_t* out_len);

unsigned char* b64_decode(const unsigned char* src, size_t len, size_t* out_len);

size_t
strlcat(char* dst,
		const char* src,
		size_t size);

#endif //TOMTOM_BACKEND_UTILS_H
