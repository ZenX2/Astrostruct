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
#define _T(x) L ## x

/**
* @brief Converts a wide string to a multibyte version.
*
* @param wideString The wide string.
*
* @return A multibyte string.
*/
std::string ToMBS(std::wstring wideString);

/*@}*/

#endif
