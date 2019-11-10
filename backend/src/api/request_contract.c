#include "request_contract.h"

void request_contract(struct pss* pss, TomTomBackend_Request_RequestContract_table_t messageContent) {
	const char* token = TomTomBackend_Request_RequestContract_token_get(messageContent);

	PGresult* res = PQexecParams(db_conn, "SELECT id, username, firstname, lastname FROM users WHERE (token=$1)",
								 1, NULL, &token, NULL, NULL, 0);

	flatcc_builder_t builder;
	flatcc_builder_t* b = &builder;
	flatcc_builder_init(b);

	TomTomBackend_Response_Message_start_as_root(b);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		log_debug("Unknown DB fail\n");
		TomTomBackend_Response_Message_error_add(b, flatbuffers_true);
		TomTomBackend_Response_Message_errorDescription_create_str(b, PQresultErrorMessage(res));
		TomTomBackend_Response_Message_end_as_root(b);
		size_t out_size;
		void* out_buf = flatcc_builder_finalize_aligned_buffer(b, &out_size);

		PQclear(res);
		flatcc_builder_clear(b);

		pss->out_data = out_buf;
		pss->out_data_len = out_size;
		return;
	}
	if (PQntuples(res) <= 0) {
		log_debug("Invalid token\n");
		TomTomBackend_Response_Message_error_add(b, flatbuffers_true);
		TomTomBackend_Response_Message_errorDescription_create_str(b, "Invalid token");
		TomTomBackend_Response_Message_end_as_root(b);
		size_t out_size;
		void* out_buf = flatcc_builder_finalize_aligned_buffer(b, &out_size);

		PQclear(res);
		flatcc_builder_clear(b);

		pss->out_data = out_buf;
		pss->out_data_len = out_size;
		return;
	}

	const char* name = TomTomBackend_Request_RequestContract_name_get(messageContent);
	const double price = TomTomBackend_Request_RequestContract_price_get(messageContent);
	const char* user_id = PQgetvalue(res, 0, 0);
	char* username = calloc(64, 1);
	strncpy(username, PQgetvalue(res, 0, 1), 64);
	char* first_name = calloc(32, 1);
	strncpy(first_name, PQgetvalue(res, 0, 2), 32);
	char* last_name = calloc(32, 1);
	strncpy(last_name, PQgetvalue(res, 0, 3), 32);

	if (price < 0) {
		log_debug("Negative price\n");
		TomTomBackend_Response_Message_error_add(b, flatbuffers_true);
		TomTomBackend_Response_Message_errorDescription_create_str(b, "Price should not be negative");
		TomTomBackend_Response_Message_end_as_root(b);
		free(username);
		free(first_name);
		free(last_name);
		size_t out_size;
		void* out_buf = flatcc_builder_finalize_aligned_buffer(b, &out_size);

		PQclear(res);
		flatcc_builder_clear(b);

		pss->out_data = out_buf;
		pss->out_data_len = out_size;
		return;
	}

	const int len = 2097152;
	char* query = malloc(len);
	strncpy(query,
			"INSERT INTO contracts (name, description, price, attachment, \"user\", created_at)\n"
			"VALUES (", len);

	// NAME
