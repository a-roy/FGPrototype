#include "InputHistory.h"

InputHistory::FrameInputPlus::FrameInputPlus()
{
	DirectionHeld = 0;
	for (unsigned int a = 0; a < 6; a++)
	{
		ButtonsHeld[a] = 0;
	}
}

InputHistory::FrameInputPlus::FrameInputPlus(FrameInput f)
{
	FInput = f;
}

void InputHistory::FrameInputPlus::PressedDuration(const std::vector<FrameInput>& i, int& full, int& partial) const
{
	// full = duration all buttons were pressed
	// partial = duration any buttons were pressed
	full = 0;
	partial = 0;
	for (unsigned int a = 0, size = i.size(); a < size;  a++)
	{
		// `duration` is for the specific input at index `a`
		int duration = std::numeric_limits<int>::max();
		Direction direction = i[a].Dir;
		// Check that direction is held
		if (direction.Numpad != 0)
		{
			if (FInput.Dir.Numpad == direction.Numpad)
			{
				duration = std::min(duration, DirectionHeld);
			}
			else
			{
				continue;
			}
		}
		// Check that buttons were held
		Buttons buttons = i[a].Pressed;
		if (FInput.IsPressed(buttons))
		{
			for (unsigned int b = 0; (1U << b) <= buttons; b++)
			{
				if (((buttons & (1U << b)) != 0))
				{
					// Duration that every button (and direction) was held
					duration = std::min(duration, ButtonsHeld[b]);
					// Duration that any button was held
					partial = std::max(partial, ButtonsHeld[b]);
				}
			}
		}
		else
		{
			continue;
		}
		// Don't decrease if another input was held longer
		full = std::max(full, duration);
	}
}

void InputHistory::FrameInputPlus::ReleasedDuration(const std::vector<FrameInput>& i, int& full, int& partial) const
{
	full = 0;
	for (unsigned int a = 0, size = i.size(); a < size; a++)
	{
		// If the input is pressed to begin with, output zeroes
		if (FInput.IsPressed(i[a]))
		{
			full = 0;
			partial = 0;
			return;
		}
		// `duration` is the "full" length for a specific input
		int duration = std::numeric_limits<int>::max();
		Buttons buttons = i[a].Pressed;
		if (buttons == None)
		{
			duration = std::min(duration, DirectionHeld);
		}
		else
		{
			// Loop through buttons of given input
			for (unsigned int b = 0; (1U << b) <= buttons; b++)
			{
				if (((buttons & (1U << b)) != 0))
				{
					// Duration that every button was released
					duration = std::min(duration, ButtonsHeld[b]);
					// Duration that any button was released
					partial = std::max(duration, ButtonsHeld[b]);
				}
			}
		}
		// Don't decrease if another input was released longer
		full = std::max(full, duration);
	}
}

InputHistory::InputData::InputData()
{
	pos = 0;
}

const InputHistory::FrameInputPlus& InputHistory::InputData::operator[](unsigned int index) const
{
	return inputs[(pos + index) % MAX_SIZE];
}

void InputHistory::InputData::push_front(FrameInput item)
{
	pos = (pos + MAX_SIZE - 1) % MAX_SIZE;
	const FrameInputPlus prev_frame = operator[](1);
	FrameInputPlus new_frame(item);

	Buttons prev_buttons = prev_frame.FInput.Pressed;
	Buttons new_buttons = item.Pressed;
	unsigned int diff_buttons = prev_buttons ^ new_buttons;
	for (unsigned int a = 0; a < 6; a++)
	{
		if (((1U << a) & diff_buttons) == 0)
		{
			new_frame.ButtonsHeld[a] = prev_frame.ButtonsHeld[a] + 1;
		}
		else
		{
			new_frame.ButtonsHeld[a] = 1;
		}
	}

	if (new_frame.FInput.Dir.Numpad == prev_frame.FInput.Dir.Numpad)
	{
		new_frame.DirectionHeld = prev_frame.DirectionHeld + 1;
	}
	else
	{
		new_frame.DirectionHeld = 1;
	}
	inputs[pos] = new_frame;
}

