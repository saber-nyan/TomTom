// automatically generated by the FlatBuffers compiler, do not modify

package online.mctf.tomtom.api.models.Response;

public final class Status {
	public static final byte Processing = 1;
	public static final byte Accepted = 2;
	public static final byte Rejected = 3;
	public static final String[] names = {"Processing", "Accepted", "Rejected",};

	private Status() {
	}

	public static String name(int e) {
		return names[e - Processing];
	}
}

