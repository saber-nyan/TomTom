#include "processing.h"

void process(struct pss* pss) {
	// Verify
	int ret;
	if ((ret = TomTomBackend_Request_Message_verify_as_root(pss->data, pss->data_len))) {
		lwsl_warn("Flatbuffer is invalid: %s\n", flatcc_verify_error_string(ret));
		return;
	}

	TomTomBackend_Request_Message_table_t root;

	if (!(root = TomTomBackend_Request_Message_as_root(pss->data))) {
		lwsl_warn("Can not read flatbuffer!\n");
		return;
	}
	flatbuffers_union_t message = TomTomBackend_Request_Message_msg_union(root);

	switch (message.type) {
		case TomTomBackend_Request_MessageType_Login:
			login(pss, message.value);
			break;
		case TomTomBackend_Request_MessageType_Register:
			_register(pss, message.value);
			break;
		case TomTomBackend_Request_MessageType_GetMe:
			get_me(pss, message.value);
			break;
		case TomTomBackend_Request_MessageType_EditMe:
			edit_me(pss, message.value);
			break;
		case TomTomBackend_Request_MessageType_RequestContract:
			request_contract(pss, message.value);
			break;
		case TomTomBackend_Request_MessageType_ListContracts:
			list_contracts(pss, message.value);
			break;
		case TomTomBackend_Request_MessageType_CheckContract:
			check_contract(pss, message.value);
			break;
		default:
			lwsl_err("request_type %d is not implemented!\n", message.type);
			break;
	}
}
