#include "NEngine.hpp"

NLuaEntity::NLuaEntity(std::string i_Name, int i_LuaReference)
{
    Name = i_Name;
    LuaReference = i_LuaReference;
}

NEntityManager::NEntityManager()
{
    std::string Gamemode = GetGame()->GetConfig()->GetString("GameMode");
    std::vector<std::string> EntityList = GetGame()->GetFileSystem()->GetFiles("gamemodes/"+Gamemode+"/entities");
    for (unsigned int i=0;i<EntityList.size();i++)
    {
        if (GetGame()->GetFileSystem()->IsDir(EntityList[i].c_str()))
        {
            std::string File = EntityList[i]+"/";
            //If we're a server, run init.lua. Otherwise run the cl_init.lua
            if (GetGame()->IsServer())
            {
                File += "init.lua";
            } else {
                File += "cl_init.lua";
            }
            lua_State* L = GetGame()->GetLua()->GetL();
            lua_newtable(L);
            lua_setglobal(L, "ENT");
            if (GetGame()->GetLua()->DoFile(File))
            {
                //If we failed, reset the global and abort.
                lua_pushnil(L);
                lua_setglobal(L,"ENT");
                continue;
            }
            //Right now i'll just include tiles into the entities table. This could cause problems later on if entities have the same names as tiles.
            lua_getglobal(L,"entity");
            if (!lua_istable(L,-1))
            {
                //If we haven't loaded in the entity module, reset the global and abort.
                lua_pop(L,1);
                lua_pushnil(L);
                lua_setglobal(L,"ENT");
                continue;
            }
            lua_getfield(L,-1,"register");
            if (!lua_isfunction(L,-1))
            {
                //If our register function doesn't exist, abort!
                lua_pop(L,2);
                lua_pushnil(L);
                lua_setglobal(L,"ENT");
                continue;
            }
            lua_remove(L,-2);
            lua_getglobal(L,"ENT");
            std::string EntityName = EntityList[i].substr(EntityList[i].find_last_of('/')+1);
            lua_pushstring(L,EntityName.c_str());
            //Finally run the register function on our new entity.
            lua_pcall(L,2,0,0);
            lua_pushnil(L);
            lua_setglobal(L,"ENT");
            lua_getglobal(L,"entity");
            if (!lua_istable(L,-1))
            {
                lua_pop(L,1);
                continue;
            }
            lua_getfield(L,-1,"get");
            if (!lua_isfunction(L,-1))
            {
                lua_pop(L,2);
                continue;
            }
            lua_remove(L,-2);
            lua_pushstring(L,EntityName.c_str());
            lua_pcall(L,1,1,0);
            NLuaEntity* Ent = new NLuaEntity(EntityName,luaL_ref(L, LUA_REGISTRYINDEX));
            Entities.push_back(Ent);
            GetGame()->GetLog()->Send("LUA",2,"Loaded entity: "+EntityName);
        }
    }
}

//Exact same as the constructor, just makes sure to clean after itself.
void NEntityManager::Reload()
{
    for (unsigned int i=0;i<Entities.size();i++)
    {
        delete Entities[i];
    }
    Entities.clear();
    std::string Gamemode = GetGame()->GetConfig()->GetString("GameMode");
    std::vector<std::string> EntityList = GetGame()->GetFileSystem()->GetFiles("gamemodes/"+Gamemode+"/entities");
    for (unsigned int i=0;i<EntityList.size();i++)
    {
        if (GetGame()->GetFileSystem()->IsDir(EntityList[i].c_str()))
        {
            std::string File = EntityList[i]+"/";
            if (GetGame()->IsServer())
            {
                File += "init.lua";
            } else {
                File += "cl_init.lua";
            }
            lua_State* L = GetGame()->GetLua()->GetL();
            lua_newtable(L);
            lua_setglobal(L, "ENT");
            if (GetGame()->GetLua()->DoFile(File))
            {
                //If we failed, reset the global and abort.
                lua_pushnil(L);
                lua_setglobal(L,"ENT");
                continue;
            }
            //Right now i'll just include tiles into the entities table. This could cause problems later on if entities have the same names as tiles.
            lua_getglobal(L,"entity");
            if (!lua_istable(L,-1))
            {
                //If we haven't loaded in the entity module, reset the global and abort.
                lua_pop(L,1);
                lua_pushnil(L);
                lua_setglobal(L,"ENT");
                continue;
            }
            lua_getfield(L,-1,"register");
            if (!lua_isfunction(L,-1))
            {
                //If our register function doesn't exist, create it!
                lua_pop(L,2);
                lua_pushnil(L);
                lua_setglobal(L,"ENT");
                continue;
            }
            lua_remove(L,-2);
            lua_getglobal(L,"ENT");
            std::string EntityName = EntityList[i].substr(EntityList[i].find_last_of('/')+1);
            lua_pushstring(L,EntityName.c_str());
            lua_pcall(L,2,0,0);
            lua_pushnil(L);
            lua_setglobal(L,"ENT");
            lua_getglobal(L,"entity");
            if (!lua_istable(L,-1))
            {
                lua_pop(L,1);
                continue;
            }
            lua_getfield(L,-1,"get");
            if (!lua_isfunction(L,-1))
            {
                lua_pop(L,2);
                continue;
            }
            lua_remove(L,-2);
            lua_pushstring(L,EntityName.c_str());
            lua_pcall(L,1,1,0);
            NLuaEntity* Ent = new NLuaEntity(EntityName,luaL_ref(L, LUA_REGISTRYINDEX));
            Entities.push_back(Ent);
            GetGame()->GetLog()->Send("LUA",2,"Loaded entity: "+EntityName);
        }
    }
}

