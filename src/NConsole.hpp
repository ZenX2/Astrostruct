/**
 * @file NConsole.hpp
 * @brief Hooks into console input in another thread to execute lua given by the user.
 * @author Dalton Nell
 * @version 1.0
 * @date 2012-05-29
 */

#ifndef NAELSTROF_CONSOLE
#define NAELSTROF_CONSOLE

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/select.h> //We need select to enable async input on linux.
#endif

/**
 * @brief Program that's split into a separate thread in order to grab user input from the console.
 *
 * @param arg Nothing, required for glfw's way of creating threads.
 */
void ConsoleThread(void* arg);

/**
 * @brief Handles creating a program thread to eat user input and feed it to lua to interpret.
 */
class NConsole
{
public:
	/**
	 * @brief Creates the ConsoleThread() thread.
	 */
	NConsole();
	/**
	 * @brief Destroys the Consolethread() thread.
	 */
	~NConsole();
private:
	GLFWthread Thread;
};

#endif
