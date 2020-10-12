#include <dumpTool.h>
#include <vector>
#include <CameraModule.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>


using namespace std;
using namespace cv;


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    static char dist_dir[64] = ".\\snapshot\\";
#elif defined(linux) || defined(__linux) || defined(__linux__)
    static char dist_dir[64] = "./snapshot/";
#endif

void setOutputDirectory(char* indir) {
    // sprintf(dist_dir, "%s", indir);
    SENSE_SPRINTF(dist_dir, "%s", indir);
}

#define CLEAR(x) memset(x, 0, sizeof(x));
void dumpBinary(CameraModule* camera, int width, int height, uint16_t const *buffer, bool keep) {
    static char file_name[64];
    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(0);

    // camera->BlockCamera();

    // uint16_t* buffer = new uint16_t[width * height];
    // std::memcpy(buffer, inut_buffer, sizeof(uint16_t) * width * height);

    // camera->ReStartCamera();

    if (camera->GetMode() == CameraModule::LRD)
    {
        Mat color = Mat(Size(width, height), CV_8UC2, (void*)buffer);
        Mat left = color(Rect(0,0,width/3,height));
        Mat right = color(Rect(width/3,0,width/3,height));

        Mat raw_data = Mat(Size(width, height), CV_16UC1, (void*)buffer);
        Mat depth = raw_data(Rect(2*width/3,0,width/3,height));
        CLEAR(file_name);
        SENSE_SPRINTF(file_name, "%s/%021lu_d.png", dist_dir, camera->GetTimestamp(buffer));
        imwrite(file_name, depth, compression_params);

        Mat l_bgr_frame;
        Mat r_bgr_frame;
        cvtColor(left, l_bgr_frame, CV_YUV2BGR_YUY2);
        cvtColor(right, r_bgr_frame, CV_YUV2BGR_YUY2);

        CLEAR(file_name);
        SENSE_SPRINTF(file_name, "%s/%021lu_l.png", dist_dir, camera->GetTimestamp(buffer));
        imwrite(file_name, l_bgr_frame, compression_params);

        CLEAR(file_name);
        SENSE_SPRINTF(file_name, "%s/%021lu_r.png", dist_dir, camera->GetTimestamp(buffer));
        imwrite(file_name, r_bgr_frame, compression_params);
    }
    else if(camera->GetMode() == CameraModule::P)
    {
        Mat raw_data = Mat(Size(width*4, height), CV_16UC1, (void*)buffer);
        CLEAR(file_name);
        SENSE_SPRINTF(file_name, "%s/%021lu_p.png", dist_dir, camera->GetTimestamp(buffer));
        imwrite(file_name, raw_data, compression_params);
    }else if (camera->GetMode() == CameraModule::LD) {
        Mat raw_data = Mat(Size(width, height), CV_16UC1, (void*)buffer);
        Mat depth = raw_data(Rect(width/2,0,width/2,height));
        CLEAR(file_name);
        SENSE_SPRINTF(file_name, "%s/%021lu_d.png", dist_dir, camera->GetTimestamp(buffer));
        imwrite(file_name, depth, compression_params);

        Mat color = Mat(Size(width, height), CV_8UC2, (void*)buffer);
        Mat left = color(Rect(0,0,width/2,height));
        Mat l_bgr_frame;
        cvtColor(left, l_bgr_frame, CV_YUV2BGR_YUY2);
        CLEAR(file_name);
        SENSE_SPRINTF(file_name, "%s/%021lu_l.png", dist_dir, camera->GetTimestamp(buffer));
        imwrite(file_name, l_bgr_frame, compression_params);

    }else {
        Mat raw_data = Mat(Size(width, height), CV_16UC1, (void*)buffer);
        CLEAR(file_name);
        SENSE_SPRINTF(file_name, "%s/%021lu_raw.png", dist_dir, camera->GetTimestamp(buffer));
        imwrite(file_name, raw_data, compression_params);
    }

    if (!keep)
        camera->removeDataReceivedCallback(camera->GetMode(), "dumpBinary");


}

void getDurationTime(CameraModule* camera, int width, int height, uint16_t const *buffer, bool keep) {
    static int count = 0;
    static double duration_sum = 0;
	using namespace std::chrono;
	static steady_clock::time_point last_time;
	auto now = steady_clock::now();
	double duration = (double)duration_cast<microseconds>(now - last_time).count();
	last_time = now;
    count++;
    duration_sum += duration;
    printf("duration :%0.3f\n", (float)(duration_sum/(count*1000000)));
    if (count > 60) {
        count = 0;
        duration_sum = 0;
    }
    if (!keep)
        camera->removeDataReceivedCallback(camera->GetMode(), "getDurationTime");
}

void printTimestamp(CameraModule *camera, int width, int height, uint16_t const *buffer, bool keep) {
    printf("timestamp : %021lu\n", camera->GetTimestamp(buffer));
    if (!keep)
        camera->removeDataReceivedCallback(camera->GetMode(), "printTimestamp");
}
void printCameraInfo(CameraModule *camera, int width, int height, uint16_t const *buffer, bool keep) {
	printf("baseline %f\n", camera->GetCameraParam("baseline"));
	printf("fx %f\n", camera->GetCameraParam("fx"));
	printf("fy %f\n", camera->GetCameraParam("fy"));
	printf("cx %f\n", camera->GetCameraParam("cx"));
	printf("cy %f\n", camera->GetCameraParam("cy"));
    if (!keep)
        camera->removeDataReceivedCallback(camera->GetMode(), "printCameraInfo");
}

