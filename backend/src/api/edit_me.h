#ifndef TOMTOM_BACKEND_EDIT_ME_H
#define TOMTOM_BACKEND_EDIT_ME_H

#include <stdbool.h>
#include <libwebsockets.h>
#include <libpq-fe.h>
#include <openssl/rand.h>

#include "request_api_verifier.h"
#include "response_api_builder.h"
#include "../utils.h"
#include "../globals.h"
#include "../utils.h"

void edit_me(struct pss* pss, TomTomBackend_Request_EditMe_table_t messageContent);

#endif //TOMTOM_BACKEND_EDIT_ME_H
