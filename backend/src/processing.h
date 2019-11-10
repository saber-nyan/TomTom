#ifndef TOMTOM_BACKEND_PROCESSING_H
#define TOMTOM_BACKEND_PROCESSING_H

#include <stddef.h>
#include <stdbool.h>
#include <libwebsockets.h>
#include <libpq-fe.h>
#include <openssl/rand.h>

#include "request_api_verifier.h"
#include "response_api_builder.h"
#include "utils.h"

#include "api/register.h"
#include "api/login.h"
#include "api/get_me.h"
#include "api/edit_me.h"
#include "api/request_contract.h"
#include "api/list_contracts.h"
#include "api/check_contract.h"

void process(struct pss* pss);

#endif //TOMTOM_BACKEND_PROCESSING_H
