#ifndef _FALCON_104_H_
#define _FALCON_104_H_

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
	#include "stdafx.h"
	#include <dshow.h>

	#include "qedit.h"
	#include <iostream>
	#include <objbase.h>
	#include <atlconv.h>
	#include <strmif.h>
	#include <Windows.h>
	#include <vidcap.h>         // For IKsTopologyInfo  
	#include <ksproxy.h>        // For IKsControl  
	#include <ks.h>
// #include <ksmedia.h>

	#include <thread>

	#include "demokit_data_type.h"
	#pragma comment(lib,"Strmiids.lib")
	#pragma comment(lib,"strmbase.lib")

	#define CAMERA_PREVIEW_VIDEO_WIDTH     960
	#define CAMERA_PREVIEW_VIDEO_HEIGHT    480

	#define USE_CAMERA_PREVIEW	0
	#define USE_UVC_CAMERA	1
#endif

#include <CameraModule.hpp>
#include <uvc_interface.h>
#include "hidapi.h"
#include <wchar.h>
#include <string.h>
#if defined(linux) || defined(__linux) || defined(__linux__) 
	#include <sys/types.h>
	#include <sys/stat.h>

	#define USE_SHOW_CAMERA_PARAM (0)
#endif

#if defined(linux) || defined(__linux) || defined(__linux__) 
class falcon_104 : public CameraModule{
private:

	typedef struct{
		void   *ptr;
		size_t size;
	} frame_buffer;
	

	int fd = -1;
	pthread_t getframe_thread_id;
	pthread_t imu_thread_id;
	uint64_t time_stamp = 0;
	struct v4l2_format v4l2_fmt;
	struct v4l2_buffer v4l2_buf;
	struct v4l2_requestbuffers v4l2_reqbuf;
	vector<frame_buffer> frame_buffers;
	int ir_freq = 1084; //500;
 	int ir_duty = 30; // 50;
	const string config_dir = "./camera_config/";
	const string dev_config_path = "/run/media/mmcblk0p1/config_dirty/";
	Json::Value camera_parameter;
	Json::Value sensor_conf_list;
	const int v4l2_buffer_number = 2;
public:
	bool is_streaming = false;
	bool ir_enable = false;
	bool hole_filling_enable = true;
public:
	
	falcon_104(Mode inmode = Mode::LRD) : CameraModule(inmode, CameraModel::PINHOLE) {
		int ret = OpenCamera();
		if (ret < 0)
			return;
		
		ShowCamInfo();
		loadCameraParameterFile();
		SetResolutionMode(Resolution_mode::VGA);
		SetSensorConf(0);
		SetMode(Mode::LRD);
		open_camera_st = 0;
		pthread_create(&imu_thread_id, NULL, &(falcon_104::run_imu_thread), this);
		ExecPython("imu.py off");
	}
	~falcon_104() {
		while (is_streaming) {
			StreamOff();
		}
		for (int i = 0; i < frame_buffers.size(); ++i) {
			munmap(frame_buffers[i].ptr, frame_buffers[i].size);
		}
		if (imu_thread_id>0)
			pthread_kill(imu_thread_id, SIGKILL);
		if (getframe_thread_id>0)
			pthread_kill(getframe_thread_id, SIGKILL);
		if (fd > 0)
			close(fd);
	}
	uint64_t GetTimestamp(uint16_t const* Buffer)
	{
		return GetTimestamp_(Buffer);
	}

	int GetSensorConfNum() {
		return sensor_conf_list.size();
	}
	vector<string> GetSensorConfNames() {
		vector<string> output;
		for (int i=0; i<sensor_conf_list.size(); i++)
			output.push_back(sensor_conf_list[i].asString());
		return output;
	}
	int GetSensorConfIndex() {
		return sensor_conf_index;
	}
	void EnableHoleFill(bool input) {
		if (input) {
			ExecPython("en_bilat.py on");
		}
		else{
			ExecPython("en_bilat.py off");
		}
	}
	void EnableIR(bool input, bool force=false) override {
		if (input) {
			ExecPython("en_ir.py on");
		}else{
			ExecPython("en_ir.py off");
		}
	}
	void EnableIMU(bool input, bool force=true) override{
		if (input) {
			ExecPython("imu.py on");
		}else{
			ExecPython("imu.py off");
		}
	}
	void SetSensorConf(int index) {
		if (index > sensor_conf_list.size()) {
			printf("%d out of index of sensor config list\n", index);
			return;
		}
		sensor_conf_index = index;
		ExecPython(sensor_conf_list[index].asString().c_str());
		notifySettingChanged();
		cout << "set sensor_conf to " << sensor_conf_list[sensor_conf_index].asString() << endl;
		
	}
	void ExecPython(const char* filename) {
		string python_path = dev_config_path + "/" + filename;
		cout << "exec python on device : " << python_path << endl;
		uvc_exec_python(fd, python_path.c_str());
	}
	void ReadFile(const char* src_name, const char* dst_dir ) {
		string dev_file_path = dev_config_path + "/" + src_name;
		cout << "read file on device : " << dev_file_path << " ->" << dst_dir << endl;
		uvc_read_file(fd, (char*)dev_file_path.c_str(), (char*)dst_dir);
	}
	void ReadFileGlobal(const char* src_name, const char* dst_dir ) {
		cout << "read file on device : " << src_name << " ->" << dst_dir << endl;
		uvc_read_file(fd, (char*)src_name, (char*)dst_dir);
	}
	void SetMode(Mode in_mode) override{
		// printf("enter SetMode\n");
		bool restart_stream = is_streaming;
		if (restart_stream)
			StreamOff();
		if (modeSupport(resolution, in_mode)) {
			// printf("before notifySettingChanged mode %d\n", mode);
			mode = in_mode;
			// printf("after notifySettingChanged mode %d\n", mode);
			cout << "set mode to " << mode << endl;
		}
		if (restart_stream)
			StreamOn();
		notifySettingChanged();
	}
	void SetResolutionMode(Resolution_mode input) {
		bool restart_stream =is_streaming;
		if (restart_stream)
			StreamOff();

		// check if mode not supported, change mode to LRD
		if (!modeSupport(input, mode)) {
			mode = Mode::LRD;
		}
		resolution = input;
		cout << "set resolution to " << input << endl;
		if (restart_stream)
			StreamOn();
		notifySettingChanged();
	}
	
	int GetFrameWidth() override{
		switch (mode)
		{
			case Mode::LRD:
			case Mode::LR:
			case Mode::LD:
			case Mode::L:
			case Mode::R:
			case Mode::D:
				return v4l2_fmt.fmt.pix.width;
			case Mode::P:
				// debug
				// cerr << "frame width: " << (v4l2_fmt.fmt.pix.width/4) << endl;
				return (v4l2_fmt.fmt.pix.width/4);
		}
        return 0;
    }
	int GetFrameHeight() override{
		return v4l2_fmt.fmt.pix.height;
	}
    void GetFrameSize(int &width, int &height) override{
    	width = GetFrameWidth();
        height = GetFrameHeight();
    }

