# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/kadir/interconnection/workspace/metu-ros-pkg/stacks/aff_learning/humanoid_aff_learning/icub_aff_learning/actionPrimitives

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kadir/interconnection/workspace/metu-ros-pkg/stacks/aff_learning/humanoid_aff_learning/icub_aff_learning/actionPrimitives

# Include any dependencies generated for this target.
include CMakeFiles/actionPrimitives.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/actionPrimitives.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/actionPrimitives.dir/flags.make

CMakeFiles/actionPrimitives.dir/src/actionPrimitives.o: CMakeFiles/actionPrimitives.dir/flags.make
CMakeFiles/actionPrimitives.dir/src/actionPrimitives.o: src/actionPrimitives.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kadir/interconnection/workspace/metu-ros-pkg/stacks/aff_learning/humanoid_aff_learning/icub_aff_learning/actionPrimitives/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/actionPrimitives.dir/src/actionPrimitives.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/actionPrimitives.dir/src/actionPrimitives.o -c /home/kadir/interconnection/workspace/metu-ros-pkg/stacks/aff_learning/humanoid_aff_learning/icub_aff_learning/actionPrimitives/src/actionPrimitives.cpp

CMakeFiles/actionPrimitives.dir/src/actionPrimitives.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/actionPrimitives.dir/src/actionPrimitives.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/kadir/interconnection/workspace/metu-ros-pkg/stacks/aff_learning/humanoid_aff_learning/icub_aff_learning/actionPrimitives/src/actionPrimitives.cpp > CMakeFiles/actionPrimitives.dir/src/actionPrimitives.i

CMakeFiles/actionPrimitives.dir/src/actionPrimitives.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/actionPrimitives.dir/src/actionPrimitives.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/kadir/interconnection/workspace/metu-ros-pkg/stacks/aff_learning/humanoid_aff_learning/icub_aff_learning/actionPrimitives/src/actionPrimitives.cpp -o CMakeFiles/actionPrimitives.dir/src/actionPrimitives.s

CMakeFiles/actionPrimitives.dir/src/actionPrimitives.o.requires:
.PHONY : CMakeFiles/actionPrimitives.dir/src/actionPrimitives.o.requires

CMakeFiles/actionPrimitives.dir/src/actionPrimitives.o.provides: CMakeFiles/actionPrimitives.dir/src/actionPrimitives.o.requires
	$(MAKE) -f CMakeFiles/actionPrimitives.dir/build.make CMakeFiles/actionPrimitives.dir/src/actionPrimitives.o.provides.build
.PHONY : CMakeFiles/actionPrimitives.dir/src/actionPrimitives.o.provides

CMakeFiles/actionPrimitives.dir/src/actionPrimitives.o.provides.build: CMakeFiles/actionPrimitives.dir/src/actionPrimitives.o
.PHONY : CMakeFiles/actionPrimitives.dir/src/actionPrimitives.o.provides.build

# Object files for target actionPrimitives
actionPrimitives_OBJECTS = \
"CMakeFiles/actionPrimitives.dir/src/actionPrimitives.o"

# External object files for target actionPrimitives
actionPrimitives_EXTERNAL_OBJECTS =

/home/kadir/my_tools/icub_tools/iCub/main/lib/libactionPrimitives.a: CMakeFiles/actionPrimitives.dir/src/actionPrimitives.o
/home/kadir/my_tools/icub_tools/iCub/main/lib/libactionPrimitives.a: CMakeFiles/actionPrimitives.dir/build.make
/home/kadir/my_tools/icub_tools/iCub/main/lib/libactionPrimitives.a: CMakeFiles/actionPrimitives.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX static library /home/kadir/my_tools/icub_tools/iCub/main/lib/libactionPrimitives.a"
	$(CMAKE_COMMAND) -P CMakeFiles/actionPrimitives.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/actionPrimitives.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/actionPrimitives.dir/build: /home/kadir/my_tools/icub_tools/iCub/main/lib/libactionPrimitives.a
.PHONY : CMakeFiles/actionPrimitives.dir/build

CMakeFiles/actionPrimitives.dir/requires: CMakeFiles/actionPrimitives.dir/src/actionPrimitives.o.requires
.PHONY : CMakeFiles/actionPrimitives.dir/requires

CMakeFiles/actionPrimitives.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/actionPrimitives.dir/cmake_clean.cmake
.PHONY : CMakeFiles/actionPrimitives.dir/clean

CMakeFiles/actionPrimitives.dir/depend:
	cd /home/kadir/interconnection/workspace/metu-ros-pkg/stacks/aff_learning/humanoid_aff_learning/icub_aff_learning/actionPrimitives && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kadir/interconnection/workspace/metu-ros-pkg/stacks/aff_learning/humanoid_aff_learning/icub_aff_learning/actionPrimitives /home/kadir/interconnection/workspace/metu-ros-pkg/stacks/aff_learning/humanoid_aff_learning/icub_aff_learning/actionPrimitives /home/kadir/interconnection/workspace/metu-ros-pkg/stacks/aff_learning/humanoid_aff_learning/icub_aff_learning/actionPrimitives /home/kadir/interconnection/workspace/metu-ros-pkg/stacks/aff_learning/humanoid_aff_learning/icub_aff_learning/actionPrimitives /home/kadir/interconnection/workspace/metu-ros-pkg/stacks/aff_learning/humanoid_aff_learning/icub_aff_learning/actionPrimitives/CMakeFiles/actionPrimitives.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/actionPrimitives.dir/depend

