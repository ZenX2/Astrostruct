#include "NEngine.hpp"
#ifdef _WIN32
void SetColor(unsigned int Color) {}
void ClearColor(){}
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
