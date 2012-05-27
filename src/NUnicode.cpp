#include "NUnicode.hpp"

#include <cstdlib>

std::string ToMBS(std::wstring wideString)
{
  // Disclaimer: Narrow doesn't mean UTF-8. It's as ambigious as 'wide'.
  
  size_t size = wcstombs(0, wideString.c_str(), 0) + 1;
  
	char* buffer = new char[size];
  
	wcstombs(buffer, wideString.c_str(), wideString.length());
  
	std::string converted(buffer);
  
	delete[] buffer;
  
  return converted;
}

std::string ToMBS(std::string narrowString)
{
  return narrowString; // Already narrow.
}
