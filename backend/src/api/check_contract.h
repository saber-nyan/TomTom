#ifndef TOMTOM_BACKEND_CHECK_CONTRACT_H
#define TOMTOM_BACKEND_CHECK_CONTRACT_H

#include <stdbool.h>
#include <libwebsockets.h>
#include <libpq-fe.h>
#include <openssl/rand.h>

#include "request_api_verifier.h"
#include "response_api_builder.h"
#include "../utils.h"
#include "../globals.h"

void check_contract(struct pss* pss, TomTomBackend_Request_CheckContract_table_t messageContent);

#endif //TOMTOM_BACKEND_CHECK_CONTRACT_H
