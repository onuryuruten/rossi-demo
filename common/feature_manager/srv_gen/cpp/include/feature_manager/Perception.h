/* Auto-generated by genmsg_cpp for file /home/asil/rossi_workspace/metu-ros-pkg/stacks/aff_learning/common/feature_manager/srv/Perception.srv */
#ifndef FEATURE_MANAGER_SERVICE_PERCEPTION_H
#define FEATURE_MANAGER_SERVICE_PERCEPTION_H
#include <string>
#include <vector>
#include <map>
#include <ostream>
#include "ros/serialization.h"
#include "ros/builtin_message_traits.h"
#include "ros/message_operations.h"
#include "ros/time.h"

#include "ros/macros.h"

#include "ros/assert.h"

#include "ros/service_traits.h"




namespace feature_manager
{
template <class ContainerAllocator>
struct PerceptionRequest_ {
  typedef PerceptionRequest_<ContainerAllocator> Type;

  PerceptionRequest_()
  : task(0)
  , arg(0)
  , arg_effect(0)
  {
  }

  PerceptionRequest_(const ContainerAllocator& _alloc)
  : task(0)
  , arg(0)
  , arg_effect(0)
  {
  }

  typedef uint8_t _task_type;
  uint8_t task;

  typedef int8_t _arg_type;
  int8_t arg;

  typedef int8_t _arg_effect_type;
  int8_t arg_effect;

  enum { DO_PERCEPT = 0 };
  enum { EXTRACT_EFFECT = 1 };

private:
  static const char* __s_getDataType_() { return "feature_manager/PerceptionRequest"; }
public:
  ROS_DEPRECATED static const std::string __s_getDataType() { return __s_getDataType_(); }

  ROS_DEPRECATED const std::string __getDataType() const { return __s_getDataType_(); }

private:
  static const char* __s_getMD5Sum_() { return "d769a2df318cc1e0668e0936b22d5b19"; }
public:
  ROS_DEPRECATED static const std::string __s_getMD5Sum() { return __s_getMD5Sum_(); }

  ROS_DEPRECATED const std::string __getMD5Sum() const { return __s_getMD5Sum_(); }

private:
  static const char* __s_getServerMD5Sum_() { return "e623ddd26925b15d65f272b948a6f870"; }
public:
  ROS_DEPRECATED static const std::string __s_getServerMD5Sum() { return __s_getServerMD5Sum_(); }

