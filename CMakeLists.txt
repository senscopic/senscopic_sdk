cmake_minimum_required(VERSION 3.1)
project (camtest)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
aux_source_directory (${CMAKE_CURRENT_LIST_DIR}/src SOURCES)
include_directories(${CMAKE_CURRENT_LIST_DIR}/src)
include_directories(${CMAKE_CURRENT_LIST_DIR}/3rdparty/jsoncpp/include)
add_library(senscopic STATIC ${SOURCES})
target_link_libraries(senscopic ${CMAKE_CURRENT_LIST_DIR}/3rdparty/jsoncpp/lib/libjsoncpp.a)

set (CMAKE_CXXFLAGS_DEBUG "$ENV{SXXFLAG} -Wall -g -ggdb")
set (CMAKE_CXXFLAGS_RELEASE "$ENV{CXXFLAG} -Wall")
find_package(Threads)

find_package(OpenCV 3.4 REQUIRED)
if(OpenCV_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenCV_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenCV_CXX_FLAGS}")
    include_directories(${OpenCV_INCLUDE_DIRS})
    message(STATUS "OpenCV SDK Found : " ${OpenCV_INCLUDE_DIRS})
endif()
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUDEV REQUIRED libudev)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)

add_executable(${PROJECT_NAME} camtest.cpp)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17) 
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} senscopic ${OpenCV_LIBS} usb-1.0 udev) 
set_target_properties(${PROJECT_NAME}  PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" )

