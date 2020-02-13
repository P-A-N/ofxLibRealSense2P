#include "BaseFrameUnit.h"


template <typename T>
BaseFrameUnit<T>::BaseFrameUnit()
{
}


template <typename T>
BaseFrameUnit<T>::~BaseFrameUnit()
{
}

template <typename T>
void BaseFrameUnit<T>::copyPixels(rs2::video_frame& frame, size_t numChannel)
{
	resetBufferIfPossible(frame.get_width(), frame.get_height(), numChannel);
	memcpy(_buff.getData(), (T*)colFrame.get_data(), frame.get_width() * frame.get_height() * sizeof(T) * numChannel);
}

template <typename T>
void BaseFrameUnit<T>::resetBufferIfPossible(int width, int height, size_t numChannel)
{
	if (!_buff.isAllocated() || _buff.getWidth() != width || _buff.getHeight() != height)
	{
		_buff.clear();
		_buff.allocate(width, height, numChannel);
	}
}

template<typename T>
void BaseFrameUnit<T>::resetTextureIfPossible(int glInternalFormat, bool useArbTex)
{
	if (!_texture->isAllocated() || _texture->getWidth() != _buff->getWidth() ||
				_texture->getHeight() != _buff.getHeight())
	{
		_texture->clear();
		_texture->allocate(_buff.getWidth(), _buff.getHeight(), glInternalFormat, useArbTex);
		_width = _buff.getWidth();
		_height = _buff.getHeight();
	}
}

template<typename T>
void BaseFrameUnit<T>::updateTexture(bool bSwizzle)
{
	if(bSwizzle) _texture->setRGToRGBASwizzles(true);
	_texture->loadData(_buff);
}

template <typename T>
ofPixels_<T>& BaseFrameUnit<T>::getData()
{
	return _buff;
}