	void ShowCamInfo() {
		struct v4l2_capability cap;
		if (ioctl(fd, VIDIOC_QUERYCAP, &cap)) {
			cerr << "VIDIOC_QUERYCAP failed : ";
		}
		printf("===================================\n");
		printf("|       Camera  Informations      |\n");
		printf("===================================\n");
		printf("driver: %s\n", cap.driver);
		printf("card: %s\n", cap.card);
		printf("bus_info: %s\n", cap.bus_info);
		printf("version:  %u.%u.%u\n", (cap.version >> 16) & 0xFF, 
			(cap.version >> 8) & 0xFF, cap.version & 0xFF);
		printf("capabilities: %08x\n", cap.capabilities);
		// print some capabilities, which are in most camera
		if(V4L2_CAP_VIDEO_CAPTURE & cap.capabilities) 
			printf("\tV4L2_CAP_VIDEO_CAPTURE\n");
		if(V4L2_CAP_STREAMING & cap.capabilities) 
			printf("\tV4L2_CAP_STREAMING\n");
		if(V4L2_CAP_DEVICE_CAPS & cap.capabilities) 
			printf("\tV4L2_CAP_DEVICE_CAPS\n");

		struct v4l2_fmtdesc supportedFmt;
		supportedFmt.index = 0;
		supportedFmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		printf("support resolutions:\n");
		while (0 <= ioctl(fd, VIDIOC_ENUM_FMT, &supportedFmt)) 
		{
			printf("%s", &supportedFmt.description[0]);

			if(V4L2_FMT_FLAG_COMPRESSED & supportedFmt.flags) 
			 printf("\tV4L2_FMT_FLAG_COMPRESSED");

			if(V4L2_FMT_FLAG_EMULATED & supportedFmt.flags) 
			 printf("\tV4L2_FMT_FLAG_EMULATED"); 

			printf("\n");

			struct v4l2_frmsizeenum frameSize;

			frameSize.index = 0;
			frameSize.pixel_format = supportedFmt.pixelformat;

			while(0 <= ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frameSize)) 
			{
				if (V4L2_FRMSIZE_TYPE_DISCRETE == frameSize.type) 
				{
				 printf("\t%dx%d, V4L2_FRMSIZE_TYPE_DISCRETE\n", 
				  frameSize.discrete.width, frameSize.discrete.height);
				}
				else if (V4L2_FRMSIZE_TYPE_STEPWISE == frameSize.type) 
				{
				 printf("\t%dx%d, V4L2_FRMSIZE_TYPE_STEPWISE\n", 
				  frameSize.stepwise.max_width, frameSize.stepwise.max_height);
				}
				frameSize.index++;
			}
			supportedFmt.index++;
		}
		printf("=============================\n");
	}
 	float GetCameraParam(string paramName) override{
		string conf_name = sensor_conf_list[sensor_conf_index].asCString();
		string resolution_str;
		switch(resolution) {
			default:
				resolution_str = "vga";
				break;
			case Resolution_mode::VGA:
				resolution_str = "vga";
				break;
			case Resolution_mode::HD:
				resolution_str = "hd";
				break;
		}
		return camera_parameter[conf_name][resolution_str][paramName].asFloat();
    }
	void StreamOn() {
		if (is_streaming)
		{
			cout << "camera is already streaming ..." << endl;
		}
		else
		{
			initStream();
			enum v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			xioctl(fd, VIDIOC_STREAMON, &buf_type);
			sleep(0.5);
			SetSensorConf(sensor_conf_index);
			cout << "stream on ..." << endl;
			is_streaming = true;
			pthread_create(&getframe_thread_id, NULL, &falcon_104::getframe, this);
			EnableIR(ir_enable);
		}
	}

	int OpenCamera() {
		if (fd != -1)
			close(fd);
		// open device`
		string camera_path;
		fd = -1;
		for (int i=0; i<10; ++i) {
			camera_path = "/dev/video" + to_string(i);
			fd = open(camera_path.c_str(), O_RDWR, 0);
			if (fd != -1) {
				struct v4l2_capability cap;
				if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == 0) {
					string name((const char*)cap.card);
					if (name.find("UVC Gadget") != name.npos) {
						break;
					}
				}
				close(fd);
				fd = -1;
			}
		}
		if (fd < 0) {
			printf("camera not found\n");
			SetOpenCameraState(-1);
			return -1;
		}
		printf("Open camera\n");
		return 1;
	}
	void StreamOff() {
		if (is_streaming) {
			EnableIR(false, true);
			is_streaming = false;
			pthread_join(getframe_thread_id, NULL);
			enum v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			xioctl(fd, VIDIOC_STREAMOFF, &buf_type);
			cout << "stream off ..." << endl;
			for (int i = 0; i < frame_buffers.size(); ++i) {
				munmap(frame_buffers[i].ptr, frame_buffers[i].size);
			}
			close(fd);
			OpenCamera();
		}else{
			cout <<"camera is already stream off..." << endl;
		}
	}
	void loadCameraParameterFile(){
		
		struct stat st = {0};
		if (stat(config_dir.c_str(), &st) == -1) {
			mkdir(config_dir.c_str(), 0700);
		}
		// load camera_parameter
		std::ifstream ifs;
		ReadFile("camera_parameter.json", config_dir.c_str());
		ifs.open(config_dir + "/camera_parameter.json");
		cout << "load config : " << config_dir << "/camera_parameter.json" << endl;
		Json::CharReaderBuilder builder;
		builder["collectComments"] = true;
		JSONCPP_STRING errs;
		
		if (!parseFromStream(builder, ifs, &camera_parameter, &errs)) {
			std::cout << errs << std::endl;
			cout << "can't parse " << config_dir << "/camera_parameter.json" << ", please check file." << endl;
			exit(EXIT_FAILURE);
		}
		ifs.close();
		
		ReadFileGlobal("/tmp/sensor_conf_list.json", (char*)config_dir.c_str());
		ifs.open(config_dir + "/sensor_conf_list.json");
		builder["collectComments"] = true;
		if (!parseFromStream(builder, ifs, &sensor_conf_list, &errs)) {
			std::cout << errs << std::endl;
			cout << "can't parse " << config_dir << "/sensor_conf_list.json" << ", please check file." << endl;
			exit(EXIT_FAILURE);
		}
		cout << endl << "===== sensor conf list =====" << endl;
		for (int i=0; i<sensor_conf_list.size(); i++) {
			cout << i << ". " << sensor_conf_list[i] << endl;
		}
		cout << "============================" << endl << endl;
		// cout << camera_parameter << endl;
		notifySettingChanged();
	}

