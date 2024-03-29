// automatically generated by the FlatBuffers compiler, do not modify

package online.mctf.tomtom.api.models.Response;

import com.google.flatbuffers.FlatBufferBuilder;
import com.google.flatbuffers.Table;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

@SuppressWarnings("unused")
public final class ListContracts extends Table {
	public static ListContracts getRootAsListContracts(ByteBuffer _bb) {
		return getRootAsListContracts(_bb, new ListContracts());
	}

	public static ListContracts getRootAsListContracts(ByteBuffer _bb, ListContracts obj) {
		_bb.order(ByteOrder.LITTLE_ENDIAN);
		return (obj.__assign(_bb.getInt(_bb.position()) + _bb.position(), _bb));
	}

	public static int createListContracts(FlatBufferBuilder builder,
										  int contractsOffset) {
		builder.startObject(1);
		ListContracts.addContracts(builder, contractsOffset);
		return ListContracts.endListContracts(builder);
	}

	public static void startListContracts(FlatBufferBuilder builder) {
		builder.startObject(1);
	}

	public static void addContracts(FlatBufferBuilder builder, int contractsOffset) {
		builder.addOffset(0, contractsOffset, 0);
	}

	public static int createContractsVector(FlatBufferBuilder builder, int[] data) {
		builder.startVector(4, data.length, 4);
		for (int i = data.length - 1; i >= 0; i--) builder.addOffset(data[i]);
		return builder.endVector();
	}

	public static void startContractsVector(FlatBufferBuilder builder, int numElems) {
		builder.startVector(4, numElems, 4);
	}

	public static int endListContracts(FlatBufferBuilder builder) {
		int o = builder.endObject();
		builder.required(o, 4);  // contracts
		return o;
	}

	public void __init(int _i, ByteBuffer _bb) {
		bb_pos = _i;
		bb = _bb;
		vtable_start = bb_pos - bb.getInt(bb_pos);
		vtable_size = bb.getShort(vtable_start);
	}

	public ListContracts __assign(int _i, ByteBuffer _bb) {
		__init(_i, _bb);
		return this;
	}

	public CheckContract contracts(int j) {
		return contracts(new CheckContract(), j);
	}

	public CheckContract contracts(CheckContract obj, int j) {
		int o = __offset(4);
		return o != 0 ? obj.__assign(__indirect(__vector(o) + j * 4), bb) : null;
	}

	public int contractsLength() {
		int o = __offset(4);
		return o != 0 ? __vector_len(o) : 0;
	}
}