  ROS_DEPRECATED const std::string __getServerMD5Sum() const { return __s_getServerMD5Sum_(); }

private:
  static const char* __s_getMessageDefinition_() { return "uint8 DO_PERCEPT = 0\n\
uint8 EXTRACT_EFFECT = 1\n\
uint8 task\n\
int8 arg\n\
int8 arg_effect\n\
\n\
"; }
public:
  ROS_DEPRECATED static const std::string __s_getMessageDefinition() { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED const std::string __getMessageDefinition() const { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED virtual uint8_t *serialize(uint8_t *write_ptr, uint32_t seq) const
  {
    ros::serialization::OStream stream(write_ptr, 1000000000);
    ros::serialization::serialize(stream, task);
    ros::serialization::serialize(stream, arg);
    ros::serialization::serialize(stream, arg_effect);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint8_t *deserialize(uint8_t *read_ptr)
  {
    ros::serialization::IStream stream(read_ptr, 1000000000);
    ros::serialization::deserialize(stream, task);
    ros::serialization::deserialize(stream, arg);
    ros::serialization::deserialize(stream, arg_effect);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint32_t serializationLength() const
  {
    uint32_t size = 0;
    size += ros::serialization::serializationLength(task);
    size += ros::serialization::serializationLength(arg);
    size += ros::serialization::serializationLength(arg_effect);
    return size;
  }

  typedef boost::shared_ptr< ::feature_manager::PerceptionRequest_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::feature_manager::PerceptionRequest_<ContainerAllocator>  const> ConstPtr;
  boost::shared_ptr<std::map<std::string, std::string> > __connection_header;
}; // struct PerceptionRequest
typedef  ::feature_manager::PerceptionRequest_<std::allocator<void> > PerceptionRequest;

typedef boost::shared_ptr< ::feature_manager::PerceptionRequest> PerceptionRequestPtr;
typedef boost::shared_ptr< ::feature_manager::PerceptionRequest const> PerceptionRequestConstPtr;


template <class ContainerAllocator>
struct PerceptionResponse_ {
  typedef PerceptionResponse_<ContainerAllocator> Type;

  PerceptionResponse_()
  : feedback(0)
  , pushable_object_center()
  , pushable_object_size()
  {
  }

  PerceptionResponse_(const ContainerAllocator& _alloc)
  : feedback(0)
  , pushable_object_center(_alloc)
  , pushable_object_size(_alloc)
  {
  }

  typedef uint8_t _feedback_type;
  uint8_t feedback;

  typedef std::vector<float, typename ContainerAllocator::template rebind<float>::other >  _pushable_object_center_type;
  std::vector<float, typename ContainerAllocator::template rebind<float>::other >  pushable_object_center;

  typedef std::vector<float, typename ContainerAllocator::template rebind<float>::other >  _pushable_object_size_type;
  std::vector<float, typename ContainerAllocator::template rebind<float>::other >  pushable_object_size;

  enum { DONE = 1 };

  ROS_DEPRECATED uint32_t get_pushable_object_center_size() const { return (uint32_t)pushable_object_center.size(); }
  ROS_DEPRECATED void set_pushable_object_center_size(uint32_t size) { pushable_object_center.resize((size_t)size); }
  ROS_DEPRECATED void get_pushable_object_center_vec(std::vector<float, typename ContainerAllocator::template rebind<float>::other > & vec) const { vec = this->pushable_object_center; }
  ROS_DEPRECATED void set_pushable_object_center_vec(const std::vector<float, typename ContainerAllocator::template rebind<float>::other > & vec) { this->pushable_object_center = vec; }
  ROS_DEPRECATED uint32_t get_pushable_object_size_size() const { return (uint32_t)pushable_object_size.size(); }
  ROS_DEPRECATED void set_pushable_object_size_size(uint32_t size) { pushable_object_size.resize((size_t)size); }
  ROS_DEPRECATED void get_pushable_object_size_vec(std::vector<float, typename ContainerAllocator::template rebind<float>::other > & vec) const { vec = this->pushable_object_size; }
  ROS_DEPRECATED void set_pushable_object_size_vec(const std::vector<float, typename ContainerAllocator::template rebind<float>::other > & vec) { this->pushable_object_size = vec; }
private:
  static const char* __s_getDataType_() { return "feature_manager/PerceptionResponse"; }
public:
  ROS_DEPRECATED static const std::string __s_getDataType() { return __s_getDataType_(); }

  ROS_DEPRECATED const std::string __getDataType() const { return __s_getDataType_(); }

private:
  static const char* __s_getMD5Sum_() { return "8e33959fa61ab56ecced35d43a555db3"; }
public:
  ROS_DEPRECATED static const std::string __s_getMD5Sum() { return __s_getMD5Sum_(); }

  ROS_DEPRECATED const std::string __getMD5Sum() const { return __s_getMD5Sum_(); }

private:
  static const char* __s_getServerMD5Sum_() { return "e623ddd26925b15d65f272b948a6f870"; }
public:
  ROS_DEPRECATED static const std::string __s_getServerMD5Sum() { return __s_getServerMD5Sum_(); }

  ROS_DEPRECATED const std::string __getServerMD5Sum() const { return __s_getServerMD5Sum_(); }

private:
  static const char* __s_getMessageDefinition_() { return "\n\
uint8 DONE = 1\n\
uint8 feedback\n\
\n\
float32[] pushable_object_center\n\
float32[] pushable_object_size\n\
\n\
\n\
"; }
public:
  ROS_DEPRECATED static const std::string __s_getMessageDefinition() { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED const std::string __getMessageDefinition() const { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED virtual uint8_t *serialize(uint8_t *write_ptr, uint32_t seq) const
  {
    ros::serialization::OStream stream(write_ptr, 1000000000);
    ros::serialization::serialize(stream, feedback);
    ros::serialization::serialize(stream, pushable_object_center);
    ros::serialization::serialize(stream, pushable_object_size);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint8_t *deserialize(uint8_t *read_ptr)
  {
    ros::serialization::IStream stream(read_ptr, 1000000000);
    ros::serialization::deserialize(stream, feedback);
    ros::serialization::deserialize(stream, pushable_object_center);
    ros::serialization::deserialize(stream, pushable_object_size);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint32_t serializationLength() const
  {
    uint32_t size = 0;
    size += ros::serialization::serializationLength(feedback);
    size += ros::serialization::serializationLength(pushable_object_center);
    size += ros::serialization::serializationLength(pushable_object_size);
    return size;
  }

  typedef boost::shared_ptr< ::feature_manager::PerceptionResponse_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::feature_manager::PerceptionResponse_<ContainerAllocator>  const> ConstPtr;
  boost::shared_ptr<std::map<std::string, std::string> > __connection_header;
}; // struct PerceptionResponse
typedef  ::feature_manager::PerceptionResponse_<std::allocator<void> > PerceptionResponse;

typedef boost::shared_ptr< ::feature_manager::PerceptionResponse> PerceptionResponsePtr;
typedef boost::shared_ptr< ::feature_manager::PerceptionResponse const> PerceptionResponseConstPtr;

struct Perception
{

typedef PerceptionRequest Request;
typedef PerceptionResponse Response;
Request request;
Response response;

typedef Request RequestType;
typedef Response ResponseType;
}; // struct Perception
} // namespace feature_manager

namespace ros
{
namespace message_traits
{
template<class ContainerAllocator> struct IsMessage< ::feature_manager::PerceptionRequest_<ContainerAllocator> > : public TrueType {};
template<class ContainerAllocator> struct IsMessage< ::feature_manager::PerceptionRequest_<ContainerAllocator>  const> : public TrueType {};
template<class ContainerAllocator>
struct MD5Sum< ::feature_manager::PerceptionRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "d769a2df318cc1e0668e0936b22d5b19";
  }

  static const char* value(const  ::feature_manager::PerceptionRequest_<ContainerAllocator> &) { return value(); } 
  static const uint64_t static_value1 = 0xd769a2df318cc1e0ULL;
  static const uint64_t static_value2 = 0x668e0936b22d5b19ULL;
};

template<class ContainerAllocator>
struct DataType< ::feature_manager::PerceptionRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "feature_manager/PerceptionRequest";
  }

  static const char* value(const  ::feature_manager::PerceptionRequest_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct Definition< ::feature_manager::PerceptionRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "uint8 DO_PERCEPT = 0\n\
uint8 EXTRACT_EFFECT = 1\n\
uint8 task\n\
int8 arg\n\
int8 arg_effect\n\
\n\
";
  }

  static const char* value(const  ::feature_manager::PerceptionRequest_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator> struct IsFixedSize< ::feature_manager::PerceptionRequest_<ContainerAllocator> > : public TrueType {};
} // namespace message_traits
} // namespace ros


namespace ros
{
namespace message_traits
{
template<class ContainerAllocator> struct IsMessage< ::feature_manager::PerceptionResponse_<ContainerAllocator> > : public TrueType {};
template<class ContainerAllocator> struct IsMessage< ::feature_manager::PerceptionResponse_<ContainerAllocator>  const> : public TrueType {};
template<class ContainerAllocator>
struct MD5Sum< ::feature_manager::PerceptionResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "8e33959fa61ab56ecced35d43a555db3";
  }

  static const char* value(const  ::feature_manager::PerceptionResponse_<ContainerAllocator> &) { return value(); } 
  static const uint64_t static_value1 = 0x8e33959fa61ab56eULL;
  static const uint64_t static_value2 = 0xcced35d43a555db3ULL;
};

template<class ContainerAllocator>
struct DataType< ::feature_manager::PerceptionResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "feature_manager/PerceptionResponse";
  }

  static const char* value(const  ::feature_manager::PerceptionResponse_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct Definition< ::feature_manager::PerceptionResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "\n\
uint8 DONE = 1\n\
uint8 feedback\n\
\n\
float32[] pushable_object_center\n\
float32[] pushable_object_size\n\
\n\
\n\
";
  }

  static const char* value(const  ::feature_manager::PerceptionResponse_<ContainerAllocator> &) { return value(); } 
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

template<class ContainerAllocator> struct Serializer< ::feature_manager::PerceptionRequest_<ContainerAllocator> >
{
  template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
  {
    stream.next(m.task);
    stream.next(m.arg);
    stream.next(m.arg_effect);
  }

  ROS_DECLARE_ALLINONE_SERIALIZER;
}; // struct PerceptionRequest_
} // namespace serialization
} // namespace ros


namespace ros
{
namespace serialization
{

template<class ContainerAllocator> struct Serializer< ::feature_manager::PerceptionResponse_<ContainerAllocator> >
{
  template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
  {
    stream.next(m.feedback);
    stream.next(m.pushable_object_center);
    stream.next(m.pushable_object_size);
  }

  ROS_DECLARE_ALLINONE_SERIALIZER;
}; // struct PerceptionResponse_
} // namespace serialization
} // namespace ros

namespace ros
{
namespace service_traits
{
template<>
struct MD5Sum<feature_manager::Perception> {
  static const char* value() 
  {
    return "e623ddd26925b15d65f272b948a6f870";
  }

  static const char* value(const feature_manager::Perception&) { return value(); } 
};

template<>
struct DataType<feature_manager::Perception> {
  static const char* value() 
  {
    return "feature_manager/Perception";
  }

  static const char* value(const feature_manager::Perception&) { return value(); } 
};

template<class ContainerAllocator>
struct MD5Sum<feature_manager::PerceptionRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "e623ddd26925b15d65f272b948a6f870";
  }

  static const char* value(const feature_manager::PerceptionRequest_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct DataType<feature_manager::PerceptionRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "feature_manager/Perception";
  }

  static const char* value(const feature_manager::PerceptionRequest_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct MD5Sum<feature_manager::PerceptionResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "e623ddd26925b15d65f272b948a6f870";
  }

  static const char* value(const feature_manager::PerceptionResponse_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct DataType<feature_manager::PerceptionResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "feature_manager/Perception";
  }

  static const char* value(const feature_manager::PerceptionResponse_<ContainerAllocator> &) { return value(); } 
};

} // namespace service_traits
} // namespace ros

#endif // FEATURE_MANAGER_SERVICE_PERCEPTION_H

