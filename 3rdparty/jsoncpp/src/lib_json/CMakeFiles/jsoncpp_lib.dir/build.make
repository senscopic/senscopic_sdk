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

# Include any dependencies generated for this target.
include 3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/depend.make

# Include the progress variables for this target.
include 3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/progress.make

# Include the compile flags for this target's objects.
include 3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/flags.make

3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/json_reader.cpp.o: 3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/flags.make
3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/json_reader.cpp.o: 3rdparty/jsoncpp/src/lib_json/json_reader.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fender/camera_module/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object 3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/json_reader.cpp.o"
	cd /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/jsoncpp_lib.dir/json_reader.cpp.o -c /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json/json_reader.cpp

3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/json_reader.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/jsoncpp_lib.dir/json_reader.cpp.i"
	cd /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json/json_reader.cpp > CMakeFiles/jsoncpp_lib.dir/json_reader.cpp.i

3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/json_reader.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/jsoncpp_lib.dir/json_reader.cpp.s"
	cd /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json/json_reader.cpp -o CMakeFiles/jsoncpp_lib.dir/json_reader.cpp.s

3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/json_value.cpp.o: 3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/flags.make
3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/json_value.cpp.o: 3rdparty/jsoncpp/src/lib_json/json_value.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fender/camera_module/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object 3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/json_value.cpp.o"
	cd /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/jsoncpp_lib.dir/json_value.cpp.o -c /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json/json_value.cpp

3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/json_value.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/jsoncpp_lib.dir/json_value.cpp.i"
	cd /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json/json_value.cpp > CMakeFiles/jsoncpp_lib.dir/json_value.cpp.i

3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/json_value.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/jsoncpp_lib.dir/json_value.cpp.s"
	cd /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json/json_value.cpp -o CMakeFiles/jsoncpp_lib.dir/json_value.cpp.s

3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/json_writer.cpp.o: 3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/flags.make
3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/json_writer.cpp.o: 3rdparty/jsoncpp/src/lib_json/json_writer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fender/camera_module/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object 3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/json_writer.cpp.o"
	cd /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/jsoncpp_lib.dir/json_writer.cpp.o -c /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json/json_writer.cpp

3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/json_writer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/jsoncpp_lib.dir/json_writer.cpp.i"
	cd /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json/json_writer.cpp > CMakeFiles/jsoncpp_lib.dir/json_writer.cpp.i

3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/json_writer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/jsoncpp_lib.dir/json_writer.cpp.s"
	cd /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json/json_writer.cpp -o CMakeFiles/jsoncpp_lib.dir/json_writer.cpp.s

# Object files for target jsoncpp_lib
jsoncpp_lib_OBJECTS = \
"CMakeFiles/jsoncpp_lib.dir/json_reader.cpp.o" \
"CMakeFiles/jsoncpp_lib.dir/json_value.cpp.o" \
"CMakeFiles/jsoncpp_lib.dir/json_writer.cpp.o"

# External object files for target jsoncpp_lib
jsoncpp_lib_EXTERNAL_OBJECTS =

lib/libjsoncpp.a: 3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/json_reader.cpp.o
lib/libjsoncpp.a: 3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/json_value.cpp.o
lib/libjsoncpp.a: 3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/json_writer.cpp.o
lib/libjsoncpp.a: 3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/build.make
lib/libjsoncpp.a: 3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/fender/camera_module/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX static library ../../../../lib/libjsoncpp.a"
	cd /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json && $(CMAKE_COMMAND) -P CMakeFiles/jsoncpp_lib.dir/cmake_clean_target.cmake
	cd /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/jsoncpp_lib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/build: lib/libjsoncpp.a

.PHONY : 3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/build

3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/clean:
	cd /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json && $(CMAKE_COMMAND) -P CMakeFiles/jsoncpp_lib.dir/cmake_clean.cmake
.PHONY : 3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/clean

3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/depend:
	cd /home/fender/camera_module && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/fender/camera_module /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json /home/fender/camera_module /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json /home/fender/camera_module/3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : 3rdparty/jsoncpp/src/lib_json/CMakeFiles/jsoncpp_lib.dir/depend
