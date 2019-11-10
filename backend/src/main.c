#include "main.h"

int main(int argc, char* argv[]) {
	signal(SIGINT, sigint_handler);
	signal(SIGTERM, sigint_handler);

	struct lws_context_creation_info info;
	int n = 0, logs = LLL_ERR | LLL_WARN;
	int port = 8000;
	void* retval;
	int threads_num = 8;
	char* db_conn_str = calloc(2048, 1);
	strncpy(db_conn_str, "user=tomtom password=program_for_food_help host=127.0.0.1 dbname=tomtom", 2048);
	salt = calloc(2048, 1);
	strncpy(salt, "weak", 2048);

	int c;
	while (true) {
		int option_index = 0;
		c = getopt_long(argc, argv, short_options, long_options, &option_index);
		if (c == -1) break;

		switch (c) {
			case 'p': {
				char* tmp_ptr = NULL;
				errno = 0;
				port = (int) strtol(optarg, &tmp_ptr, 10);
				if (tmp_ptr == optarg || errno == ERANGE || errno == EINVAL || port < 1 || port > 65536) {
					lwsl_err("Incorrect port\n");
					free(db_conn_str);
					free(salt);
					return EXIT_FAILURE_ARGS;
				}
				break;
			}
			case 'd':
				strncpy(db_conn_str, optarg, 2048);
				break;
			case 'v':
				logs |= LLL_USER | LLL_NOTICE;
				break;
			case 's':
				strncpy(salt, optarg, 2048);
				break;
			case 't': {
				char* tmp_ptr = NULL;
				errno = 0;
				threads_num = (int) strtol(optarg, &tmp_ptr, 10);
				if (tmp_ptr == optarg || errno == ERANGE || errno == EINVAL || threads_num < 1) {
					lwsl_err("Incorrect number of threads\n");
					free(db_conn_str);
					free(salt);
					return EXIT_FAILURE_ARGS;
				}
				break;
			}
			default:
				free(db_conn_str);
				free(salt);
				return EXIT_FAILURE_ARGS;
		}
	}

	pthread_t* pthread_service = calloc(threads_num, sizeof(pthread_t));

	lws_set_log_level(logs, NULL);

	lwsl_notice("libpq version: %d\n", PQlibVersion());
	db_conn = PQconnectdb(db_conn_str);
	free(db_conn_str);
	if (PQstatus(db_conn) != CONNECTION_OK) {
		lwsl_err("Failed to connect to PostgreSQL: %s\n", PQerrorMessage(db_conn));
		free(salt);
		return EXIT_FAILURE_SERVER_INIT_DB;
	}
	lwsl_user("Test DB connection succeed\n");

	PGresult* res = PQexec(db_conn, "DROP INDEX IF EXISTS \"idx_contracts__user\" CASCADE;\n"
									"DROP TABLE IF EXISTS \"contracts\", \"users\" CASCADE;\n"
									"\n"
									"CREATE TABLE \"users\"\n(\n"
									"\t\"id\"        SERIAL PRIMARY KEY,\n"
									"\t\"username\"  VARCHAR(64) UNIQUE NOT NULL,\n"
									"\t\"password\"  VARCHAR(128)       NOT NULL,\n"
									"\t\"firstname\" VARCHAR(32)        NOT NULL,\n"
									"\t\"lastname\"  VARCHAR(32)        NOT NULL,\n"
									"\t\"admin\"     BOOL DEFAULT FALSE NOT NULL,\n"
									"\t\"token\"     VARCHAR(128)       NOT NULL\n"
									");\n"
									"\n"
									"CREATE TABLE \"contracts\"\n(\n"
									"\t\"id\"          SERIAL PRIMARY KEY,\n"
									"\t\"name\"        VARCHAR(64)      NOT NULL,\n"
									"\t\"description\" TEXT,\n"
									"\t\"price\"       DOUBLE PRECISION NOT NULL,\n"
									"\t\"attachment\"  BYTEA,\n"
									"\t\"status\"      SMALLINT         NOT NULL DEFAULT 1,\n"
									"\t\"user\"        BIGINT           NOT NULL,\n"
									"\t\"created_at\"  BIGINT           NOT NULL\n"
									");\n"
									"\n"
									"CREATE INDEX \"idx_contracts__user\" ON \"contracts\" (\"user\");\n"
									"\n"
									"ALTER TABLE \"contracts\"\n\tADD CONSTRAINT \"fk_contracts__user\" FOREIGN KEY (\"user\") REFERENCES \"users\" (\"id\") ON DELETE CASCADE");

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		lwsl_err("DB command execution error: %s\n", PQresultErrorMessage(res));
		free(salt);
		PQclear(res);
		return EXIT_FAILURE_SERVER_INIT_DB;
	}
	PQclear(res);

	memset(&info, 0, sizeof info);
	info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT
				   | LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE
				   | LWS_SERVER_OPTION_DISABLE_OS_CA_CERTS
				   | LWS_SERVER_OPTION_EXPLICIT_VHOSTS;
	info.count_threads = threads_num;

	context = lws_create_context(&info);
	if (!context) {
		lwsl_err("libwebsockets init failed!\n");
		free(pthread_service);
		free(salt);
		return EXIT_FAILURE_SERVER_INIT;
	}

	lwsl_notice("Service threads: %d\n", lws_get_count_threads(context));

#ifdef DEBUG
	// HTTP
	info.port = port - 1;
	info.pprotocols = pprotocols;
	info.mounts = &mount;
	info.vhost_name = "http";

	if (!lws_create_vhost(context, &info)) {
		lwsl_err("Failed to create http vhost!\n");
		lws_context_destroy(context);
		free(pthread_service);
		free(salt);
		return EXIT_FAILURE_SERVER_INIT_VHOST;
	}
	lwsl_notice("HTTP server started at http://0.0.0.0:%d\n", port - 1);
