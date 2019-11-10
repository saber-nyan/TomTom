#ifndef TOMTOM_BACKEND_MAIN_H
#define TOMTOM_BACKEND_MAIN_H

#include <stdio.h>
#include <stdbool.h>
#include <libwebsockets.h>
#include <getopt.h>
#include <libpq-fe.h>
#include <signal.h>

#include "globals.h"
#include "cert.h"
#include "processing.h"

static int http_callback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len);

void* thread_service(void* threadid);

void sigint_handler(int __unused sig);

static bool interrupted = false;

static struct lws_context* context;

static const struct lws_protocols protocol = {
		"http",
		http_callback,
		sizeof(struct pss),
		0
};

static const struct lws_protocols* pprotocols[] = {
		&protocol,
		NULL
};

static const struct lws_http_mount mount = {
		NULL,        /* linked-list "next" */
		"/",        /* mountpoint URL */
		NULL, /* serve from dir */
		NULL,    /* default filename */
		"http",
		NULL,
		NULL,
		NULL,
		0,
		0,
		0,
		0,
		0,
		0,
		LWSMPRO_CALLBACK,    /* files in a dir */
		1,        /* char count */
		NULL,
};

static const char short_options[] = "vp:t:d:s:";

static const struct option long_options[] = {
		{"verbose", no_argument,       0, 'v'},
		{"port",    required_argument, 0, 'p'},
		{"threads", required_argument, 0, 't'},
		{"db",      required_argument, 0, 'd'},
		{"salt",    required_argument, 0, 's'},
		{0, 0,                         0, 0}
};

#endif //TOMTOM_BACKEND_MAIN_H