private:
	static void *run_imu_thread(void *context)
    {
        return ((falcon_104 *)context)->imu_thread();
    }
	void *imu_thread() {
		int res;
		unsigned char buf[64];
		hid_device *handle;

	#ifdef WIN32
		UNREFERENCED_PARAMETER(argc);
		UNREFERENCED_PARAMETER(argv);
	#endif

		struct hid_device_info *devs, *cur_dev;
		
		if (hid_init())
			return &res;

#ifdef DEBUG_HID
		devs = hid_enumerate(0x0, 0x0);
		cur_dev = devs;	
		while (cur_dev) {
			printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
			printf("\n");
			printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
			printf("  Product:      %ls\n", cur_dev->product_string);
			printf("  Release:      %hx\n", cur_dev->release_number);
			printf("  Interface:    %d\n",  cur_dev->interface_number);
			printf("  Usage (page): 0x%hx (0x%hx)\n", cur_dev->usage, cur_dev->usage_page);
			printf("\n");
			cur_dev = cur_dev->next;
		}
		hid_free_enumeration(devs);

		// Set up the command buffer.
		memset(buf,0x00,sizeof(buf));
		buf[0] = 0x01;
		buf[1] = 0x81;
		
#endif
		// Open the device using the VID, PID,
		// and optionally the Serial number.
		////handle = hid_open(0x4d8, 0x3f, L"12345");
		handle = hid_open(0x1d6b, 0x0102, NULL);
		if (!handle) {
			printf("unable to open hid device\n");
			return &res;
		}
#ifdef DEBUG_HID
	wchar_t wstr[256];
		// Read the Manufacturer String
		wstr[0] = 0x0000;
		res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
		if (res < 0)
			printf("Unable to read manufacturer string\n");
		printf("Manufacturer String: %ls\n", wstr);

		// Read the Product String
		wstr[0] = 0x0000;
		res = hid_get_product_string(handle, wstr, MAX_STR);
		if (res < 0)
			printf("Unable to read product string\n");
		printf("Product String: %ls\n", wstr);

		// Read the Serial Number String
		wstr[0] = 0x0000;
		res = hid_get_serial_number_string(handle, wstr, MAX_STR);
		if (res < 0)
			printf("Unable to read serial number string\n");
		printf("Serial Number String: (%d) %ls", wstr[0], wstr);
		printf("\n");

		// Read Indexed String 1
		wstr[0] = 0x0000;
		res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
		if (res < 0)
			printf("Unable to read indexed string 1\n");
		printf("Indexed String 1: %ls\n", wstr);
#endif
		// Set the hid_read() function to be non-blocking.
		if(hid_set_nonblocking(handle, 0) == 0) {
			printf("setting block mode successed.\n");
		}
		// Read requested state. hid_read() has been set to be
		// non-blocking by the call to hid_set_nonblocking() above.
		// This loop demonstrates the non-blocking nature of hid_read().
		res = 0;
		imu_data_st imu_data;
		while (1) {
			res = hid_read(handle, buf, sizeof(buf));
			if (res > 0)
			{
				// Print out the returned buffer.
				memcpy(&imu_data.imu_type, buf , 1);
				memcpy(&imu_data.x, buf + 1, 2); 
				memcpy(&imu_data.y, buf + 3, 2); 
				memcpy(&imu_data.z, buf + 5, 2); 
				memcpy(&imu_data.nano_second, buf + 7, 4); 
				memcpy(&imu_data.second, buf + 11, 4); 
			}
			notify_imu_data_receiver((const imu_data_st *)&imu_data);
		}
		hid_close(handle);

		/* Free static HIDAPI objects. */
		hid_exit();

	#ifdef WIN32
		system("pause");
	#endif

		return 0;
		}
		static void *getframe(void *context)
		{
			return ((falcon_104 *)context)->getFrame();
		}
		void *getFrame() {
			struct timeval t;
			static int ret = -1;
			static int v4l2_idx = 0;
#if USE_SHOW_CAMERA_PARAM
			printf("=========== show camera_parameter ==========\n");
			printf("camera_module mode : %d\n", mode);
			printf("sensor_conf_index  : %s\n", sensor_conf_list[sensor_conf_index].asString().c_str());
			printf("baseline           : %f\n", GetCameraParam("baseline"));
			printf("fx                 : %f\n", GetCameraParam("fx"));
			printf("fy                 : %f\n", GetCameraParam("fy"));
			printf("cx                 : %f\n", GetCameraParam("cx"));
			printf("cy                 : %f\n", GetCameraParam("cy"));
			// printf("resolution         : %dx%d\n", v4l2_fmt.fmt.pix.width, v4l2_fmt.fmt.pix.height);
			printf("==========================================\n");
#endif
			v4l2_idx = 0;
			memset(&(v4l2_buf), 0, sizeof(v4l2_buf));
			v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			v4l2_buf.memory = V4L2_MEMORY_MMAP;
			while (is_streaming) {
				// printf("before DQBUF\n");
				v4l2_buf.index = v4l2_idx;
				ret = xioctl(fd, VIDIOC_DQBUF, &v4l2_buf);
				if (ret<0)
					continue;
				// printf("after DQBUF\n");

				// printf("v4l2_buf.index : %d\n", v4l2_buf.index);
				notifyDataReceiver((const uint16_t *)frame_buffers[v4l2_buf.index].ptr);
				// printf("before QBUF\n");
				ret = xioctl(fd, VIDIOC_QBUF, &v4l2_buf);
				v4l2_idx = (v4l2_idx +1) % v4l2_buffer_number;
				// printf("after QBUF\n");
				
			}
	}
	
	uint64_t GetTimestamp_(uint16_t const *Buffer)
	{
		uint64_t timestamp = 0;
		int timestamp_offset = 1;
		uint16_t *ptr = (uint16_t*)Buffer;
		switch(GetMode()) {
			case CameraModule::LD:
			case CameraModule::LRD:
				ptr += timestamp_offset;
				for (int i=0; i<64; i++) {
					timestamp += (((uint64_t)*ptr & 0x1) << i);
					ptr++;
				}
				break;
			case CameraModule::P:
				ptr += (timestamp_offset*7);
				for (int i=0; i<64; i++) {
					timestamp += ((*ptr & 0x1) << i);
					ptr+=4;
				}
				break;
			default:
				cout << "TODO: support timestamp " << endl;
		}
		return timestamp;
		// printf("timestamp %lu\n", time_stamp);
	}
	int xioctl(int fd, int request, void *arg)
	{
		// static int count = 0;
		// count = 0;
		static int ret;
		do {
//				cout << "IO request :" << request << endl;
			ret = ioctl(fd, request, arg);
			if (ret == -1) {
				// count++;
				// if (count > 10) {
					// fprintf(stderr, "[io err %d] %s, more than %d\n", errno, strerror(errno), count);
				// }
				switch(errno) {
					case EINTR:
						break;
					case EAGAIN:
						usleep(1000);
						break;
					default:
						fprintf(stderr, "[io err %d] %s\n", errno, strerror(errno));
				}
			}
		} while (ret == -1 && ((errno == EINTR) || (errno == EAGAIN) || (errno == EBUSY)));

		if (ret == -1) {
			fprintf(stderr, "[io err %d] %s\n", errno, strerror(errno));
		}
		return ret;
	}
	void initStream() 
	{
		
		int prefer_frame_w, prefer_frame_h, prefer_buffer_num, prefer_pixel_format;
		switch (resolution) {
			case Resolution_mode::HD:
				prefer_frame_w = 1280;
				prefer_frame_h = 720;
				prefer_buffer_num = v4l2_buffer_number; // more buffer count , more delay
				prefer_pixel_format = V4L2_PIX_FMT_YUYV;
				break;
			case Resolution_mode::VGA:
				prefer_frame_w = 640;
				prefer_frame_h = 480;
				prefer_buffer_num = v4l2_buffer_number;
				prefer_pixel_format = V4L2_PIX_FMT_YUYV;
				break;
			default:
				prefer_frame_w = 1280;
				prefer_frame_h = 720;
				prefer_buffer_num = v4l2_buffer_number;
				prefer_pixel_format = V4L2_PIX_FMT_YUYV;
		}
		switch (mode) {
			case Mode::L:
			case Mode::R:
			case Mode::D:
				break;
			case Mode::LD:
			case Mode::LR:
				prefer_frame_w *= 2;
				break;
			case Mode::LRD:
				prefer_frame_w *= 3;
				break;
			case Mode::P:
				prefer_frame_w *= 4;
				break;
		}
		memset(&v4l2_fmt, 0, sizeof(v4l2_fmt));
		v4l2_fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		v4l2_fmt.fmt.pix.width       = prefer_frame_w;
		v4l2_fmt.fmt.pix.height      = prefer_frame_h;
		v4l2_fmt.fmt.pix.pixelformat = prefer_pixel_format;
		v4l2_fmt.fmt.pix.field       = V4L2_FIELD_ANY;
		xioctl(fd, VIDIOC_S_FMT, &v4l2_fmt);
		if (v4l2_fmt.fmt.pix.width != prefer_frame_w || v4l2_fmt.fmt.pix.height != prefer_frame_h)
		{
			fprintf(stderr, "[warning] request resolution couldn't be reached -> [%d, %d]\n", prefer_frame_w, prefer_frame_h );
		}
		
		if (prefer_pixel_format != v4l2_fmt.fmt.pix.pixelformat)
		{
			char fmtstr[8];
			memset(&fmtstr[0], 0, 8);
			memcpy(&fmtstr[0], &prefer_pixel_format, 4);
			printf("\tpixelformat: %s\n", &fmtstr[0]);
			fprintf(stderr, "[warning] reauest format couldn't be reached -> %s\n", &fmtstr[0]);
		}
		// get commit format
		xioctl(fd, VIDIOC_G_FMT, &v4l2_fmt);
		// check format
		// Print Stream Format
		static char pixelFmtStr[8];
		printf("Stream Format Informations:\n");
		printf("\ttype: %d\n", v4l2_fmt.type);
		printf("\twidth: %d\n", v4l2_fmt.fmt.pix.width);
		printf("\theight: %d\n", v4l2_fmt.fmt.pix.height);
		memset(&pixelFmtStr[0], 0, 8);
		memcpy(&pixelFmtStr[0], &v4l2_fmt.fmt.pix.pixelformat, 4);
		printf("\tpixelformat: %s\n", &pixelFmtStr[0]);
		printf("\tfield: %d\n", v4l2_fmt.fmt.pix.field);
		printf("\tbytesperline: %d\n", v4l2_fmt.fmt.pix.bytesperline);
		printf("\tsizeimage: %d\n", v4l2_fmt.fmt.pix.sizeimage);
		printf("\tcolorspace: %d\n", v4l2_fmt.fmt.pix.colorspace);
		printf("\tpriv: %d\n", v4l2_fmt.fmt.pix.priv);
		printf("\traw_date: %s\n", v4l2_fmt.fmt.raw_data);
		
		// request v4l2 buffers
		/*after calling ioctl(fd, VIDIOC_REQBUFS, &v4l2_reqbuf), the v4l2_reqbuf.count maybe modified*/
		memset(&v4l2_reqbuf, 0, sizeof(v4l2_reqbuf));
		v4l2_reqbuf.count = prefer_buffer_num;
		v4l2_reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		v4l2_reqbuf.memory = V4L2_MEMORY_MMAP;
		xioctl(fd, VIDIOC_REQBUFS, &v4l2_reqbuf);
		fprintf(stderr, "v4l2_reqbuf.count = %d\n", v4l2_reqbuf.count);

		frame_buffers.clear();
		for (int i=0; i<v4l2_reqbuf.count; ++i ) {
			memset(&v4l2_buf, 0, sizeof(v4l2_buf));
			v4l2_buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            v4l2_buf.memory      = V4L2_MEMORY_MMAP;
            v4l2_buf.index       = frame_buffers.size();
			xioctl(fd, VIDIOC_QUERYBUF, &v4l2_buf);
			frame_buffer buffer;
			buffer.size = v4l2_buf.length;
			buffer.ptr = mmap(NULL, v4l2_buf.length, PROT_READ | PROT_WRITE, MAP_SHARED,
                				fd, v4l2_buf.m.offset);
			frame_buffers.push_back(buffer);
			if (MAP_FAILED == frame_buffers[i].ptr) {
            	perror("v4l2 buffer mapping");
                // exit(EXIT_FAILURE);
			}
		}
		for (int i = 0; i < frame_buffers.size(); ++i) {
			memset(&v4l2_buf, 0, sizeof(v4l2_buf));
			v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			v4l2_buf.memory = V4L2_MEMORY_MMAP;
			v4l2_buf.index = i;
			xioctl(fd, VIDIOC_QBUF, &v4l2_buf);
		}
	}
	bool modeSupport(Resolution_mode inRes  , Mode inMode) {
		switch (inRes) {
			case Resolution_mode::HD:
				if ((inMode == Mode::LRD) || (inMode == Mode::LD) || (inMode == Mode::P)) {
					return true;
				}
				break;
			case Resolution_mode::VGA:
 				if ((inMode == Mode::LRD) || (inMode == Mode::P)) {
					return true;
 				}
				break;
		}
		cerr << "mode:" << inMode << " not supported" << endl;
		return false;
	}
	bool SetRegisterFromFile(string file_path) {
		ifstream fin(file_path, ios_base::in);
		if(!fin){
			cerr << "load Register config file: " << file_path << " failed" << endl;
			return false;
		}
		cout << "load Register config file: " << file_path << endl;

		string s;
		bool is_address = true;
		unsigned int address = 0;
		unsigned int val = 0;
		int count = 0;
		while( fin >> s) 
		{    
			if (is_address) {
				address = std::strtoul(s.c_str(), 0, 16);
			}else {
				val = std::strtoul(s.c_str(), 0, 16);
				writeMMIO(fd, address, val);
			}
			is_address = !is_address;
			++count;
		}
		if (count%2 == 1) {
			cout << "register config file: " << file_path << " format in valid." << endl;
		}
		return true;
	}