void InputHistory::InputData::InputDuration(const Input i, int frame, int& full, int& partial) const
{
	const std::vector<FrameInput>& value = i.GetValue();
	bool pressOK = i.IsPressOK();
	bool releaseOK = i.IsReleaseOK() || !(pressOK || i.IsHoldOK());
	full = 0;
	partial = 0;
	int added = 0;
	do
	{
		int difference = 0;
		if (pressOK)
		{
			operator[](frame + full).PressedDuration(value, added, difference);
		}
		if (added == 0 && releaseOK)
		{
			operator[](frame + full).ReleasedDuration(value, added, difference);
		}
		partial = std::max(partial, full + difference);
		full += added;
	} while (added > 0 && full < MAX_SIZE);
	full -= i.Charge;
	partial -= i.Charge;
}

void InputHistory::FlushBuffer()
{
	data = InputData();
}

void InputHistory::CheckSingleInput(const Input i, unsigned int start, int range, int& first, int& last) const
{
	bool holdOK = i.IsHoldOK();
	bool pressOK = i.IsPressOK();
	bool releaseOK = i.IsReleaseOK();
	//bool noHoldOK = !holdOK && !pressOK && !releaseOK;

	for (first = 0; first < range; first++)
	{
		int fduration, fpartial;
		data.InputDuration(i, start + first, fduration, fpartial);
		if (fduration <= 0)
		{
			// 0 frame must have a valid input
			if (start == 0)
			{
				first = -1;
				last = -1;
				return;
			}
			continue;
		}
		else if (!holdOK && (pressOK || releaseOK) &&
			(fpartial > hold_buffer))
		{
			if (start == 0)
			{
				first = -1;
				last = -1;
				return;
			}
			first = fpartial - hold_buffer;
			continue;
		}

		for (last = range - 1; last >= first + fduration; last--)
		{
			int lduration, lpartial;
			data.InputDuration(i, start + last, lduration, lpartial);
			if (lduration > 0)
			{
				if ((!holdOK && (pressOK || releaseOK)) &&
					(lpartial > hold_buffer))
				{
					if (pressOK != releaseOK)
					{
						last--;
					}
					continue;
				}
				else
				{
					break;
				}
			}
		}
		return;
	}
	// Valid input not found
	first = -1;
	last = -1;
}

bool InputHistory::CheckCommandInput(const Command c) const
{
	//! \TODO Possible further optimization:
	//!       Skip second search for last input

	// Because command checking works by starting from the current frame (0) and tracing inputs
	// backwards, it searches up to the length of the input buffer for one input, noting the first
	// and last valid occurrences. The time between these two becomes a "bonus" buffer.
	const std::vector<Input>& command = c.GetData();
	unsigned int currentIndex = 0;
	unsigned int bonus_buffer = 0;
	bool inputFound = true; // tracks if a previous optional input was skipped

	for (int i = command.size() - 1; i >= 0; i--)
	{
		Input nextInput = command[i];

		unsigned int buffer = bonus_buffer + (nextInput.IsIncreaseBuffer() ? increased_buffer : standard_buffer);

		int first, last;
		CheckSingleInput(nextInput, currentIndex, buffer, first, last);
		if (first < 0)
		{
			if (inputFound && !nextInput.IsRequired())
			{
				inputFound = false;
			}
			else
			{
				return false;
			}
		}
		else
		{
			bonus_buffer = last - first;
			if (nextInput.IsRequired())
			{
				currentIndex += first + 1;
			}
			else
			{
				bonus_buffer += first + 1;
			}
		}
	}
	return true;
}

bool InputHistory::CheckCommandInput(const std::vector<Command>& c) const
{
	for (unsigned int a = 0, c_size = c.size(); a < c_size; a++)
	{
		if (CheckCommandInput(c[a]))
		{
			return true;
		}
	}
	return false;
}

void InputHistory::AddFrame(FrameInput inputs)
{
	data.push_front(inputs);
}