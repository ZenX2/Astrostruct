#include "NEngine.hpp"

void SignalHandler(int Signal)
{
	switch (Signal)
	{
		case SIGINT:
		{
			SetColor(Blue);
			std::cout << "SIGNAL INFO: ";
			ClearColor();
			std::cout << "Recieved signal " << SignalToString(Signal) << ": exiting...\n";
			GetGame()->Close();
			break;
		}
		case SIGSEGV:
		{
			SetColor(Yellow);
			std::cout << "SIGNAL WARN: ";
			ClearColor();
			std::cout << "Recieved signal " << SignalToString(Signal) << ": this could cause stability issues and break a lot of things!\n";
			break;
		}
		case SIGFPE:
		{
			SetColor(Yellow);
			std::cout << "SIGNAL WARN: ";
			ClearColor();
			std::cout << "Recieved signal " << SignalToString(Signal) << ": aw damn we divided by zero or something dumb like that. Not sure how well things will run from here!\n";
			break;
		}
		case SIGILL:
		{
			SetColor(Yellow);
			std::cout << "SIGNAL WARN: ";
			ClearColor();
			std::cout << "Recieved signal " << SignalToString(Signal) << ": uh oh, this could be due to code corruption or memory corruption. Or we could be attempting to execute raw data. Not sure how well things will run from here!\n";
			break;
		}
		case SIGABRT:
		{
			SetColor(Yellow);
			std::cout << "SIGNAL WARN: ";
			ClearColor();
			std::cout << "Recieved signal " << SignalToString(Signal) << ": something wants us to terminate because they're mad, unfortunately I should comply...\n";
			GetGame()->Close();
			break;
		}
		case SIGTERM:
		{
			SetColor(Blue);
			std::cout << "SIGNAL INFO: ";
			ClearColor();
			std::cout << "Recieved signal " << SignalToString(Signal) << ": exiting...\n";
			GetGame()->Close();
			break;
		}
		default:
		{
			SetColor(Yellow);
			std::cout << "SIGNAL WARN: ";
			ClearColor();
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
	signal(SIGINT,SignalHandler);
	signal(SIGSEGV,SignalHandler);
	signal(SIGTERM,SignalHandler);
}

NSignalInterceptor::~NSignalInterceptor()
{
	signal(SIGABRT,SIG_DFL);
	signal(SIGFPE,SIG_DFL);
	signal(SIGILL,SIG_DFL);
	signal(SIGINT,SIG_DFL);
	signal(SIGSEGV,SIG_DFL);
	signal(SIGTERM,SIG_DFL);
}
