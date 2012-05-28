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

bool NLua::DoFile(std::string FileName)
{
	NFile File = GetGame()->GetFileSystem()->GetFile(FileName);
	if (!File.Good())
	{
		SetColor(Yellow);
		std::cout << "LUA WARN: ";
		ClearColor();
		std::cout << "Could not do file " << FileName << ", it doesn't exist!\n";
		return false;
	}
	char* FileData = new char[File.Size()+1];
	File.Read(FileData,File.Size());
	FileData[File.Size()] = '\0'; //Ensure the 'string' is null terminated, we could be reading some random file for all we know.
	if (luaL_dostring(L,FileData))
	{
		delete[] FileData;
		SetColor(Yellow);
		std::cout << "LUA WARN: ";
		ClearColor();
		std::cout << lua_tostring(L, -1) << "\n";
		return Fail;
	}
	delete[] FileData;
	return Success;
}

bool NLua::DoFolder(std::string Folder)
{
	SetColor(Blue);
	std::cout << "LUA INFO: ";
	ClearColor();
	std::cout << "Scanning " << Folder << " for lua files...\n";
	std::string ParentFolder = Folder;
	bool Result = Success;
	std::vector<std::string> Files = GetGame()->GetFileSystem()->GetFiles(Folder);
	for (unsigned int i=0;i<Files.size();i++)
	{
		if (Files[i].find(".lua")!=std::string::npos && Files[i][Files[i].length()-1] == 'a')
		{
			SetColor(Blue);
			std::cout << "LUA INFO: ";
			ClearColor();
			std::cout << "Found " << Files[i] << "\n";
			Result = (DoFile(Files[i]) && Result);
		}
	}
	return Result;
}

lua_State* NLua::GetL()
{
	return L;
}
