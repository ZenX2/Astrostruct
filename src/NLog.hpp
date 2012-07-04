#ifndef NAELSTROF_LOG
#define NAELSTROF_LOG

class NLog
{
public:
    void Send(std::string System, unsigned int Level, std::string Message);
};

#endif
