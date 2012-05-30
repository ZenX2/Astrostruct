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
	static HANDLE* Console = NULL;
	/**
	 * @brief In windows, it returns a handle to the console in order to change it's color, this shouldn't be called by anything in the engine else it'll lose it's multi-platform-ness.
	 *
	 * @return A handle to the console.
	 */
	HANDLE* GetConsole();
    #define Red 12
    #define Green 10
    #define Yellow 14
    #define Blue 9
#else
    #define Red 1
    #define Green 2
    #define Yellow 3
    #define Blue 4
#endif

/**
* @brief Sets the color of text output.
*
* @param Color The specific color as an int: (1)Red, (2)Green, (3)Yellow, (4)Blue.
*/
void SetColor(unsigned int Color);
/**
* @brief Resets color output to normal color.
*/
void ClearColor();
/**
 * @brief In windows it frees the console handle if it exists, in Linux it does nothing :u.
 */
void NFreeConsole();
/*@}*/

#endif
