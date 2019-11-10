// automatically generated by the FlatBuffers compiler, do not modify

package online.mctf.tomtom.api.models.Request;

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
									 int usernameOffset,
									 int passwordOffset,
									 int firstNameOffset,
									 int lastNameOffset) {
		builder.startObject(4);
		Register.addLastName(builder, lastNameOffset);
		Register.addFirstName(builder, firstNameOffset);
		Register.addPassword(builder, passwordOffset);
		Register.addUsername(builder, usernameOffset);
		return Register.endRegister(builder);
	}

	public static void startRegister(FlatBufferBuilder builder) {
		builder.startObject(4);
	}

	public static void addUsername(FlatBufferBuilder builder, int usernameOffset) {
		builder.addOffset(0, usernameOffset, 0);
	}

	public static void addPassword(FlatBufferBuilder builder, int passwordOffset) {
		builder.addOffset(1, passwordOffset, 0);
	}

	public static void addFirstName(FlatBufferBuilder builder, int firstNameOffset) {
		builder.addOffset(2, firstNameOffset, 0);
	}

	public static void addLastName(FlatBufferBuilder builder, int lastNameOffset) {
		builder.addOffset(3, lastNameOffset, 0);
	}

	public static int endRegister(FlatBufferBuilder builder) {
		int o = builder.endObject();
		builder.required(o, 4);  // username
		builder.required(o, 6);  // password
		builder.required(o, 8);  // firstName
		builder.required(o, 10);  // lastName
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

	public String username() {
		int o = __offset(4);
		return o != 0 ? __string(o + bb_pos) : null;
	}

	public ByteBuffer usernameAsByteBuffer() {
		return __vector_as_bytebuffer(4, 1);
	}

	public ByteBuffer usernameInByteBuffer(ByteBuffer _bb) {
		return __vector_in_bytebuffer(_bb, 4, 1);
	}

	public String password() {
		int o = __offset(6);
		return o != 0 ? __string(o + bb_pos) : null;
	}

	public ByteBuffer passwordAsByteBuffer() {
		return __vector_as_bytebuffer(6, 1);
	}

	public ByteBuffer passwordInByteBuffer(ByteBuffer _bb) {
		return __vector_in_bytebuffer(_bb, 6, 1);
	}

	public String firstName() {
		int o = __offset(8);
		return o != 0 ? __string(o + bb_pos) : null;
	}

	public ByteBuffer firstNameAsByteBuffer() {
		return __vector_as_bytebuffer(8, 1);
	}

	public ByteBuffer firstNameInByteBuffer(ByteBuffer _bb) {
		return __vector_in_bytebuffer(_bb, 8, 1);
	}

	public String lastName() {
		int o = __offset(10);
		return o != 0 ? __string(o + bb_pos) : null;
	}

	public ByteBuffer lastNameAsByteBuffer() {
		return __vector_as_bytebuffer(10, 1);
	}

	public ByteBuffer lastNameInByteBuffer(ByteBuffer _bb) {
		return __vector_in_bytebuffer(_bb, 10, 1);
	}
}

