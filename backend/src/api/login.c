#include "login.h"

void login(struct pss* pss, TomTomBackend_Request_Login_table_t messageContent) {
	const char* username = TomTomBackend_Request_Login_username_get(messageContent);
	const char* password = TomTomBackend_Request_Login_password_get(messageContent);

	unsigned char* pwd_hash = calloc(64, sizeof(unsigned char));
	unsigned char* pwd_hash_encoded;

	PKCS5_PBKDF2_HMAC_SHA1(password, -1, salt, strlen(salt), 1024, 64, pwd_hash);

	pwd_hash_encoded = b64_encode(pwd_hash, 64, NULL);

//	const char* params[2] = {
//			username,
//			pwd_hash_encoded
//	};
//
//	PGresult* res = PQexecParams(db_conn, "SELECT token FROM users WHERE (username=$1 AND password=$2)",
//								 2, NULL, params, NULL, NULL, 0);

	char* query = calloc(8192, 1);
	snprintf(query, 8192, "SELECT token FROM users WHERE (username='%s' AND password='%s')",
			 username, pwd_hash_encoded); // VULNERABILITY: SELECT SQLi (blind)
	PGresult* res = PQexec(db_conn, query);

	flatcc_builder_t builder;
	flatcc_builder_t* b = &builder;
	flatcc_builder_init(b);

	TomTomBackend_Response_Message_start_as_root(b);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		log_debug("Unknown DB fail\n");
		TomTomBackend_Response_Message_error_add(b, flatbuffers_true);
//		TomTomBackend_Response_Message_errorDescription_create_str(b, PQresultErrorMessage(res));
		TomTomBackend_Response_Message_errorDescription_create_str(b, "Failed");
	} else {
		if (PQntuples(res) <= 0) {
			log_debug("No such username/password pair\n");
			TomTomBackend_Response_Message_error_add(b, flatbuffers_true);
			TomTomBackend_Response_Message_errorDescription_create_str(b, "Wrong username or password");
		} else {
			const char* token = PQgetvalue(res, 0, 0); // DO NOT FREE, COPY!
			TomTomBackend_Response_Message_error_add(b, flatbuffers_false);

			TomTomBackend_Response_Login_ref_t msg_r = TomTomBackend_Response_Login_create(
					b,
					flatbuffers_string_create_str(
							b,
							token
					)
			);

			TomTomBackend_Response_MessageType_union_ref_t msg_u = TomTomBackend_Response_MessageType_as_Login(msg_r);
			TomTomBackend_Response_Message_msg_add(b, msg_u);
		}
	}
	TomTomBackend_Response_Message_end_as_root(b);

	size_t out_size;
	void* out_buf = flatcc_builder_finalize_aligned_buffer(b, &out_size);

	PQclear(res);
	free(pwd_hash);
	free(pwd_hash_encoded);
	flatcc_builder_clear(b);

	pss->out_data = out_buf;
	pss->out_data_len = out_size;
}
