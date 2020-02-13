#include "BaseFrameUnit.h"

template <typename T>
BaseFrameUnit<T>::BaseFrameUnit(int width, int height, float fps, 
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


template <typename T>
BaseFrameUnit<T>::~BaseFrameUnit()
{
}

template <typename T>
void BaseFrameUnit<T>::copyPixels(rs2::video_frame& frame)
{
	resetBufferIfPossible(frame.get_width(), frame.get_height(), _numChannel);
	memcpy(_buff->getData(), (T*)colFrame.get_data(), frame.get_width() * frame.get_height() * sizeof(T) * numChannel);
}

template <typename T>
void BaseFrameUnit<T>::resetBufferIfPossible(int width, int height)
{
	if (!_buff->isAllocated() || _buff->getWidth() != width || _buff->getHeight() != height)
	{
		_buff->clear();
		_buff->allocate(width, height, _numChannel);
		_width = width;
		_height = height;
	}
}

template<typename T>
void BaseFrameUnit<T>::resetTextureIfPossible()
{
	if (!_texture->isAllocated() || _texture->getWidth() != _buff->getWidth() ||
				_texture->getHeight() != _buff->getHeight())
	{
		_texture->clear();
		_texture->allocate(_buff->getWidth(), _buff->getHeight(), _glInternalFormat, _useArbTex);
	}
}

template<typename T>
void BaseFrameUnit<T>::updateTexture(bool bSwizzle)
{
	if(bSwizzle) _texture->setRGToRGBASwizzles(true);
	_texture->loadData(*_buff);
}

template<typename T>
void BaseFrameUnit<T>::drawTexture(int x, int y)
{
	_texture->draw(x, y);
}

