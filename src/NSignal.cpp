#include "NEngine.hpp"

void SignalHandler(int Signal)
{
	switch (Signal)
	{
		case SIGSEGV:
		{
			NTerminal::SetColor(Red);
			std::cout << "SIGNAL ERROR: ";
			NTerminal::ClearColor();
			std::cout << "Recieved signal " << SignalToString(Signal) << ": please use valgrind to get some debug info and send bug reports!\n";
			exit(1);
			break;
		}
		case SIGFPE:
		{
			NTerminal::SetColor(Yellow);
			std::cout << "SIGNAL WARN: ";
			NTerminal::ClearColor();
			std::cout << "Recieved signal " << SignalToString(Signal) << ": aw damn we divided by zero or something dumb like that. Not sure how well things will run from here!\n";
			break;
		}
		case SIGILL:
		{
			NTerminal::SetColor(Yellow);
			std::cout << "SIGNAL WARN: ";
			NTerminal::ClearColor();
			std::cout << "Recieved signal " << SignalToString(Signal) << ": uh oh, this could be due to code corruption or memory corruption. Or we could be attempting to execute raw data. Not sure how well things will run from here!\n";
			break;
		}
		case SIGABRT:
		{
			NTerminal::SetColor(Red);
			std::cout << "SIGNAL ERROR: ";
			NTerminal::ClearColor();
			std::cout << "Recieved signal " << SignalToString(Signal) << ": something wants us to terminate because they're mad, unfortunately I should comply...\n";
			GetGame()->Close();
			break;
		}
		case SIGTERM:
		{
			NTerminal::SetColor(Blue);
			std::cout << "SIGNAL INFO: ";
			NTerminal::ClearColor();
			std::cout << "Recieved signal " << SignalToString(Signal) << ": exiting...\n";
			GetGame()->Close();
			break;
		}
		default:
		{
			NTerminal::SetColor(Yellow);
			std::cout << "SIGNAL WARN: ";
			NTerminal::ClearColor();
			std::cout << "Recieved unhandled signal: " << SignalToString(Signal) << "\n";
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
