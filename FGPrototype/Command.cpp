#include "Command.h"
#include <sstream>

Command::Command(std::vector<Input> inputs)
{
	data = inputs;
}

const std::vector<Input>& Command::GetData() const
{
	return data;
}

const std::string Command::ToString() const
{
	std::ostringstream s;
	for (unsigned int i = 0; i < data.size(); i++)
	{
		const std::vector<FrameInput>& a = data[i].GetValue();
		if (a.size() > 1)
		{
			s << '<';
		}
		for (std::vector<FrameInput>::const_iterator b = a.begin(); b != a.end(); ++b)
		{
			if ((*b).Dir.Numpad != 0 && (*b).Pressed != None)
			{
				s << '(';
			}
			if ((*b).Dir.Numpad != 0)
			{
				s << (*b).Dir.Numpad;
			}
			if ((*b).Pressed != None)
			{
				s << ButtonName((*b).Pressed);
			}
			if ((*b).Dir.Numpad != 0 && (*b).Pressed != None)
			{
				s << ')';
			}
		}
		if (a.size() > 1)
		{
			s << '>';
		}
		if (!data[i].IsRequired())
		{
			s << '?';
		}
		if (!data[i].IsHoldOK())
		{
			s << '*';
		}
		if (data[i].IsReleaseOK())
		{
			s << '!';
		}
		if (!data[i].IsPressOK())
		{
			s << '@';
		}
		if (data[i].IsIncreaseBuffer())
		{
			s << '_';
		}
	}
	return s.str();
}

std::string Command::ButtonName(Buttons b)
{
	if (b == None)
	{
		return "None";
	}
	std::ostringstream s;
	Buttons recorded = None;
	for (Buttons x = B1; x <= B6; x = (Buttons)((int)x * 2))
	{
		if ((b & x) == x)
		{
			switch (x)
			{
			case B1:
				s << "LP";
				break;
			case B2:
				s << "MP";
				break;
			case B3:
				s << "HP";
				break;
			case B4:
				s << "LK";
				break;
			case B5:
				s << "MK";
				break;
			case B6:
				s << "HK";
				break;
			}
			recorded = (Buttons)(recorded | x);
			if (recorded < b)
			{
				s << "+";
			}
		}
	}
	return s.str();
}

std::ostream& operator <<(std::ostream& out, const Command& c)
{
	out << c.ToString();
	return out;
}