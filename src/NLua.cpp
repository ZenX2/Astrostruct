#include "NEngine.hpp"

NLua::NLua()
{
    L = luaL_newstate();
    //First load in the panic manager
    lua_atpanic(L,OnPanic);
    //Load all lua libraries.
    luaL_openlibs(L);
    //List of all global functions
    static const luaL_Reg BaseFunctions[] =
    {
        {"include",Include},
        {"GetEntitiesByName",LuaGetEntitiesByName},
        {"LoadShader",LoadShader},
        {"help",ConsoleHelp},
        {"quit",Quit},
        {"exit",Quit},
        {"load",MapLoad},
        {"save",MapSave},
        {"print",Print},
        {NULL,NULL}
    };
    //Register the global functions into "_G"lobal
    lua_getglobal(L,"_G");
    luaL_register(L, NULL, BaseFunctions);
    //Create a global bool SERVER and CLIENT so lua knows which one we're running as.
    lua_pushboolean(L,GetGame()->IsServer());
    lua_setfield(L,-2,"SERVER");
    lua_pushboolean(L,!GetGame()->IsServer());
    lua_setfield(L,-2,"CLIENT");
    lua_pop(L,1);

    //Animations and textures
    static const luaL_Reg AniBaseFunctions[] = {
        {"Animation",CreateAnimation},
        {"LoadTexture",LoadTexture},
        {NULL,NULL}
    };
    lua_getglobal(L,"_G");
    luaL_register(L,NULL,AniBaseFunctions);
    lua_pop(L,1);
    luaL_getmetatable(L,"AnimationBase");
    if (lua_isnoneornil(L,-1))
    {
        lua_pop(L,1);
        luaL_newmetatable(L,"AnimationBase");
    }
    static const luaL_Reg AnimationMethods[] = {
        {"__newindex",Animation__newindex},
        {NULL,NULL}
    };
    luaL_register(L,NULL,AnimationMethods);
    lua_pushstring(L,"Animation");
    lua_setfield(L,-2,"__type");
    lua_pop(L,1);

    //Vectors
    static const luaL_Reg VecBaseFunctions[] = {
        {"Vector",CreateVector},
        {NULL,NULL}
    };
    lua_getglobal(L,"_G");
    luaL_register(L,NULL,VecBaseFunctions);
    lua_pop(L,1);
    luaL_getmetatable(L,"VectorBase");
    if (lua_isnoneornil(L,-1))
    {
        lua_pop(L,1);
        luaL_newmetatable(L,"VectorBase");
    }
    static const luaL_Reg VectorMethods[] = {
        {"__index",Vector__index},
        {"__newindex",Vector__newindex},
        {"__sub",Vector__sub},
        {"__add",Vector__add},
        {NULL,NULL}
    };
    luaL_register(L,NULL,VectorMethods);
    lua_pushstring(L,"Vector");
    lua_setfield(L,-2,"__type");
    lua_pop(L,1);

    //Colors
    static const luaL_Reg ColBaseFunctions[] = {
        {"Color",CreateColor},
        {NULL,NULL}
    };
    lua_getglobal(L,"_G");
    luaL_register(L,NULL,ColBaseFunctions);
    lua_pop(L,1);
    luaL_getmetatable(L,"ColorBase");
    if (lua_isnoneornil(L,-1))
    {
        lua_pop(L,1);
        luaL_newmetatable(L,"ColorBase");
    }
    static const luaL_Reg ColorMethods[] = {
        {"__index",Color__index},
        {"__newindex",Color__newindex},
        {"__sub",Color__sub},
        {"__add",Color__add},
        {NULL,NULL}
    };
    luaL_register(L,NULL,ColorMethods);
    lua_pushstring(L,"Color");
    lua_setfield(L,-2,"__type");
    lua_pop(L,1);

    //Tiles
    luaL_getmetatable(L,"Tile");
    if (lua_isnoneornil(L,-1))
    {
        lua_pop(L,1);
        luaL_newmetatable(L,"Tile");
    }
    static const luaL_Reg TileMethods[] = {
        {"__index", Tile__index},
        {"__newindex", Tile__newindex},
        {"GetPos", TileGetPos},
        {NULL,NULL}
    };
    luaL_register(L,NULL,TileMethods);
    lua_pushstring(L,"Tile");
    lua_setfield(L,-2,"__type");
    lua_pop(L,1);

    //Lights
    static const luaL_Reg LightBaseFunctions[] = {
        {"Light",CreateLight},
        {NULL,NULL}
    };
    lua_getglobal(L,"_G");
    luaL_register(L,NULL,LightBaseFunctions);
    lua_pop(L,1);
    luaL_getmetatable(L,"LightBase");
    if (lua_isnoneornil(L,-1))
    {
        lua_pop(L,1);
        luaL_newmetatable(L,"LightBase");
    }
    static const luaL_Reg LightMethods[] = {
        {"__index",Light__index},
        {"__newindex",Light__newindex},
        {"SetPos",LightSetPos},
        {"GetPos",LightGetPos},
        {"SetRadius",LightSetRadius},
        {"SetColor",LightSetColor},
        {"Remove",LightRemove},
        {NULL,NULL}
    };
    luaL_register(L,NULL,LightMethods);
    lua_pushstring(L,"Light");
    lua_setfield(L,-2,"__type");
    lua_pop(L,1);
    DoFile("gamemodes/entity.lua");

    //Map
    luaL_getmetatable(L,"Map");
    if (lua_isnoneornil(L,-1))
    {
        lua_pop(L,1);
        luaL_newmetatable(L,"Map");
    }
    static const luaL_Reg MapMethods[] = {
        {"__index", Map__index},
        {"__newindex", Map__newindex},
        {NULL,NULL}
    };
    luaL_register(L,NULL,MapMethods);
    lua_pushstring(L,"Map");
    lua_setfield(L,-2,"__type");
    lua_pop(L,1);

    //Player
    luaL_getmetatable(L,"Player");
    if (lua_isnoneornil(L,-1))
    {
        lua_pop(L,1);
        luaL_newmetatable(L,"Player");
    }
    static const luaL_Reg PlayerMethods[] = {
        {"__index", Player__index},
        {"__newindex", Player__newindex},
        {"SetPos", PlayerSetPos},
        {NULL,NULL}
    };
    luaL_register(L,NULL,PlayerMethods);
    lua_pushstring(L,"Player");
    lua_setfield(L,-2,"__type");
    lua_pop(L,1);

    //Entity
    luaL_getmetatable(L,"Entity");
    if (lua_isnoneornil(L,-1))
    {
        lua_pop(L,1);
        luaL_newmetatable(L,"Entity");
    }
    static const luaL_Reg EntityMethods[] = {
        {"__index", Entity__index},
        {"__newindex", Entity__newindex},
        {"GetPos", EntityGetPos},
        {"SetPos", EntitySetPos},
        {NULL,NULL}
    };
    luaL_register(L,NULL,EntityMethods);
    lua_pushstring(L,"Entity");
    lua_setfield(L,-2,"__type");
    lua_pop(L,1);

    //PhysicsObject
    /*luaL_getmetatable(L,"PhysicsObject");
    if (lua_isnoneornil(L,-1))
    {
        lua_pop(L,1);
        luaL_newmetatable(L,"PhysicsObject");
    }
    static const luaL_Reg PhysicsObjectMethods[] = {
        {"__index", PhysicsObject__index},
        {"__newindex", PhysicsObject__newindex},
        {"GetPos", PhysicsObjectGetPos},
        {"SetPos", PhysicsObjectSetPos},
        {NULL,NULL}
    };
    luaL_register(L,NULL,PhysicsObjectMethods);
    lua_pushstring(L,"PhysicsObject");
    lua_setfield(L,-2,"__type");
    lua_pop(L,1);*/
}

