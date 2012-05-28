#include "NEngine.hpp"

std::string ToMBS(std::wstring WideString)
{
	// Disclaimer: Narrow doesn't mean UTF-8. It's as ambigious as 'wide'.
	char* Buffer = new char[WideString.length()+1];
	wcstombs(Buffer, WideString.c_str(), WideString.length());
	Buffer[WideString.length()] = '\0';
	std::string Converted(Buffer);
	delete[] Buffer;
	return Converted;
}
