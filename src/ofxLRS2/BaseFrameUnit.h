#pragma once
#include "ofMain.h"
#include <librealsense2/rs.hpp> 

template <typename T = unsigned char>
class BaseFrameUnit
{
public:
	BaseFrameUnit(int width, int height, float fps,
		int numChannel, int glInternalFormat, bool bUseArbTex)
	{
		_buff = make_shared<ofPixels_<T>>();
		_texture = make_shared<ofTexture>();
		this->_fps = fps;
		this->_useArbTex = bUseArbTex;
		this->_glInternalFormat = glInternalFormat;
		this->_numChannel = numChannel;
		resetBufferIfPossible(width, height);
		resetTextureIfPossible();
	}
	~BaseFrameUnit() {}
	
	//---- buffer / texture ----//
	void resetTextureIfPossible()
	{
		if (!_texture->isAllocated() || _texture->getWidth() != _buff->getWidth() ||
			_texture->getHeight() != _buff->getHeight())
		{
			_texture->clear();
			_texture->allocate(_buff->getWidth(), _buff->getHeight(), _glInternalFormat, _useArbTex);
		}
	}

	void resetBufferIfPossible(int width, int height)
	{
		if (!_buff->isAllocated() || _buff->getWidth() != width || _buff->getHeight() != height)
		{
			_buff->clear();
			_buff->allocate(width, height, _numChannel);
			_width = width;
			_height = height;
		}
	}
	
	void updateTexture(bool bswizzle = false)
	{
		if (bswizzle) _texture->setRGToRGBASwizzles(true);
		_texture->loadData(*_buff);
	}

	void copyPixels(rs2::video_frame& frame)
	{
		resetBufferIfPossible(frame.get_width(), frame.get_height());
		memcpy(_buff->getData(), (T*)frame.get_data(), frame.get_width() * frame.get_height() * sizeof(T) * _numChannel);
	}

	//---- drawer ----//
	void drawTexture(int x, int y)
	{
		_texture->draw(x, y);
	}


	//----getter / setter ----///
	shared_ptr<ofTexture> getTexture() { return _texture; }
	float getWidth() { return _width; }
	float getHeight() { return _height; }
	//void setWidth(float width) { _width = width; }
	//void setHeight(float height) { _height = height; }
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

