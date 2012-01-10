#include "iostream"
#include "sstream"
#include "string"

//#include "gazebo_plugins/gaze"
#include "gazebo/XMLConfig.hh"
#include "gazebo/SpawnModel.h"

#include "ros/package.h"
#include "ros/ros.h"

bool addObject(std::string object_model_file_name) {
	gazebo::SpawnModel spawn_model;
	std::string file_path = ros::package::getPath("pr2_affordances_manager")
			+ "/objects/" + object_model_file_name;
	//  std::cout<<file_path<<std::endl;
	std::string file_contents;
	std::ifstream point_file(file_path.c_str());
	std::string line;
	if (point_file.is_open()) {
		//    std::cout<<"file is opened"<<std::endl;
		while (point_file.good()) {
			getline(point_file, line);
			file_contents += line;
		}
		point_file.close();
	}
	spawn_model.request.model_name = "model";
	spawn_model.request.model_xml = file_contents;
	spawn_model.request.initial_pose.position.x = 0.65;
	spawn_model.request.initial_pose.position.y = -0.3;
	spawn_model.request.initial_pose.position.z = 0.9;
	//  std::cout<<file_contents<<std::endl;
	bool service_called = ros::service::call("gazebo/spawn_gazebo_model",
			spawn_model);
	if (service_called) {
		if (spawn_model.response.success) {
			std::cout << "service is called" << std::endl;
			return true;
		} else {
			ROS_ERROR_STREAM(spawn_model.response.status_message);
			return false;
		}
	} else {
		std::cout << "service is not called" << std::endl;
		return false;
	}
}

int main() {
	return 0;
}
