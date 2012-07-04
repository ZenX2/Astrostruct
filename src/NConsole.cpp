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
            GetGame()->GetLog()->Send("LUA",1,lua_tostring(L,-1));
        }
        Line.clear();
        FlushConsoleInputBuffer(STDIN);
    }
}
#else
void ConsoleThread(void* arg)
{
    while (GetGame()->Running())
    {
        const int STDIN = 0;
        struct timeval TimeOut;
        fd_set ReadFDS;
        TimeOut.tv_sec = 1; //Wait exactly one second to timeout.
        TimeOut.tv_usec = 0;
        FD_ZERO(&ReadFDS); //Set up ReadFDS to eat stdin
        FD_SET(STDIN, &ReadFDS);
        int Check = select(STDIN+1,&ReadFDS,NULL,NULL,&TimeOut);
        if (Check == -1) // Use select to make sure we don't get stuck on getline().
        {
            GetGame()->GetLog()->Send("CONSOLE",0,"Failed to asyncronously grab input! Shutting down console...");
            return; 
        }
        if (!FD_ISSET(STDIN,&ReadFDS)) // If we don't have any input ready for us, restart the loop!
        {
            continue;
        }
        //If we have some input, run it right through lua.
        std::string Line;
        getline(std::cin,Line);
        if (Line.length() == 0)
        {
            GetGame()->GetLog()->Send("CONSOLE",0,"Failed to asyncronously grab input! Shutting down console...");
            return; 
        }
        lua_State* L = GetGame()->GetLua()->GetL();
        if (luaL_dostring(L,Line.c_str()))
        {
            GetGame()->GetLog()->Send("LUA",1,lua_tostring(L,-1));
        }
    }
}
#endif

NConsole::NConsole()
{
    Thread = glfwCreateThread(ConsoleThread,NULL);
    if (Thread<0)
    {
        GetGame()->GetLog()->Send("CONSOLE",0,"Failed to create a thread for console input!");
        return;
    }
    GetGame()->GetLog()->Send("CONSOLE",2,"Console is ready for input, it runs raw lua calls; type help() if you want more info!");
}

NConsole::~NConsole()
{
    if (Thread<0)
    {
        return;
    }
    glfwWaitThread(Thread, GLFW_WAIT); //Wait for the thread to shut down.
}
