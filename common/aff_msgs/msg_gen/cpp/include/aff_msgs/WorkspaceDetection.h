/* Auto-generated by genmsg_cpp for file /home/kadir/interconnection/workspace/metu-ros-pkg/stacks/aff_learning/common/aff_msgs/msg/WorkspaceDetection.msg */
#ifndef AFF_MSGS_MESSAGE_WORKSPACEDETECTION_H
#define AFF_MSGS_MESSAGE_WORKSPACEDETECTION_H
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

#include "tabletop_object_detector/TabletopDetectionResult.h"
#include "tabletop_object_detector/Table.h"

namespace aff_msgs
{
template <class ContainerAllocator>
struct WorkspaceDetection_ {
  typedef WorkspaceDetection_<ContainerAllocator> Type;

  WorkspaceDetection_()
  : tabletop()
  , table()
  {
  }

  WorkspaceDetection_(const ContainerAllocator& _alloc)
  : tabletop(_alloc)
  , table(_alloc)
  {
  }

  typedef  ::tabletop_object_detector::TabletopDetectionResult_<ContainerAllocator>  _tabletop_type;
   ::tabletop_object_detector::TabletopDetectionResult_<ContainerAllocator>  tabletop;

  typedef  ::tabletop_object_detector::Table_<ContainerAllocator>  _table_type;
   ::tabletop_object_detector::Table_<ContainerAllocator>  table;


private:
  static const char* __s_getDataType_() { return "aff_msgs/WorkspaceDetection"; }
public:
  ROS_DEPRECATED static const std::string __s_getDataType() { return __s_getDataType_(); }

  ROS_DEPRECATED const std::string __getDataType() const { return __s_getDataType_(); }

private:
  static const char* __s_getMD5Sum_() { return "780ef7d01de10730ff3a1a4575dba282"; }
public:
  ROS_DEPRECATED static const std::string __s_getMD5Sum() { return __s_getMD5Sum_(); }

