#include "check_contract.h"

void check_contract(struct pss* pss, TomTomBackend_Request_CheckContract_table_t messageContent) {
	char* id_str = calloc(32, 1);
	const int64_t id = TomTomBackend_Request_CheckContract_id_get(messageContent);
	snprintf(id_str, 32, "%ld", id);
	PGresult* res = PQexecParams(db_conn, // VULNERABILITY: token is not checked, hacker can brute contracts.id
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
								 "WHERE (contracts.id = $1)",
								 1, NULL, (const char* const*) &id_str,
								 NULL, NULL, 0);
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
		free(id_str);

		pss->out_data = out_buf;
		pss->out_data_len = out_size;
		return;
	} else if (PQntuples(res) <= 0) {
		log_debug("Invalid contracts.id\n");
		TomTomBackend_Response_Message_error_add(b, flatbuffers_true);
		TomTomBackend_Response_Message_errorDescription_create_str(b, "Invalid id");
		TomTomBackend_Response_Message_end_as_root(b);
		size_t out_size;
		void* out_buf = flatcc_builder_finalize_aligned_buffer(b, &out_size);

		PQclear(res);
		flatcc_builder_clear(b);
		free(id_str);

		pss->out_data = out_buf;
		pss->out_data_len = out_size;
		return;
	}

	TomTomBackend_Response_CheckContract_start(b);
	char* name = PQgetvalue(res, 0, 0);
	TomTomBackend_Response_CheckContract_name_add(b, flatbuffers_string_create_str(b, name));
	if (PQgetisnull(res, 0, 1) == 0) { // is not NULL
		char* description = PQgetvalue(res, 0, 1);
		TomTomBackend_Response_CheckContract_description_add(b, flatbuffers_string_create_str(b, description));
	}
	char* price = PQgetvalue(res, 0, 2);
	TomTomBackend_Response_CheckContract_price_add(b, atof(price));
	if (PQgetisnull(res, 0, 3) == 0) {
		size_t out_len = 0;
		char* attach = PQgetvalue(res, 0, 3);
		unsigned char* unescaped = PQunescapeBytea(attach, &out_len);
		flatbuffers_uint8_vec_ref_t attachments_out = flatbuffers_uint8_vec_create(b, unescaped, out_len);
		TomTomBackend_Response_CheckContract_attachment_add(b, attachments_out);
		free(unescaped);
	}
	char* status = PQgetvalue(res, 0, 4);
	TomTomBackend_Response_CheckContract_status_add(b, atoi(status));
	char* created_at = PQgetvalue(res, 0, 5);
	TomTomBackend_Response_CheckContract_created_at_add(b, atol(created_at));

	// contracts.id
	char* contract_id = PQgetvalue(res, 0, 6);
	TomTomBackend_Response_CheckContract_id_add(b, atol(contract_id));

	TomTomBackend_Response_User_start(b);
	char* username = PQgetvalue(res, 0, 7);
	TomTomBackend_Response_User_username_add(b, flatbuffers_string_create_str(b, username));
	char* first_name = PQgetvalue(res, 0, 8);
	TomTomBackend_Response_User_firstName_add(b, flatbuffers_string_create_str(b, first_name));
	char* last_name = PQgetvalue(res, 0, 9);
	TomTomBackend_Response_User_lastName_add(b, flatbuffers_string_create_str(b, last_name));
	TomTomBackend_Response_User_ref_t user = TomTomBackend_Response_User_end(b);

	TomTomBackend_Response_CheckContract_by_user_add(b, user);

	TomTomBackend_Response_CheckContract_ref_t msg_r = TomTomBackend_Response_CheckContract_end(b);


	TomTomBackend_Response_MessageType_union_ref_t msg_u = TomTomBackend_Response_MessageType_as_CheckContract(
			msg_r);
	TomTomBackend_Response_Message_msg_add(b, msg_u);
	TomTomBackend_Response_Message_end_as_root(b);

	size_t out_size;
	void* out_buf = flatcc_builder_finalize_aligned_buffer(b, &out_size);

	PQclear(res);
	flatcc_builder_clear(b);

	free(id_str);

	pss->out_data = out_buf;
	pss->out_data_len = out_size;
}
