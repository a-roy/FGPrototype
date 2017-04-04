#include "Input.h"

Input::Input(std::vector<FrameInput> val, InputOptions o, int charge)
{
	value = val;
	options = o;
	Charge = charge;
}

const std::vector<FrameInput>& Input::GetValue() const
{
	return value;
}

bool Input::IsRequired() const
{
	return (options & Required) == Required;
}

bool Input::IsPressOK() const
{
	return (options & PressOK) == PressOK;
}

bool Input::IsReleaseOK() const
{
	return (options & ReleaseOK) == ReleaseOK;
}

bool Input::IsHoldOK() const
{
	return (options & HoldOK) == HoldOK;
}

bool Input::IsIncreaseBuffer() const
{
	return (options & IncreaseBuffer) == IncreaseBuffer;
}
