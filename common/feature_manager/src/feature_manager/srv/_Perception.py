"""autogenerated by genmsg_py from PerceptionRequest.msg. Do not edit."""
import roslib.message
import struct


class PerceptionRequest(roslib.message.Message):
  _md5sum = "d769a2df318cc1e0668e0936b22d5b19"
  _type = "feature_manager/PerceptionRequest"
  _has_header = False #flag to mark the presence of a Header object
  _full_text = """uint8 DO_PERCEPT = 0
uint8 EXTRACT_EFFECT = 1
uint8 task
int8 arg
int8 arg_effect

"""
  # Pseudo-constants
  DO_PERCEPT = 0
  EXTRACT_EFFECT = 1

  __slots__ = ['task','arg','arg_effect']
  _slot_types = ['uint8','int8','int8']

  def __init__(self, *args, **kwds):
    """
    Constructor. Any message fields that are implicitly/explicitly
    set to None will be assigned a default value. The recommend
    use is keyword arguments as this is more robust to future message
    changes.  You cannot mix in-order arguments and keyword arguments.
    
    The available fields are:
       task,arg,arg_effect
    
    @param args: complete set of field values, in .msg order
    @param kwds: use keyword arguments corresponding to message field names
    to set specific fields. 
    """
    if args or kwds:
      super(PerceptionRequest, self).__init__(*args, **kwds)
      #message fields cannot be None, assign default values for those that are
      if self.task is None:
        self.task = 0
      if self.arg is None:
        self.arg = 0
      if self.arg_effect is None:
        self.arg_effect = 0
    else:
      self.task = 0
      self.arg = 0
      self.arg_effect = 0

  def _get_types(self):
    """
    internal API method
    """
    return self._slot_types

  def serialize(self, buff):
    """
    serialize message into buffer
    @param buff: buffer
    @type  buff: StringIO
    """
    try:
      _x = self
      buff.write(_struct_B2b.pack(_x.task, _x.arg, _x.arg_effect))
    except struct.error as se: self._check_types(se)
    except TypeError as te: self._check_types(te)

  def deserialize(self, str):
    """
    unpack serialized message in str into this message instance
    @param str: byte array of serialized message
    @type  str: str
    """
    try:
      end = 0
      _x = self
      start = end
      end += 3
      (_x.task, _x.arg, _x.arg_effect,) = _struct_B2b.unpack(str[start:end])
      return self
    except struct.error as e:
      raise roslib.message.DeserializationError(e) #most likely buffer underfill


  def serialize_numpy(self, buff, numpy):
    """
    serialize message with numpy array types into buffer
    @param buff: buffer
    @type  buff: StringIO
    @param numpy: numpy python module
    @type  numpy module
    """
    try:
      _x = self
      buff.write(_struct_B2b.pack(_x.task, _x.arg, _x.arg_effect))
    except struct.error as se: self._check_types(se)
    except TypeError as te: self._check_types(te)

  def deserialize_numpy(self, str, numpy):
    """
    unpack serialized message in str into this message instance using numpy for array types
    @param str: byte array of serialized message
    @type  str: str
    @param numpy: numpy python module
    @type  numpy: module
    """
    try:
      end = 0
      _x = self
      start = end
      end += 3
      (_x.task, _x.arg, _x.arg_effect,) = _struct_B2b.unpack(str[start:end])
      return self
    except struct.error as e:
      raise roslib.message.DeserializationError(e) #most likely buffer underfill

_struct_I = roslib.message.struct_I
_struct_B2b = struct.Struct("<B2b")
"""autogenerated by genmsg_py from PerceptionResponse.msg. Do not edit."""
import roslib.message
import struct