int NEntityManager::GetLuaEntity(std::string Name)
{
    for (unsigned int i=0;i<Entities.size();i++)
    {
        if (Entities[i]->Name == Name)
        {
            return Entities[i]->LuaReference;
        }
    }
    return LUA_NOREF;
}

NEntityManager::~NEntityManager()
{
    for (unsigned int i=0;i<Entities.size();i++)
    {
        delete Entities[i];
    }
}

NEntity::NEntity(std::string i_Name) : NNode(NodeEntity)
{
    Name = i_Name;
    lua_State* L = GetGame()->GetLua()->GetL();
    //Create a new table to store our per-entity variables.
    lua_newtable(L);
    SelfReference = luaL_ref(L,LUA_REGISTRYINDEX);
    //Grab our already-loaded entity from our entity manager.
    LuaSelf = GetGame()->GetEntityManager()->GetLuaEntity(Name);
    Changed = true;
    Friction = 2;
    if (GetGame()->IsServer())
    {
        return;
    }
    glGenBuffers(2,Buffers);
    //Try to grab a texture to display from the lua
    std::string Tex = GetString("Texture");
    if (Tex != "NULL")
    {
        //Hey we got a match, try to load it.
        Texture = GetGame()->GetRender()->GetTexture(Tex);
    } else {
        //Can't do shit captain
        Texture = NULL;
    }
    float W = GetFloat("Width");
    float H = GetFloat("Height");
    //If no width or height is specified, assume the size of texture.
    if (W == 0 && H == 0)
    {
        if (Texture)
        {
            SetScale(glm::vec3(Texture->GetSize().x,Texture->GetSize().y,1.f));
            std::cout << GetScale().x;
        }
    } else {
        //If there's no texture or sizes specified, assume 0,0,1!
        SetScale(glm::vec3(W,H,1.f));
    }
    //Load in our shader
    Shader = GetGame()->GetRender()->GetShader("flat3D");
    if (Shader)
    {
        TextureLoc = Shader->GetUniformLocation("Texture");
        MatrixLoc = Shader->GetUniformLocation("Model");
        ColorLoc = Shader->GetUniformLocation("Color");
    }
    //make sure we call our initialization function.

    float TS = GetGame()->GetMap()->GetTileSize();
    bool drop = GetBool("SnapToTile");
    glm::vec3 Pos = GetPos();
    if (drop==true)
    {
        SetPos(glm::vec3(floor(Pos.x/TS)*TS,floor(Pos.y/TS)*TS,floor(Pos.z/TS)*TS));
    }

    float OX = GetFloat("OffsetX");
    float OY = GetFloat("OffsetY");
    float OZ = GetFloat("OffsetZ");
    if (OX == 0 && OY == 0 && OZ == 0)
    {
        //no point in trying
    } else {
        SetPos(GetPos()+glm::vec3(OX,OY,OZ));
    }

    CallMethod("OnInitialize");
}

