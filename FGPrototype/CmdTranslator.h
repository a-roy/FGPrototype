//! \file
//! CmdTranslator class declaration
#pragma once

#include <array>
#include <stack>
#include <string>
#include "Command.h"

#define optional '?'
#define no_hold '*'
#define release_ok '!'
#define no_press '@'
#define increase_buffer '_'
#define inter1 '<'
#define inter2 '>'
#define simul1 '('
#define simul2 ')'
#define charge1 '['
#define charge2 ']'

//! Translates input commands from plain text.
class CmdTranslator
{
public:
	//! Default constructor
	//!
	//! Uses default button names, "LP", "MP", "HP", "LK", "MK", "HK".
	CmdTranslator();
	//! Non-default constructor
	//!
	//! \param but Describes 6 button names to be used in command definitions.
	//! \warning   Do not use any button names that are equivalent to or a
	//!            substring of another button name (e.g. "HS" and "S")
	CmdTranslator(std::array<std::string, 6> but);
	//! Checks if a command string has a legal conformation of brackets.
	//!
	//! \param s      the string to translate
	//! \retval true
	//! \retval false
	bool CheckBrackets(std::string s);
	//! Translates a string into a Command object.
	//!
	//! Examples:
	//!   <123>                          Hold a down direction (e.g. crouch)
	//!   8                              Hold up (e.g. neutral jump)
	//!   LP*                            Press LP (e.g. st.LP)
	//!   <(2LP)(1LP)(3LP)>*             Press LP while holding any down direction (e.g. cr.LP)
	//!   63?2?1?4_<(5LP)(4LP)(7LP)>*    hcb+LP with leniency (e.g. Yoga Flame)
	//!   <417>_[50]6_<(5LP)(6LP)(9LP)>* [b]~f+LP with 50f charge time (e.g. Sonic Boom)
	//! \param s  the string to translate
	//! \return a pointer to the generated Command
	Command Translate(std::string s);
private:
	std::array<std::string, 6> buttons;
};