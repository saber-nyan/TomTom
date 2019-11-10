#ifndef TOMTOM_BACKEND_REGISTER_H
#define TOMTOM_BACKEND_REGISTER_H

#include <libpq-fe.h>
#include <openssl/rand.h>

#include "request_api_verifier.h"
#include "response_api_builder.h"
#include "../utils.h"
#include "../globals.h"

void _register(struct pss* pss, TomTomBackend_Request_Register_table_t messageContent);

#endif //TOMTOM_BACKEND_REGISTER_H
