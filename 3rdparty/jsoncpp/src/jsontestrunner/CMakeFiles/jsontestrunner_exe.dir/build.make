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
include 3rdparty/jsoncpp/src/jsontestrunner/CMakeFiles/jsontestrunner_exe.dir/depend.make

# Include the progress variables for this target.
include 3rdparty/jsoncpp/src/jsontestrunner/CMakeFiles/jsontestrunner_exe.dir/progress.make

# Include the compile flags for this target's objects.
include 3rdparty/jsoncpp/src/jsontestrunner/CMakeFiles/jsontestrunner_exe.dir/flags.make

3rdparty/jsoncpp/src/jsontestrunner/CMakeFiles/jsontestrunner_exe.dir/main.cpp.o: 3rdparty/jsoncpp/src/jsontestrunner/CMakeFiles/jsontestrunner_exe.dir/flags.make
3rdparty/jsoncpp/src/jsontestrunner/CMakeFiles/jsontestrunner_exe.dir/main.cpp.o: 3rdparty/jsoncpp/src/jsontestrunner/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/fender/camera_module/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object 3rdparty/jsoncpp/src/jsontestrunner/CMakeFiles/jsontestrunner_exe.dir/main.cpp.o"
	cd /home/fender/camera_module/3rdparty/jsoncpp/src/jsontestrunner && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/jsontestrunner_exe.dir/main.cpp.o -c /home/fender/camera_module/3rdparty/jsoncpp/src/jsontestrunner/main.cpp

3rdparty/jsoncpp/src/jsontestrunner/CMakeFiles/jsontestrunner_exe.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/jsontestrunner_exe.dir/main.cpp.i"
	cd /home/fender/camera_module/3rdparty/jsoncpp/src/jsontestrunner && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/fender/camera_module/3rdparty/jsoncpp/src/jsontestrunner/main.cpp > CMakeFiles/jsontestrunner_exe.dir/main.cpp.i

3rdparty/jsoncpp/src/jsontestrunner/CMakeFiles/jsontestrunner_exe.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/jsontestrunner_exe.dir/main.cpp.s"
	cd /home/fender/camera_module/3rdparty/jsoncpp/src/jsontestrunner && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/fender/camera_module/3rdparty/jsoncpp/src/jsontestrunner/main.cpp -o CMakeFiles/jsontestrunner_exe.dir/main.cpp.s

# Object files for target jsontestrunner_exe
jsontestrunner_exe_OBJECTS = \
"CMakeFiles/jsontestrunner_exe.dir/main.cpp.o"

# External object files for target jsontestrunner_exe
jsontestrunner_exe_EXTERNAL_OBJECTS =

bin/jsontestrunner_exe: 3rdparty/jsoncpp/src/jsontestrunner/CMakeFiles/jsontestrunner_exe.dir/main.cpp.o
bin/jsontestrunner_exe: 3rdparty/jsoncpp/src/jsontestrunner/CMakeFiles/jsontestrunner_exe.dir/build.make
bin/jsontestrunner_exe: lib/libjsoncpp.a
bin/jsontestrunner_exe: 3rdparty/jsoncpp/src/jsontestrunner/CMakeFiles/jsontestrunner_exe.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/fender/camera_module/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../../../bin/jsontestrunner_exe"
	cd /home/fender/camera_module/3rdparty/jsoncpp/src/jsontestrunner && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/jsontestrunner_exe.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
3rdparty/jsoncpp/src/jsontestrunner/CMakeFiles/jsontestrunner_exe.dir/build: bin/jsontestrunner_exe

.PHONY : 3rdparty/jsoncpp/src/jsontestrunner/CMakeFiles/jsontestrunner_exe.dir/build

3rdparty/jsoncpp/src/jsontestrunner/CMakeFiles/jsontestrunner_exe.dir/clean:
	cd /home/fender/camera_module/3rdparty/jsoncpp/src/jsontestrunner && $(CMAKE_COMMAND) -P CMakeFiles/jsontestrunner_exe.dir/cmake_clean.cmake
.PHONY : 3rdparty/jsoncpp/src/jsontestrunner/CMakeFiles/jsontestrunner_exe.dir/clean

3rdparty/jsoncpp/src/jsontestrunner/CMakeFiles/jsontestrunner_exe.dir/depend:
	cd /home/fender/camera_module && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/fender/camera_module /home/fender/camera_module/3rdparty/jsoncpp/src/jsontestrunner /home/fender/camera_module /home/fender/camera_module/3rdparty/jsoncpp/src/jsontestrunner /home/fender/camera_module/3rdparty/jsoncpp/src/jsontestrunner/CMakeFiles/jsontestrunner_exe.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : 3rdparty/jsoncpp/src/jsontestrunner/CMakeFiles/jsontestrunner_exe.dir/depend

