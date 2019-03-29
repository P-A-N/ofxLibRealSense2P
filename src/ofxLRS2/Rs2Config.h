#pragma once
#include "ofMain.h"
#include <librealsense2/rs.hpp> 
#include <librealsense2/rsutil.h>
class StreamSetting
{
public:
	rs2_stream stream_type;
	int stream_index;
	int width;
	int height;
	rs2_format format;
	int framerate = 0;
};
class Rs2Config
{
public:
	Rs2Config();
	~Rs2Config();
	void enable_device(string device_serial)
	{
		_device_serial = device_serial;
	}

	void enable_stream(rs2_stream stream_type, int stream_index, int width, int height, rs2_format format = RS2_FORMAT_ANY, int framerate = 0)
	{
		StreamSetting setting;
		setting.stream_type = stream_type;
		setting.stream_index = stream_index;
		setting.width = width;
		setting.height = height;
		setting.format = format;
		setting.framerate = framerate;
		streamsettings.push_back(setting);
	}

	rs2::config getConfig()
	{
		rs2::config config;
		config.enable_device(_device_serial);
		for (auto s : streamsettings)
		{
			config.enable_stream(s.stream_type, s.stream_index, s.width, s.height, s.format, s.framerate);
		}
		return config;
	}

	string getDeviceSerial()
	{
		return _device_serial;
	}
private:
	string _device_serial;
	string _file_path;
	vector< StreamSetting > streamsettings;
};

