#include "get_me.h"

void get_me(struct pss* pss, TomTomBackend_Request_GetMe_table_t messageContent) {
	const char* token = TomTomBackend_Request_GetMe_token_get(messageContent);

	const char* params[1] = {
			token
	};

	PGresult* res = PQexecParams(db_conn,
								 "SELECT username, password, firstname, lastname, admin FROM users WHERE (token=$1)",
								 1, NULL, params, NULL, NULL, 0);

	flatcc_builder_t builder;
	flatcc_builder_t* b = &builder;
	flatcc_builder_init(b);

	TomTomBackend_Response_Message_start_as_root(b);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		log_debug("Unknown DB fail\n");
		TomTomBackend_Response_Message_error_add(b, flatbuffers_true);
		TomTomBackend_Response_Message_errorDescription_create_str(b, PQresultErrorMessage(res));
	} else {
		if (PQntuples(res) <= 0) {
			log_debug("Invalid token\n");
			TomTomBackend_Response_Message_error_add(b, flatbuffers_true);
			TomTomBackend_Response_Message_errorDescription_create_str(b, "Invalid token");
		} else {
			TomTomBackend_Response_Message_error_add(b, flatbuffers_false);

			TomTomBackend_Response_GetMe_ref_t msg_r = TomTomBackend_Response_GetMe_create(
					b,
					flatbuffers_string_create_str(b, PQgetvalue(res, 0, 0)),
					flatbuffers_string_create_str(b, PQgetvalue(res, 0, 1)),
					flatbuffers_string_create_str(b, PQgetvalue(res, 0, 2)),
					flatbuffers_string_create_str(b, PQgetvalue(res, 0, 3)),
					(PQgetvalue(res, 0, 4)[0] == 'f') ? flatbuffers_false : flatbuffers_true
			);

			TomTomBackend_Response_MessageType_union_ref_t msg_u = TomTomBackend_Response_MessageType_as_GetMe(msg_r);
			TomTomBackend_Response_Message_msg_add(b, msg_u);
		}
	}
	TomTomBackend_Response_Message_end_as_root(b);

	size_t out_size;
	void* out_buf = flatcc_builder_finalize_aligned_buffer(b, &out_size);

	PQclear(res);
	flatcc_builder_clear(b);

	pss->out_data = out_buf;
	pss->out_data_len = out_size;
}
