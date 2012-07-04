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
        {"load",MapLoad},
        {"save",MapSave},
        {"print",Print},
        {NULL,NULL}
    };
    lua_getglobal(L,"_G");
    luaL_register(L, NULL, BaseFunctions);
    lua_pop(L,1);
}

NLua::~NLua()
{
    lua_close(L);
}

int Quit(lua_State* L)
{
    GetGame()->GetLog()->Send("LUA",2,"Recieved a command to exit!");
    GetGame()->Close();
}

int ConsoleHelp(lua_State* L)
{
    lua_getglobal(L,"_G");
    lua_getfield(L,-1,"_VERSION");
    GetGame()->GetLog()->Send("LUA",2,std::string("Hello from ") + luaL_checkstring(L,-1) + "!");
    GetGame()->GetLog()->Send("LUA",2,"This console is ran using lua!");
    GetGame()->GetLog()->Send("LUA",2,"Run any lua commands by typing them here!");
    GetGame()->GetLog()->Send("LUA",2,"Here are some examples:");
    GetGame()->GetLog()->Send("LUA",2,"quit()\t\t\tExits the program.");
    GetGame()->GetLog()->Send("LUA",2,"print(\"Hello world!\")\tPrints 'Hello world!'.");
    GetGame()->GetLog()->Send("LUA",2,"load(\"Map\")\t\tLoads the map maps/Map.map.");
    GetGame()->GetLog()->Send("LUA",2,"save(\"Map\")\t\tSaves the current map to maps/Map.map.");
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
    NFile File = GetGame()->GetFileSystem()->GetFile(FileName,false);
    if (!File.Good())
    {
        GetGame()->GetLog()->Send("LUA",1,std::string("Could not do file ") + FileName + ", it doesn't exist!");
        return false;
    }
    char* FileData = new char[File.Size()+1];
    File.Read(FileData,File.Size());
    FileData[File.Size()] = '\0'; //Ensure the 'string' is null terminated, we could be reading some random file for all we know.
    if (luaL_dostring(L,FileData))
    {
        delete[] FileData;
        GetGame()->GetLog()->Send("LUA",1,lua_tostring(L,-1));
        return Fail;
    }
    delete[] FileData;
    return Success;
}

bool NLua::DoFolder(std::string Folder)
{
    GetGame()->GetLog()->Send("LUA",2,std::string("Scanning ") + Folder + " for lua files...");
    std::string ParentFolder = Folder;
    bool Result = Success;
    std::vector<std::string> Files = GetGame()->GetFileSystem()->GetFiles(Folder);
    for (unsigned int i=0;i<Files.size();i++)
    {
        if (Files[i].find(".lua")!=std::string::npos && Files[i][Files[i].length()-1] == 'a')
        {
            GetGame()->GetLog()->Send("LUA",2,std::string("Executing ") + Files[i] + ".");
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
    GetGame()->GetRender()->AddCachedAnimation(Animation);
    lua_pushAnimation(L,Animation);
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
    } else if (!strcmp(Field,"Filter"))
    {
        GLuint Enum = GetGame()->GetRender()->StringToEnum(luaL_checkstring(L,3));
        for (unsigned int i=0;i<Animation->Frames.size();i++)
        {
            Animation->Frames[i]->SetFilter(Enum);
        }
    } else {
        if (lua_isnumber(L,3))
        {
            Animation->AddNumber(Field,luaL_checknumber(L,3));
        }
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

int MapLoad(lua_State* L)
{
    const char* Name = luaL_checkstring(L,1);
    GetGame()->GetMap()->Load(Name);
    return 0;
}

int MapSave(lua_State* L)
{
    const char* Name = luaL_checkstring(L,1);
    GetGame()->GetMap()->Save(Name);
    return 0;
}

int Print(lua_State* L) //Grabbed this from http://www.lua.org/source/5.1/lbaselib.c.html#luaB_print
{
    std::stringstream Message;
    int n = lua_gettop(L);  /* number of arguments */
    int i;
    lua_getglobal(L, "tostring");
    for (i=1; i<=n; i++) {
        const char *s;
        lua_pushvalue(L, -1);  /* function to be called */
        lua_pushvalue(L, i);   /* value to print */
        lua_call(L, 1, 1);
        s = lua_tostring(L, -1);  /* get result */
        if (s == NULL)
        {
            return luaL_error(L, LUA_QL("tostring") " must return a string to " LUA_QL("print"));
        }
        if (i>1)
        {
            Message << "\t";
        }
        Message << s;
        lua_pop(L, 1);  /* pop result */
    }
    GetGame()->GetLog()->Send("LUA",2,Message.str());
    return 0;
}
