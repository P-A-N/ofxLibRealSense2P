#pragma once
#include "ofMain.h"
#include <librealsense2/rs.hpp> 

template <typename T = unsigned char>
class BaseFrameUnit
{
public:
	BaseFrameUnit();
	~BaseFrameUnit();
	float getWidth() { return _height; }
	float getHeight() { return _width; }
	void setWidth(float width) { _width = width; }
	void setHeight(float height) { _height = height; }
	void resetTextureIfPossible(int glInternalFormat, bool useArbTex);
	void resetBufferIfPossible(int width, int height, size_t numChannel);
	void updateTexture(bool bswizzle = false);
	void copyPixels(rs2::video_frame& frame, size_t numChannel);
	float getFps() { return _fps; }
	void setFps(float fps) { _fps = fps; }
	//Ç¢ÇÁÇ»Ç¢ÇÒÇ∂Ç·Ç»Ç¢Ç©ê‡
	ofPixels_<T>& getData();
private:
	ofPixels_<T> _buff;
	ofPtr<ofTexture> _texture;
	float _width, _height;
	float _fps;
	bool useArbTex;
};

