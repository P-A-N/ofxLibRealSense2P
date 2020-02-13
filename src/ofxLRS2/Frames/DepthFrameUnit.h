#pragma once
#include "BaseFrameUnit.h"
template <typename T = unsigned char>
class DepthFrameUnit : public BaseFrameUnit<T>
{
public:
	DepthFrameUnit();
	~DepthFrameUnit();
};

