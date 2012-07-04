#include "NEngine.hpp"

void NLog::Send(std::string System, unsigned int Level, std::string Message)
{
    switch (Level)
    {
        case 0: NTerminal::SetColor(NTerminal::Red);       break;
        case 1: NTerminal::SetColor(NTerminal::Yellow);    break;
        case 2: NTerminal::SetColor(NTerminal::Blue);      break;
    }
    std::cout << System;
    switch (Level)
    {
        case 0: std::cout << "\tERRO: ";          break;
        case 1: std::cout << "\tWARN: ";          break;
        case 2: std::cout << "\tINFO: ";          break;
    }
    NTerminal::ClearColor();
    std::cout << Message << "\n";
    if (Level == 0)
    {
        std::cout << std::flush;
    }
}
