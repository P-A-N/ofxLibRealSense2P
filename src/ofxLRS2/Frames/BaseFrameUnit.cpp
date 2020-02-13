#include "BaseFrameUnit.h"


template <typename T>
BaseFrameUnit<T>::BaseFrameUnit()
{
}


template <typename T>
BaseFrameUnit<T>::~BaseFrameUnit()
{
}

template<typename T>
void BaseFrameUnit<T>::resetTexture(int glInternalFormat)
{
	if (!_texture->isAllocated() || _texture->getWidth() != _buff->getWidth() ||
				_texture->getHeight() != _buff.getHeight())
	{
		_texture->clear();
		_texture->allocate(_buff.getWidth(), _buff.getHeight(), glInternalFormat, bUseArbTexDepth);
	}
}

template <typename T>
T* BaseFrameUnit<T>::getData()
{
	return _buff.getData();
}
