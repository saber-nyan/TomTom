#ifndef TOMTOM_BACKEND_GLOBALS_H
#define TOMTOM_BACKEND_GLOBALS_H

#define __unused __attribute__((unused))

#define EXIT_FAILURE_ARGS 1
#define EXIT_FAILURE_SERVER_INIT 2
#define EXIT_FAILURE_SERVER_INIT_VHOST 4
#define EXIT_FAILURE_SERVER_INIT_DB 8

#ifdef DEBUG
#define log_debug(...) lwsl_user(__VA_ARGS__)
#else
#define log_debug(...) do {} while(0)
#endif

PGconn* db_conn;

unsigned char* salt;

struct pss {
	char* data;
	size_t data_len;
	unsigned char* out_data;
	long long out_data_len;

	int i;
};

#endif //TOMTOM_BACKEND_GLOBALS_H
