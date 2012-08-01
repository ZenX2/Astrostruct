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
            if (GetGame()->IsServer())
            {
                File += "init.lua";
            } else {
                File += "cl_init.lua";
            }
            lua_State* L = GetGame()->GetLua()->GetL();
            lua_newtable(L);
            lua_setglobal(L, "ENT");
            if (!GetGame()->GetLua()->DoFile(File))
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
            if (!GetGame()->GetLua()->DoFile(File))
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
    lua_newtable(L);
    SelfReference = luaL_ref(L,LUA_REGISTRYINDEX);
    LuaSelf = GetGame()->GetEntityManager()->GetLuaEntity(Name);
    Velocity = glm::vec3(0);
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
            std::cout << GetScale().x;
        }
    } else {
        SetScale(glm::vec3(W,H,1.f));
    }
    Shader = GetGame()->GetRender()->GetShader("flat");
    if (Shader)
    {
        TextureLoc = Shader->GetUniformLocation("Texture");
        MatrixLoc = Shader->GetUniformLocation("MVP");
        ColorLoc = Shader->GetUniformLocation("Color");
    }
    CallMethod("OnInitialize");
}

NEntity::NEntity(std::string i_Name, glm::vec3 i_Position) : NNode(NodeEntity)
{
    Name = i_Name;
    lua_State* L = GetGame()->GetLua()->GetL();
    lua_newtable(L);
    SelfReference = luaL_ref(L,LUA_REGISTRYINDEX);
    LuaSelf = GetGame()->GetEntityManager()->GetLuaEntity(Name);
    Velocity = glm::vec3(0);
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
    Shader = GetGame()->GetRender()->GetShader("flat");
    if (Shader)
    {
        TextureLoc = Shader->GetUniformLocation("Texture");
        MatrixLoc = Shader->GetUniformLocation("MVP");
        ColorLoc = Shader->GetUniformLocation("Color");
    }
    SetPos(i_Position);
    CallMethod("OnInitialize");
}

NEntity::~NEntity()
{
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
    CallMethod("OnTick");
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
    GenerateBuffers();
    if (Texture == NULL || GetColor().w == 0)
    {
        return;
    }
    if (Shader == NULL)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
        glVertexPointer(2,GL_FLOAT,0,NULL);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
        glTexCoordPointer(2,GL_FLOAT,0,NULL);

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        if (Texture != NULL)
        {
            glBindTexture(GL_TEXTURE_2D,Texture->GetID());
        }

        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(&View->GetOrthoMatrix()[0][0]);
        glMatrixMode(GL_MODELVIEW);
        glm::mat4 MVP = View->GetViewMatrix()*GetModelMatrix();
        glLoadMatrixf(&MVP[0][0]);

        glColor4fv(&(GetColor()[0]));
        glDrawArrays(GL_QUADS,0,Verts.size());
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        return;
    }
    glUseProgram(Shader->GetID());
    glActiveTexture(GL_TEXTURE0);
    if (Texture != NULL)
    {
        glBindTexture(GL_TEXTURE_2D,Texture->GetID());
    }
    glUniform1i(TextureLoc,0);
    glm::mat4 MVP = View->GetPerspMatrix()*View->GetPerspViewMatrix()*GetModelMatrix();
    glUniformMatrix4fv(MatrixLoc,1,GL_FALSE,&MVP[0][0]);
    glUniform4fv(ColorLoc,1,&(GetColor()[0]));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,NULL);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glDrawArrays(GL_QUADS,0,Verts.size());
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glUseProgram(0);
}

glm::vec3 NEntity::GetVel()
{
    return Velocity;
}

void NEntity::SetVel(glm::vec3 i_Velocity)
{
    Velocity = i_Velocity;
}

void NEntity::Remove()
{
    delete (NEntity*)this;
}

void NEntity::CallMethod(std::string VarName, unsigned int AdditionalVars,  ...)
{
    if (LuaSelf == LUA_NOREF)
    {
        return;
    }
    lua_State* L = GetGame()->GetLua()->GetL();
    lua_rawgeti(L,LUA_REGISTRYINDEX,LuaSelf);
    lua_getfield(L,-1,VarName.c_str());
    if (!lua_isfunction(L,-1))
    {
        lua_pop(L,2);
    } else {
        lua_pushEntity(L,this);
        va_list vl;
        va_start(vl,AdditionalVars);
        for (unsigned int i=0;i<AdditionalVars;i++)
        {
            NNode* Temp = va_arg(vl,NNode*);
            switch(Temp->GetType())
            {
                case NodePlayer: lua_pushPlayer(L,(NPlayer*)Temp);
            }
        }
        lua_call(L,1+AdditionalVars,0);
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
