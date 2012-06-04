#include "NEngine.hpp"

NLua::NLua()
{
	L = luaL_newstate();
	luaL_openlibs(L);
	static const luaL_Reg BaseFunctions[] =
	{
		{"include",Include},
		{"help",ConsoleHelp},
		{"quit",Quit},
		{"exit",Quit},
		{NULL,NULL}
	};
	lua_getglobal(L,"_G");
	luaL_register(L, NULL, BaseFunctions);
}

NLua::~NLua()
{
	lua_close(L);
}

int Quit(lua_State* L)
{
	SetColor(Blue);
	std::cout << "LUA INFO: ";
	ClearColor();
	std::cout << "Recieved a command to exit!\n";
	GetGame()->Close();
}

int ConsoleHelp(lua_State* L)
{
	lua_getglobal(L,"_G");
	lua_getfield(L,-1,"_VERSION");
	SetColor(Blue);
	std::cout << "CONSOLE INFO: ";
	ClearColor();
	std::cout << "Hello from " << luaL_checkstring(L,-1) << "!\n";
	SetColor(Blue);
	std::cout << "CONSOLE INFO: ";
	ClearColor();
	std::cout << "This console is ran using lua!\n";
	SetColor(Blue);
	std::cout << "CONSOLE INFO: ";
	ClearColor();
	std::cout << "Run any lua commands by typing them here!\n";
	SetColor(Blue);
	std::cout << "CONSOLE INFO: ";
	ClearColor();
	std::cout << "Here are some examples:\n";
	SetColor(Blue);
	std::cout << "CONSOLE INFO: ";
	ClearColor();
	std::cout << "quit()\t\t\texits the program\n";
	SetColor(Blue);
	std::cout << "CONSOLE INFO: ";
	ClearColor();
	std::cout << "print(\"Hello world!\")\tprints Hello world! to cout\n";
	lua_pop(L,2);
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

NAnimation* lua_toAnimation(lua_State* L, int index)
{
	NAnimation** Animation = (NAnimation**)luaL_checkudata(L,index,"AnimationBase");
	return *Animation;
}

NAnimation* lua_checkAnimation(lua_State* L, int narg)
{
	NAnimation* Foo = lua_toAnimation(L,narg);
	if (Foo == NULL)
	{
		luaL_argerror(L,narg,"attempt to index a NULL Animation");
	}
	return Foo;
}

void lua_pushAnimation(lua_State* L, NAnimation* Animation)
{
	NAnimation** Pointer = (NAnimation**)lua_newuserdata(L,sizeof(NAnimation*));
	*Pointer = Animation;
	luaL_getmetatable(L,"AnimationBase");
	lua_setmetatable(L,-2);
}

int CreateAnimation(lua_State* L)
{
	unsigned int ArgCount = lua_gettop(L);
	if (ArgCount == 0)
	{
		return 0;
	}
	NAnimation* Animation = new NAnimation();
	for (unsigned int i=1;i<1+ArgCount;i++)
	{
		const char* Field = luaL_checkstring(L,i);
		if (Field != NULL)
		{
			Animation->AddFrame(Field);
		}
	}
	lua_newtable(L);
	Animation->Reference = luaL_ref(L,LUA_REGISTRYINDEX);
	GetGame()->GetRender()->AddCachedAnimation(Animation);
	lua_pushAnimation(L,Animation);
	return 1;
}

int Animation__index(lua_State* L)
{
	NAnimation* Animation = lua_toAnimation(L,1);
	if (Animation == NULL)
	{
		lua_Debug ar1;
		lua_getstack(L,1,&ar1);
		lua_getinfo(L,"fl",&ar1);
		lua_Debug ar2;
		lua_getinfo(L,">S",&ar2);
		lua_pushfstring(L, "%s:%d: attempt to index a NULL Animation", ar2.short_src, ar1.currentline);
		return lua_error(L);
	}
	std::string Field = luaL_checkstring(L,2);
	if (Field == "FPS")
	{
		lua_pushnumber(L,Animation->FPS);
	} else if (Field == "Name")
	{
		lua_pushstring(L,Animation->GetName().c_str());
	} else {
		lua_getmetatable(L,1);
		lua_pushvalue(L,2);
		lua_gettable(L,-2);
		if (lua_isnil(L,-1))
		{
			lua_pop(L,1);
			//lua_getref(L,Animation->Reference);
			lua_rawgeti(L,LUA_REGISTRYINDEX,Animation->Reference);
			lua_pushvalue(L,2);
			lua_gettable(L,-2);
		}
	}
	return 1;
}

int Animation__newindex(lua_State* L)
{
	NAnimation* Animation = lua_toAnimation(L,1);
	if (Animation == NULL)
	{
		lua_Debug ar1;
		lua_getstack(L,1,&ar1);
		lua_getinfo(L,"fl",&ar1);
		lua_Debug ar2;
		lua_getinfo(L,">S",&ar2);
		lua_pushfstring(L, "%s:%d: attempt to index a NULL Animation", ar2.short_src, ar1.currentline);
		return lua_error(L);
	}
	const char* Field = luaL_checkstring(L,2);
	if (!strcmp(Field,"FPS"))
	{
		Animation->FPS = luaL_checknumber(L,3);
	} else if (!strcmp(Field,"Name"))
	{
		Animation->SetName(luaL_checkstring(L,3));
	} else {
		//lua_getref(L,Animation->Reference);
		lua_rawgeti(L,LUA_REGISTRYINDEX,Animation->Reference);
		lua_pushvalue(L,3);
		lua_setfield(L,-2,luaL_checkstring(L,2));
	}
	return 0;
}

int LoadTexture(lua_State* L)
{
	unsigned int ArgCount = lua_gettop(L);
	NTexture* Texture = new NTexture(luaL_checkstring(L,1));
	for (unsigned int i=2;i<1+ArgCount;i++)
	{
		Texture->AddAnimation(lua_checkAnimation(L,i));
	}
	GetGame()->GetRender()->AddTexture(Texture);
	return 0;
}

int LoadSound(lua_State* L)
{
    const char* Name = luaL_checkstring(L,1);
    const char* DataDir = luaL_checkstring(L,2);
    NSoundData* Data = new NSoundData(Name);
    if (!Data->Load(DataDir))
    {
	delete Data;
	return 0;
    }
    GetGame()->GetSoundSystem()->AddSoundData(Data);
    return 0;
}

int LoadFace(lua_State* L)
{
	const char* Name = luaL_checkstring(L,1);
	const char* Data = luaL_checkstring(L,2);
	NFace* FontFace = new NFace(Name);
	if (FontFace->Load(Data))
	{
		GetGame()->GetTextSystem()->AddFace(FontFace);
	} else {
		delete FontFace;
	}
	return 0;
}
