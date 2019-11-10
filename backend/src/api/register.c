#include "register.h"

void _register(struct pss* pss, TomTomBackend_Request_Register_table_t messageContent) {
	const char* username = TomTomBackend_Request_Register_username_get(messageContent);
	const char* password = TomTomBackend_Request_Register_password_get(messageContent);
	const char* firstName = TomTomBackend_Request_Register_firstName_get(messageContent);
	const char* lastName = TomTomBackend_Request_Register_lastName_get(messageContent);

	unsigned char* access_token = calloc(32, sizeof(unsigned char));
	unsigned char* token_encoded;
	RAND_bytes(access_token, 32);
	token_encoded = b64_encode(access_token, 32, NULL);

	unsigned char* pwd_hash = calloc(64, sizeof(unsigned char));
	unsigned char* pwd_hash_encoded;

	PKCS5_PBKDF2_HMAC_SHA1(password, -1, salt, strlen(salt), 1024, 64, pwd_hash);

	pwd_hash_encoded = b64_encode(pwd_hash, 64, NULL);

	const char* params[5] = {
			username,
			pwd_hash_encoded,
			firstName,
			lastName,
			token_encoded
	};

	PGresult* res = PQexecParams(db_conn, "INSERT INTO users (username, password, firstname, lastname, token)\n"
										  "VALUES ($1, $2, $3, $4, $5)",
								 5, NULL, params, NULL, NULL, 0);

	flatcc_builder_t builder;
	flatcc_builder_t* b = &builder;
	flatcc_builder_init(b);

	TomTomBackend_Response_Message_start_as_root(b);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		TomTomBackend_Response_Message_error_add(b, flatbuffers_true);
		if (PQresultErrorField(res, PG_DIAG_CONSTRAINT_NAME) != NULL) {
			TomTomBackend_Response_Message_errorDescription_create_str(b, "User already exists");
		} else {
			TomTomBackend_Response_Message_errorDescription_create_str(b, PQresultErrorMessage(res));
		}
	} else {
		TomTomBackend_Response_Message_error_add(b, flatbuffers_false);

		TomTomBackend_Response_Register_ref_t msg_r = TomTomBackend_Response_Register_create(
				b,
				flatbuffers_string_create_str(
						b,
						token_encoded
				)
		);

		TomTomBackend_Response_MessageType_union_ref_t msg_u = TomTomBackend_Response_MessageType_as_Register(msg_r);
		TomTomBackend_Response_Message_msg_add(b, msg_u);
	}
	TomTomBackend_Response_Message_end_as_root(b);

	size_t out_size;
	void* out_buf = flatcc_builder_finalize_aligned_buffer(b, &out_size);

	PQclear(res);
	free(access_token);
	free(token_encoded);
	free(pwd_hash);
	free(pwd_hash_encoded);
	flatcc_builder_clear(b);

	pss->out_data = out_buf;
	pss->out_data_len = out_size;
}