  ROS_DEPRECATED const std::string __getMD5Sum() const { return __s_getMD5Sum_(); }

private:
  static const char* __s_getMessageDefinition_() { return "# Contains all the information from one run of the tabletop detection node\n\
tabletop_object_detector/TabletopDetectionResult tabletop\n\
\n\
# Informs that a planar table has been detected at a given location\n\
tabletop_object_detector/Table table\n\
\n\
================================================================================\n\
MSG: tabletop_object_detector/TabletopDetectionResult\n\
# Contains all the information from one run of the tabletop detection node\n\
\n\
# The information for the plane that has been detected\n\
Table table\n\
\n\
# The raw clusters detected in the scan \n\
sensor_msgs/PointCloud[] clusters\n\
\n\
# The list of potential models that have been detected for each cluster\n\
# An empty list will be returned for a cluster that has no recognition results at all\n\
household_objects_database_msgs/DatabaseModelPoseList[] models\n\
\n\
# For each cluster, the index of the list of models that was fit to that cluster\n\
# keep in mind that multiple raw clusters can correspond to a single fit\n\
int32[] cluster_model_indices\n\
\n\
# Whether the detection has succeeded or failed\n\
int32 NO_CLOUD_RECEIVED = 1\n\
int32 NO_TABLE = 2\n\
int32 OTHER_ERROR = 3\n\
int32 SUCCESS = 4\n\
int32 result\n\
\n\
================================================================================\n\
MSG: tabletop_object_detector/Table\n\
# Informs that a planar table has been detected at a given location\n\
\n\
# The pose gives you the transform that take you to the coordinate system\n\
# of the table, with the origin somewhere in the table plane and the \n\
# z axis normal to the plane\n\
geometry_msgs/PoseStamped pose\n\
\n\
# These values give you the observed extents of the table, along x and y,\n\
# in the table's own coordinate system (above)\n\
# there is no guarantee that the origin of the table coordinate system is\n\
# inside the boundary defined by these values. \n\
float32 x_min\n\
float32 x_max\n\
float32 y_min\n\
float32 y_max\n\
\n\
# There is no guarantee that the table doe NOT extend further than these \n\
# values; this is just as far as we've observed it.\n\
\n\
================================================================================\n\
MSG: geometry_msgs/PoseStamped\n\
# A Pose with reference coordinate frame and timestamp\n\
Header header\n\
Pose pose\n\
\n\
================================================================================\n\
MSG: std_msgs/Header\n\
# Standard metadata for higher-level stamped data types.\n\
# This is generally used to communicate timestamped data \n\
# in a particular coordinate frame.\n\
# \n\
# sequence ID: consecutively increasing ID \n\
uint32 seq\n\
#Two-integer timestamp that is expressed as:\n\
# * stamp.secs: seconds (stamp_secs) since epoch\n\
# * stamp.nsecs: nanoseconds since stamp_secs\n\
# time-handling sugar is provided by the client library\n\
time stamp\n\
#Frame this data is associated with\n\
# 0: no frame\n\
# 1: global frame\n\
string frame_id\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Pose\n\
# A representation of pose in free space, composed of postion and orientation. \n\
Point position\n\
Quaternion orientation\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Point\n\
# This contains the position of a point in free space\n\
float64 x\n\
float64 y\n\
float64 z\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Quaternion\n\
# This represents an orientation in free space in quaternion form.\n\
\n\
float64 x\n\
float64 y\n\
float64 z\n\
float64 w\n\
\n\
================================================================================\n\
MSG: sensor_msgs/PointCloud\n\
# This message holds a collection of 3d points, plus optional additional\n\
# information about each point.\n\
\n\
# Time of sensor data acquisition, coordinate frame ID.\n\
Header header\n\
\n\
# Array of 3d points. Each Point32 should be interpreted as a 3d point\n\
# in the frame given in the header.\n\
geometry_msgs/Point32[] points\n\
\n\
# Each channel should have the same number of elements as points array,\n\
# and the data in each channel should correspond 1:1 with each point.\n\
# Channel names in common practice are listed in ChannelFloat32.msg.\n\
ChannelFloat32[] channels\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Point32\n\
# This contains the position of a point in free space(with 32 bits of precision).\n\
# It is recommeded to use Point wherever possible instead of Point32.  \n\
# \n\
# This recommendation is to promote interoperability.  \n\
#\n\
# This message is designed to take up less space when sending\n\
# lots of points at once, as in the case of a PointCloud.  \n\
\n\
float32 x\n\
float32 y\n\
float32 z\n\
================================================================================\n\
MSG: sensor_msgs/ChannelFloat32\n\
# This message is used by the PointCloud message to hold optional data\n\
# associated with each point in the cloud. The length of the values\n\
# array should be the same as the length of the points array in the\n\
# PointCloud, and each value should be associated with the corresponding\n\
# point.\n\
\n\
# Channel names in existing practice include:\n\
#   \"u\", \"v\" - row and column (respectively) in the left stereo image.\n\
#              This is opposite to usual conventions but remains for\n\
#              historical reasons. The newer PointCloud2 message has no\n\
#              such problem.\n\
#   \"rgb\" - For point clouds produced by color stereo cameras. uint8\n\
#           (R,G,B) values packed into the least significant 24 bits,\n\
#           in order.\n\
#   \"intensity\" - laser or pixel intensity.\n\
#   \"distance\"\n\
\n\
# The channel name should give semantics of the channel (e.g.\n\
# \"intensity\" instead of \"value\").\n\
string name\n\
\n\
# The values array should be 1-1 with the elements of the associated\n\
# PointCloud.\n\
float32[] values\n\
\n\
================================================================================\n\
MSG: household_objects_database_msgs/DatabaseModelPoseList\n\
# stores a list of possible database models recognition results\n\
DatabaseModelPose[] model_list\n\
================================================================================\n\
MSG: household_objects_database_msgs/DatabaseModelPose\n\
# Informs that a specific model from the Model Database has been \n\
# identified at a certain location\n\
\n\
# the database id of the model\n\
int32 model_id\n\
\n\
# the pose that it can be found in\n\
geometry_msgs/PoseStamped pose\n\
\n\
# a measure of the confidence level in this detection result\n\
float32 confidence\n\
"; }
public:
  ROS_DEPRECATED static const std::string __s_getMessageDefinition() { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED const std::string __getMessageDefinition() const { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED virtual uint8_t *serialize(uint8_t *write_ptr, uint32_t seq) const
  {
    ros::serialization::OStream stream(write_ptr, 1000000000);
    ros::serialization::serialize(stream, tabletop);
    ros::serialization::serialize(stream, table);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint8_t *deserialize(uint8_t *read_ptr)
  {
    ros::serialization::IStream stream(read_ptr, 1000000000);
    ros::serialization::deserialize(stream, tabletop);
    ros::serialization::deserialize(stream, table);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint32_t serializationLength() const
  {
    uint32_t size = 0;
    size += ros::serialization::serializationLength(tabletop);
    size += ros::serialization::serializationLength(table);
    return size;
  }

  typedef boost::shared_ptr< ::aff_msgs::WorkspaceDetection_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::aff_msgs::WorkspaceDetection_<ContainerAllocator>  const> ConstPtr;
  boost::shared_ptr<std::map<std::string, std::string> > __connection_header;
}; // struct WorkspaceDetection
typedef  ::aff_msgs::WorkspaceDetection_<std::allocator<void> > WorkspaceDetection;

typedef boost::shared_ptr< ::aff_msgs::WorkspaceDetection> WorkspaceDetectionPtr;
typedef boost::shared_ptr< ::aff_msgs::WorkspaceDetection const> WorkspaceDetectionConstPtr;


template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const  ::aff_msgs::WorkspaceDetection_<ContainerAllocator> & v)
{
  ros::message_operations::Printer< ::aff_msgs::WorkspaceDetection_<ContainerAllocator> >::stream(s, "", v);
  return s;}

} // namespace aff_msgs

namespace ros
{
namespace message_traits
{
template<class ContainerAllocator> struct IsMessage< ::aff_msgs::WorkspaceDetection_<ContainerAllocator> > : public TrueType {};
template<class ContainerAllocator> struct IsMessage< ::aff_msgs::WorkspaceDetection_<ContainerAllocator>  const> : public TrueType {};
template<class ContainerAllocator>
struct MD5Sum< ::aff_msgs::WorkspaceDetection_<ContainerAllocator> > {
  static const char* value() 
  {
    return "780ef7d01de10730ff3a1a4575dba282";
  }

  static const char* value(const  ::aff_msgs::WorkspaceDetection_<ContainerAllocator> &) { return value(); } 
  static const uint64_t static_value1 = 0x780ef7d01de10730ULL;
  static const uint64_t static_value2 = 0xff3a1a4575dba282ULL;
};

template<class ContainerAllocator>
struct DataType< ::aff_msgs::WorkspaceDetection_<ContainerAllocator> > {
  static const char* value() 
  {
    return "aff_msgs/WorkspaceDetection";
  }

  static const char* value(const  ::aff_msgs::WorkspaceDetection_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct Definition< ::aff_msgs::WorkspaceDetection_<ContainerAllocator> > {
  static const char* value() 
  {
    return "# Contains all the information from one run of the tabletop detection node\n\
tabletop_object_detector/TabletopDetectionResult tabletop\n\
\n\
# Informs that a planar table has been detected at a given location\n\
tabletop_object_detector/Table table\n\
\n\
================================================================================\n\
MSG: tabletop_object_detector/TabletopDetectionResult\n\
# Contains all the information from one run of the tabletop detection node\n\
\n\
# The information for the plane that has been detected\n\
Table table\n\
\n\
# The raw clusters detected in the scan \n\
sensor_msgs/PointCloud[] clusters\n\
\n\
# The list of potential models that have been detected for each cluster\n\
# An empty list will be returned for a cluster that has no recognition results at all\n\
household_objects_database_msgs/DatabaseModelPoseList[] models\n\
\n\
# For each cluster, the index of the list of models that was fit to that cluster\n\
# keep in mind that multiple raw clusters can correspond to a single fit\n\
int32[] cluster_model_indices\n\
\n\
# Whether the detection has succeeded or failed\n\
int32 NO_CLOUD_RECEIVED = 1\n\
int32 NO_TABLE = 2\n\
int32 OTHER_ERROR = 3\n\
int32 SUCCESS = 4\n\
int32 result\n\
\n\
================================================================================\n\
MSG: tabletop_object_detector/Table\n\
# Informs that a planar table has been detected at a given location\n\
\n\
# The pose gives you the transform that take you to the coordinate system\n\
# of the table, with the origin somewhere in the table plane and the \n\
# z axis normal to the plane\n\
geometry_msgs/PoseStamped pose\n\
\n\
# These values give you the observed extents of the table, along x and y,\n\
# in the table's own coordinate system (above)\n\
# there is no guarantee that the origin of the table coordinate system is\n\
# inside the boundary defined by these values. \n\
float32 x_min\n\
float32 x_max\n\
float32 y_min\n\
float32 y_max\n\
\n\
# There is no guarantee that the table doe NOT extend further than these \n\
# values; this is just as far as we've observed it.\n\
\n\
================================================================================\n\
MSG: geometry_msgs/PoseStamped\n\
# A Pose with reference coordinate frame and timestamp\n\
Header header\n\
Pose pose\n\
\n\
================================================================================\n\
MSG: std_msgs/Header\n\
# Standard metadata for higher-level stamped data types.\n\
# This is generally used to communicate timestamped data \n\
# in a particular coordinate frame.\n\
# \n\
# sequence ID: consecutively increasing ID \n\
uint32 seq\n\
#Two-integer timestamp that is expressed as:\n\
# * stamp.secs: seconds (stamp_secs) since epoch\n\
# * stamp.nsecs: nanoseconds since stamp_secs\n\
# time-handling sugar is provided by the client library\n\
time stamp\n\
#Frame this data is associated with\n\
# 0: no frame\n\
# 1: global frame\n\
string frame_id\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Pose\n\
# A representation of pose in free space, composed of postion and orientation. \n\
Point position\n\
Quaternion orientation\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Point\n\
# This contains the position of a point in free space\n\
float64 x\n\
float64 y\n\
float64 z\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Quaternion\n\
# This represents an orientation in free space in quaternion form.\n\
\n\
float64 x\n\
float64 y\n\
float64 z\n\
float64 w\n\
\n\
================================================================================\n\
MSG: sensor_msgs/PointCloud\n\
# This message holds a collection of 3d points, plus optional additional\n\
# information about each point.\n\
\n\
# Time of sensor data acquisition, coordinate frame ID.\n\
Header header\n\
\n\
# Array of 3d points. Each Point32 should be interpreted as a 3d point\n\
# in the frame given in the header.\n\
geometry_msgs/Point32[] points\n\
\n\
# Each channel should have the same number of elements as points array,\n\
# and the data in each channel should correspond 1:1 with each point.\n\
# Channel names in common practice are listed in ChannelFloat32.msg.\n\
ChannelFloat32[] channels\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Point32\n\
# This contains the position of a point in free space(with 32 bits of precision).\n\
# It is recommeded to use Point wherever possible instead of Point32.  \n\
# \n\
# This recommendation is to promote interoperability.  \n\
#\n\
# This message is designed to take up less space when sending\n\
# lots of points at once, as in the case of a PointCloud.  \n\
\n\
float32 x\n\
float32 y\n\
float32 z\n\
================================================================================\n\
MSG: sensor_msgs/ChannelFloat32\n\
# This message is used by the PointCloud message to hold optional data\n\
# associated with each point in the cloud. The length of the values\n\
# array should be the same as the length of the points array in the\n\
# PointCloud, and each value should be associated with the corresponding\n\
# point.\n\
\n\
# Channel names in existing practice include:\n\
#   \"u\", \"v\" - row and column (respectively) in the left stereo image.\n\
#              This is opposite to usual conventions but remains for\n\
#              historical reasons. The newer PointCloud2 message has no\n\
#              such problem.\n\
#   \"rgb\" - For point clouds produced by color stereo cameras. uint8\n\
#           (R,G,B) values packed into the least significant 24 bits,\n\
#           in order.\n\
#   \"intensity\" - laser or pixel intensity.\n\
#   \"distance\"\n\
\n\
# The channel name should give semantics of the channel (e.g.\n\
# \"intensity\" instead of \"value\").\n\
string name\n\
\n\
# The values array should be 1-1 with the elements of the associated\n\
# PointCloud.\n\
float32[] values\n\
\n\
================================================================================\n\
MSG: household_objects_database_msgs/DatabaseModelPoseList\n\
# stores a list of possible database models recognition results\n\
DatabaseModelPose[] model_list\n\
================================================================================\n\
MSG: household_objects_database_msgs/DatabaseModelPose\n\
# Informs that a specific model from the Model Database has been \n\
# identified at a certain location\n\
\n\
# the database id of the model\n\
int32 model_id\n\
\n\
# the pose that it can be found in\n\
geometry_msgs/PoseStamped pose\n\
\n\
# a measure of the confidence level in this detection result\n\
float32 confidence\n\
";
  }

  static const char* value(const  ::aff_msgs::WorkspaceDetection_<ContainerAllocator> &) { return value(); } 
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

template<class ContainerAllocator> struct Serializer< ::aff_msgs::WorkspaceDetection_<ContainerAllocator> >
{
  template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
  {
    stream.next(m.tabletop);
    stream.next(m.table);
  }

  ROS_DECLARE_ALLINONE_SERIALIZER;
}; // struct WorkspaceDetection_
} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::aff_msgs::WorkspaceDetection_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const  ::aff_msgs::WorkspaceDetection_<ContainerAllocator> & v) 
  {
    s << indent << "tabletop: ";
s << std::endl;
    Printer< ::tabletop_object_detector::TabletopDetectionResult_<ContainerAllocator> >::stream(s, indent + "  ", v.tabletop);
    s << indent << "table: ";
s << std::endl;
    Printer< ::tabletop_object_detector::Table_<ContainerAllocator> >::stream(s, indent + "  ", v.table);
  }
};


} // namespace message_operations
} // namespace ros

#endif // AFF_MSGS_MESSAGE_WORKSPACEDETECTION_H

