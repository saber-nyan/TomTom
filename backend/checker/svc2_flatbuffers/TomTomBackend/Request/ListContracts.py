# automatically generated by the FlatBuffers compiler, do not modify

# namespace: Request

import flatbuffers

class ListContracts(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsListContracts(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = ListContracts()
        x.Init(buf, n + offset)
        return x

    # ListContracts
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

    # ListContracts
    def Token(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.String(o + self._tab.Pos)
        return None

def ListContractsStart(builder): builder.StartObject(1)
def ListContractsAddToken(builder, token): builder.PrependUOffsetTRelativeSlot(0, flatbuffers.number_types.UOffsetTFlags.py_type(token), 0)
def ListContractsEnd(builder): return builder.EndObject()
