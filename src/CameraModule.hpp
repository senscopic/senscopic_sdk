#ifndef CAMERAMODULE_H
#define CAMERAMODULE_H

#include <sstream>
#include <iostream>
#include <stdio.h>
#include <cstdint>
#include <map>
#include <functional>
#include <stdexcept>
#include <mutex>
#include <stdlib.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    #define SENSE_SPRINTF(x, ...) sprintf_s(x, sizeof(x), __VA_ARGS__)
    #define SENSE_STRCPY(x, ...) strcpy_s(x, sizeof(x), __VA_ARGS__)
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #define SENSE_SPRINTF   sprintf
    #define SENSE_STRCPY    strcpy
#endif

#if defined(linux) || defined(__linux) || defined(__linux__) 
	#include <sys/ioctl.h>
	#include <sys/types.h>
	#include <sys/time.h>
	#include <sys/mman.h>
	#include <unistd.h>
	#include <linux/videodev2.h>
	#include <uvc_interface.h>
#endif

#include <string.h>
#include <time.h>

#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <climits>

#include <queue>
#include <json/json.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    #include "demokit_data_type.h"
#endif

using namespace std;

typedef struct{
    uint8_t imu_type;
    short x;
    short y;
    short z;
    int nano_second;
    int second;
}imu_data_st;

class CameraModule{
public:
    
    //data stream simulation mode enum{{{
	enum Resolution_mode{
		HD = 0, //default
		VGA,
		RESOLUTION_MODE_NUM,
	};
    enum Mode{
        LRD = 0, //default
        LR,
        LD,
        D,
        L,
        R,
        P,
        CAMERA_MODE_NUM
    };
    const string ModeText[Mode::CAMERA_MODE_NUM] = {
        "LRD",
        "LR",
        "LD",
        "D",
        "L",
        "R",
        "P"
    };

    enum CameraModel{
        PINHOLE = 0,
        CAMERA_MODEL_NUM
    };

    const string CameraModelText[CameraModel::CAMERA_MODEL_NUM] = {
        "Pinhole"
    };
    //}}}

    CameraModule(Mode inmode, CameraModel inCameraModel) : mode(inmode), cameraModel(inCameraModel){}
    virtual ~CameraModule(){}

    using imu_data_callback_t = void (imu_data_st const *);
    using DataReceivedCallback_t = void (int width, int height, uint16_t const *);
    using SettingChangedCallback_t = void ();

    //{{{int addDataReceivedCallback(Mode mode_id, string listenerName, function<DataReceiveCallback_t>)
    //return position index
    void addDataReceivedCallback(Mode mode_id, string listenerName, function<DataReceivedCallback_t> fn){
        //TODO check id is in ModeText
        dataCallbackMap[mode_id][listenerName] = fn;
    }
    //}}}

    //{{{void removeDataReceivedCallback(Mode mode_id, string listenerName)
    void removeDataReceivedCallback(Mode mode_id, string listenerName){
        auto it = dataCallbackMap.find(mode_id);
        if(it == dataCallbackMap.end()){
            cerr << "CameraModule : remove from callback[" << mode_id << "] but that item is not exist!" << endl;
            return;
        }

        auto it2 = it->second.find(listenerName);
        if(it2 == it->second.end()){
            cerr << "CameraModule : remove from callback[" << mode_id << "][" << listenerName << "] but that item is not exist!" << endl;
            return;
        }

        deleteDataListenerMap[mode_id] = listenerName;
    }
    //}}}

    //{{{void addSettingChangedCallback(string listenerName, function<SettingChangedCallback>)
    void addSettingChangedCallback(string listenerName, function<SettingChangedCallback_t> fn){
        settingCallbackMap[listenerName] = fn;
    }
    //}}}

    void removeSettingChangedCallback(string listenerName){
        auto it = settingCallbackMap.find(listenerName);
        if(it == settingCallbackMap.end()){
            stringstream ss;
            ss << "listenerName = " << listenerName << " not exist!";
            throw invalid_argument(ss.str());
        }

        deleteSettingListenerVec.push_back(listenerName);

    }

     void add_imu_data_callback(string listenerName, function<imu_data_callback_t> fn){
        imu_data_callback_map[listenerName] = fn;
    }
    //}}}

    void remove_imu_data_callback(string listenerName){
        auto it = imu_data_callback_map.find(listenerName);
        if(it == imu_data_callback_map.end()){
            stringstream ss;
            ss << "listenerName = " << listenerName << " not exist!";
            throw invalid_argument(ss.str());
        }

        deleteImuDataListenerVec.push_back(listenerName);
    }
    //should use param class to handle this
    virtual int GetSensorConfNum()= 0;
    virtual int GetSensorConfIndex() = 0;
    virtual vector<string> GetSensorConfNames() = 0;
	virtual void EnableIR(bool input, bool force=false) = 0;
    virtual void EnableIMU(bool input, bool force=true) = 0;
	virtual void StreamOn() = 0;
	virtual void StreamOff() = 0;
	virtual uint64_t GetTimestamp(uint16_t const* Buffer) = 0;
    virtual void EnableHoleFill(bool input) = 0;
    virtual void SetMode(Mode inMode) = 0;
    virtual float GetCameraParam(string paramName) = 0;
	virtual void SetResolutionMode(Resolution_mode input) = 0;
	virtual void SetSensorConf(int index) = 0;    // must call after streamon
    virtual int GetFrameWidth() = 0;
    virtual int GetFrameHeight() = 0;
    virtual void GetFrameSize(int &width, int &height) = 0;
    virtual void ExecPython(const char* filepath) = 0;
    //============================================

    Mode GetMode(){
        return mode;
    }
    CameraModel GetCameraModel(){
        return cameraModel;
    }
	Resolution_mode GetResolutionMode() {
		return resolution;
	}

protected:
    Mode mode;
    CameraModel cameraModel;
    std::mutex mode_mutex;
	Resolution_mode resolution;
    int sensor_conf_index = 0;
	void notifyDataReceiver(const uint16_t * buffer) {
		for (const auto &cbs : dataCallbackMap[mode]){
            // cout << "call callback " << cbs.first << endl; 
			cbs.second(GetFrameWidth(), GetFrameHeight(), buffer);
		}

        for (auto p : deleteDataListenerMap)
        {
            dataCallbackMap[p.first].erase(p.second);
        }

        deleteDataListenerMap.clear();
	}
	void notifySettingChanged() {
		for (const auto &cbs : settingCallbackMap){
			cout << "call callback " << cbs.first << endl; 
			cbs.second();
		}

        for (auto x : deleteSettingListenerVec)
        {
            settingCallbackMap.erase(x);
        }

        deleteSettingListenerVec.clear();
	}
    void notify_imu_data_receiver(const imu_data_st* data) {
		for (const auto &cbs : imu_data_callback_map){
            // cout << "call callback " << cbs.first << endl; 
			cbs.second(data);
		}

        for (auto x : deleteImuDataListenerVec)
        {
            imu_data_callback_map.erase(x);
        }

        deleteImuDataListenerVec.clear();
	}
    //DataStream Name => listener name => callback
    map<Mode, map<string, function<DataReceivedCallback_t> > > dataCallbackMap;
    map<Mode, string> deleteDataListenerMap;
    //listener name => callback
    map<string, function<SettingChangedCallback_t> > settingCallbackMap;
    vector<string> deleteSettingListenerVec;

    map<string, function<imu_data_callback_t>> imu_data_callback_map;
    vector<string> deleteImuDataListenerVec;

};


#endif
