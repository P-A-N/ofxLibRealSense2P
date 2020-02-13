#pragma once
#include "BaseFrameUnit.h"
template <typename T = unsigned char>
class ColorFrameUnit : public BaseFrameUnit<T>
{
public:
	ColorFrameUnit();
	~ColorFrameUnit();
};

