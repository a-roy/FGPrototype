#include "CmdTranslator.h"
#include <cmath>

CmdTranslator::CmdTranslator()
{
	buttons[0] = "LP";
	buttons[1] = "MP";
	buttons[2] = "HP";
	buttons[3] = "LK";
	buttons[4] = "MK";
	buttons[5] = "HK";
}

CmdTranslator::CmdTranslator(std::array<std::string, 6> but)
{
	buttons = but;
}

bool CmdTranslator::CheckBrackets(std::string s)
{
	std::stack<char> brackets;
	for (unsigned int a = 0; a < s.length(); a++)
	{
		if (s[a] == inter1)
		{
			if (brackets.empty())
			{
				brackets.push(inter1);
			}
			else
			{
				return false;
			}
		}
		else if (s[a] == simul1)
		{
			if (brackets.empty() || brackets.top() == inter1)
			{
				brackets.push(simul1);
			}
			else
			{
				return false;
			}
		}
		else if (s[a] == inter2)
		{
			if (!brackets.empty() && brackets.top() == inter1)
			{
				brackets.pop();
			}
			else
			{
				return false;
			}
		}
		else if (s[a] == simul2)
		{
			if (!brackets.empty() && brackets.top() == simul1)
			{
				brackets.pop();
			}
			else
			{
				return false;
			}
		}
		else if (s[a] == charge1)
		{
			if (brackets.empty())
			{
				brackets.push(charge1);
			}
			else
			{
				return false;
			}
		}
		else if (s[a] == charge2)
		{
			if (!brackets.empty() && brackets.top() == charge1)
			{
				brackets.pop();
			}
			else
			{
				return false;
			}
		}
	}
	return brackets.empty();
}

Command CmdTranslator::Translate(std::string s)
{
	// check if brackets match up before doing anything else to make this easier
	if (!CheckBrackets(s))
	{
		//! \TODO raise error
		return Command(std::vector<Input>());
	}
	std::vector<Input> inputs;
	std::stack<char> brackets;
	std::vector<FrameInput> interchangeable;
	Direction dir;
	Buttons but = None;
	for (unsigned int a = 0; a < s.length(); a++)
	{
		if (s[a] == inter1 || s[a] == simul1)
		{
			brackets.push(s[a]);
			continue;
		}
		else if (s[a] == inter2)
		{
			brackets.pop();
		}
		else
		{
			if (s[a] == simul2)
			{
				brackets.pop();
			}
			else
			{
				bool valid = false;
				if (s[a] >= '1' && s[a] <= '9')
				{
					dir.Numpad = (int)(s[a] - '0');
					valid = true;
				}
				else
				{
					for (unsigned int b = 0; b < buttons.size(); b++)
					{
						if (a + buttons[b].size() <= s.length() && s.find(buttons[b], a) == a)
						{
							but = (Buttons)(but | (1U << b));
							valid = true;
							a += buttons[b].length() - 1;
						}
					}
					if (!valid)
					{
						continue;
					}
				}
			}
			if ((brackets.empty() || brackets.top() != simul1))
			{
				interchangeable.push_back(FrameInput(dir, but));
				dir.Numpad = 0;
				but = None;
			}
		}
		if (brackets.empty())
		{
			int charge = 0;
			Input::InputOptions options = (Input::InputOptions)(Input::Required | Input::PressOK | Input::HoldOK); // 0x1A
			for (a++; a < s.length(); a++)
			{
				if (s[a] == charge1)
				{
					for (; a < s.length(); a++)
					{
						if (s[a] >= '0' && s[a] <= '9')
						{
							charge *= 10;
							charge += (int)(s[a] - '0');
						}
						else if (s[a] == charge2)
						{
							break;
						}
					}
				}
				else if (s[a] == optional)
				{
					options = (Input::InputOptions)(options & ~Input::Required);
				}
				else if (s[a] == no_press)
				{
					options = (Input::InputOptions)(options & ~Input::PressOK);
				}
				else if (s[a] == release_ok)
				{
					options = (Input::InputOptions)(options | Input::ReleaseOK);
				}
				else if (s[a] == no_hold)
				{
					// taking into account that HoldOK includes the PressOK flag
					options = (Input::InputOptions)(options & (~Input::HoldOK | Input::PressOK));
				}
				else if (s[a] == increase_buffer)
				{
					options = (Input::InputOptions)(options | Input::IncreaseBuffer);
				}
				else
				{
					break;
				}
			}
			a--;

			inputs.push_back(Input(interchangeable, options, charge));
			interchangeable.clear();
		}
	}
	return Command(inputs);
}