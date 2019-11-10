#include "edit_me.h"

void edit_me(struct pss* pss, TomTomBackend_Request_EditMe_table_t messageContent) {
	const char* token = TomTomBackend_Request_EditMe_token_get(messageContent);

	const int len = 16384;
	char* query = calloc(len, 1);
	bool regenerate_token = false;
	strncpy(query, "UPDATE users SET admin = ", len); // VULNERABILITY: user can set admin flag on himself!

	// Primitive type bool is required in flatbuff
	strlcat(query, TomTomBackend_Request_EditMe_admin_get(messageContent) ? "TRUE" : "FALSE", len);

	if (TomTomBackend_Request_EditMe_username_is_present(messageContent)) {
		char* username = PQescapeLiteral(db_conn, TomTomBackend_Request_EditMe_username_get(messageContent), 64);
		strlcat(query, ", username = ", len);
		strlcat(query, username, len);

		regenerate_token = true;

		free(username);
	}
	if (TomTomBackend_Request_EditMe_password_is_present(messageContent)) {
		const char* password = TomTomBackend_Request_EditMe_password_get(messageContent);
		unsigned char* pwd_hash = calloc(64, sizeof(unsigned char));
		char* pwd_hash_encoded;

		PKCS5_PBKDF2_HMAC_SHA1(password, -1, salt, strlen(salt), 1024, 64, pwd_hash);
		pwd_hash_encoded = PQescapeLiteral(
				db_conn,
				b64_encode(pwd_hash, 64, NULL),
				128
		);
		strlcat(query, ", password = ", len);
		strlcat(query, pwd_hash_encoded, len);

		regenerate_token = true;

		free(pwd_hash);
		free(pwd_hash_encoded);
	}
	if (TomTomBackend_Request_EditMe_firstName_is_present(messageContent)) {
		char* first_name = PQescapeLiteral(db_conn, TomTomBackend_Request_EditMe_firstName_get(messageContent), 32);

		strlcat(query, ", firstname = ", len);
		strlcat(query, first_name, len);

		free(first_name);
	}
	if (TomTomBackend_Request_EditMe_lastName_is_present(messageContent)) {
		char* last_name = PQescapeLiteral(db_conn, TomTomBackend_Request_EditMe_lastName_get(messageContent), 32);

		strlcat(query, ", lastname = ", len);
		strlcat(query, last_name, len);

		free(last_name);
	}

	char* new_token;
	if (regenerate_token) {
		unsigned char* access_token = calloc(32, sizeof(unsigned char));
		char* token_encoded;
		RAND_bytes(access_token, 32);
		new_token = b64_encode(access_token, 32, NULL);
		token_encoded = PQescapeLiteral(db_conn, new_token, 128);

		strlcat(query, ", token = ", len);
		strlcat(query, token_encoded, len);
		free(access_token);
		free(token_encoded);
	} else {
		new_token = calloc(32, 1);
		strncpy(new_token, token, 32);
	}

	char* escaped_old_token = PQescapeLiteral(db_conn, token, 128);
	strlcat(query, " WHERE token = ", len);
	strlcat(query, escaped_old_token, len);
	free(escaped_old_token);

	log_debug("Final query: %s\n", query);
	PGresult* res = PQexec(db_conn, query);

	free(query);

	flatcc_builder_t builder;
	flatcc_builder_t* b = &builder;
	flatcc_builder_init(b);

	TomTomBackend_Response_Message_start_as_root(b);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		TomTomBackend_Response_Message_error_add(b, flatbuffers_true);
		if (PQresultErrorField(res, PG_DIAG_CONSTRAINT_NAME) != NULL) {
			TomTomBackend_Response_Message_errorDescription_create_str(b, "User with that username already exists");
		} else {
			TomTomBackend_Response_Message_errorDescription_create_str(b, PQresultErrorMessage(res));
		}
	} else {
		TomTomBackend_Response_Message_error_add(b, flatbuffers_false);

		TomTomBackend_Response_EditMe_ref_t msg_r = TomTomBackend_Response_EditMe_create(
				b,
				flatbuffers_string_create_str(
						b,
						new_token
				)
		);

		TomTomBackend_Response_MessageType_union_ref_t msg_u = TomTomBackend_Response_MessageType_as_EditMe(msg_r);
		TomTomBackend_Response_Message_msg_add(b, msg_u);
	}
	TomTomBackend_Response_Message_end_as_root(b);

	size_t out_size;
	void* out_buf = flatcc_builder_finalize_aligned_buffer(b, &out_size);

	PQclear(res);
	free(new_token);
	flatcc_builder_clear(b);

	pss->out_data = out_buf;
	pss->out_data_len = out_size;
}