private:
	int open_camera_st;
private:
	void SetOpenCameraState(int state)
	{
		open_camera_st = state;
	}

public:
	int GetOpenCameraState(void)
	{
		return open_camera_st;
	}
};

#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

typedef void (*pNotifyDataRecFun)(double , BYTE* );

class CSampleGrabberCB : public ISampleGrabberCB
{
public:
	
	long Width;
	long Height;

	HANDLE BufferEvent;
	LONGLONG prev, step;
	DWORD lastTime;
	// Fake out any COM ref counting
	STDMETHODIMP_(ULONG) AddRef() { return 2; }
	STDMETHODIMP_(ULONG) Release() { return 1; }

	using DataRecCallback_t = void(double SampleTime, BYTE* pBuffer);

	function<DataRecCallback_t> m_fun;

	CSampleGrabberCB()
	{
		lastTime = 0;
	}

	void addDataReceivedCallback(function<DataRecCallback_t> fn)
	{
		m_fun = fn;
	}
	// Fake out any COM QI'ing
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
	{
		//CheckPointer(ppv,E_POINTER);

		if (riid == IID_ISampleGrabberCB || riid == IID_IUnknown)
		{
			*ppv = (void*) static_cast<ISampleGrabberCB*> (this);
			return NOERROR;
		}

		return E_NOINTERFACE;
	}

	STDMETHODIMP SampleCB(double SampleTime, IMediaSample* pSample)
	{
		std::cout << "SampleCB" << std::endl;

		return S_OK;
	}

	STDMETHODIMP BufferCB(double SampleTime, BYTE* pBuffer, long BufferSize)
	{

		m_fun(SampleTime, pBuffer);

#if 0
		std::cout << "SampleTime = " << SampleTime*100000 << std::endl;

		std::cout << "BufferCB " << BufferSize << std::endl;

		AM_MEDIA_TYPE mt;
		hr = pSampleGrabber->GetConnectedMediaType(&mt);
		if (FAILED(hr))
		{
			return -1;
		}

		VIDEOINFOHEADER* vih = (VIDEOINFOHEADER*)mt.pbFormat;
		if (!FAILED(hr))
		{
			std::cout << "biHeight=" << vih->bmiHeader.biHeight << std::endl;
			std::cout << "biWidth=" << vih->bmiHeader.biWidth << std::endl;
			std::cout << "dwBitRate=" << vih->dwBitRate << std::endl;
			std::cout << "AvgTimePerFrame=" << vih->AvgTimePerFrame << std::endl;
		}

		std::cout << "cb---" << std::this_thread::get_id() << std::endl;

		pNotifyDataRecFunction((const uint16_t*)pBuffer);

		return S_OK;

#endif

		return S_OK;

	}

};

class falcon_104 : public CameraModule {
private:
	// pthread_t pid;
	uint64_t time_stamp = 0;
	int fd, ret;
	int ir_freq = 1084; //500;
	int ir_duty = 30; // 50;
	float focus;

	Json::Value camera_parameter;
	Json::Value sensor_conf_list;
	const string config_dir = ".\\camera_module\\camera_config\\";
	const string dev_config_path = "/run/media/mmcblk0p1/config_dirty/";

	string config_name;

	// directShow use
	IMediaControl* pMediaControl;
	IMediaEvent* pMediaEvent;
	IGraphBuilder* pGraphBuilder;
	ICaptureGraphBuilder2* pCaptureGraphBuilder2;
	IVideoWindow* pVideoWindow;
	IMoniker* pMonikerVideo;
	IBaseFilter* pVideoCaptureFilter;
	IBaseFilter* pGrabberF;
	ISampleGrabber* pSampleGrabber;

	IBaseFilter* pGrabberStill;
	ISampleGrabber* pSampleGrabberStill;
	IBaseFilter* pFilterNull;

	enum PLAYSTATE { Stopped, Paused, Running, Init };
	PLAYSTATE psCurrent;
	long prefer_frame_w;
	long prefer_frame_h;

	int videoCaptureState;

	bool imuThreadStopFlag;
	thread* pImuThread;

	hid_device* hid_handle;

private:
	CSampleGrabberCB* GrabberCB;

public:
	int camera_config_choose = 0;
	bool is_streaming = false;
	bool ir_enable = false;
	bool hole_filling_enable = true;
public:

