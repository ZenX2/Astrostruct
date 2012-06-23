#include "NEngine.hpp"
#ifdef _WIN32
void NTerminal::SetColor(unsigned int Color)
{
    if (Color>255)
    {
        return;
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),Color);
}
void NTerminal::ClearColor()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),15);
}
#else
void NTerminal::SetColor(unsigned int Color)
{
    std::cout << "\e[1;" << Color+30 << 'm';
}

void NTerminal::ClearColor()
{
    std::cout << "\e[0m";
}
#endif
