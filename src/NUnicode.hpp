/**
* @file NUnicode.hpp
* @brief Handles Unicode
* @author Jookia
* @version 1.0
* @date 2012-05-27
*/

#ifndef NAELSTROF_UNICODE
#define NAELSTROF_UNICODE

/** @addtogroup Unicode
 * @{*/

#define _UNICODE
#define UNICODE
// _MBCS means you can use Unicode paths with FreeType.
#define _MBCS

#ifdef _UNICODE
	#define tcout wcout
	#define tofstream wofstream
	#define tchar wchar_t
	#define tstringstream wstringstream
	#define tstring wstring
	#define _T(x) L ## x
#else
	#define tcout cout
	#define tofstream ofstream
	#define tchar char
	#define tstringstream stringstream
	#define tstring string
	#define _T(x) x
#endif

#include <string>

/**
* @brief Converts a wide string to a multibyte version.
*
* @param wideString The wide string.
*
* @return A multibyte string.
*/
std::string ToMBS(std::wstring wideString);

/**
* @brief Fallback if Unicode support isn't enabled..
*/
std::string ToMBS(std::string narrowString);

/*@}*/

#endif
