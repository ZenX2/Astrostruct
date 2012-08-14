#include "NEngine.hpp"

void SignalHandler(int Signal)
{
    switch (Signal)
    {
        case SIGSEGV:
        {
            GetGame()->GetLog()->Send("SIGNAL",0,"Recieved signal " + SignalToString(Signal) + ": please use valgrind to get some debug info and send bug reports!");
            exit(1);
            break;
        }
        case SIGFPE:
        {
            GetGame()->GetLog()->Send("SIGNAL",0,"Recieved signal " + SignalToString(Signal) + ": aw damn we divided by zero or something dumb like that. We must exit now :(");
            exit(1);
            break;
        }
        case SIGILL:
        {
            GetGame()->GetLog()->Send("SIGNAL",0,"Recieved signal " + SignalToString(Signal) + ": uh oh, this could be due to code corruption or memory corruption. Or we could be attempting to execute raw data. Run for your life!");
            exit(1);
            break;
        }
        case SIGABRT:
        {
            GetGame()->GetLog()->Send("SIGNAL",0,"Recieved signal " + SignalToString(Signal) + ": something wants us to terminate because they're mad, unfortunately I have to comply. :(");
            GetGame()->Close();
            break;
        }
        case SIGTERM:
        {
            GetGame()->GetLog()->Send("SIGNAL",0,"Recieved signal " + SignalToString(Signal) + ": exiting...");
            GetGame()->Close();
            break;
        }
        default:
        {
            GetGame()->GetLog()->Send("SIGNAL",0,"Recieved unknown signal " + SignalToString(Signal) + ". Not sure what I should do about it, so I'll happily do nothing!");
            break;
        }
    }
}

std::string SignalToString(int Signal)
{
    switch (Signal)
    {
        case SIGTERM: return "SIGTERM";
        case SIGABRT: return "SIGABRT";
        case SIGFPE: return "SIGFPE";
        case SIGILL: return "SIGILL";
        case SIGINT: return "SIGINT";
        case SIGSEGV: return "SIGSEGV";
        default:
        {
            std::stringstream String;
            String << Signal;
            return String.str();
        }
    }
}

NSignalInterceptor::NSignalInterceptor()
{
    signal(SIGABRT,SignalHandler);
    signal(SIGFPE,SignalHandler);
    signal(SIGILL,SignalHandler);
    signal(SIGSEGV,SignalHandler);
    signal(SIGTERM,SignalHandler);
}

NSignalInterceptor::~NSignalInterceptor()
{
    signal(SIGABRT,SIG_DFL);
    signal(SIGFPE,SIG_DFL);
    signal(SIGILL,SIG_DFL);
    signal(SIGSEGV,SIG_DFL);
    signal(SIGTERM,SIG_DFL);
}