NEntity::NEntity(std::string i_Name, glm::vec3 i_Position) : NNode(NodeEntity)
{
    //See above function for comments.
    Name = i_Name;
    lua_State* L = GetGame()->GetLua()->GetL();
    lua_newtable(L);
    SelfReference = luaL_ref(L,LUA_REGISTRYINDEX);
    LuaSelf = GetGame()->GetEntityManager()->GetLuaEntity(Name);
    Changed = true;
    Friction = 2;
    if (GetGame()->IsServer())
    {
        return;
    }
    glGenBuffers(2,Buffers);
    std::string Tex = GetString("Texture");
    if (Tex != "NULL")
    {
        Texture = GetGame()->GetRender()->GetTexture(Tex);
    } else {
        Texture = NULL;
    }
    float W = GetFloat("Width");
    float H = GetFloat("Height");
    if (W == 0 && H == 0)
    {
        if (Texture)
        {
            SetScale(glm::vec3(Texture->GetSize().x,Texture->GetSize().y,1.f));
        }
    } else {
        SetScale(glm::vec3(W,H,1.f));
    }
    Shader = GetGame()->GetRender()->GetShader("flat3D");
    if (Shader)
    {
        TextureLoc = Shader->GetUniformLocation("Texture");
        MatrixLoc = Shader->GetUniformLocation("Model");
        ColorLoc = Shader->GetUniformLocation("Color");
    }
    float TS = GetGame()->GetMap()->GetTileSize();
    bool drop = GetBool("SnapToTile");
    if (drop==true)
    {
        SetPos(glm::vec3(floor(i_Position.x/TS)*TS,floor(i_Position.y/TS)*TS,floor(i_Position.z/TS)*TS));
    }
    else
    {
        SetPos(i_Position);
    }

    float OX = GetFloat("OffsetX");
    float OY = GetFloat("OffsetY");
    float OZ = GetFloat("OffsetZ");
    if (OX == 0 && OY == 0 && OZ == 0)
    {
        //no point in trying
    } else {
        SetPos(GetPos()+glm::vec3(OX,OY,OZ));
    }

    CallMethod("OnInitialize");
}

NEntity::~NEntity()
{
    CallMethod("OnRemove");
    if (Texture)
    {
        delete Texture;
    }
    glDeleteBuffers(2,Buffers);
}

std::string NEntity::GetName()
{
    return Name;
}

void NEntity::Tick(double DT)
{
    CallMethod("OnTick", "f", (float)DT);
    if (!GetGame()->IsServer())
    {
        if (Texture)
        {
            Texture->Tick(DT);
        }
    }
}

void NEntity::GenerateBuffers()
{
    //Just generates a plain quad that's sized at one pixel.
    if (!Changed || !Texture)
    {
        return;
    }
    Verts.clear();
    UVs.clear();
    Verts.push_back(glm::vec2(-.5,-.5));
    UVs.push_back(glm::vec2(0,1));
    Verts.push_back(glm::vec2(-.5,.5));
    UVs.push_back(glm::vec2(0,0));
    Verts.push_back(glm::vec2(.5,.5));
    UVs.push_back(glm::vec2(1,0));
    Verts.push_back(glm::vec2(.5,-.5));
    UVs.push_back(glm::vec2(1,1));
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER,Verts.size()*sizeof(glm::vec2),&Verts[0],GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
    glBufferData(GL_ARRAY_BUFFER,UVs.size()*sizeof(glm::vec2),&UVs[0],GL_STATIC_DRAW);
    Changed = false;
}

