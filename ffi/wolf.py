from __future__ import annotations
import ctypes
import typing

T = typing.TypeVar("T")
c_lib = None

def init_lib(path):
    """Initializes the native library. Must be called at least once before anything else."""
    global c_lib
    c_lib = ctypes.cdll.LoadLibrary(path)

    c_lib.wolf_version.argtypes = []
    c_lib.wolf_string.argtypes = [ctypes.POINTER(w_string)]

    c_lib.wolf_version.restype = ctypes.c_uint32
    c_lib.wolf_string.restype = ctypes.c_int



def wolf_version() -> int:
    """ Returns the version of this API."""
    return c_lib.wolf_version()

def wolf_string(p_arg: ctypes.POINTER(w_string)) -> ctypes.c_int:
    """ Returns the version of this API."""
    return c_lib.wolf_string(p_arg)





TRUE = ctypes.c_uint8(1)
FALSE = ctypes.c_uint8(0)


def _errcheck(returned, success):
    """Checks for FFIErrors and converts them to an exception."""
    if returned == success: return
    else: raise Exception(f"Function returned error: {returned}")


class CallbackVars(object):
    """Helper to be used `lambda x: setattr(cv, "x", x)` when getting values from callbacks."""
    def __str__(self):
        rval = ""
        for var in  filter(lambda x: "__" not in x, dir(self)):
            rval += f"{var}: {getattr(self, var)}"
        return rval


class _Iter(object):
    """Helper for slice iterators."""
    def __init__(self, target):
        self.i = 0
        self.target = target

    def __iter__(self):
        self.i = 0
        return self

    def __next__(self):
        if self.i >= self.target.len:
            raise StopIteration()
        rval = self.target[self.i]
        self.i += 1
        return rval


class FFIError:
    #  All went fine.
    Ok = 0


class w_string(ctypes.Structure):

    # These fields represent the underlying C data layout
    _fields_ = [
        ("data", ctypes.POINTER(ctypes.c_uint8)),
        ("len", ctypes.c_uint64),
    ]

    def __init__(self, data: ctypes.POINTER(ctypes.c_uint8) = None, len: int = None):
        if data is not None:
            self.data = data
        if len is not None:
            self.len = len

    @property
    def data(self) -> ctypes.POINTER(ctypes.c_uint8):
        return ctypes.Structure.__get__(self, "data")

    @data.setter
    def data(self, value: ctypes.POINTER(ctypes.c_uint8)):
        return ctypes.Structure.__set__(self, "data", value)

    @property
    def len(self) -> int:
        return ctypes.Structure.__get__(self, "len")

    @len.setter
    def len(self, value: int):
        return ctypes.Structure.__set__(self, "len", value)




class callbacks:
    """Helpers to define callbacks."""


