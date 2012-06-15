#include "NEngine.hpp"

#ifdef _WIN32
void ConsoleThread(void* arg)
{
	const HANDLE STDIN = GetStdHandle(STD_INPUT_HANDLE);
	while (GetGame()->Running())
	{
		if (WaitForMultipleObjects(1,&STDIN,true,1000) == WAIT_TIMEOUT) //Make sure we have input so we don't get stuck on cin.
		{
			continue;
		}
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
#else
void ConsoleThread(void* arg)
{
	struct timeval TimeOut;
	fd_set ReadFDS;
	const int STDIN = 0;
	while (GetGame()->Running())
	{
		TimeOut.tv_sec = 1; //Wait exactly one second to timeout.
		TimeOut.tv_usec = 0;
		FD_ZERO(&ReadFDS); //Set up ReadFDS to eat stdin
		FD_SET(STDIN, &ReadFDS);
		if (select(STDIN+1,&ReadFDS,NULL,NULL,&TimeOut) == -1) // Use select to ensure we don't get stuck on cin.
		{
			SetColor(Red);
			std::cout << "CONSOLE ERROR: ";
			ClearColor();
			std::cout << "Select failed to asyncronously grab input!\n";
			return;	
		}
		if (!FD_ISSET(STDIN,&ReadFDS)) // If we don't have any input ready for us, restart the loop!
		{
			continue;
		}
		//If we have some input, run it right through lua.
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
#endif

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
	glfwWaitThread(Thread, GLFW_WAIT); //Wait for the thread to shut down.
}