#endif


	// HTTPS
	info.port = port;
	info.pprotocols = pprotocols;
	info.mounts = &mount;
	info.vhost_name = "https";
	info.server_ssl_cert_mem = __cert_der;
	info.server_ssl_cert_mem_len = __cert_der_len;
	info.server_ssl_private_key_mem = __key_der;
	info.server_ssl_private_key_mem_len = __key_der_len;

	if (!lws_create_vhost(context, &info)) {
		lwsl_err("Failed to create https vhost!\n");
		lws_context_destroy(context);
		free(pthread_service);
		free(salt);
		return EXIT_FAILURE_SERVER_INIT_VHOST;
	}
	lwsl_notice("HTTPS server started at https://0.0.0.0:%d\n", port);

	/* start all the service threads */
	for (n = 0; n < lws_get_count_threads(context); n++) {
		if (pthread_create(&pthread_service[n], NULL, thread_service, (void*) (lws_intptr_t) n)) {
			lwsl_err("Failed to start service thread\n");
		}
	}

	/* wait for all the service threads to exit */
	while ((--n) >= 0)
		pthread_join(pthread_service[n], &retval);

	lws_context_destroy(context);
	free(pthread_service);
	free(salt);
	PQfinish(db_conn);

	return EXIT_SUCCESS;
}

static int http_callback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len) {
//	log_debug("Callback hit, reason: %d, len: %lu\n", reason, len);
	struct pss* pss = (struct pss*) user;
	uint8_t buf[LWS_PRE + LWS_RECOMMENDED_MIN_HEADER_SPACE];
	uint8_t* start = &buf[LWS_PRE];
	uint8_t* p = start;
	uint8_t* end = &buf[sizeof(buf) - LWS_PRE - 1];
	enum lws_write_protocol n;

	switch (reason) {
		case LWS_CALLBACK_HTTP:
			log_debug("  On LWS_CALLBACK_HTTP!\n");
			if (strcmp((const char*) in, "api") != 0) { // 404 if not API
				log_debug("Wrong path, skipping...\n");
				return -1;
			}

			if (!lws_hdr_total_length(wsi, WSI_TOKEN_POST_URI)) {
				log_debug("Not a POST, skipping...\n");
				return -1;
			}

			if (lws_add_http_common_headers(wsi, HTTP_STATUS_OK,
											"text/html",
											LWS_ILLEGAL_HTTP_CONTENT_LEN, /* no content len */
											&p, end)) {
				log_debug("Failed to add headers, skipping...\n");
				return 1;
			}
			if (lws_finalize_write_http_header(wsi, start, &p, end)) return 1;

			memset(&pss, 0, sizeof(pss));
			return 0;
		case LWS_CALLBACK_HTTP_BODY:
			log_debug("  On LWS_CALLBACK_HTTP_BODY!\n");
			if (pss->data == NULL) { // load first 4KB chunk
				pss->data = calloc(len, 1);
				pss->data_len = len;
				memcpy(pss->data, in, len);
			} else { // append 4KB chunk to existing data
				pss->data = realloc(pss->data, pss->data_len + len);
				memcpy(pss->data + pss->data_len, in, len);
				pss->data_len += len;
			}
			break;
		case LWS_CALLBACK_HTTP_BODY_COMPLETION:
			log_debug("  On LWS_CALLBACK_HTTP_BODY_COMPLETION!\n");
			pss->data[pss->data_len - 1] = '\0'; // unneeded with binary data?

			process(pss);

			lws_callback_on_writable(wsi);
			free(pss->data);
			pss->data = NULL;
			return 0;
		case LWS_CALLBACK_HTTP_WRITEABLE:
			log_debug("  On LWS_CALLBACK_HTTP_WRITEABLE!\n");
			if (!pss || !pss->out_data) return -1;

			// This callback get called multiple times on large responses,
			// so we write data chunk by chunk (usually 2KB)
			unsigned long chunk_size = end - p;
			const unsigned long current_offset = chunk_size * pss->i;
			if (pss->out_data_len - (long long) chunk_size < 0) {
				chunk_size = pss->out_data_len;
			}
			pss->out_data_len -= chunk_size;

			n = (pss->out_data_len <= 0) ? LWS_WRITE_HTTP_FINAL : LWS_WRITE_HTTP;

			if (!lws_write(wsi, pss->out_data + current_offset, chunk_size, n)) {
				log_debug("\tlws_write failed, skipping...\n");
				return 1;
			}

			++pss->i;

			if (n == LWS_WRITE_HTTP_FINAL) {
				if (lws_http_transaction_completed(wsi)) {
					log_debug("lws_http_transaction_completed failed, skipping...\n");
					return -1;
				}
			} else {
				lws_callback_on_writable(wsi);
			}

			break;
		case LWS_CALLBACK_HTTP_DROP_PROTOCOL:
			free(pss->data);
			flatcc_builder_aligned_free(pss->out_data);
			pss->data = NULL;
			pss->out_data = NULL;
			return 0;
		default:
//			log_debug("Not implemented\n");
			break;
	}

	return lws_callback_http_dummy(wsi, reason, user, in, len);
}

void* thread_service(void* threadid) {
	while (lws_service_tsi(context, 10000,
						   (int) (lws_intptr_t) threadid) >= 0
		   && !interrupted);

	pthread_exit(NULL);
}

void sigint_handler(int __unused sig) {
	lwsl_warn("Interrupted!\n");
	interrupted = true;
	lws_cancel_service(context);
}
