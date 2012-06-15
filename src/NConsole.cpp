#include "NEngine.hpp"

void ConsoleThread(void* arg)
{
	while (GetGame()->Running())
	{
		std::string Line;
		getline(std::cin,Line);
		lua_State* L = GetGame()->GetLua()->GetL();
		if (luaL_dostring(L,Line.c_str()))
		{
			SetColor(Yellow);
			std::cout << "LUA WARN: ";
			ClearColor();
			std::cout << lua_tostring(L, -1) << "\n";
		}
	}
}

NConsole::NConsole()
{
	Thread = glfwCreateThread(ConsoleThread,NULL);
	if (Thread<0)
	{
		SetColor(Yellow);
		std::cout << "CONSOLE WARN: ";
		ClearColor();
		std::cout << "Failed to create thread for console input!\n";
		return;
	}
	SetColor(Blue);
	std::cout << "CONSOLE INFO: ";
	ClearColor();
	std::cout << "Console is ready for input, it runs raw lua calls; type help() if you want more info!\n";
}

NConsole::~NConsole()
{
	if (Thread<0)
	{
		return;
	}
	glfwDestroyThread(Thread);
}
