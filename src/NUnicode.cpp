#include "NEngine.hpp"

std::string ToMBS(std::wstring WideString)
{
    switch(sizeof(wchar_t))
    {
        case 2:
        {
            std::string utf8line;
            utf8::utf16to8(WideString.begin(),WideString.end(),back_inserter(utf8line));
            return utf8line;
        }
        case 4:
        {
            std::string utf8line;
            utf8::utf32to8(WideString.begin(),WideString.end(),back_inserter(utf8line));
            return utf8line;
        }
        default:
        {
            return "UTF8 ERROR: Error converting wide string to narrow!";
        }
    }
}

std::wstring ToMBS(std::string NarrowString)
{
    switch(sizeof(wchar_t))
    {
        case 2:
        {
            std::wstring utf16line;
            utf8::utf8to16(NarrowString.begin(),NarrowString.end(),back_inserter(utf16line));
            return utf16line;
        }
        case 4:
        {
            std::wstring utf32line;
            utf8::utf8to32(NarrowString.begin(),NarrowString.end(),back_inserter(utf32line));
            return utf32line;
        }
        default:
        {
            return _t("UTF8 ERROR: Error converting narrow string to wide!");
        }
    }
}
