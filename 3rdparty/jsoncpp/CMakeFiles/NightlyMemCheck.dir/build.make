# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/fender/camera_module

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/fender/camera_module

# Utility rule file for NightlyMemCheck.

# Include the progress variables for this target.
include 3rdparty/jsoncpp/CMakeFiles/NightlyMemCheck.dir/progress.make

3rdparty/jsoncpp/CMakeFiles/NightlyMemCheck:
	cd /home/fender/camera_module/3rdparty/jsoncpp && /usr/local/bin/ctest -D NightlyMemCheck

NightlyMemCheck: 3rdparty/jsoncpp/CMakeFiles/NightlyMemCheck
NightlyMemCheck: 3rdparty/jsoncpp/CMakeFiles/NightlyMemCheck.dir/build.make

.PHONY : NightlyMemCheck

# Rule to build all files generated by this target.
3rdparty/jsoncpp/CMakeFiles/NightlyMemCheck.dir/build: NightlyMemCheck

.PHONY : 3rdparty/jsoncpp/CMakeFiles/NightlyMemCheck.dir/build

3rdparty/jsoncpp/CMakeFiles/NightlyMemCheck.dir/clean:
	cd /home/fender/camera_module/3rdparty/jsoncpp && $(CMAKE_COMMAND) -P CMakeFiles/NightlyMemCheck.dir/cmake_clean.cmake
.PHONY : 3rdparty/jsoncpp/CMakeFiles/NightlyMemCheck.dir/clean

3rdparty/jsoncpp/CMakeFiles/NightlyMemCheck.dir/depend:
	cd /home/fender/camera_module && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/fender/camera_module /home/fender/camera_module/3rdparty/jsoncpp /home/fender/camera_module /home/fender/camera_module/3rdparty/jsoncpp /home/fender/camera_module/3rdparty/jsoncpp/CMakeFiles/NightlyMemCheck.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : 3rdparty/jsoncpp/CMakeFiles/NightlyMemCheck.dir/depend

