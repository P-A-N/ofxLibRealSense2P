#pragma once
#include "ofMain.h"
#include <librealsense2/rs.hpp> 

template <typename T = unsigned char>
class BaseFrameUnit
{
public:
	BaseFrameUnit(int width, int height, float fps,
		int numChannel, int glInternalFormat, bool bUseArbTex);
	~BaseFrameUnit();
	
	//---- buffer / texture ----//
	void resetTextureIfPossible();
	void resetBufferIfPossible(int width, int height);
	void updateTexture(bool bswizzle = false);
	void copyPixels(rs2::video_frame& frame);

	//---- drawer ----//
	void drawTexture(int x, int y);


	//----getter / setter ----///
	shared_ptr<ofTexture> getTexture() { return _texture; }
	float getWidth() { return _height; }
	float getHeight() { return _width; }
	void setWidth(float width) { _width = width; }
	void setHeight(float height) { _height = height; }
	float getFps() { return _fps; }
	void setFps(float fps) { _fps = fps; }


private:
	shared_ptr<ofPixels_<T>> _buff;
	shared_ptr<ofTexture> _texture;
	float _width, _height;
	float _fps;
	bool _useArbTex;
	int _glInternalFormat;
	int _numChannel;
};

