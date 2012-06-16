#include "NEngine.hpp"
#ifdef _WIN32
void SetColor(unsigned int Color)
{
	if (Color>255)
	{
		return;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),Color);
}
void ClearColor()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),15);
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
