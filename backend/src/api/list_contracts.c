#include "list_contracts.h"

void list_contracts(struct pss* pss, TomTomBackend_Request_ListContracts_table_t messageContent) {
	const char* token = TomTomBackend_Request_ListContracts_token_get(messageContent);

	PGresult* res = PQexecParams(db_conn, "SELECT id, admin FROM users WHERE (token=$1)",
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
	} else if (PQntuples(res) <= 0) {
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

	char* user_id = calloc(128, 1);
	strncpy(user_id, PQgetvalue(res, 0, 0), 128);
	char* is_admin = PQgetvalue(res, 0, 1);


	if (*is_admin == 't') {
		PQclear(res);
		res = PQexec(db_conn,
					 "SELECT name,\n"
					 "\t   description,\n"
					 "\t   price,\n"
					 "\t   attachment,\n"
					 "\t   status,\n"
					 "\t   created_at,\n"
					 "\t   contracts.id,\n"
					 "\t   u.username,\n"
					 "\t   u.firstname,\n"
					 "\t   u.lastname\n"
					 "FROM contracts\n"
					 "\t\t INNER JOIN users u on contracts.\"user\" = u.id\n"
					 "ORDER BY -created_at\n"
					 "LIMIT 50");
	} else {
		PQclear(res);
		res = PQexecParams(db_conn,
						   "SELECT name,\n"
						   "\t   description,\n"
						   "\t   price,\n"
						   "\t   attachment,\n"
						   "\t   status,\n"
						   "\t   created_at,\n"
						   "\t   contracts.id,\n"
						   "\t   u.username,\n"
						   "\t   u.firstname,\n"
						   "\t   u.lastname\n"
						   "FROM contracts\n"
						   "\t\t INNER JOIN users u on contracts.\"user\" = u.id\nWHERE (\"user\" = $1)\n"
						   "ORDER BY -created_at\n"
						   "LIMIT 50",
						   1, NULL, (const char* const*) &user_id, NULL, NULL, 0);
	}
	free(user_id);
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

	TomTomBackend_Response_CheckContract_vec_start(b);
	size_t rows_num = PQntuples(res);
	for (size_t i = 0; i < rows_num; ++i) {
		TomTomBackend_Response_CheckContract_start(b);
		char* name = PQgetvalue(res, i, 0);
		TomTomBackend_Response_CheckContract_name_add(b, flatbuffers_string_create_str(b, name));
		if (PQgetisnull(res, i, 1) == 0) { // is not NULL
			char* description = PQgetvalue(res, i, 1);
			TomTomBackend_Response_CheckContract_description_add(b, flatbuffers_string_create_str(b, description));
		}
		char* price = PQgetvalue(res, i, 2);
		TomTomBackend_Response_CheckContract_price_add(b, atof(price));
		if (PQgetisnull(res, i, 3) == 0) {
			size_t out_len = 0;
			char* attach = PQgetvalue(res, i, 3);
			unsigned char* unescaped = PQunescapeBytea(attach, &out_len);
			flatbuffers_uint8_vec_ref_t attachments_out = flatbuffers_uint8_vec_create(b, unescaped, out_len);
			TomTomBackend_Response_CheckContract_attachment_add(b, attachments_out);
			free(unescaped);
		}
		char* status = PQgetvalue(res, i, 4);
		TomTomBackend_Response_CheckContract_status_add(b, atoi(status));
		char* created_at = PQgetvalue(res, i, 5);
		TomTomBackend_Response_CheckContract_created_at_add(b, atol(created_at));

		// contracts.id
		char* id = PQgetvalue(res, i, 6);
		TomTomBackend_Response_CheckContract_id_add(b, atoll(id));

		TomTomBackend_Response_User_start(b);
		char* username = PQgetvalue(res, i, 7);
		TomTomBackend_Response_User_username_add(b, flatbuffers_string_create_str(b, username));
		char* first_name = PQgetvalue(res, i, 8);
		TomTomBackend_Response_User_firstName_add(b, flatbuffers_string_create_str(b, first_name));
		char* last_name = PQgetvalue(res, i, 9);
		TomTomBackend_Response_User_lastName_add(b, flatbuffers_string_create_str(b, last_name));
		TomTomBackend_Response_User_ref_t user = TomTomBackend_Response_User_end(b);

		TomTomBackend_Response_CheckContract_by_user_add(b, user);

		TomTomBackend_Response_CheckContract_ref_t element = TomTomBackend_Response_CheckContract_end(b);
		TomTomBackend_Response_CheckContract_vec_push(b, element);
	}
	TomTomBackend_Response_CheckContract_ref_t array = TomTomBackend_Response_CheckContract_vec_end(b);

	TomTomBackend_Response_ListContracts_ref_t msg_r = TomTomBackend_Response_ListContracts_create(
			b,
			array
	);

	TomTomBackend_Response_MessageType_union_ref_t msg_u = TomTomBackend_Response_MessageType_as_ListContracts(
			msg_r);
	TomTomBackend_Response_Message_msg_add(b, msg_u);
	TomTomBackend_Response_Message_end_as_root(b);

	size_t out_size;
	void* out_buf = flatcc_builder_finalize_aligned_buffer(b, &out_size);

	PQclear(res);
	flatcc_builder_clear(b);

	pss->out_data = out_buf;
	pss->out_data_len = out_size;
}