	falcon_104(CSampleGrabberCB* sampleGrabberCB, Mode inmode = Mode::LRD) : CameraModule(inmode, CameraModel::PINHOLE) {

		pMediaControl = NULL;
		pMediaEvent = NULL;
		pGraphBuilder = NULL;
		pCaptureGraphBuilder2 = NULL;
		pVideoWindow = NULL;
		pMonikerVideo = NULL;
		pVideoCaptureFilter = NULL;
		pGrabberF = NULL;
		pSampleGrabber = NULL;

		pGrabberStill = NULL;
		pSampleGrabberStill = NULL;
		pFilterNull = NULL;
		psCurrent = Init;

		videoCaptureState = 1;

		if (sampleGrabberCB)
		{
			GrabberCB = sampleGrabberCB;

			//register dataReceived callback
			auto cb = [this](double SampleTime, BYTE* pBuffer) {
				this->WrapNotifyDataRec(SampleTime, pBuffer);
			};

			GrabberCB->addDataReceivedCallback(cb);
		}

		ShowCamInfo();
		// loadCameraParameterFile();
		SetResolutionMode(Resolution_mode::VGA);
		SetSensorConf(0);
		SetMode(Mode::LRD);

		hid_handle = NULL;
		imuThreadStopFlag = false;
		//pthread_create(&imu_thread_id, NULL, &(falcon_104::run_imu_thread), this);
		pImuThread = new thread(&falcon_104::imu_thread, this);
		ExecPython4Self("imu.py off");
	}
	~falcon_104() {
		if (is_streaming) {
			StreamOff();
		}

		if (hid_handle)
		{
			imuThreadStopFlag = true;

			Sleep(1);
		}

		CloseHid();

		if (pImuThread)
		{
			pImuThread->detach();

			pImuThread = NULL;
		}
	}

private:
	
	void imu_thread(void) {
		int res;
		unsigned char buf[64];

		struct hid_device_info* devs, * cur_dev;

		if (hid_init())
		{
			return;
		}

#ifdef DEBUG_HID
		devs = hid_enumerate(0x0, 0x0);
		cur_dev = devs;
		while (cur_dev) {
			printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
			printf("\n");
			printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
			printf("  Product:      %ls\n", cur_dev->product_string);
			printf("  Release:      %hx\n", cur_dev->release_number);
			printf("  Interface:    %d\n", cur_dev->interface_number);
			printf("  Usage (page): 0x%hx (0x%hx)\n", cur_dev->usage, cur_dev->usage_page);
			printf("\n");
			cur_dev = cur_dev->next;
		}
		hid_free_enumeration(devs);

		// Set up the command buffer.
		memset(buf, 0x00, sizeof(buf));
		buf[0] = 0x01;
		buf[1] = 0x81;

#endif
		// Open the device using the VID, PID,
		// and optionally the Serial number.
		////handle = hid_open(0x4d8, 0x3f, L"12345");
		hid_handle = hid_open(0x1d6b, 0x0102, NULL);
		if (!hid_handle) {
			printf("unable to open hid device\n");
			return;
		}
#ifdef DEBUG_HID
		wchar_t wstr[256];
		// Read the Manufacturer String
		wstr[0] = 0x0000;
		res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
		if (res < 0)
			printf("Unable to read manufacturer string\n");
		printf("Manufacturer String: %ls\n", wstr);

		// Read the Product String
		wstr[0] = 0x0000;
		res = hid_get_product_string(handle, wstr, MAX_STR);
		if (res < 0)
			printf("Unable to read product string\n");
		printf("Product String: %ls\n", wstr);

		// Read the Serial Number String
		wstr[0] = 0x0000;
		res = hid_get_serial_number_string(handle, wstr, MAX_STR);
		if (res < 0)
			printf("Unable to read serial number string\n");
		printf("Serial Number String: (%d) %ls", wstr[0], wstr);
		printf("\n");

		// Read Indexed String 1
		wstr[0] = 0x0000;
		res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
		if (res < 0)
			printf("Unable to read indexed string 1\n");
		printf("Indexed String 1: %ls\n", wstr);
#endif
		// Set the hid_read() function to be non-blocking.
		if (hid_set_nonblocking(hid_handle, 0) == 0) {
			printf("setting block mode successed.\n");
		}
		// Read requested state. hid_read() has been set to be
		// non-blocking by the call to hid_set_nonblocking() above.
		// This loop demonstrates the non-blocking nature of hid_read().
		res = 0;
		imu_data_st imu_data;
		while (!imuThreadStopFlag) {

			res = hid_read(hid_handle, buf, sizeof(buf));
			if (res > 0)
			{
				// Print out the returned buffer.
				memcpy(&imu_data.imu_type, buf, 1);
				memcpy(&imu_data.x, buf + 1, 2);
				memcpy(&imu_data.y, buf + 3, 2);
				memcpy(&imu_data.z, buf + 5, 2);
				memcpy(&imu_data.nano_second, buf + 7, 4);
				memcpy(&imu_data.second, buf + 11, 4);
			}
			notify_imu_data_receiver((const imu_data_st*)&imu_data);
		}

		return;

	}

	void CloseHid(void)
	{
		if (hid_handle)
		{
			hid_close(hid_handle);

			/* Free static HIDAPI objects. */
			hid_exit();
		}
	}

public:

	uint64_t GetTimestamp(uint16_t const* Buffer)
	{
		return GetTimestamp_(Buffer);
	}

	int GetSensorConfNum() {
		return sensor_conf_list.size();
	}
	vector<string> GetSensorConfNames() {
		vector<string> output;
		for (int i = 0; i < sensor_conf_list.size(); i++)
			output.push_back(sensor_conf_list[i].asString());
		return output;
	}
	int GetSensorConfIndex() {
		return sensor_conf_index;
	}
	void EnableHoleFill(bool input) {
		if (input) {
			ExecPython4Self("en_bilat.py on");
		}
		else {
			ExecPython4Self("en_bilat.py off");
		}
	}
	void EnableIR(bool input, bool force = false) override {
		if (input) {
			ExecPython4Self("en_ir.py on");
		}
		else {
			ExecPython4Self("en_ir.py off");
		}
	}

	void EnableIMU(bool input, bool force = true) override {
		if (input) {
			ExecPython4Self("imu.py on");
		}
		else {
			ExecPython4Self("imu.py off");
		}
	}

	void SetSensorConf(int index) {
		if (index > sensor_conf_list.size()) {
			printf("%d out of index of sensor config list\n", index);
			return;
		}
		sensor_conf_index = index;
		ExecPython4Self(sensor_conf_list[index].asString());
		notifySettingChanged();
		cout << "set sensor_conf to " << sensor_conf_list[sensor_conf_index].asString() << endl;

	}

	void SetMode(Mode in_mode) override {
		// printf("enter SetMode\n");
		bool restart_stream = is_streaming;
		if (restart_stream)
			StreamOff();
		if (modeSupport(resolution, in_mode)) {
			// printf("before notifySettingChanged mode %d\n", mode);
			mode = in_mode;
			// printf("after notifySettingChanged mode %d\n", mode);
			cout << "set mode to " << mode << endl;
		}
		notifySettingChanged();
		if (restart_stream)
			StreamOn();
	}
	void SetResolutionMode(Resolution_mode input) {
		bool restart_stream = is_streaming;
		if (restart_stream)
			StreamOff();

		// check if mode not supported, change mode to LRD
		if (!modeSupport(input, mode)) {
			mode = Mode::LRD;
		}
		resolution = input;
		cout << "set resolution to " << input << endl;
		notifySettingChanged();
		if (restart_stream)
			StreamOn();
	}

	int GetFrameWidth() override {
		switch (mode)
		{
		case Mode::LRD:
		case Mode::LR:
		case Mode::LD:
		case Mode::L:
		case Mode::R:
		case Mode::D:
			return (int)prefer_frame_w;
			return 0;
		case Mode::P:
			// debug
			// cerr << "frame width: " << (v4l2_fmt.fmt.pix.width/4) << endl;
			return (int)(prefer_frame_w / 4);
			return 0;
		}
		return 0;
	}
	int GetFrameHeight() override {
		return (int)prefer_frame_h;

	}
	void GetFrameSize(int& width, int& height) override {
		width = GetFrameWidth();
		height = GetFrameHeight();
	}
	