void NEntity::Draw(NCamera* View)
{
    if (GetGame()->GetMap()->GetLevel() != GetGame()->GetMap()->GetLevel(GetRealPos()))
    {
        return;
    }

    GenerateBuffers();
    //Make sure we can draw
    if (Texture == NULL || GetColor().w == 0 || Shader == NULL)
    {
        return;
    }
    glUseProgram(Shader->GetID());
    glActiveTexture(GL_TEXTURE0);
    if (Texture != NULL)
    {
        glBindTexture(GL_TEXTURE_2D,Texture->GetID());
    }
    glUniform1i(TextureLoc,0);
    glUniformMatrix4fv(MatrixLoc,1,GL_FALSE,glm::value_ptr(GetModelMatrix()));
    glUniform4fv(ColorLoc,1,&(GetColor()[0]));
    glEnableVertexAttribArray(Shader->GetVertexAttribute());
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
    glVertexAttribPointer(Shader->GetVertexAttribute(),2,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(Shader->GetUVAttribute());
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
    glVertexAttribPointer(Shader->GetUVAttribute(),2,GL_FLOAT,GL_FALSE,0,NULL);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glDrawArrays(GL_QUADS,0,Verts.size());
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glDisableVertexAttribArray(Shader->GetVertexAttribute());
    glDisableVertexAttribArray(Shader->GetUVAttribute());
    glUseProgram(0);
}

void NEntity::Unallocate()
{
    delete (NEntity*)this;
}

void NEntity::CallMethod(std::string VarName, std::string AdditionalVars,  ...)
{
    if (LuaSelf == LUA_NOREF)
    {
        return;
    }
    lua_State* L = GetGame()->GetLua()->GetL();
    lua_rawgeti(L,LUA_REGISTRYINDEX,LuaSelf);
    lua_getfield(L,-1,VarName.c_str());
    //Make sure our function is valid, otherwise abort.
    if (!lua_isfunction(L,-1))
    {
        lua_pop(L,2);
    } else {
        //Push on our entity so we can use self in the scripts.
        lua_pushEntity(L,this);
        va_list vl;
        va_start(vl,AdditionalVars);
        for (unsigned int i=0;i<AdditionalVars.length();i++)
        {
            switch(AdditionalVars[i])
            {
                case 'n':
                {
                    NNode* Temp = va_arg(vl,NNode*);
                    //FIXME: we only accept one kind of additional var right now! Players! :u
                    switch(Temp->GetType())
                    {
                        case NodePlayer: lua_pushPlayer(L,(NPlayer*)Temp); break;
                        default: GetGame()->GetLog()->Send("LUA",0,"Some features not implemented yet, FIXME"); break;
                    }
                    break;
                }
                case 's':
                {
                    lua_pushstring(L,va_arg(vl,char*));
                    break;
                }
                case 'f':
                {
                    lua_pushnumber(L,va_arg(vl,double));
                    break;
                }
                default:
                {
                    GetGame()->GetLog()->Send("LUA",1,std::string("Attempted to push an unkown class with key ") + AdditionalVars[i] + " as a function argument!"); 
                    break;
                }
            }
        }
        lua_protcall(L,1+AdditionalVars.length(),0);
        lua_pop(L,1);
    }
}

std::vector<std::string> NEntityManager::GetEntityNames()
{
    std::vector<std::string> Names;
    for (unsigned int i=0;i<Entities.size();i++)
    {
        Names.push_back(Entities[i]->Name);
    }
    return Names;
}

std::string NEntity::GetString(std::string VarName)
{
    if (LuaSelf == LUA_NOREF)
    {
        return "NULL";
    }
    lua_State* L = GetGame()->GetLua()->GetL();
    lua_rawgeti(L,LUA_REGISTRYINDEX,LuaSelf);
    lua_getfield(L,-1,VarName.c_str());
    if (!lua_isstring(L,-1))
    {
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

float NEntity::GetFloat(std::string VarName)
{
    if (LuaSelf == LUA_NOREF)
    {
        return 0;
    }
    lua_State* L = GetGame()->GetLua()->GetL();
    lua_rawgeti(L,LUA_REGISTRYINDEX,LuaSelf);
    lua_getfield(L,-1,VarName.c_str());
    if (!lua_isnumber(L,-1))
    {
        lua_pop(L,2);
        return 0;
    }
    float Number = lua_tonumber(L,-1);
    lua_pop(L,2);
    return Number;
}

bool NEntity::GetBool(std::string VarName)
{
    if (LuaSelf == LUA_NOREF)
    {
        return 0;
    }
    lua_State* L = GetGame()->GetLua()->GetL();
    lua_rawgeti(L,LUA_REGISTRYINDEX,LuaSelf);
    lua_getfield(L,-1,VarName.c_str());
    if (!lua_isboolean(L,-1))
    {
        lua_pop(L,2);
        return 0;
    }
    bool Bool = lua_toboolean(L,-1);
    lua_pop(L,2);
    return Bool;
}

void NEntity::PlayAnim(std::string AnimName)
{
    if (Texture)
    {
        Texture->Play(AnimName);
    }
}

void NEntity::ResetAnimOnPlay(bool reset)
{
    if (Texture)
    {
        Texture->SetResetOnPlay(reset);
    }
}