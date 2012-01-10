"""autogenerated by genmsg_py from ActionRequest.msg. Do not edit."""
import roslib.message
import struct


class ActionRequest(roslib.message.Message):
  _md5sum = "6f28010420559760ce532a04644ecab7"
  _type = "behavior_manager_asil/ActionRequest"
  _has_header = False #flag to mark the presence of a Header object
  _full_text = """int8 DONT_ACT = -1
int8 PUSH_RIGHT = 0
int8 PUSH_LEFT =1
int8 PUSH_FORWARD =2
int8 PUSH_BACKWARD=3
int8 LIFT=4
int8 GRASP=5
int8 HOME=6
int8 POINT=7
int8 HIDE=8
int8 CANCEL=9
int8 STOP=10
int8 SHOW=11
int8 TUCK_ARMS=12
int8 task
float32[] pushable_object_center
float32[] pushable_object_size

"""
  # Pseudo-constants
  DONT_ACT = -1
  PUSH_RIGHT = 0
  PUSH_LEFT = 1
  PUSH_FORWARD = 2
  PUSH_BACKWARD = 3
  LIFT = 4
  GRASP = 5
  HOME = 6
  POINT = 7
  HIDE = 8
  CANCEL = 9
  STOP = 10
  SHOW = 11
  TUCK_ARMS = 12

  __slots__ = ['task','pushable_object_center','pushable_object_size']
  _slot_types = ['int8','float32[]','float32[]']

  def __init__(self, *args, **kwds):
    """
    Constructor. Any message fields that are implicitly/explicitly
    set to None will be assigned a default value. The recommend
    use is keyword arguments as this is more robust to future message
    changes.  You cannot mix in-order arguments and keyword arguments.
    
    The available fields are:
       task,pushable_object_center,pushable_object_size
    
    @param args: complete set of field values, in .msg order
    @param kwds: use keyword arguments corresponding to message field names
    to set specific fields. 
    """
    if args or kwds:
      super(ActionRequest, self).__init__(*args, **kwds)
      #message fields cannot be None, assign default values for those that are
      if self.task is None:
        self.task = 0
      if self.pushable_object_center is None:
        self.pushable_object_center = []
      if self.pushable_object_size is None:
        self.pushable_object_size = []
    else:
      self.task = 0
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
      buff.write(_struct_b.pack(self.task))
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
      (self.task,) = _struct_b.unpack(str[start:end])
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
      buff.write(_struct_b.pack(self.task))
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
      (self.task,) = _struct_b.unpack(str[start:end])
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
_struct_b = struct.Struct("<b")
"""autogenerated by genmsg_py from ActionResponse.msg. Do not edit."""
import roslib.message
import struct


class ActionResponse(roslib.message.Message):
  _md5sum = "0c33925582a4cf8db9f48c73fab8a9c4"
  _type = "behavior_manager_asil/ActionResponse"
  _has_header = False #flag to mark the presence of a Header object
  _full_text = """
uint8 DONE = 1
uint8 feedback


"""
  # Pseudo-constants
  DONE = 1

  __slots__ = ['feedback']
  _slot_types = ['uint8']

  def __init__(self, *args, **kwds):
    """
    Constructor. Any message fields that are implicitly/explicitly
    set to None will be assigned a default value. The recommend
    use is keyword arguments as this is more robust to future message
    changes.  You cannot mix in-order arguments and keyword arguments.
    
    The available fields are:
       feedback
    
    @param args: complete set of field values, in .msg order
    @param kwds: use keyword arguments corresponding to message field names
    to set specific fields. 
    """
    if args or kwds:
      super(ActionResponse, self).__init__(*args, **kwds)
      #message fields cannot be None, assign default values for those that are
      if self.feedback is None:
        self.feedback = 0
    else:
      self.feedback = 0

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
      return self
    except struct.error as e:
      raise roslib.message.DeserializationError(e) #most likely buffer underfill

_struct_I = roslib.message.struct_I
_struct_B = struct.Struct("<B")
class Action(roslib.message.ServiceDefinition):
  _type          = 'behavior_manager_asil/Action'
  _md5sum = '9900281ecfdf116827776adaa867e6d8'
  _request_class  = ActionRequest
  _response_class = ActionResponse