	// just for compile
	void ExecPython(const char* filepath)
	{
	
	}

	void ShowCamInfo() {

	}
	float GetCameraParam(string paramName) override {
		string conf_name = sensor_conf_list[sensor_conf_index].asCString();
		string resolution_str;
		switch (resolution) {
		default:
			resolution_str = "vga";
			break;
		case Resolution_mode::VGA:
			resolution_str = "vga";
			break;
		case Resolution_mode::HD:
			resolution_str = "hd";
			break;
		}
		return camera_parameter[conf_name][resolution_str][paramName].asFloat();
	}

	void StreamOn() {

		int ret = ConfigVideoCapture();
		if (ret < 0)
		{
			SetVideoCaptureState(ret);

			return;
		}

		printf("Open camera\n");
		initStream();
		cout << "stream on" << endl;
		is_streaming = true;
		EnableIR(ir_enable);

	} // [StreamOn]

	void StreamOff() {
		RealStop();

	} // [StreamOff]

	void loadCameraParameterFile() {
		
#if 0
		struct stat st = { 0 };
		if (stat(config_dir.c_str(), &st) == -1) {
			mkdir(config_dir.c_str(), 0700);
		}
#endif

		// load camera_parameter
		std::ifstream ifs;
		ReadFile("camera_parameter.json", config_dir);
		ifs.open(config_dir + "/camera_parameter.json");
		cout << "load config : " << config_dir << "/camera_parameter.json" << endl;
		Json::CharReaderBuilder builder;
		builder["collectComments"] = true;
		JSONCPP_STRING errs;

		if (!parseFromStream(builder, ifs, &camera_parameter, &errs)) {
			std::cout << errs << std::endl;
			cout << "can't parse " << config_dir << "/camera_parameter.json" << ", please check file." << endl;
			exit(EXIT_FAILURE);
		}
		ifs.close();

		ReadFileGlobal("/tmp/sensor_conf_list.json", config_dir);
		ifs.open(config_dir + "/sensor_conf_list.json");
		builder["collectComments"] = true;
		if (!parseFromStream(builder, ifs, &sensor_conf_list, &errs)) {
			std::cout << errs << std::endl;
			cout << "can't parse " << config_dir << "/sensor_conf_list.json" << ", please check file." << endl;
			exit(EXIT_FAILURE);
		}
		cout << endl << "===== sensor conf list =====" << endl;
		for (int i = 0; i < sensor_conf_list.size(); i++) {
			cout << i << ". " << sensor_conf_list[i] << endl;
		}
		cout << "============================" << endl << endl;
		// cout << camera_parameter << endl;
		notifySettingChanged();
	}

private:

private:
	uint64_t GetTimestamp_(uint16_t const* Buffer)
	{
		uint64_t timestamp = 0;
		int timestamp_offset = 1;
		uint16_t* ptr = (uint16_t*)Buffer;
		switch (GetMode()) {
		case CameraModule::LD:
		case CameraModule::LRD:

			ptr += timestamp_offset;
			for (int i = 0; i < 64; i++) {
				timestamp += (((uint64_t)*ptr & 0x1) << i);
				ptr++;
			}
			break;
		case CameraModule::P:
			ptr += (timestamp_offset * 7);
			for (int i = 0; i < 64; i++) {
				timestamp += ((*ptr & 0x1) << i);
				ptr += 4;
			}
			break;
		default:
			cout << "TODO: support timestamp " << endl;
		}

		return timestamp;
		// printf("timestamp %lu\n", time_stamp);
	}

	int xioctl(int fd, int request, void* arg)
	{
		return 0;
	}

	void initStream()
	{
		switch (resolution) {
		case Resolution_mode::HD:
			prefer_frame_w = 1280;
			prefer_frame_h = 720;
			break;
		case Resolution_mode::VGA:
			prefer_frame_w = 640;
			prefer_frame_h = 480;
			break;
		default:
			prefer_frame_w = 1280;
			prefer_frame_h = 720;
		}
		switch (mode) {
		case Mode::L:
		case Mode::R:
		case Mode::D:
			break;
		case Mode::LD:
		case Mode::LR:
			prefer_frame_w *= 2;
			break;
		case Mode::LRD:
			prefer_frame_w *= 3;
			break;
		case Mode::P:
			prefer_frame_w *= 4;
			break;
		}

		// set camera param
		IAMStreamConfig* pConfig = NULL;
		HRESULT hr = pCaptureGraphBuilder2->FindInterface(
			&PIN_CATEGORY_CAPTURE,
			&MEDIATYPE_Video,
			pVideoCaptureFilter,
			IID_IAMStreamConfig,
			(void**)&pConfig
		);
#if USE_UVC_CAMERA
		if (!SetCameraParam(prefer_frame_w, prefer_frame_h))
		{
			cout << "set camera para fail!" << endl;

			return;
		}
#endif

		printf("Stream Format Informations:\n");
		printf("\twidth: %d\n", prefer_frame_w);
		printf("\theight: %d\n", prefer_frame_h);

	} // [initStream]

	bool modeSupport(Resolution_mode inRes, Mode inMode) {
		switch (inRes) {
		case Resolution_mode::HD:
			if ((inMode == Mode::LRD) || (inMode == Mode::LD) || (inMode == Mode::P)) {
				return true;
			}
			break;
		case Resolution_mode::VGA:
			if ((inMode == Mode::LRD) || (inMode == Mode::P)) {
				return true;
			}
			break;
		}
		cerr << "mode:" << inMode << " not supported" << endl;
		return false;
	}
	bool SetRegisterFromFile(string file_path) {
		return true;
	} // [SetRegisterFromFile]

private:
	int ConfigVideoCapture(void)
	{
		// init COM
		HRESULT hr = CoInitialize(NULL);

		// create Filter Graph Manager.
		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
			IID_IGraphBuilder, (void**)&pGraphBuilder);
		if (FAILED(hr))
			return -1;
		// create Capture Graph Builder.
		hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC,
			IID_ICaptureGraphBuilder2, (void**)&pCaptureGraphBuilder2);
		if (FAILED(hr))
			return -1;

		pCaptureGraphBuilder2->SetFiltergraph(pGraphBuilder);

		if (!FindCameraDevice())
			return -2;

		hr = pGraphBuilder->AddFilter(pVideoCaptureFilter, L"VideoCaptureFilter");
		if (FAILED(hr))
		{
			cout << "Couldn't add video capture filter to graph!" << endl;
			if(pVideoCaptureFilter)
				pVideoCaptureFilter->Release();

			return -2;
		}

		hr = pGraphBuilder->QueryInterface(IID_IMediaControl, (LPVOID*)&pMediaControl);
		if (FAILED(hr))
			return -3;

		hr = pGraphBuilder->QueryInterface(IID_IVideoWindow, (LPVOID*)&pVideoWindow);
		if (FAILED(hr))
			return -3;

		hr = pGraphBuilder->QueryInterface(IID_IMediaEvent, (LPVOID*)&pMediaEvent);
		if (FAILED(hr))
			return -3;

		hr = CoCreateInstance(CLSID_SampleGrabber, NULL,
			CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pGrabberStill);
		if (FAILED(hr))
			return -3;

		pGraphBuilder->AddFilter(pGrabberStill, L"GrabberStill");

		hr = pGrabberStill->QueryInterface(IID_ISampleGrabber, (void**)&pSampleGrabberStill);
		if (FAILED(hr))
			return -3;

		hr = CoCreateInstance(CLSID_NullRenderer, NULL,
			CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pFilterNull);
		if (FAILED(hr))
			return -3;

		pGraphBuilder->AddFilter(pFilterNull, L"FilterNull");

		return 1;
	} // [ConfigVideoCapture] 

	bool FindCameraDevice(void)
	{
		USES_CONVERSION;
		HRESULT hr;
		ULONG cFetched;

		// scan devices
		ICreateDevEnum* pCreateDevEnum;
		hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
			CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pCreateDevEnum);
		if (FAILED(hr))
		{
			cout << "Failed to enumerate all video and audio capture devices!" << endl;
			return false;
		}

		IEnumMoniker* pEnumMoniker;
		hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
			&pEnumMoniker, 0);
		if (FAILED(hr) || !pEnumMoniker)
		{
			cout << "Failed to create ClassEnumerator!" << endl;
			return false;
		}

		while (hr = pEnumMoniker->Next(1, &pMonikerVideo, &cFetched), hr == S_OK)
		{
			IPropertyBag* pPropBag = NULL;

			HRESULT hr = pMonikerVideo->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
			if (FAILED(hr))
			{
				pMonikerVideo->Release();
				continue;
			}

			VARIANT var;
			VariantInit(&var);

			// Get description or friendly name.
			hr = pPropBag->Read(L"Description", &var, 0);
			if (FAILED(hr))
			{
				hr = pPropBag->Read(L"FriendlyName", &var, 0);
			}

			if (FAILED(hr))
			{
				cout << "Get description or Friendly name fail" << endl;
				return false;
			}

			printf("FriendlyName: %S\n", var.bstrVal);

			std::string devname = std::string(W2A(var.bstrVal));
#if USE_UVC_CAMERA
			if (devname.find("UVC Camera") == -1)
#else
			if (devname.find("Integrated Camera") == -1)
#endif
			{
				VariantClear(&var);
				pMonikerVideo->Release();
				continue;
			}

			VariantClear(&var);

			hr = pMonikerVideo->BindToObject(0, 0, IID_IBaseFilter, (void**)&pVideoCaptureFilter);
			if (FAILED(hr))
			{
				cout << "Couldn't bind moniker to filter object!" << endl;
				return false;
			}

			if (pVideoCaptureFilter == NULL)
			{
				cout << "Error Cannot create video capture filter" << endl;

				return false;
			}

			pPropBag->Release();
			pMonikerVideo->Release();
		} // end while

		pEnumMoniker->Release();
		pCreateDevEnum->Release();
		return true;
	} // [FindCameraDevice] 

