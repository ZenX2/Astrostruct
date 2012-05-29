#include "NEngine.hpp"
#ifdef _WIN32
HANDLE* GetConsole()
{
	if (Console == NULL)
	{
		Console = new HANDLE();
		*Console = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	return Console;
}
void SetColor(unsigned int Color)
{
	if (Color>255)
	{
		return;
	}
	SetConsoleTextAttribute(*GetConsole(),Color);
}
void ClearColor()
{
	SetConsoleTextAttribute(*GetConsole(),15);
}
#else
void SetColor(unsigned int Color)
{
	std::cout << "\e[1;" << Color+30 << 'm';
}

void ClearColor()
{
	std::cout << "\e[0m";
}
#endif