//	char* name_esc = PQescapeLiteral(db_conn, name, 64);
//	strlcat(query, name_esc, len);
//	free(name_esc);
	strlcat(query, "'", len);
	strlcat(query, name, len); // VULNERABILITY: INSERT SQLi
	strlcat(query, "'", len);

	// DESCRIPTION
	const char* description = NULL;
	if (TomTomBackend_Request_RequestContract_description_is_present(messageContent)) {
		description = TomTomBackend_Request_RequestContract_description_get(messageContent);
		char* description_esc = PQescapeLiteral(db_conn, description, 16384);
		strlcat(query, ", ", len);
		strlcat(query, description_esc, len);

		free(description_esc);
	} else {
		strlcat(query, ", NULL", len);
	}

	// PRICE
	strlcat(query, ", ", len);
	char price_str[128];
	snprintf(price_str, 128, "%f", price);
	strlcat(query, price_str, len);

	// ATTACHMENT
	size_t attachments_len = 0;
	flatbuffers_uint8_vec_t attachments = NULL;
	flatbuffers_uint8_vec_ref_t attachments_out;
	if (TomTomBackend_Request_RequestContract_attachment_is_present(messageContent)) {
		attachments = TomTomBackend_Request_RequestContract_attachment_get(messageContent);
		attachments_len = flatbuffers_uint8_vec_len(attachments);
		if (attachments_len == 0) {
			log_debug("Vector is present, but empty!\n");
			goto no_vec; // И да простит меня Б-г...
		}
		if (attachments_len > 131072) {
			lwsl_notice("Vector is too big (%lu), skipping...\n", attachments_len);
			goto no_vec;
		}

		// TODO: imagemagick processing vulnerability?

		size_t out_len = 0;
		strlcat(query, ", '", len);
		unsigned char* escaped = PQescapeByteaConn(db_conn, attachments, attachments_len, &out_len);
		strlcat(query, escaped, len); // Guaranteed to be zero-terminated, dunno why unsigned...
		strlcat(query, "'", len);
		PQfreemem(escaped);
	} else {
		no_vec:
		strlcat(query, ", NULL", len);
		attachments_len = 0;
	}

	// USER
	strlcat(query, ", ", len);
	strlcat(query, user_id, len);

	// CREATED_AT
	uintmax_t created_time = time(NULL);
	char time_str[64];
	snprintf(time_str, 64, "%lu", created_time);
	strlcat(query, ", ", len);
	strlcat(query, time_str, len);
	strlcat(query, ") RETURNING id", len);

	PQclear(res);

	res = PQexec(db_conn, query);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		TomTomBackend_Response_Message_error_add(b, flatbuffers_true);
		TomTomBackend_Response_Message_errorDescription_create_str(b, PQresultErrorMessage(res));
	} else {
		TomTomBackend_Response_Message_error_add(b, flatbuffers_false);

		TomTomBackend_Response_CheckContract_start(b);
		TomTomBackend_Response_CheckContract_id_add(b, atoll(PQgetvalue(res, 0, 0)));
		TomTomBackend_Response_CheckContract_name_add(b, flatbuffers_string_create_str(b, name));
		if (description != NULL) {
			TomTomBackend_Response_CheckContract_description_add(b, flatbuffers_string_create_str(b, description));
		}
		TomTomBackend_Response_CheckContract_price_add(b, price);
		if (attachments_len != 0) {
			attachments_out = flatbuffers_uint8_vec_create(b, attachments, attachments_len);
			TomTomBackend_Response_CheckContract_attachment_add(b, attachments_out);
		}
		TomTomBackend_Response_CheckContract_status_add(b, TomTomBackend_Response_Status_Processing);
		TomTomBackend_Response_CheckContract_created_at_add(b, created_time);

		TomTomBackend_Response_User_start(b);
		TomTomBackend_Response_User_username_add(b, flatbuffers_string_create_str(b, username));
		TomTomBackend_Response_User_firstName_add(b, flatbuffers_string_create_str(b, first_name));
		TomTomBackend_Response_User_lastName_add(b, flatbuffers_string_create_str(b, last_name));
		TomTomBackend_Response_User_ref_t user = TomTomBackend_Response_User_end(b);

		TomTomBackend_Response_CheckContract_by_user_add(b, user);

		TomTomBackend_Response_CheckContract_ref_t msg_inner = TomTomBackend_Response_CheckContract_end(b);

		TomTomBackend_Response_RequestContract_ref_t msg_r = TomTomBackend_Response_RequestContract_create(
				b,
				msg_inner
		);

		TomTomBackend_Response_MessageType_union_ref_t msg_u = TomTomBackend_Response_MessageType_as_RequestContract(
				msg_r);
		TomTomBackend_Response_Message_msg_add(b, msg_u);
	}
	TomTomBackend_Response_Message_end_as_root(b);

	free(query);

	size_t out_size;
	void* out_buf = flatcc_builder_finalize_aligned_buffer(b, &out_size);

	PQclear(res);
	flatcc_builder_clear(b);

	free(username);
	free(first_name);
	free(last_name);

	pss->out_data = out_buf;
	pss->out_data_len = out_size;
}
