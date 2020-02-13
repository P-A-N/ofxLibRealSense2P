#pragma once
#include "BaseFrameUnit.h"
template <typename T = unsigned char>
class IrFrameUnit : public BaseFrameUnit<T>
{
public:
	IrFrameUnit();
	~IrFrameUnit();
};

