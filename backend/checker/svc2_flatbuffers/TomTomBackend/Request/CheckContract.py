# automatically generated by the FlatBuffers compiler, do not modify

# namespace: Request

import flatbuffers

class CheckContract(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsCheckContract(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = CheckContract()
        x.Init(buf, n + offset)
        return x

    # CheckContract
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

    # CheckContract
    def Token(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.String(o + self._tab.Pos)
        return None

    # CheckContract
    def Id(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(6))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Int64Flags, o + self._tab.Pos)
        return -1

def CheckContractStart(builder): builder.StartObject(2)
def CheckContractAddToken(builder, token): builder.PrependUOffsetTRelativeSlot(0, flatbuffers.number_types.UOffsetTFlags.py_type(token), 0)
def CheckContractAddId(builder, id): builder.PrependInt64Slot(1, id, -1)
def CheckContractEnd(builder): return builder.EndObject()
