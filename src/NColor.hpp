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
    #include <windows.h>
#endif

namespace NTerminal
{
    #ifdef _WIN32
        enum Color
        {
            Red = 12,
            Green = 10,
            Yellow = 14,
            Blue = 9
        };
    #else
        enum Color
        {
            Red = 1,
            Green,
            Yellow,
            Blue
        };
    #endif
    /**
    * @brief Sets the color of text output.
    *
    * @param Color The specific color as an enum: Red, Green, Yellow, and Blue.
    */
    void SetColor(unsigned int Color);
    /**
    * @brief Resets color output to normal color.
    */
    void ClearColor();
}
/*@}*/

#endif
