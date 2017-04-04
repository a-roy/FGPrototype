//! \file
//! InputHistory class declaration
#pragma once

#include "Command.h"
#include <algorithm>
#include <vector>

static const unsigned int MAX_SIZE = 100;
static const unsigned int
hold_buffer = 4,
release_buffer = 4,
standard_buffer = 4,
increased_buffer = 8;

//! Stores inputs from a player and interprets commands.
class InputHistory
{
public:
	//! Clears all inputs.
	void FlushBuffer();
	//! Helper for CheckCommandInput
	//!
	//! \param[in]  i     An input to check for.
	//! \param[in]  start The frame to start searching from.
	//! \param[in]  range The number of frames to search.
	//! \param[out] first The number of frames from _start_ to the first (anti-chronological) valid
	//!                   input in the range, or -1 if not found.
	//! \param[out] last  The number of frames from _start_ to the last (anti-chronological) valid
	//!                   input in the range, or -1 if not found.
	void CheckSingleInput(const Input i, unsigned int start, int range, int& first, int& last) const;
	//! Check if a specific command has been inputted.
	//!
	//! \param[in] c A command to check for.
	//! \return      A boolean indicating whether the command was inputted.
	bool CheckCommandInput(const Command c) const;
	//! Check if any of a set of commands have been inputted.
	//!
	//! \param[in] c A vector containing the commands to check for.
	//! \return      A boolean indicating whether any of the commands were inputted.
	bool CheckCommandInput(const std::vector<Command>& c) const;
	//! Adds a frame of data to the "front" of the InputHistory.
	//!
	//! \param[in] inputs The inputs of the new frame.
	void AddFrame(FrameInput inputs);
private:
	//! \internal By "redundantly" storing the length of time that inputs are held, we can very
	//!           easily check command inputs with the hold buffer, and we can especially easily
	//!           check those involving charge times. We can also support arbitrarily long charge
	//!           times, like Boxer's Turn Around Punch and Raiden's Dropkick without storing all
	//!           of the inputs.
	struct FrameInputPlus
	{
		FrameInput FInput;
		int DirectionHeld;
		int ButtonsHeld[6];

		FrameInputPlus();
		FrameInputPlus(FrameInput f);
		void PressedDuration(const std::vector<FrameInput>& i, int& full, int& partial) const;
		void ReleasedDuration(const std::vector<FrameInput>& i, int& full, int& partial) const;
	};

	//! \internal I rolled my own container here because STL is slow. Data is stored in an array
	//!           with modulo indexing. It uses a constant contiguous block of memory to maximize
	//!           efficiency. Pushing elements to the front automatically replaces the elements
	//!           at the end. Negative index lookup isn't allowed.
	class InputData
	{
	public:
		InputData();
		const FrameInputPlus& operator[](unsigned int index) const;
		void push_front(FrameInput item);
		//! Measure the duration for which an input was valid.
		//!
		//! \param[in]  i       The input to check for.
		//! \param[in]  frame   The frame to start measuring from.
		//! \param[out] full    The duration that the full input was valid.
		//! \param[out] partial The duration that any of the input was valid.
		void InputDuration(const Input i, int frame, int& full, int& partial) const;
	private:
		FrameInputPlus inputs[MAX_SIZE];
		unsigned int pos;
	};
	InputData data;
};