class PerceptionResponse(roslib.message.Message):
  _md5sum = "8e33959fa61ab56ecced35d43a555db3"
  _type = "feature_manager/PerceptionResponse"
  _has_header = False #flag to mark the presence of a Header object
  _full_text = """
uint8 DONE = 1
uint8 feedback

float32[] pushable_object_center
float32[] pushable_object_size


"""
  # Pseudo-constants
  DONE = 1

  __slots__ = ['feedback','pushable_object_center','pushable_object_size']
  _slot_types = ['uint8','float32[]','float32[]']

  def __init__(self, *args, **kwds):
    """
    Constructor. Any message fields that are implicitly/explicitly
    set to None will be assigned a default value. The recommend
    use is keyword arguments as this is more robust to future message
    changes.  You cannot mix in-order arguments and keyword arguments.
    
    The available fields are:
       feedback,pushable_object_center,pushable_object_size
    
    @param args: complete set of field values, in .msg order
    @param kwds: use keyword arguments corresponding to message field names
    to set specific fields. 
    """
    if args or kwds:
      super(PerceptionResponse, self).__init__(*args, **kwds)
      #message fields cannot be None, assign default values for those that are
      if self.feedback is None:
        self.feedback = 0
      if self.pushable_object_center is None:
        self.pushable_object_center = []
      if self.pushable_object_size is None:
        self.pushable_object_size = []
    else:
      self.feedback = 0
      self.pushable_object_center = []
      self.pushable_object_size = []

  def _get_types(self):
    """
    internal API method
    """
    return self._slot_types

  def serialize(self, buff):
    """
    serialize message into buffer
    @param buff: buffer
    @type  buff: StringIO
    """
    try:
      buff.write(_struct_B.pack(self.feedback))
      length = len(self.pushable_object_center)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(struct.pack(pattern, *self.pushable_object_center))
      length = len(self.pushable_object_size)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(struct.pack(pattern, *self.pushable_object_size))
    except struct.error as se: self._check_types(se)
    except TypeError as te: self._check_types(te)

  def deserialize(self, str):
    """
    unpack serialized message in str into this message instance
    @param str: byte array of serialized message
    @type  str: str
    """
    try:
      end = 0
      start = end
      end += 1
      (self.feedback,) = _struct_B.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      end += struct.calcsize(pattern)
      self.pushable_object_center = struct.unpack(pattern, str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      end += struct.calcsize(pattern)
      self.pushable_object_size = struct.unpack(pattern, str[start:end])
      return self
    except struct.error as e:
      raise roslib.message.DeserializationError(e) #most likely buffer underfill


  def serialize_numpy(self, buff, numpy):
    """
    serialize message with numpy array types into buffer
    @param buff: buffer
    @type  buff: StringIO
    @param numpy: numpy python module
    @type  numpy module
    """
    try:
      buff.write(_struct_B.pack(self.feedback))
      length = len(self.pushable_object_center)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(self.pushable_object_center.tostring())
      length = len(self.pushable_object_size)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(self.pushable_object_size.tostring())
    except struct.error as se: self._check_types(se)
    except TypeError as te: self._check_types(te)

  def deserialize_numpy(self, str, numpy):
    """
    unpack serialized message in str into this message instance using numpy for array types
    @param str: byte array of serialized message
    @type  str: str
    @param numpy: numpy python module
    @type  numpy: module
    """
    try:
      end = 0
      start = end
      end += 1
      (self.feedback,) = _struct_B.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      end += struct.calcsize(pattern)
      self.pushable_object_center = numpy.frombuffer(str[start:end], dtype=numpy.float32, count=length)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      end += struct.calcsize(pattern)
      self.pushable_object_size = numpy.frombuffer(str[start:end], dtype=numpy.float32, count=length)
      return self
    except struct.error as e:
      raise roslib.message.DeserializationError(e) #most likely buffer underfill

_struct_I = roslib.message.struct_I
_struct_B = struct.Struct("<B")
class Perception(roslib.message.ServiceDefinition):
  _type          = 'feature_manager/Perception'
  _md5sum = 'e623ddd26925b15d65f272b948a6f870'
  _request_class  = PerceptionRequest
  _response_class = PerceptionResponse
