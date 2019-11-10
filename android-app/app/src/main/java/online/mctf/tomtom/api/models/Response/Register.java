// automatically generated by the FlatBuffers compiler, do not modify

package online.mctf.tomtom.api.models.Response;

import com.google.flatbuffers.FlatBufferBuilder;
import com.google.flatbuffers.Table;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

@SuppressWarnings("unused")
public final class Register extends Table {
	public static Register getRootAsRegister(ByteBuffer _bb) {
		return getRootAsRegister(_bb, new Register());
	}

	public static Register getRootAsRegister(ByteBuffer _bb, Register obj) {
		_bb.order(ByteOrder.LITTLE_ENDIAN);
		return (obj.__assign(_bb.getInt(_bb.position()) + _bb.position(), _bb));
	}

	public static int createRegister(FlatBufferBuilder builder,
									 int tokenOffset) {
		builder.startObject(1);
		Register.addToken(builder, tokenOffset);
		return Register.endRegister(builder);
	}

	public static void startRegister(FlatBufferBuilder builder) {
		builder.startObject(1);
	}

	public static void addToken(FlatBufferBuilder builder, int tokenOffset) {
		builder.addOffset(0, tokenOffset, 0);
	}

	public static int endRegister(FlatBufferBuilder builder) {
		int o = builder.endObject();
		builder.required(o, 4);  // token
		return o;
	}

	public void __init(int _i, ByteBuffer _bb) {
		bb_pos = _i;
		bb = _bb;
		vtable_start = bb_pos - bb.getInt(bb_pos);
		vtable_size = bb.getShort(vtable_start);
	}

	public Register __assign(int _i, ByteBuffer _bb) {
		__init(_i, _bb);
		return this;
	}

	public String token() {
		int o = __offset(4);
		return o != 0 ? __string(o + bb_pos) : null;
	}

	public ByteBuffer tokenAsByteBuffer() {
		return __vector_as_bytebuffer(4, 1);
	}

	public ByteBuffer tokenInByteBuffer(ByteBuffer _bb) {
		return __vector_in_bytebuffer(_bb, 4, 1);
	}
}