NLua::~NLua()
{
    lua_close(L);
}

int Quit(lua_State* L)
{
    GetGame()->GetLog()->Send("LUA",2,"Recieved a command to exit!");
    GetGame()->Close();
    return 0;
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
    return 0;
}

int Include(lua_State* L)
{
    std::string File = luaL_checkstring(L,1);
    std::string FullPath = GetGame()->GetLua()->GetCurrentDoFile();
    std::string Path = FullPath.substr(0,FullPath.find_last_of('/')+1);
    GetGame()->GetLua()->DoFile(Path+File);
    //Make sure we reset the file we're doing in case someone wants to include more than one file.
    GetGame()->GetLua()->SetCurrentDoFile(FullPath);
    //This stuff was made by amcfaggot which doesn't work when my filesystem is virtualized.
    /*lua_Debug ar1;
    lua_getstack(L,1,&ar1);
    lua_getinfo(L,"f",&ar1);
    lua_Debug ar2;
    lua_getinfo(L,">S",&ar2);
    std::string Source = ar2.source;
    int Found = Source.find_last_of("/\\");
    std::string Folder = Source.substr(1,Found);
    std::string File = luaL_checkstring(L,1);
    GetGame()->GetLua()->DoFile(Folder+"/"+File);*/
    return 0;
}

