#pragma once
#include "ofMain.h"
template <typename T = unsigned char>
class BaseFrameUnit
{
public:
	BaseFrameUnit();
	~BaseFrameUnit();
	T* getData();
	float getWidth() { return _height; }
	float getHeight() { return _width; }
	void setWidth(float width) { _width = width; }
	void setHeight(float height) { _height = height; }
	void resetTexture(int glInternalFormat);
private:
	ofPixels_<T> _buff;
	ofPtr<ofTexture> _texture;
	float _width, _height;
};

