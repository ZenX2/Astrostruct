#include "NEngine.hpp"

NLog::NLog()
{
    std::string Path = PHYSFS_getBaseDir();
    File.open((Path+"/log.txt").c_str());
}

NLog::~NLog()
{
    File.close();
}

void NLog::Send(std::string System, unsigned int Level, std::string Message)
{
    switch (Level)
    {
        case 0:     NTerminal::SetColor(NTerminal::Red);       break;
        case 1:     NTerminal::SetColor(NTerminal::Yellow);    break;
        case 2:     NTerminal::SetColor(NTerminal::Blue);      break;
        default:    NTerminal::ClearColor();                   break;
    }
    switch (Level)
    {
        case 0:     System.append("\tERRO: "); break;
        case 1:     System.append("\tWARN: "); break;
        case 2:     System.append("\tINFO: "); break;
        default:    System.append("\tVERB: "); break;
    }
    std::cout << System;
    NTerminal::ClearColor();
    Message.push_back('\n');
    std::cout << Message;
    File << System << Message;
    if (Level == 0)
    {
        std::cout << std::flush;
    }
}

void NLog::Log(std::string Message)
{
    File << Message << '\n';
}
