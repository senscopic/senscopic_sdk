# Intro

Streaming api for falcon.

# How to use

### 1. Install dependencies
* install [opencv 3.4.4](https://github.com/opencv/opencv/releases/tag/3.4.4)
* run install_3rd.sh
### 2. compile :
```bash
cd camera_module
./install_3rd.sh
./compile.sh
```
### 3. run camtest:
```bash
./camtest
```

### 4. camtest function

* press 1 : switch sensor config : 3,6,9,12 cm

* press 2 : switch resolution VGA/HD

* press 3 : enable depthThreshold

* press q : ir pattern on

* press w : ir pattern off

* press e : dump image

* press r : bypass_rectify on 

* press t : bypass_rectify off

* press y : print camera_parameter

# example code 

* camtest.cpp : main function, show how to use callback function.

* src/dumpTool.cpp : callback function example

* stc/falcon_104.hpp : api of CameraModule 

# image format

* image format : 

	* Left-Color & Right-Color : YUY2

	* Disparity : 2 bytes per pixel
  
* image content : 
  * VGA : 
    * Left-Color (640 x 480) + Right-Color (640 x 480) + Disparity (640 x 480) = (1920 width x 480 height)
  * HD : 
    * Left-Color (1280x720) + Right-Color (1280x720) + Disparity (1280x720) = (3840*720)

# 3rdparty library

* jsoncpp

* libudev-dev 

* libusb-1.0-0-dev

* opencv 3.4.4
