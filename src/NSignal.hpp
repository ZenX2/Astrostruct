#ifndef NAELSTROF_SIGNAL
#define NAELSTROF_SIGNAL

void SignalHandler(int Signal);
std::string SignalToString(int Signal);
class NSignalInterceptor
{
public:
	NSignalInterceptor();
	~NSignalInterceptor();
};

#endif
