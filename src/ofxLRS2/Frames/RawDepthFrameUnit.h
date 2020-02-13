#pragma once
#include "BaseFrameUnit.h"
template <typename T = unsigned char>
class RawDepthFrameUnit : public BaseFrameUnit<T>
{
public:
	RawDepthFrameUnit();
	~RawDepthFrameUnit();
};