void NLua::SetCurrentDoFile(std::string File)
{
    CurrentDoFile = File;
}
std::string NLua::GetCurrentDoFile()
{
    return CurrentDoFile;
}

int luaL_nloadstring(lua_State* L, const char* s, const char* name)
{
    return luaL_loadbuffer(L, s, strlen(s), name);
}

bool NLua::DoFile(std::string FileName)
{
    //Set the current do file so our lua Include will know what to include.
    SetCurrentDoFile(FileName);
    NReadFile File = GetGame()->GetFileSystem()->GetReadFile(FileName);
    if (!File.Good())
    {
        GetGame()->GetLog()->Send("LUA",1,std::string("Could not do file ") + FileName + ", it doesn't exist!");
        return false;
    }
    char* FileData = new char[File.Size()+1];
    File.Read(FileData,File.Size());
    FileData[File.Size()] = '\0'; //Ensure the 'string' is null terminated, we could be reading some random file for all we know.
    //Use our special string loading function to associate the code with our file.
    if (luaL_ndostring(L,FileData,FileName.c_str()))
    {
        delete[] FileData;
        //Parse the error and replace the string with the file path.
        std::string Error = lua_tostring(L,-1);
        std::string RealError = Error.substr(Error.find(']')+1);
        GetGame()->GetLog()->Send("LUA",1,"[file \""+FileName+"\"]"+RealError);
        return 1;
    }
    delete[] FileData;
    return 0;
}

