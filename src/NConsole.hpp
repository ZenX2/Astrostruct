/**
 * @file NConsole.hpp
 * @brief Hooks into console input in another thread to execute lua given by the user.
 * @author Dalton Nell
 * @version 1.0
 * @date 2012-05-29
 */

#ifndef NAELSTROF_CONSOLE
#define NAELSTROF_CONSOLE

void ConsoleThread(void* arg);

class NConsole
{
public:
	NConsole();
	~NConsole();
private:
	GLFWthread Thread;
};

#endif
