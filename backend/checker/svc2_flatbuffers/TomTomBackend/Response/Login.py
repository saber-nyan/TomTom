# automatically generated by the FlatBuffers compiler, do not modify

# namespace: Response

import flatbuffers

class Login(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsLogin(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = Login()
        x.Init(buf, n + offset)
        return x

    # Login
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

    # Login
    def Token(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.String(o + self._tab.Pos)
        return None

def LoginStart(builder): builder.StartObject(1)
def LoginAddToken(builder, token): builder.PrependUOffsetTRelativeSlot(0, flatbuffers.number_types.UOffsetTFlags.py_type(token), 0)
def LoginEnd(builder): return builder.EndObject()