bool NLua::DoFolder(std::string Folder)
{
    GetGame()->GetLog()->Send("LUA",2,std::string("Scanning ") + Folder + " for lua files...");
    std::string ParentFolder = Folder;
    bool Result = 0;
    std::vector<std::string> Files = GetGame()->GetFileSystem()->GetFiles(Folder);
    for (unsigned int i=0;i<Files.size();i++)
    {
        //executes all files that has .lua in them and has a at the end.
        if (Files[i].find(".lua")!=std::string::npos && Files[i][Files[i].length()-1] == 'a')
        {
            //GetGame()->GetLog()->Send("LUA",2,std::string("Executing ") + Files[i] + ".");
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
    if (Data->Load(DataDir))
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
    if (!FontFace->Load(Data))
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

NTile* lua_toTile(lua_State* L, int index)
{
    NTile** Tile = (NTile**)luaL_checkudata(L,index,"Tile");
    return *Tile;
}

NTile* lua_checkTile(lua_State* L, int narg)
{
    NTile* Foo = lua_toTile(L,narg);
    if (Foo == NULL)
    {
        luaL_argerror(L,narg,"attempt to index a NULL Tile");
    }
    return Foo;
}

int Tile__index(lua_State* L)
{
    NTile* Foo = lua_toTile(L,1);
    if (Foo == NULL)
    {
        lua_Debug ar1;
        lua_getstack(L,1,&ar1);
        lua_getinfo(L,"fl",&ar1);
        lua_Debug ar2;
        lua_getinfo(L,">S",&ar2);
        lua_pushfstring(L, "%s:%d: attempt to index a NULL Tile", ar2.short_src, ar1.currentline);
        return lua_error(L);
    }
    const char* Field = luaL_checkstring(L,2);
    if (!strcmp(Field,"ID"))
    {
        lua_pushnumber(L,Foo->ID);
    } else {
        lua_getmetatable(L,1);
        lua_pushvalue(L,2);
        lua_gettable(L,-2);
        if (lua_isnil(L,-1))
        {
            lua_pop(L,1);
            //lua_getref(L,Foo->SelfReference);
            lua_rawgeti(L,LUA_REGISTRYINDEX,Foo->SelfReference);
            lua_pushvalue(L,2);
            lua_gettable(L,-2);
        }
    }
    return 1;
}

int Tile__newindex(lua_State* L)
{
    NTile* Foo = lua_toTile(L,1);
    if (Foo == NULL)
    {
        lua_Debug ar1;
        lua_getstack(L,1,&ar1);
        lua_getinfo(L,"fl",&ar1);
        lua_Debug ar2;
        lua_getinfo(L,">S",&ar2);
        lua_pushfstring(L, "%s:%d: attempt to index a NULL Tile", ar2.short_src, ar1.currentline);
        return lua_error(L);
    }
    //const char* Field = luaL_checkstring(L,2);
    lua_rawgeti(L,LUA_REGISTRYINDEX,Foo->SelfReference);
    lua_pushvalue(L,3);
    lua_setfield(L,-2,luaL_checkstring(L,2));
    return 0;
}
int TileGetPos(lua_State* L)
{
    NTile* Foo = lua_checkTile(L,1);
    float TileSize = GetGame()->GetMap()->GetTileSize();
    lua_pushVector(L, glm::vec3(Foo->X*TileSize + TileSize/2.f,Foo->Y*TileSize+TileSize/2.f,Foo->Z*TileSize+TileSize/2.f));
    return 1;
}

void lua_pushTile(lua_State* L, NTile* Tile)
{
    NTile** Pointer = (NTile**)lua_newuserdata(L,sizeof(NTile*));
    *Pointer = Tile;
    luaL_getmetatable(L,"Tile");
    lua_setmetatable(L,-2);
}

int CreateLight(lua_State* L)
{
    NLight* Light = new NLight(luaL_checkstring(L,1));
    lua_pushLight(L,Light);
    return 1;
}

int Light__index(lua_State* L)
{
    NLight* Foo = lua_toLight(L,1);
    if (Foo == NULL)
    {
        lua_Debug ar1;
        lua_getstack(L,1,&ar1);
        lua_getinfo(L,"fl",&ar1);
        lua_Debug ar2;
        lua_getinfo(L,">S",&ar2);
        lua_pushfstring(L, "%s:%d: attempt to index a NULL Light", ar2.short_src, ar1.currentline);
        return lua_error(L);
    }
    lua_getmetatable(L,1);
    lua_pushvalue(L,2);
    lua_gettable(L,-2);
    if (lua_isnil(L,-1))
    {
        lua_pop(L,1);
        return 0;
    }
    return 1;
}

int Light__newindex(lua_State* L)
{
    NLight* Foo = lua_toLight(L,1);
    if (Foo == NULL)
    {
        lua_Debug ar1;
        lua_getstack(L,1,&ar1);
        lua_getinfo(L,"fl",&ar1);
        lua_Debug ar2;
        lua_getinfo(L,">S",&ar2);
        lua_pushfstring(L, "%s:%d: attempt to index a NULL Light", ar2.short_src, ar1.currentline);
        return lua_error(L);
    }
    /*const char* Field = luaL_checkstring(L,2);
    lua_getref(L,Foo->LuaReference);
    lua_pushvalue(L,3);
    lua_setfield(L,-2,luaL_checkstring(L,2));*/
    //todo: hook up light to lua
    return 0;
}

NLight* lua_toLight(lua_State* L, int index)
{
    NLight** Light = (NLight**)luaL_checkudata(L,index,"LightBase");
    return *Light;
}

NLight* lua_checkLight(lua_State* L, int narg)
{
    NLight* Foo = lua_toLight(L,narg);
    if (Foo == NULL)
    {
        luaL_argerror(L,narg,"attempt to index a NULL Light");
    }
    return Foo;
}

int LightSetRadius(lua_State* L)
{
    NLight* Foo = lua_checkLight(L,1);
    float Rad = luaL_checknumber(L,2);
    Foo->SetScale(glm::vec3(Rad,Rad,1.0));
    return 0;
}

int LightSetPos(lua_State* L)
{
    NLight* Foo = lua_checkLight(L,1);
    glm::vec3* Bar = lua_checkVector(L,2);
    Foo->SetPos(*Bar);
    return 0;
}

int LightGetPos(lua_State* L)
{
    NLight* Foo = lua_checkLight(L,1);
    lua_pushVector(L,Foo->GetPos());
    return 1;
}

int LightSetColor(lua_State* L)
{
    NLight* Foo = lua_checkLight(L,1);
    glm::vec4* Bar = lua_checkColor(L,2);
    Foo->SetColor(*Bar);
    return 0;
}

void lua_pushLight(lua_State* L, NLight* Light)
{
    NLight** Pointer = (NLight**)lua_newuserdata(L,sizeof(NLight*));
    *Pointer = Light;
    luaL_getmetatable(L,"LightBase");
    lua_setmetatable(L,-2);
}

int LightRemove(lua_State* L)
{
    NLight* Foo = lua_checkLight(L,1);
    GetGame()->GetScene()->Remove(Foo);
    return 0;
}
int CreateVector(lua_State* L)
{
    lua_pushVector(L,glm::vec3(luaL_checknumber(L,1),luaL_checknumber(L,2),luaL_checknumber(L,3)));
    return 1;
}
int Vector__sub(lua_State* L)
{
    glm::vec3* Foo = lua_checkVector(L,1);
    glm::vec3* Bar = lua_checkVector(L,2);
    lua_pushVector(L,*Foo-*Bar);
    return 1;
}
int Vector__add(lua_State* L)
{
    glm::vec3* Foo = lua_checkVector(L,1);
    glm::vec3* Bar = lua_checkVector(L,2);
    lua_pushVector(L,*Foo+*Bar);
    return 1;
}
glm::vec3* lua_toVector(lua_State* L, int index)
{
    glm::vec3* Vector = (glm::vec3*)luaL_checkudata(L,index,"VectorBase");
    return Vector;
}

void lua_pushVector(lua_State* L, glm::vec3 Foo)
{
    glm::vec3* Pointer = (glm::vec3*)lua_newuserdata(L,sizeof(glm::vec3));
    *Pointer = Foo;
    luaL_getmetatable(L,"VectorBase");
    lua_setmetatable(L,-2);
}

int Vector__index(lua_State* L)
{
    glm::vec3* Foo = lua_toVector(L,1);
    if (Foo == NULL)
    {
        lua_Debug ar1;
        lua_getstack(L,1,&ar1);
        lua_getinfo(L,"fl",&ar1);
        lua_Debug ar2;
        lua_getinfo(L,">S",&ar2);
        lua_pushfstring(L, "%s:%d: attempt to index a NULL Vector", ar2.short_src, ar1.currentline);
        return lua_error(L);
    }
    const char* Field = luaL_checkstring(L,2);
    if (!strcmp(Field,"x"))
    {
        lua_pushnumber(L,Foo->x);
    } else if (!strcmp(Field,"y"))
    {
        lua_pushnumber(L,Foo->y);
    } else if (!strcmp(Field,"z"))
    {
        lua_pushnumber(L,Foo->z);
    } else {
        return 0;
    }
    return 1;
}

int Vector__newindex(lua_State* L)
{
    glm::vec3* Foo = lua_toVector(L,1);
    if (Foo == NULL)
    {
        lua_Debug ar1;
        lua_getstack(L,1,&ar1);
        lua_getinfo(L,"fl",&ar1);
        lua_Debug ar2;
        lua_getinfo(L,">S",&ar2);
        lua_pushfstring(L, "%s:%d: attempt to index a NULL Vector", ar2.short_src, ar1.currentline);
        return lua_error(L);
    }
    const char* Field = luaL_checkstring(L,2);
    if (!strcmp(Field,"x"))
    {
        Foo->x = luaL_checknumber(L,3);
    } else if (!strcmp(Field,"y"))
    {
        Foo->y = luaL_checknumber(L,3);
    } else if (!strcmp(Field,"z"))
    {
        Foo->z = luaL_checknumber(L,3);
    }
    return 0;
}

glm::vec3* lua_checkVector(lua_State* L, int narg)
{
    glm::vec3* Foo = lua_toVector(L,narg);
    if (Foo == NULL)
    {
        luaL_argerror(L,narg,"attempt to index a NULL Vector");
    }
    return Foo;
}

int CreateColor(lua_State* L)
{
    lua_pushColor(L,glm::vec4(luaL_checknumber(L,1),luaL_checknumber(L,2),luaL_checknumber(L,3),luaL_checknumber(L,4)));
    return 1;
}
int Color__sub(lua_State* L)
{
    glm::vec4* Foo = lua_checkColor(L,1);
    glm::vec4* Bar = lua_checkColor(L,2);
    lua_pushColor(L,*Foo-*Bar);
    return 1;
}
int Color__add(lua_State* L)
{
    glm::vec4* Foo = lua_checkColor(L,1);
    glm::vec4* Bar = lua_checkColor(L,2);
    lua_pushColor(L,*Foo+*Bar);
    return 1;
}
glm::vec4* lua_toColor(lua_State* L, int index)
{
    glm::vec4* Color = (glm::vec4*)luaL_checkudata(L,index,"ColorBase");
    return Color;
}

void lua_pushColor(lua_State* L, glm::vec4 Color)
{
    glm::vec4* Pointer = (glm::vec4*)lua_newuserdata(L,sizeof(glm::vec4));
    *Pointer = Color;
    luaL_getmetatable(L,"ColorBase");
    lua_setmetatable(L,-2);
}

int Color__index(lua_State* L)
{
    glm::vec4* Foo = lua_toColor(L,1);
    if (Foo == NULL)
    {
        lua_Debug ar1;
        lua_getstack(L,1,&ar1);
        lua_getinfo(L,"fl",&ar1);
        lua_Debug ar2;
        lua_getinfo(L,">S",&ar2);
        lua_pushfstring(L, "%s:%d: attempt to index a NULL Color", ar2.short_src, ar1.currentline);
        return lua_error(L);
    }
    const char* Field = luaL_checkstring(L,2);
    if (!strcmp(Field,"r"))
    {
        lua_pushnumber(L,Foo->x);
    } else if (!strcmp(Field,"g"))
    {
        lua_pushnumber(L,Foo->y);
    } else if (!strcmp(Field,"b"))
    {
        lua_pushnumber(L,Foo->z);
    } else if (!strcmp(Field,"a"))
    {
        lua_pushnumber(L,Foo->w);
    } else {
        return 0;
    }
    return 1;
}

int Color__newindex(lua_State* L)
{
    glm::vec4* Foo = lua_toColor(L,1);
    if (Foo == NULL)
    {
        lua_Debug ar1;
        lua_getstack(L,1,&ar1);
        lua_getinfo(L,"fl",&ar1);
        lua_Debug ar2;
        lua_getinfo(L,">S",&ar2);
        lua_pushfstring(L, "%s:%d: attempt to index a NULL Color", ar2.short_src, ar1.currentline);
        return lua_error(L);
    }
    const char* Field = luaL_checkstring(L,2);
    if (!strcmp(Field,"r"))
    {
        Foo->x = luaL_checknumber(L,3);
    } else if (!strcmp(Field,"g"))
    {
        Foo->y = luaL_checknumber(L,3);
    } else if (!strcmp(Field,"b"))
    {
        Foo->z = luaL_checknumber(L,3);
    } else if (!strcmp(Field,"a"))
    {
        Foo->w = luaL_checknumber(L,3);
    }
    return 0;
}

glm::vec4* lua_checkColor(lua_State* L, int narg)
{
    glm::vec4* Foo = lua_toColor(L,narg);
    if (Foo == NULL)
    {
        luaL_argerror(L,narg,"attempt to index a NULL Color");
    }
    return Foo;
}


NMap* lua_toMap(lua_State* L, int index)
{
    NMap** Map = (NMap**)luaL_checkudata(L,index,"Map");
    return *Map;
}

NMap* lua_checkMap(lua_State* L, int narg)
{
    NMap* Foo = lua_toMap(L,narg);
    if (Foo == NULL)
    {
        luaL_argerror(L,narg,"attempt to index a NULL Map");
    }
    return Foo;
}

int Map__index(lua_State* L)
{
    NMap* Foo = lua_toMap(L,1);
    if (Foo == NULL)
    {
        lua_Debug ar1;
        lua_getstack(L,1,&ar1);
        lua_getinfo(L,"fl",&ar1);
        lua_Debug ar2;
        lua_getinfo(L,">S",&ar2);
        lua_pushfstring(L, "%s:%d: attempt to index a NULL Map", ar2.short_src, ar1.currentline);
        return lua_error(L);
    }
    const char* Field = luaL_checkstring(L,2);
    if (!strcmp(Field,"TileSize"))
    {
        lua_pushnumber(L,Foo->GetTileSize());
    } else {
        lua_getmetatable(L,1);
        lua_pushvalue(L,2);
        lua_gettable(L,-2);
        if (lua_isnil(L,-1))
        {
            lua_pop(L,1);
            //lua_getref(L,Foo->SelfReference);
            lua_rawgeti(L,LUA_REGISTRYINDEX,Foo->SelfReference);
            lua_pushvalue(L,2);
            lua_gettable(L,-2);
        }
    }
    return 1;
}

int Map__newindex(lua_State* L)
{
    NMap* Foo = lua_toMap(L,1);
    if (Foo == NULL)
    {
        lua_Debug ar1;
        lua_getstack(L,1,&ar1);
        lua_getinfo(L,"fl",&ar1);
        lua_Debug ar2;
        lua_getinfo(L,">S",&ar2);
        lua_pushfstring(L, "%s:%d: attempt to index a NULL Map", ar2.short_src, ar1.currentline);
        return lua_error(L);
    }
    //const char* Field = luaL_checkstring(L,2);
    lua_rawgeti(L,LUA_REGISTRYINDEX,Foo->SelfReference);
    lua_pushvalue(L,3);
    lua_setfield(L,-2,luaL_checkstring(L,2));
    return 0;
}
void lua_pushMap(lua_State* L, NMap* Map)
{
    NMap** Pointer = (NMap**)lua_newuserdata(L,sizeof(NMap*));
    *Pointer = Map;
    luaL_getmetatable(L,"Map");
    lua_setmetatable(L,-2);
}

NPlayer* lua_toPlayer(lua_State* L, int index)
{
    NPlayer** Player = (NPlayer**)luaL_checkudata(L,index,"Player");
    return *Player;
}

NPlayer* lua_checkPlayer(lua_State* L, int narg)
{
    NPlayer* Foo = lua_toPlayer(L,narg);
    if (Foo == NULL)
    {
        luaL_argerror(L,narg,"attempt to index a NULL Player");
    }
    return Foo;
}

int Player__index(lua_State* L)
{
    NPlayer* Foo = lua_toPlayer(L,1);
    if (Foo == NULL)
    {
        lua_Debug ar1;
        lua_getstack(L,1,&ar1);
        lua_getinfo(L,"fl",&ar1);
        lua_Debug ar2;
        lua_getinfo(L,">S",&ar2);
        lua_pushfstring(L, "%s:%d: attempt to index a NULL Player", ar2.short_src, ar1.currentline);
        return lua_error(L);
    }
    const char* Field = luaL_checkstring(L,2);
    if (!strcmp(Field,"Name"))
    {
        lua_pushstring(L,Foo->GetName().c_str());
    } else {
        lua_getmetatable(L,1);
        lua_pushvalue(L,2);
        lua_gettable(L,-2);
        if (lua_isnil(L,-1))
        {
            lua_pop(L,1);
            //lua_getref(L,Foo->SelfReference);
            lua_rawgeti(L,LUA_REGISTRYINDEX,Foo->SelfReference);
            lua_pushvalue(L,2);
            lua_gettable(L,-2);
        }
    }
    return 1;
}

int Player__newindex(lua_State* L)
{
    NPlayer* Foo = lua_toPlayer(L,1);
    if (Foo == NULL)
    {
        lua_Debug ar1;
        lua_getstack(L,1,&ar1);
        lua_getinfo(L,"fl",&ar1);
        lua_Debug ar2;
        lua_getinfo(L,">S",&ar2);
        lua_pushfstring(L, "%s:%d: attempt to index a NULL Player", ar2.short_src, ar1.currentline);
        return lua_error(L);
    }
    //const char* Field = luaL_checkstring(L,2);
    lua_rawgeti(L,LUA_REGISTRYINDEX,Foo->SelfReference);
    lua_pushvalue(L,3);
    lua_setfield(L,-2,luaL_checkstring(L,2));
    return 0;
}
void lua_pushPlayer(lua_State* L, NPlayer* Player)
{
    NPlayer** Pointer = (NPlayer**)lua_newuserdata(L,sizeof(NPlayer*));
    *Pointer = Player;
    luaL_getmetatable(L,"Player");
    lua_setmetatable(L,-2);
}

NEntity* lua_toEntity(lua_State* L, int index)
{
    NEntity** Entity = (NEntity**)luaL_checkudata(L,index,"Entity");
    return *Entity;
}

NEntity* lua_checkEntity(lua_State* L, int narg)
{
    NEntity* Foo = lua_toEntity(L,narg);
    if (Foo == NULL)
    {
        luaL_argerror(L,narg,"attempt to index a NULL Entity");
    }
    return Foo;
}

int Entity__index(lua_State* L)
{
    NEntity* Foo = lua_toEntity(L,1);
    if (Foo == NULL)
    {
        lua_Debug ar1;
        lua_getstack(L,1,&ar1);
        lua_getinfo(L,"fl",&ar1);
        lua_Debug ar2;
        lua_getinfo(L,">S",&ar2);
        lua_pushfstring(L, "%s:%d: attempt to index a NULL Entity", ar2.short_src, ar1.currentline);
        return lua_error(L);
    }
    //const char* Field = luaL_checkstring(L,2);
    //if (!strcmp(Field,"Name"))
    //{
    //    lua_pushstring(L,Foo->GetName().c_str());
    //} else {
        lua_getmetatable(L,1);
        lua_pushvalue(L,2);
        lua_gettable(L,-2);
        if (lua_isnil(L,-1))
        {
            lua_pop(L,1);
            //lua_getref(L,Foo->SelfReference);
            lua_rawgeti(L,LUA_REGISTRYINDEX,Foo->SelfReference);
            lua_pushvalue(L,2);
            lua_gettable(L,-2);
        }
    //}
    return 1;
}

int Entity__newindex(lua_State* L)
{
    NEntity* Foo = lua_toEntity(L,1);
    if (Foo == NULL)
    {
        lua_Debug ar1;
        lua_getstack(L,1,&ar1);
        lua_getinfo(L,"fl",&ar1);
        lua_Debug ar2;
        lua_getinfo(L,">S",&ar2);
        lua_pushfstring(L, "%s:%d: attempt to index a NULL Entity", ar2.short_src, ar1.currentline);
        return lua_error(L);
    }
    //const char* Field = luaL_checkstring(L,2);
    lua_rawgeti(L,LUA_REGISTRYINDEX,Foo->SelfReference);
    lua_pushvalue(L,3);
    lua_setfield(L,-2,luaL_checkstring(L,2));
    return 0;
}
void lua_pushEntity(lua_State* L, NEntity* Entity)
{
    NEntity** Pointer = (NEntity**)lua_newuserdata(L,sizeof(NEntity*));
    *Pointer = Entity;
    luaL_getmetatable(L,"Entity");
    lua_setmetatable(L,-2);
}

int EntityGetPos(lua_State* L)
{
    NEntity* Foo = lua_checkEntity(L,1);
    lua_pushVector(L,Foo->GetPos());
    return 1;
}
int EntitySetPos(lua_State* L)
{
    NEntity* Foo = lua_checkEntity(L,1);
    glm::vec3* Bar = lua_checkVector(L,2);
    Foo->SetPos(*Bar);
    return 0;
}

int LuaGetEntitiesByName(lua_State* L)
{
    const char* Name = luaL_checkstring(L,1);
    lua_newtable(L);
    std::vector<NNode*> Nodes = GetGame()->GetScene()->GetNodesByType(NodeEntity);
    int Size = 0;
    for (unsigned int i=0;i<Nodes.size();i++)
    {
        NEntity* Ent = (NEntity*)Nodes[i];
        if (Ent->GetName() == Name)
        {
            lua_pushEntity(L,Ent);
            lua_rawseti(L,-2,Size+1);
            Size++;
        }
    }
    lua_pushinteger(L,Size);
    return 2;
}

int PlayerSetPos(lua_State* L)
{
    NPlayer* Foo = lua_checkPlayer(L,1);
    glm::vec3* Bar = lua_checkVector(L,2);
    Foo->SetPos(*Bar);
    return 0;
}

int OnPanic(lua_State* L)
{
    GetGame()->GetLog()->Send("LUA",0,lua_tostring(L,-1));
    return 0;
}

int lua_protcall(lua_State* L, int nargs, int nresults)
{
    if (lua_pcall(L,nargs,nresults,0))
    {
        GetGame()->GetLog()->Send("LUA",1,lua_tostring(L,-1));
        return 1;
    }
    return 0;
}

std::wstring NLua::Translate(LanguageElement Foo)
{
    if (Language.length() == 0)
    {
        Language = GetGame()->GetConfig()->GetString("Language");
        GetGame()->GetLog()->Send("LUA",2,"Using language " + Language + ".");
    }
    lua_getglobal(L,"_G");
    lua_getfield(L,-1,Language.c_str());
    if (!lua_istable(L,-1))
    {
        lua_pop(L,2);
        return _t("NULL");
    }
    lua_pushnumber(L,(float)Foo);
    lua_gettable(L,-2);
    if (!lua_isstring(L,-1))
    {
        lua_pop(L,3);
        return _t("NULL");
    }
    const char* String = lua_tostring(L,-1);
    lua_pop(L,3);
    return ToMBS(String);
}

int LoadShader(lua_State* L)
{
    NShader* Shader = new NShader(luaL_checkstring(L,1));
    std::string File = luaL_checkstring(L,2);
    if (!Shader->Load(File+".vert",File+".frag"))
    {
        GetGame()->GetRender()->AddShader(Shader);
    } else {
        delete Shader;
    }
    return 0;
}
