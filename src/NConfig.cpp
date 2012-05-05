#include "NEngine.hpp"

NConfig::NConfig(std::string i_File)
{
	lua_State* L = GetGame()->GetLua()->GetL();
	lua_getglobal(L,"_G");
	File = i_File;
	GetGame()->GetLua()->DoFile(File);
}

NConfig::~NConfig()
{
}

float NConfig::GetFloat(std::string Name)
{
	lua_State* L = GetGame()->GetLua()->GetL();
	lua_getglobal(L,"_G");
	lua_getfield(L,-1,Name.c_str());
	if (!lua_isnumber(L,-1))
	{
		SetColor(Yellow);
		std::cout << "CONFIG WARN: ";
		ClearColor();
		std::cout << "Tried to use variable " << Name << " as a number (It's not a number!). Does it exist in " << File << "?\n";
		lua_pop(L,2);
		return 0;
	}
	float Number = lua_tonumber(L,-1);
	lua_pop(L,2);
	return Number;
}

std::string NConfig::GetString(std::string Name)
{
	lua_State* L = GetGame()->GetLua()->GetL();
	lua_getglobal(L,"_G");
	lua_getfield(L,-1,Name.c_str());
	if (!lua_isstring(L,-1))
	{
		SetColor(Yellow);
		std::cout << "CONFIG WARN: ";
		ClearColor();
		std::cout << "Tried to use variable " << Name << " as a string (It's not a string!). Does it exist in " << File << "?\n";
		lua_pop(L,2);
		return "NULL";
	}
	const char* Text = lua_tostring(L,-1);
	if (Text == NULL)
	{
		return "NULL";
	}
	std::string ReturnString = Text;
	lua_pop(L,2);
	return ReturnString;
}

bool NConfig::GetBool(std::string Name)
{
	lua_State* L = GetGame()->GetLua()->GetL();
	lua_getglobal(L,"_G");
	lua_getfield(L,-1,Name.c_str());
	if (!lua_isboolean(L,-1))
	{
		SetColor(Yellow);
		std::cout << "CONFIG WARN: ";
		ClearColor();
		std::cout << "Tried to use variable " << Name << " as a boolean (It's not a boolean!). Does it exist in " << File << "?\n";
		lua_pop(L,2);
		return false;
	}
	bool ReturnBool = (bool)lua_toboolean(L,-1);
	lua_pop(L,2);
	return ReturnBool;
}
