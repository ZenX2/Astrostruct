#include "NEngine.hpp"

#ifdef _WIN32
void NTerminal::SetColor(unsigned int Color)
{
    //SetConsoleTextAttribute only takes a char, which can overflow if the value is > 255
    if (Color>255)
    {
        return;
    }
    //Some nerdy window-specific function because windows doesn't abide to linux/gnu specifications.
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),Color);
}
void NTerminal::ClearColor()
{
    //15 is the default text color (white)
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),15);
}
#else
void NTerminal::SetColor(unsigned int Color)
{
    //Send an escaped code which hopefully will change the color of new incoming text if the terminal supports it.
    std::cout << "\e[1;" << Color+30 << 'm';
}

void NTerminal::ClearColor()
{
    //Send an escaped code that will set the terminal back to the default color.
    std::cout << "\e[0m";
}
#endif
