#include "NEngine.hpp"

#ifdef _WIN32
//This god awful peice of gobbledegook is required to make windows not wait on console input.
void ConsoleThread(void* arg)
{
	HANDLE STDIN = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE STDOUT = GetStdHandle(STD_OUTPUT_HANDLE);
	std::string Line;
	FlushConsoleInputBuffer(STDIN);
	while (GetGame()->Running())
	{
		if (WaitForSingleObject(STDIN,1000) == WAIT_TIMEOUT) //If no input has been entered in the past second, restart the loop to ensure the game is still running.
		{
			std::cout << "Waited!\n";
			continue;
		}
		bool Valid = false;
		INPUT_RECORD InBuff[128];
		DWORD ReadCount;
		ReadConsoleInput(STDIN,InBuff,128,&ReadCount);
		for (unsigned int i=0;i<ReadCount;i++) //Grab and loop through keyboard events, pushing printable characters into what we want to run in lua.
		{
			if (InBuff[i].EventType == KEY_EVENT)
			{
				if (InBuff[i].Event.KeyEvent.bKeyDown)
				{
					char Ch = InBuff[i].Event.KeyEvent.uChar.AsciiChar;
					if (isprint(Ch))
					{
						Line.push_back(Ch);
						std::cout << Ch;
					}
					if (InBuff[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN) //If the user presses enter, disregard the rest of the input and execute the code!
					{
						std::cout << "\n";
						Valid = true;
						break;
					}
					if (InBuff[i].Event.KeyEvent.wVirtualKeyCode == VK_BACK) //If the user presses backspace, delete a previous character and move the cursor back one.
					{
						_CONSOLE_SCREEN_BUFFER_INFO Info[1];
						GetConsoleScreenBufferInfo(STDOUT,Info);
						COORD Pos = Info[0].dwCursorPosition;
						if (Pos.X>0)
						{
							Pos.X-=1;
							SetConsoleCursorPosition(STDOUT,Pos);
							std::cout << " " << std::flush;
						}
						SetConsoleCursorPosition(STDOUT,Pos);
						if (Line.length() > 0)
						{
							Line.erase(Line.end()-1);
						}
					}
				}
			}
		}
		if (!Valid) //If the user hasn't pressed enter yet, continue the loop!
		{
			continue;
		}
		lua_State* L = GetGame()->GetLua()->GetL();
		if (luaL_dostring(L,Line.c_str()))
		{
			SetColor(Yellow);
			std::cout << "LUA WARN: ";
			ClearColor();
			std::cout << lua_tostring(L, -1) << "\n" << std::flush;
		}
		Line.clear();
		FlushConsoleInputBuffer(STDIN);
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