public:
	bool RealStart(void)
	{
		HRESULT hr;
#if USE_CAMERA_PREVIEW
		hr = pCaptureGraphBuilder2->RenderStream(
										&PIN_CATEGORY_CAPTURE,
										&MEDIATYPE_Video,
										pVideoCaptureFilter,
										pGrabberStill,
										NULL
										);
#else
		hr = pCaptureGraphBuilder2->RenderStream(
										&PIN_CATEGORY_CAPTURE,
										&MEDIATYPE_Video,
										pVideoCaptureFilter,
										pGrabberStill,
										pFilterNull
										);
#endif
		if (FAILED(hr))
		{
			cout << "[1]Couldn't render video capture stream. The device may already be in use." << endl;

			if(pVideoCaptureFilter)
				pVideoCaptureFilter->Release();
			return false;
		}

		//configure the Sample Grabber so that it buffers samples :
		hr = pSampleGrabberStill->SetOneShot(FALSE);
		hr = pSampleGrabberStill->SetBufferSamples(TRUE);
		
		// get media type
		AM_MEDIA_TYPE mt;
		// hr = pSampleGrabber->GetConnectedMediaType(&mt);
		hr = pSampleGrabberStill->GetConnectedMediaType(&mt);
		if (FAILED(hr))
		{
			cout << "Get connect media type fail!" << endl;
			return false;
		}

		if ((mt.majortype == MEDIATYPE_Video) && (mt.subtype == MEDIASUBTYPE_YUY2))
		{
			VIDEOINFOHEADER* vih = (VIDEOINFOHEADER*)mt.pbFormat;

			GrabberCB->Width = vih->bmiHeader.biWidth;
			GrabberCB->Height = vih->bmiHeader.biHeight;

			cout << vih->dwBitRate << endl;
			cout << vih->AvgTimePerFrame << endl;
		}

		// set sample callback style, 0:sample callback, 1: buffer call back
		hr = pSampleGrabberStill->SetCallback(GrabberCB, 1);
		if (FAILED(hr))
		{
			cout << "set still trigger call back failed" << endl;

			return false;
		}

#if USE_CAMERA_PREVIEW
		pVideoWindow->put_Left(0);
		pVideoWindow->put_Width(CAMERA_PREVIEW_VIDEO_WIDTH);
		pVideoWindow->put_Top(0);
		pVideoWindow->put_Height(CAMERA_PREVIEW_VIDEO_HEIGHT);
		// pVideoWindow->put_Caption(L"Camerea Preview Video Window");
#endif

		hr = pMediaControl->Run();
		if (FAILED(hr))
		{
			cout << "Couldn't run the graph!" << endl;
			return false;
		}
		else
		{
			psCurrent = Running;
		}

		return true;
	} // [RealStart]

