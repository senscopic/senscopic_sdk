#ifndef _DUMPTOOL_H_
#define _DUMPTOOL_H_

#include <fstream>
#include <CameraModule.hpp>

void printCameraInfo(CameraModule *camera, int width, int height, uint16_t const *buffer, bool keep);
void dumpBinary(CameraModule* camera, int width, int height, uint16_t const *buffer, bool keep);
void getDurationTime(CameraModule* camera, int width, int height, uint16_t const *buffer, bool keep);
void printTimestamp(CameraModule *camera, int width, int height, uint16_t const *buffer, bool keep);
#endif