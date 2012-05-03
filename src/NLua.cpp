#include "NEngine.hpp"

NLua::NLua()
{
	L = lua_open();
	luaL_openlibs(L);
	static const luaL_Reg BaseFunctions[] =
	{
		{"include",Include},
		{NULL,NULL}
	};
	lua_getglobal(L,"_G");
	luaL_register(L, NULL, BaseFunctions);
}

NLua::~NLua()
{
	lua_close(L);
}

int Include(lua_State* L)
{
	lua_Debug ar1;
	lua_getstack(L,1,&ar1);
	lua_getinfo(L,"f",&ar1);
	lua_Debug ar2;
	lua_getinfo(L,">S",&ar2);
	std::string Source = ar2.source;
	int Found = Source.find_last_of("/\\");
	std::string Folder = Source.substr(1,Found);
	std::string File = luaL_checkstring(L,1);
}

bool NLua::DoFile(std::string File)
{
	if (luaL_dofile(L,File.c_str()))
	{
		SetColor(Yellow);
		std::cout << "LUA WARN: ";
		ClearColor();
		std::cout << lua_tostring(L, -1) << "\n";
		return Fail;
	}
	return Success;
}

lua_State* NLua::GetL()
{
	return L;
}
