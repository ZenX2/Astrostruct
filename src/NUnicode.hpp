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

/**
 * @brief Turns string literals into a unicode string.
 *
 * @param The string literal.
 *
 * @return A wide version of the literal.
 */
#define _t(x) L ## x

/**
* @brief Converts a wide string to a multibyte version.
*
* @param WideString The wide string.
*
* @return A multibyte string.
*/
std::string ToMBS(std::wstring WideString);
/**
 * @brief Converts a narrow string to a wide one.
 *
 * @param NarrowString The narrow string.
 *
 * @return A wide string.
 */
std::wstring ToMBS(std::string NarrowString);

/*@}*/

#endif
