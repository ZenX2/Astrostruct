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
    //Check what the message level is on to set the color of the tag.
    switch (Level)
    {
        case 0:     GetGame()->GetTerminal()->SetColor(Red);       break;
        case 1:     GetGame()->GetTerminal()->SetColor(Yellow);    break;
        case 2:     GetGame()->GetTerminal()->SetColor(Blue);      break;
        default:    GetGame()->GetTerminal()->ClearColor();        break;
    }
    //Now tag the system name with the message level.
    switch (Level)
    {
        case 0:     System.append("\tERRO: "); break;
        case 1:     System.append("\tWARN: "); break;
        case 2:     System.append("\tINFO: "); break;
        default:    System.append("\tVERB: "); break;
    }
    //Output the system that the message is coming from.
    std::cout << System;
    //Make sure the system color is back to default for the actual message though.
    GetGame()->GetTerminal()->ClearColor();
    Message.push_back('\n');
    std::cout << Message;
    //Write the message to file.
    File << System << Message;
    //If the message is an ERROR, flush the file buffer because the program is probably going to shutdown or segfault without being able to flush the buffer itself.
    if (Level == 0)
    {
        std::cout << std::flush;
        File.flush();
    }
}

void NLog::Log(std::string Message)
{
    //Not sure if i used this anywhere, but writes stright up messages to the log file.
    File << Message << '\n';
}
