#if defined(linux) || defined(__linux) || defined(__linux__)
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <dumpTool.h>
#include <falcon_104.hpp>
#include <opencv2/opencv.hpp>
#include <CameraModule.hpp>
#include <inttypes.h>

using namespace std::chrono;
using namespace std;
using namespace cv;

static int cur_x, cur_y;
static int right_x, right_y;
void find_right_point(int x, int y, uint16_t disparity, int &result_x, int &result_y) {
	if (disparity)
		result_x = x-(disparity);
	result_y = y;
}
static char cur_pos[64];
void show_image(CameraModule* camera, int width, int height, uint16_t const *frame_ptr, Mat *out_image) {
	if (camera->GetMode() == CameraModule::P) {
		width *= 4;
	}
	Mat frame = Mat(Size(width, height), CV_8UC2, (void*)frame_ptr);
	Mat bgr_frame;
	cvtColor(frame, bgr_frame, CV_YUV2BGR_YUY2);
	bgr_frame.copyTo(*out_image);
}
void print_imu(const imu_data_st* data)
{
	printf("id %1x x %5d y %5d z %5d %ds %dns.\n", data->imu_type, data->x, data->y, data->z, data->second, data->nano_second);
}


int main(void)
{

	// ---------- camera init ---------- //	
	static int sensor_index;
	static int resolution_index;
	vector<string> sensor_confs;
	falcon_104 camera;
	camera.SetResolutionMode(CameraModule::HD); // frame Width x Height : 1280x720
	camera.SetMode(CameraModule::LRD); // frame width x height : 1280*720 *3 = 3840*720

	sensor_confs = camera.GetSensorConfNames();
	sensor_index = camera.GetSensorConfIndex();
	Mat image = Mat::zeros(cv::Size(100, 100), CV_16UC1);
	// Mat dispairty = Mat::zeros(cv::Size(1, 1), CV_16UC1);
	camera.addDataReceivedCallback(CameraModule::LRD, "show_image", [&camera, &image](int width, int height, uint16_t const *pBuffer){show_image(&camera, width, height, pBuffer, &image);});
	// camera.add_imu_data_callback("imu_callback", print_imu);

	// ----------- remove callback : remove callback ---------- //
	// camera.removeDataReceivedCallback(CameraModule::LRD, "show_image");
	
	resolution_index = camera.GetResolutionMode();
	camera.StreamOn();
	resolution_index = camera.GetResolutionMode();
	char exit_key = 27; // 27 is ESC
	int delay = 25;  //@param delay Delay in milliseconds. 0 is the special value that means "forever".
	bool show = true;
	int depth_max;
	int depth_min;
	int depth_mode;
	while(show) {
		imshow("Display frame", image);
		// camera.ExecPython("/run/media/mmcblk0p1/config/test_print.py");
		char c=(char)waitKey(delay);
		switch (c)
		{
			case 27: // 27 is ESC
				destroyAllWindows();
				camera.StreamOff();
				show = false;
				break;
			case '1':
				sensor_index++;
				if (sensor_index >= sensor_confs.size())
					sensor_index = 0;
				camera.SetSensorConf(sensor_index);
				cout << "switch to " << sensor_confs[sensor_index] << endl;
				break;
			case '2':
				resolution_index++;
				if (resolution_index >= CameraModule::Resolution_mode::RESOLUTION_MODE_NUM)
					resolution_index = 0;
				camera.SetResolutionMode((CameraModule::Resolution_mode)resolution_index);
				break;
			case '3':
				depth_max = 2000; // mm
				depth_min = 500;  // mm
				depth_mode = 0;   // 0: disparity mode 1: depth mode
				camera.DepthThreshold(depth_max, depth_min, depth_mode);
				break;
			case 'q':
			case 'Q':
				camera.ExecPython("en_ir.py on");
				break;
			case 'w':
			case 'W':
				camera.ExecPython("en_ir.py off");
				break;
			case 'e':
			case 'E':
				camera.addDataReceivedCallback(CameraModule::LRD, "dumpBinary", [&camera](int width, int height, uint16_t const *pBuffer){bool keep_doing = false;dumpBinary(&camera, width, height, pBuffer, keep_doing);});
				break;
			case 'r':
			case 'R':
				camera.ExecPython("bypass_rect.py on");
				break;
			case 't':
			case 'T':
				camera.ExecPython("bypass_rect.py off");
				break;
			case 'y':
			case 'Y':
				camera.addDataReceivedCallback(CameraModule::LRD, "printCameraInfo", [&camera](int width, int height, uint16_t const *pBuffer){bool keep_doing = false;printCameraInfo(&camera, width, height, pBuffer, keep_doing);});
				break;
		}
	}
	
	return 1;
}

#endif
