# automatically generated by the FlatBuffers compiler, do not modify

# namespace: Request

import flatbuffers

class Register(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsRegister(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = Register()
        x.Init(buf, n + offset)
        return x

    # Register
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

    # Register
    def Username(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.String(o + self._tab.Pos)
        return None

    # Register
    def Password(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(6))
        if o != 0:
            return self._tab.String(o + self._tab.Pos)
        return None

    # Register
    def FirstName(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(8))
        if o != 0:
            return self._tab.String(o + self._tab.Pos)
        return None

    # Register
    def LastName(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(10))
        if o != 0:
            return self._tab.String(o + self._tab.Pos)
        return None

def RegisterStart(builder): builder.StartObject(4)
def RegisterAddUsername(builder, username): builder.PrependUOffsetTRelativeSlot(0, flatbuffers.number_types.UOffsetTFlags.py_type(username), 0)
def RegisterAddPassword(builder, password): builder.PrependUOffsetTRelativeSlot(1, flatbuffers.number_types.UOffsetTFlags.py_type(password), 0)
def RegisterAddFirstName(builder, firstName): builder.PrependUOffsetTRelativeSlot(2, flatbuffers.number_types.UOffsetTFlags.py_type(firstName), 0)
def RegisterAddLastName(builder, lastName): builder.PrependUOffsetTRelativeSlot(3, flatbuffers.number_types.UOffsetTFlags.py_type(lastName), 0)
def RegisterEnd(builder): return builder.EndObject()
