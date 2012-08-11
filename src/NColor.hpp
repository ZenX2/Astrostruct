/**
* @file NColor.hpp
* @brief Changes color of text output; only works on linux at the moment.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-01
*/

#ifndef NAELSTROF_COLOR
#define NAELSTROF_COLOR

/** @addtogroup TerminalColor
 * @{*/

#ifdef _WIN32
enum NTerminalColor
{
    Red = 12,
    Green = 10,
    Yellow = 14,
    Blue = 9
};
#else
enum NTerminalColor
{
    Red = 1,
    Green,
    Yellow,
    Blue
};
#endif

/**
 * @brief Class that handles terminal specific stuff, like coloring and uh. Oh that's all it supports.
 */
class NTerminal
{
public:
    /**
    * @brief Sets the color of text output.
    *
    * @param Color The specific color as an enum: Red, Green, Yellow, and Blue.
    */
    void SetColor(unsigned int NTerminalColor);
    /**
    * @brief Resets color output to normal color.
    */
    void ClearColor();
};
/*@}*/

#endif
