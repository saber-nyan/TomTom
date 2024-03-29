// automatically generated by the FlatBuffers compiler, do not modify

package online.mctf.tomtom.api.models.Response;

import com.google.flatbuffers.FlatBufferBuilder;
import com.google.flatbuffers.Table;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

@SuppressWarnings("unused")
public final class RequestContract extends Table {
	public static RequestContract getRootAsRequestContract(ByteBuffer _bb) {
		return getRootAsRequestContract(_bb, new RequestContract());
	}

	public static RequestContract getRootAsRequestContract(ByteBuffer _bb, RequestContract obj) {
		_bb.order(ByteOrder.LITTLE_ENDIAN);
		return (obj.__assign(_bb.getInt(_bb.position()) + _bb.position(), _bb));
	}

	public static int createRequestContract(FlatBufferBuilder builder,
											int resultOffset) {
		builder.startObject(1);
		RequestContract.addResult(builder, resultOffset);
		return RequestContract.endRequestContract(builder);
	}

	public static void startRequestContract(FlatBufferBuilder builder) {
		builder.startObject(1);
	}

	public static void addResult(FlatBufferBuilder builder, int resultOffset) {
		builder.addOffset(0, resultOffset, 0);
	}

	public static int endRequestContract(FlatBufferBuilder builder) {
		int o = builder.endObject();
		builder.required(o, 4);  // result
		return o;
	}

	public void __init(int _i, ByteBuffer _bb) {
		bb_pos = _i;
		bb = _bb;
		vtable_start = bb_pos - bb.getInt(bb_pos);
		vtable_size = bb.getShort(vtable_start);
	}

	public RequestContract __assign(int _i, ByteBuffer _bb) {
		__init(_i, _bb);
		return this;
	}

	public CheckContract result() {
		return result(new CheckContract());
	}

	public CheckContract result(CheckContract obj) {
		int o = __offset(4);
		return o != 0 ? obj.__assign(__indirect(o + bb_pos), bb) : null;
	}
}