private:
	bool RealStop(void)
	{
		if (pMediaControl)
			pMediaControl->Stop();

		if (pVideoWindow)
			pVideoWindow->put_Visible(OAFALSE);

		if(pMediaControl)
			pMediaControl->Release();

		if(pGraphBuilder)
			pGraphBuilder->Release();

		if(pVideoWindow)
			pVideoWindow->Release();

		if(pCaptureGraphBuilder2)
			pCaptureGraphBuilder2->Release();

		CoUninitialize();

		psCurrent = Stopped;

		return true;

	} // [RealStop]

	void WrapNotifyDataRec(double SampleTime, BYTE* pBuffer)
	{
		notifyDataReceiver((const uint16_t*)pBuffer);

		GetTimestamp((uint16_t*)pBuffer);

	} // [WrapNotifyDataRec]

	bool SetCameraParam(long frame_w, long frame_h)
	{
		// set camera param
		IAMStreamConfig* pConfig = NULL;
		HRESULT hr = pCaptureGraphBuilder2->FindInterface(
			&PIN_CATEGORY_CAPTURE,
			&MEDIATYPE_Video,
			pVideoCaptureFilter,
			IID_IAMStreamConfig,
			(void**)&pConfig
		);

		if (FAILED(hr))
		{
			printf("Get IAMStreamConfig interface fail.  hr=0x%x\n", hr);
			if(pVideoCaptureFilter)
				pVideoCaptureFilter->Release();
			return false;
		}

		AM_MEDIA_TYPE* pmt;
		hr = pConfig->GetFormat(&pmt);
		if (FAILED(hr))
		{
			printf("pConfig get format fail.  hr=0x%x\n", hr);
			pVideoCaptureFilter->Release();
			return false;
		}

		if (pmt->formattype == FORMAT_VideoInfo)
		{
			VIDEOINFOHEADER* pvi = (VIDEOINFOHEADER*)pmt->pbFormat;
			pvi->bmiHeader.biWidth = frame_w;
			pvi->bmiHeader.biHeight = frame_h;

			hr = pConfig->SetFormat(pmt);
			if (FAILED(hr))
			{
				printf("pConfig set format fail.  hr=0x%x\n", hr);
				pVideoCaptureFilter->Release();
				return false;
			}
		}

		return true;
	} // [SetCameraParam]

	// for device control

	#define SENSE_DEVICE_CONTROL_BUFFER_SIZE 60

	void ExecPython4Self(string str) {
		string temp = string(dev_config_path + str);
		char data[SENSE_DEVICE_CONTROL_BUFFER_SIZE];

		while (temp.size() > 0)
		{
			memset(data, 0, SENSE_DEVICE_CONTROL_BUFFER_SIZE);

			data[0] = 0;
			data[1] = 0;
			data[2] = 'F';
			data[3] = 'E';

			if (temp.size() >= 56)
			{
				memcpy(&data[4], temp.c_str(), 56);
				temp = temp.substr(56);
			}
			else
			{
				memcpy(&data[4], temp.c_str(), temp.size());
				temp.clear();
			}

			SetExtensionUnit(data);
		} // end while
	}

	void ReadFile(string src_name, string dst_dir)
	{
		string dev_file_path = dev_config_path + "/" + src_name;
		cout << "read file on device : " << dev_file_path << " ->" << dst_dir << endl;
		fileread(dev_file_path, dst_dir);
	}

	void ReadFileGlobal(string src_name, string dst_dir)
	{
		cout << "read file on device : " << src_name << " ->" << dst_dir << endl;
		fileread(src_name, dst_dir);
	}


	void fileread(string file_path, string out_dir)
	{
		string temp_str = string(file_path);
		int n = file_path.rfind('/');
		if (n == string::npos)
			return;

		string file_name = file_path.substr(n + 1);

		char data[60];

		bool isFirst = true;

		int i = 0;

		while (temp_str.size() > 0)
		{
			memset(data, 0, 60);

			data[0] = 0;
			data[1] = 0;
			data[2] = 'F';

			if (isFirst)
			{
				isFirst = false;
				data[3] = 'R';
			}
			else
			{
				data[3] = 'C';
			}

			if (temp_str.size() >= 56)
			{
				memcpy(&data[4], temp_str.c_str(), 56);
				temp_str = temp_str.substr(56);
			}
			else
			{
				memcpy(&data[4], temp_str.c_str(), temp_str.size());
				temp_str.clear();
			}

			SetExtensionUnit(data);

		} // end while

		int len = 128 - file_path.size();
		if (len < 0)
			return;

		while (len > 0)
		{
			memset(data, 0, 60);

			data[0] = 0;
			data[1] = 0;
			data[2] = 'F';
			data[3] = 'C';

			if (len >= 56)
			{
				memcpy(&data[4], temp_str.c_str(), 56);
				len -= 56;
			}
			else
			{
				memcpy(&data[4], temp_str.c_str(), len);
				len = 0;
			}

			SetExtensionUnit(data);
		}

		GetExtensionUnit(data);

		UNNDATA unndata;
		unndata.u32 = 0;

		unndata.u8[3] = data[3];
		unndata.u8[2] = data[2];
		unndata.u8[1] = data[1];
		unndata.u8[0] = data[0];

		uint file_size = unndata.u32;

		if (file_size == 0)
			return;

		string output_file = out_dir + "\\" + file_name;
		FILE* fp = NULL;
		fopen_s(&fp, output_file.c_str(), "wb+");
		if (!fp)
		{
			printf("open %s fail\n", file_name.c_str());
			return;
		}

		while (file_size > 0)
		{
			memset(data, 0, 60);

			data[0] = 0;
			data[1] = 0;
			data[2] = 'F';
			data[3] = 'C';
			SetExtensionUnit(data);

			GetExtensionUnit(data);

			if (file_size >= 60)
			{
				fwrite(data, 1, 60, fp);
				file_size -= 60;
			}
			else
			{
				fwrite(data, 1, file_size, fp);
				file_size -= file_size;
			}
		} // end while

		fclose(fp);
	}

	bool SenseMemW(uint addr, uint value, uint mask = 0xffffffff)
	{
		char data[SENSE_DEVICE_CONTROL_BUFFER_SIZE];

		uint regAdd = 0x574d0000;

		memset(data, 0, SENSE_DEVICE_CONTROL_BUFFER_SIZE);

		memcpy(&data[0], &regAdd, 4);
		memcpy(&data[4], &addr, 4);
		memcpy(&data[8], &mask, 4);
		memcpy(&data[12], &value, 4);

		return SetExtensionUnit(data);
	}

	bool SenseMemR(uint addr, uint* out)
	{
		if (!out)
			return false;

		char data[SENSE_DEVICE_CONTROL_BUFFER_SIZE];
		uint regAdd = 0x524d0000;

		memset(data, 0, SENSE_DEVICE_CONTROL_BUFFER_SIZE);

		memcpy(&data[0], &regAdd, 4);
		memcpy(&data[4], &addr, 4);

		if (!SetExtensionUnit(data))
		{
			return false;
		}

		if (GetExtensionUnit(data))
		{
			UNNDATA unndata;
			unndata.u32 = 0;

			unndata.u8[3] = data[3];
			unndata.u8[2] = data[2];
			unndata.u8[1] = data[1];
			unndata.u8[0] = data[0];

			*out = unndata.u32;

			return true;
		}

		return false;
	}

	bool SetExtensionUnit(char* databuffer)
	{
		ULONG flags, readCount;
		flags = KSPROPERTY_TYPE_SET | KSPROPERTY_TYPE_TOPOLOGY;
		HRESULT hr;

		hr = SetGetExtensionUnit(3, 2, flags, (void*)databuffer, SENSE_DEVICE_CONTROL_BUFFER_SIZE, &readCount);
		if (SUCCEEDED(hr))
			return true;
		else
			return false;
	}

	bool GetExtensionUnit(char* databuffer)
	{
		ULONG flags, readCount;
		flags = KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_TOPOLOGY;
		HRESULT hr;

		hr = SetGetExtensionUnit(3, 2, flags, (void*)databuffer, SENSE_DEVICE_CONTROL_BUFFER_SIZE, &readCount);
		if (SUCCEEDED(hr))
			return true;
		else
			return false;
	}

	#define CHECK_HR_RESULT(hr, msg, ...) if (hr != S_OK) {printf("info: Function: %s, %s failed, Error code: 0x%.2x \n", __FUNCTION__, msg, hr, __VA_ARGS__); goto done; }

	HRESULT SetGetExtensionUnit(DWORD dwExtensionNode, ULONG xuPropertyId, ULONG flags, void* data, int len, ULONG* readCount)
	{
		GUID pNodeType;
		IUnknown* unKnown;
		IKsControl* ks_control = NULL;
		IKsTopologyInfo* pKsTopologyInfo = NULL;
		KSP_NODE kspNode;

		static const GUID xuGuidAN75779 =
		{ 0x9AB33DDC, 0xDE86, 0x43C7, { 0xA1, 0x8B, 0x47, 0x18, 0x98, 0x5B, 0x70, 0x7B } };

		if (!pVideoCaptureFilter)
		{
			return S_FALSE;
		}

		HRESULT hr = pVideoCaptureFilter->QueryInterface(__uuidof(IKsTopologyInfo), (void**)&pKsTopologyInfo);
		CHECK_HR_RESULT(hr, "IMFMediaSource::QueryInterface(IKsTopologyInfo)");

		if (hr != S_OK)
		{
			return hr;
		}

		hr = pKsTopologyInfo->get_NodeType(dwExtensionNode, &pNodeType);
		CHECK_HR_RESULT(hr, "IKsTopologyInfo->get_NodeType(...)");

		if (hr != S_OK)
		{
			return hr;
		}

		hr = pKsTopologyInfo->CreateNodeInstance(dwExtensionNode, IID_IUnknown, (LPVOID*)&unKnown);
		CHECK_HR_RESULT(hr, "ks_topology_info->CreateNodeInstance(...)");

		if (hr != S_OK)
		{
			return hr;
		}

		hr = unKnown->QueryInterface(__uuidof(IKsControl), (void**)&ks_control);
		CHECK_HR_RESULT(hr, "ks_topology_info->QueryInterface(...)");

		if (hr != S_OK)
		{
			return hr;
		}

		kspNode.Property.Set = xuGuidAN75779;              // XU GUID
		kspNode.NodeId = (ULONG)dwExtensionNode;   // XU Node ID
		kspNode.Property.Id = xuPropertyId;         // XU control ID
		kspNode.Property.Flags = flags;             // Set/Get request

		hr = ks_control->KsProperty((PKSPROPERTY)&kspNode, sizeof(kspNode), (PVOID)data, len, readCount);
		CHECK_HR_RESULT(hr, "ks_control->KsProperty(...)");

	done:
		if (ks_control)
			ks_control->Release();
		return hr;
	}

private:
	void SetVideoCaptureState(int state)
	{
		videoCaptureState = state;
	}

public:
	int GetVideoCaptureState(void)
	{
		return videoCaptureState;
	}
	
};

#endif

#endif
