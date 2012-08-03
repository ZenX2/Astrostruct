#include "NEngine.hpp"

NMap::NMap(std::string i_TileSet) : NNode(NodeMap)
{
    lua_State* L = GetGame()->GetLua()->GetL();
    lua_newtable(L);
    SelfReference = luaL_ref(L,LUA_REGISTRYINDEX);
    LuaReference = LUA_NOREF;
    Ready = false;
    RealTileSize = 64;
    Width = Height = Depth = 0;
    ViewingLevel = 0;
    Texture = NULL;
    if (GetGame()->IsServer())
    {
        return;
    }
    TileSize = 0;
    Shader = GetGame()->GetRender()->GetShader("map");
    OutlineShader = GetGame()->GetRender()->GetShader("normal");
    Texture = GetGame()->GetRender()->GetTexture(i_TileSet);
    if (Texture)
    {
        TileSize = Texture->GetFloat("TileSize");
        TextureWidth = Texture->GetSize().x;
        TextureHeight = Texture->GetSize().y;
    }
    if (Shader != NULL)
    {
        TextureLoc = Shader->GetUniformLocation("Texture");
        MatrixLoc = Shader->GetUniformLocation("MVP");
        ColorLoc = Shader->GetUniformLocation("Color");
    }
    if (OutlineShader != NULL)
    {
        OutlineColorLoc = OutlineShader->GetUniformLocation("Color");
        OutlineMatrixLoc = OutlineShader->GetUniformLocation("MVP");
    }
}
unsigned int NMap::GetDepth()
{
    return Depth;
}
NMap::~NMap()
{
    for (unsigned int x=0;x<Width;x++)
    {
        for (unsigned int y=0;y<Height;y++)
        {
            for (unsigned int z=0;z<Depth;z++)
            {
                delete Tiles[x][y][z];
            }
        }
    }
    if (!GetGame()->IsServer())
    {
        for (unsigned int i=0;i<Buffers.size();i++)
        {
            glDeleteBuffers(5,Buffers[i]);
            delete[] Buffers[i];
        }
        if (Texture)
        {
            delete Texture;
        }
    }
}
void NMap::DeInit()
{
    Ready = false;
    for (unsigned int x=0;x<Width;x++)
    {
        for (unsigned int y=0;y<Height;y++)
        {
            for (unsigned int z=0;z<Depth;z++)
            {
                Tiles[x][y][z]->CallMethod("OnRemove");
                delete Tiles[x][y][z];
            }
        }
    }
    Width = 0;
    Height = 0;
    Depth = 0;
    ViewingLevel = 0;
    Changed.clear();
    Verts.clear();
    UVs.clear();
    BoxVerts.clear();
    BoxUVs.clear();
    Outline.clear();
    Tiles.clear();
    if (Texture)
    {
        delete Texture;
        Texture = NULL;
    }
    GetGame()->GetScene()->RemoveByType(NodeEntity);
}

void NMap::Init(unsigned int i_Width, unsigned int i_Height, unsigned int i_Depth)
{
    GetGameMode();
    Texture = GetGame()->GetRender()->GetTexture(GetGame()->GetConfig()->GetString("MapSkin"));
    if (Texture)
    {
        TileSize = Texture->GetFloat("TileSize");

        lua_State* L = GetGame()->GetLua()->GetL();
        lua_getglobal(L,"_G");
        lua_pushnumber(L,TileSize);
        lua_setfield(L,-2,"TileSize");
        lua_pop(L,1);

        TextureWidth = Texture->GetSize().x;
        TextureHeight = Texture->GetSize().y;
    }
    MaxTiles = GetGame()->GetConfig()->GetFloat("MaxTiles");
    Width = i_Width;
    Height = i_Height;
    Depth = i_Depth;
    Changed.resize(Depth,true);
    std::vector<glm::vec3> Foo2;
    Verts.resize(Depth, Foo2);
    std::vector<glm::vec2> Bar;
    UVs.resize(Depth, Bar);
    BoxUVs.resize(Depth, Bar);
    Outline.resize(Depth,Foo2);
    BoxVerts.resize(Depth,Foo2);
    std::vector<std::vector<NTile* > > Foo;
    Tiles.resize(Width,Foo);
    for (unsigned int i=0;i<Width;i++)
    {
        std::vector<NTile* > Bar;
        Tiles[i].resize(Height,Bar);
        for (unsigned int o=0;o<Height;o++)
        {
            NTile* Asdf;
            Tiles[i][o].resize(Depth, Asdf);
        }
    }
    for (unsigned int x=0;x<Width;x++)
    {
        for (unsigned int y=0;y<Height;y++)
        {
            for (unsigned int z=0;z<Depth;z++)
            {
                Tiles[x][y][z] = new NTile(x,y,z);
            }
        }
    }
    Ready = true;
}
NTile* NMap::GetTile(unsigned int X, unsigned int Y, unsigned int Z)
{
    if (X < 0 || X >= Width)
    {
        return NULL;
    }
    if (Y < 0 || Y >= Height)
    {
        return NULL;
    }
    if (Z < 0 || Z >= Depth)
    {
        return NULL;
    }
    return Tiles[X][Y][Z];
}
NTile* NMap::GetTile(glm::vec3 Pos)
{
    Pos = Pos-GetPos();
    int X = floor(Pos.x/RealTileSize);
    int Y = floor(Pos.y/RealTileSize);
    int Z = floor(Pos.z/RealTileSize);
    if (X < 0 || X >= Width)
    {
        return NULL;
    }
    if (Y < 0 || Y >= Height)
    {
        return NULL;
    }
    if (Z < 0 || Z >= Depth)
    {
        return NULL;
    }
    return Tiles[X][Y][Z];
}
glm::vec3 NMap::TilePos(glm::vec3 Pos)
{
    int X = floor(Pos.x/RealTileSize);
    int Y = floor(Pos.y/RealTileSize);
    int Z = floor(Pos.z/RealTileSize);
    if (X < 0)
    {
        X = 0;
    } else if (X >= Width)
    {
        X = Width-1;
    }
    if (Y < 0)
    {
        Y = 0;
    } else if (Y >= Height)
    {
        Y = Height-1;
    }
    if (Z < 0)
    {
        Z = 0;
    } else if (Z >= Depth)
    {
        Z = Depth-1;
    }
    return glm::vec3(X*RealTileSize+RealTileSize/2.f,Y*RealTileSize+RealTileSize/2.f,Z*RealTileSize);
}
void NMap::ViewLevel(int Level)
{
    if (Level > Depth - 1)
    {
        ViewingLevel = Depth-1;
        return;
    } else if (Level < 0)
    {
        ViewingLevel = 0;
        return;
    }
    ViewingLevel = Level;
}
int NMap::GetLevel()
{
    return ViewingLevel;
}
int NMap::GetLevel(glm::vec3 Pos)
{
    return floor(Pos.z/TileSize);
}
float NMap::GetTileSize()
{
    return RealTileSize;
}
void NMap::GenerateBuffers()
{
    if (Buffers.size() != Depth)
    {
        for (unsigned int i=0;i<Buffers.size();i++)
        {
            glDeleteBuffers(5,Buffers[i]);
            delete[] Buffers[i];
        }
        Buffers.clear();
        Buffers.resize(Depth);
        for (unsigned int i=0;i<Depth;i++)
        {
            Buffers[i] = new GLuint[6];
            glGenBuffers(5,Buffers[i]);
        }
    }
    for (int i=ViewingLevel;i>=0;i--)
    {
        if (!Changed[i])
        {
            continue;
        }
        Verts[i].clear();
        UVs[i].clear();
        Outline[i].clear();
        BoxVerts[i].clear();
        BoxUVs[i].clear();
        for (unsigned int x=0;x<Width;x++)
        {
            for (unsigned int y=0;y<Height;y++)
            {
                unsigned int ID = Tiles[x][y][i]->ID;
                if (ID == 0)
                {
                    continue;
                }
                float B = .5;
                float TS = RealTileSize;
                float X = x*TS-B;
                float Y = y*TS-B;
                float Z = i*TS-B;
                float UTW = TS/(float)TextureWidth;
                float UTH = TS/(float)TextureHeight;
                float UTWS = UTW*((ID-1)%((unsigned int)(TextureWidth/TileSize)));
                float UTHS = UTH*((ID-1)/((unsigned int)(TextureHeight/TileSize)));
                switch(Tiles[x][y][i]->GetSlope())
                {
                    case SlopeNone:
                    {
                        Verts[i].push_back(glm::vec3(X,Y,Z));
                        UVs[i].push_back(glm::vec2(UTWS,UTHS+UTH));
                        Verts[i].push_back(glm::vec3(X+TS+B,Y,Z));
                        UVs[i].push_back(glm::vec2(UTWS+UTW,UTHS+UTH));
                        Verts[i].push_back(glm::vec3(X+TS+B,Y+TS+B,Z));
                        UVs[i].push_back(glm::vec2(UTWS+UTW,UTHS));
                        Verts[i].push_back(glm::vec3(X,Y+TS+B,Z));
                        UVs[i].push_back(glm::vec2(UTWS,UTHS));
                        break;
                    }
                    case SlopeNorth:
                    {
                        Verts[i].push_back(glm::vec3(X,Y,Z));
                        UVs[i].push_back(glm::vec2(UTWS,UTHS+UTH));
                        Verts[i].push_back(glm::vec3(X+TS+B,Y,Z));
                        UVs[i].push_back(glm::vec2(UTWS+UTW,UTHS+UTH));
                        Verts[i].push_back(glm::vec3(X+TS+B,Y+TS+B,Z+TS));
                        UVs[i].push_back(glm::vec2(UTWS+UTW,UTHS));
                        Verts[i].push_back(glm::vec3(X,Y+TS+B,Z+TS));
                        UVs[i].push_back(glm::vec2(UTWS,UTHS));
                        break;
                    }
                    case SlopeEast:
                    {
                        Verts[i].push_back(glm::vec3(X,Y,Z));
                        UVs[i].push_back(glm::vec2(UTWS,UTHS+UTH));
                        Verts[i].push_back(glm::vec3(X+TS+B,Y,Z+TS));
                        UVs[i].push_back(glm::vec2(UTWS+UTW,UTHS+UTH));
                        Verts[i].push_back(glm::vec3(X+TS+B,Y+TS+B,Z+TS));
                        UVs[i].push_back(glm::vec2(UTWS+UTW,UTHS));
                        Verts[i].push_back(glm::vec3(X,Y+TS+B,Z));
                        UVs[i].push_back(glm::vec2(UTWS,UTHS));
                        break;
                    }
                    case SlopeSouth:
                    {
                        Verts[i].push_back(glm::vec3(X,Y,Z+TS));
                        UVs[i].push_back(glm::vec2(UTWS,UTHS+UTH));
                        Verts[i].push_back(glm::vec3(X+TS+B,Y,Z+TS));
                        UVs[i].push_back(glm::vec2(UTWS+UTW,UTHS+UTH));
                        Verts[i].push_back(glm::vec3(X+TS+B,Y+TS+B,Z));
                        UVs[i].push_back(glm::vec2(UTWS+UTW,UTHS));
                        Verts[i].push_back(glm::vec3(X,Y+TS+B,Z));
                        UVs[i].push_back(glm::vec2(UTWS,UTHS));
                        break;
                    }
                    case SlopeWest:
                    {
                        Verts[i].push_back(glm::vec3(X,Y,Z+TS));
                        UVs[i].push_back(glm::vec2(UTWS,UTHS+UTH));
                        Verts[i].push_back(glm::vec3(X+TS+B,Y,Z));
                        UVs[i].push_back(glm::vec2(UTWS+UTW,UTHS+UTH));
                        Verts[i].push_back(glm::vec3(X+TS+B,Y+TS+B,Z));
                        UVs[i].push_back(glm::vec2(UTWS+UTW,UTHS));
                        Verts[i].push_back(glm::vec3(X,Y+TS+B,Z+TS));
                        UVs[i].push_back(glm::vec2(UTWS,UTHS));
                        break;
                    }
                }
                if (!Tiles[x][y][i]->IsSolid() || Tiles[x][y][i]->GetSlope() != SlopeNone) //If we are solid object, outline it and make sure it has a 3d version!
                {
                    continue;
                }
                int left = int(x)-1;
                int right = x+1;
                int bottom = int(y)-1;
                int top = y+1;
                if (left < 0 || (!Tiles[left][y][i]->IsSolid()))
                {
                    //outline left
                    Outline[i].push_back(glm::vec3(X,Y,Z));
                    Outline[i].push_back(glm::vec3(X,Y+TS+B,Z));
                }
                if (right >= Width || (!Tiles[right][y][i]->IsSolid()))
                {
                    //outline right
                    Outline[i].push_back(glm::vec3(X+TS+B,Y,Z));
                    Outline[i].push_back(glm::vec3(X+TS+B,Y+TS+B,Z));
                }
                if (bottom < 0 || (!Tiles[x][bottom][i]->IsSolid()))
                {
                    //outline bottom
                    Outline[i].push_back(glm::vec3(X,Y,Z));
                    Outline[i].push_back(glm::vec3(X+TS+B,Y,Z));
                }
                if (top >= Height || (!Tiles[x][top][i]->IsSolid()))
                {
                    //outline top
                    Outline[i].push_back(glm::vec3(X,Y+TS+B,Z));
                    Outline[i].push_back(glm::vec3(X+TS+B,Y+TS+B,Z));
                }
                BoxVerts[i].push_back(glm::vec3(X,Y,Z));
                BoxUVs[i].push_back(glm::vec2(UTWS,UTHS+UTH));
                BoxVerts[i].push_back(glm::vec3(X+TS+B,Y,Z));
                BoxUVs[i].push_back(glm::vec2(UTWS+UTW,UTHS+UTH));
                BoxVerts[i].push_back(glm::vec3(X+TS+B,Y,Z+TS));
                BoxUVs[i].push_back(glm::vec2(UTWS+UTW,UTHS));
                BoxVerts[i].push_back(glm::vec3(X,Y,Z+TS));
                BoxUVs[i].push_back(glm::vec2(UTWS,UTHS));

                BoxVerts[i].push_back(glm::vec3(X,Y+TS+B,Z));
                BoxUVs[i].push_back(glm::vec2(UTWS,UTHS+UTH));
                BoxVerts[i].push_back(glm::vec3(X+TS+B,Y+TS+B,Z));
                BoxUVs[i].push_back(glm::vec2(UTWS+UTW,UTHS+UTH));
                BoxVerts[i].push_back(glm::vec3(X+TS+B,Y+TS+B,Z+TS));
                BoxUVs[i].push_back(glm::vec2(UTWS+UTW,UTHS));
                BoxVerts[i].push_back(glm::vec3(X,Y+TS+B,Z+TS));
                BoxUVs[i].push_back(glm::vec2(UTWS,UTHS));

                BoxVerts[i].push_back(glm::vec3(X,Y,Z));
                BoxUVs[i].push_back(glm::vec2(UTWS,UTHS+UTH));
                BoxVerts[i].push_back(glm::vec3(X,Y+TS+B,Z));
                BoxUVs[i].push_back(glm::vec2(UTWS+UTW,UTHS+UTH));
                BoxVerts[i].push_back(glm::vec3(X,Y+TS+B,Z+TS));
                BoxUVs[i].push_back(glm::vec2(UTWS+UTW,UTHS));
                BoxVerts[i].push_back(glm::vec3(X,Y,Z+TS));
                BoxUVs[i].push_back(glm::vec2(UTWS,UTHS));

                BoxVerts[i].push_back(glm::vec3(X+TS+B,Y,Z));
                BoxUVs[i].push_back(glm::vec2(UTWS,UTHS+UTH));
                BoxVerts[i].push_back(glm::vec3(X+TS+B,Y+TS+B,Z));
                BoxUVs[i].push_back(glm::vec2(UTWS+UTW,UTHS+UTH));
                BoxVerts[i].push_back(glm::vec3(X+TS+B,Y+TS,Z+TS));
                BoxUVs[i].push_back(glm::vec2(UTWS+UTW,UTHS));
                BoxVerts[i].push_back(glm::vec3(X+TS+B,Y,Z+TS+B));
                BoxUVs[i].push_back(glm::vec2(UTWS,UTHS));
            }
        }
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][0]);
        glBufferData(GL_ARRAY_BUFFER,Verts[i].size()*sizeof(glm::vec3),&Verts[i][0],GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][1]);
        glBufferData(GL_ARRAY_BUFFER,UVs[i].size()*sizeof(glm::vec2),&UVs[i][0],GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][2]);
        glBufferData(GL_ARRAY_BUFFER,Outline[i].size()*sizeof(glm::vec3),&Outline[i][0],GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][3]);
        glBufferData(GL_ARRAY_BUFFER,BoxVerts[i].size()*sizeof(glm::vec3),&BoxVerts[i][0],GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][4]);
        glBufferData(GL_ARRAY_BUFFER,BoxUVs[i].size()*sizeof(glm::vec2),&BoxUVs[i][0],GL_STATIC_DRAW);
        Changed[i] = false;
    }
}
void NMap::Draw(NCamera* View)
{
    if (!Texture || !Ready)
    {
        return;
    }
    GenerateBuffers();
    if (Shader == NULL)
    {

        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(&View->GetPerspMatrix()[0][0]);
        glMatrixMode(GL_MODELVIEW);
        glm::mat4 MVP = View->GetPerspViewMatrix()*GetModelMatrix();
        glLoadMatrixf(&MVP[0][0]);

        glColor4fv(&(GetColor()[0]));
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D,Texture->GetID());
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        int Check = ViewingLevel - 2;
        if (Check < 0)
        {
            Check = 0;
        }
        for (unsigned int i=Check;i<=ViewingLevel;i++)
        {
            glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][0]);
            glVertexPointer(3,GL_FLOAT,0,NULL);

            glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][1]);
            glTexCoordPointer(2,GL_FLOAT,0,NULL);

            glDrawArrays(GL_QUADS,0,Verts[i].size());
        }
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glColor4f(0,0,0,1);
        for (unsigned int i=0;i<=ViewingLevel;i++)
        {
            glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][2]);
            glVertexPointer(3,GL_FLOAT,0,NULL);
            
            glDrawArrays(GL_LINES,0,Outline[i].size());
        }
        glDisableClientState(GL_VERTEX_ARRAY);
        return;
    }
    glUseProgram(Shader->GetID());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,Texture->GetID());
    glUniform1i(TextureLoc,0);
    glm::mat4 MVP = View->GetPerspMatrix()*View->GetPerspViewMatrix()*GetModelMatrix();
    glUniformMatrix4fv(MatrixLoc,1,GL_FALSE,&MVP[0][0]);
    glUniform4fv(ColorLoc,1,&(GetColor()[0]));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    int Check = ViewingLevel - 2;
    if (Check < 0)
    {
        Check = 0;
    }
    for (unsigned int i=Check;i<=ViewingLevel;i++)
    {
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][0]);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][1]);
        glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,NULL);
        
        glDrawArrays(GL_QUADS,0,Verts[i].size());
        if (i != ViewingLevel)
        {
            glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][3]);
            glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
            glBindBuffer(GL_ARRAY_BUFFER,Buffers[i][4]);
            glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,NULL);
            
            glDrawArrays(GL_QUADS,0,BoxVerts[i].size());
        }
    }
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisableVertexAttribArray(1);

    glUseProgram(OutlineShader->GetID());
    glUniform4f(OutlineColorLoc,0,0,0,1);
    glUniformMatrix4fv(OutlineMatrixLoc,1,GL_FALSE,&MVP[0][0]);
    glLineWidth(3);
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[ViewingLevel][2]);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
    
    glDrawArrays(GL_LINES,0,Outline[ViewingLevel].size());
    glDisableVertexAttribArray(0);
}
void NMap::Tick(double DT)
{
    if (!GetGame()->IsServer())
    {
        if (Texture)
        {
            Texture->Tick(DT);
        }
        int Level = floor((GetGame()->GetRender()->GetCamera()->GetPos().z-500)/TileSize);
        ViewLevel(Level);
    }
}
void NMap::Unallocate()
{
    delete (NMap*)this;
}
void NMap::SetChanged(int Level)
{
    if (Level < 0)
    {
        return;
    }
    if (Level >= Depth)
    {
        return;
    }
    Changed[Level] = true;
}

NTile::NTile(unsigned int x, unsigned int y, unsigned int z)
{
    Body = NULL;
    LuaReference = LUA_NOREF;
    lua_State* L = GetGame()->GetLua()->GetL();
    lua_newtable(L);
    SelfReference = luaL_ref(L,LUA_REGISTRYINDEX);
    Slope = SlopeNone;
    ID = 0;
    dSolid = false;
    Solid = false;
    ForceSolid = false;
    dOpaque = false;
    Opaque = false;
    ForceOpaque = false;
    X = x;
    Y = y;
    Z = z;
}
NTile::NTile(unsigned int i_ID)
{
    Body = NULL;
    ID = i_ID; 
    LuaReference = GetGame()->GetMap()->GetLuaTile(ID);
    lua_State* L = GetGame()->GetLua()->GetL();
    lua_newtable(L);
    SelfReference = luaL_ref(L,LUA_REGISTRYINDEX);
    Slope = SlopeNone;
    Solid = false;
    dSolid = GetBool("Solid");
    ForceSolid = false;
    Opaque = false;
    dOpaque = GetBool("Opaque");
    ForceOpaque = false;
    X = 0;
    Y = 0;
    Z = 0;
}

int NMap::GetLuaTile(unsigned int ID)
{
    if (ID == 0)
    {
        return LUA_NOREF;
    }
    if (LuaTiles.size() <= ID)
    {
        LuaTiles.resize(ID+1,LUA_NOREF);
    }
    if (LuaTiles[ID] == LUA_NOREF)
    {
        std::string Gamemode = GetGame()->GetConfig()->GetString("GameMode");
        std::stringstream TileDir;
        TileDir << "gamemodes/" << Gamemode << "/tiles/" << ID << "/";
        if (GetGame()->IsServer())
        {
            TileDir << "init.lua";
        } else {
            TileDir << "cl_init.lua";
        }
        lua_State* L = GetGame()->GetLua()->GetL();
        lua_newtable(L);
        lua_setglobal(L, "TILE");
        if (!GetGame()->GetLua()->DoFile(TileDir.str()))
        {
            //If we failed, reset the global and abort.
            lua_pushnil(L);
            lua_setglobal(L,"TILE");
            return LUA_NOREF;
        }
        //Right now i'll just include tiles into the entities table. This could cause problems later on if entities have the same names as tiles.
        lua_getglobal(L,"entity");
        if (!lua_istable(L,-1))
        {
            //If we haven't loaded in the entity module, reset the global and abort.
            lua_pop(L,1);
            lua_pushnil(L);
            lua_setglobal(L,"TILE");
            return LUA_NOREF;
        }
        lua_getfield(L,-1,"register");
        if (!lua_isfunction(L,-1))
        {
            //If our register function doesn't exist, create it!
            lua_pop(L,2);
            lua_pushnil(L);
            lua_setglobal(L,"TILE");
            return LUA_NOREF;
        }
        lua_remove(L,-2);
        lua_getglobal(L,"TILE");
        std::stringstream TileID;
        TileID << ID;
        lua_pushstring(L,TileID.str().c_str());
        lua_pcall(L,2,0,0);
        lua_pushnil(L);
        lua_setglobal(L,"TILE");
        lua_getglobal(L,"entity");
        if (!lua_istable(L,-1))
        {
            lua_pop(L,1);
            return LUA_NOREF;
        }
        lua_getfield(L,-1,"get");
        if (!lua_isfunction(L,-1))
        {
            lua_pop(L,2);
            return LUA_NOREF;
        }
        lua_remove(L,-2);
        lua_pushstring(L,TileID.str().c_str());
        lua_pcall(L,1,1,0);
        LuaTiles[ID] = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    return LuaTiles[ID];
}

int NMap::GetGameMode()
{
    if (LuaReference == LUA_NOREF)
    {
        std::string Gamemode = GetGame()->GetConfig()->GetString("GameMode");
        std::string Dir = "gamemodes/"+Gamemode+"/";
        if (GetGame()->IsServer())
        {
            Dir += "init.lua";
        } else {
            Dir += "cl_init.lua";
        }
        lua_State* L = GetGame()->GetLua()->GetL();
        lua_newtable(L);
        lua_setglobal(L, "GAME");
        if (!GetGame()->GetLua()->DoFile(Dir))
        {
            //If we failed, reset the global and abort.
            lua_pushnil(L);
            lua_setglobal(L,"GAME");
            return LUA_NOREF;
        }
        //Right now i'll just include game modes into the entities table. This could cause problems later on if entities have the same names as the gamemode.
        lua_getglobal(L,"entity");
        if (!lua_istable(L,-1))
        {
            //If we haven't loaded in the entity module, reset the global and abort.
            lua_pop(L,1);
            lua_pushnil(L);
            lua_setglobal(L,"GAME");
            return LUA_NOREF;
        }
        lua_getfield(L,-1,"register");
        if (!lua_isfunction(L,-1))
        {
            //If our register function doesn't exist, create it!
            lua_pop(L,2);
            lua_pushnil(L);
            lua_setglobal(L,"GAME");
            return LUA_NOREF;
        }
        lua_remove(L,-2);
        lua_getglobal(L,"GAME");
        lua_pushstring(L,Gamemode.c_str());
        lua_pcall(L,2,0,0);
        lua_pushnil(L);
        lua_setglobal(L,"GAME");
        lua_getglobal(L,"entity");
        if (!lua_istable(L,-1))
        {
            lua_pop(L,1);
            return LUA_NOREF;
        }
        lua_getfield(L,-1,"get");
        if (!lua_isfunction(L,-1))
        {
            lua_pop(L,2);
            return LUA_NOREF;
        }
        lua_remove(L,-2);
        lua_pushstring(L,Gamemode.c_str());
        lua_pcall(L,1,1,0);
        LuaReference = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    return LuaReference;
}

void NTile::SetID(int i_ID)
{
    GetGame()->GetScene()->UpdateLights();
    GetGame()->GetMap()->SetChanged(Z);
    if (ID == i_ID)
    {
        return;
    }
    ID = i_ID;
    if (ID == 0)
    {
        dSolid = false;
        dOpaque = false;
        return;
    }
    lua_State* L = GetGame()->GetLua()->GetL();
    CallMethod("OnRemove");
    LuaReference = GetGame()->GetMap()->GetLuaTile(ID);
    dSolid = GetBool("Solid");
    dOpaque = GetBool("Opaque");
    CallMethod("OnInitialize");
    GenerateBody();
}

bool NTile::GetBool(std::string Name)
{
    if (LuaReference == LUA_NOREF)
    {
        return false;
    }
    lua_State* L = GetGame()->GetLua()->GetL();
    lua_rawgeti(L,LUA_REGISTRYINDEX,LuaReference);
    lua_getfield(L,-1,Name.c_str());
    if (!lua_isboolean(L,-1))
    {
        lua_pop(L,2);
        return false;
    }
    bool Bool = lua_toboolean(L,-1);
    lua_pop(L,2);
    return Bool;
}

NTile::~NTile()
{
    if (Body != NULL)
    {
        GetGame()->GetPhysics()->GetWorld()->removeRigidBody(Body);
        delete Body->getMotionState();
        delete Body;
    }
}
void NTile::GenerateBody()
{
    if (GetSlope() != SlopeNone)
    {
        float TS = GetGame()->GetMap()->GetTileSize();
        if (Body != NULL)
        {
            GetGame()->GetPhysics()->GetWorld()->removeRigidBody(Body);
            delete Body->getMotionState();
            delete Body;
            delete Shape;
        }
        Shape = new btBoxShape(btVector3(TS/2.f,TS/2.f,1));
        btQuaternion Angle = btQuaternion(0,0,0,1);
        switch(GetSlope())
        {
            case SlopeNorth: Angle = btQuaternion(btVector3(1,0,0),45); break;
            case SlopeSouth: Angle = btQuaternion(btVector3(1,0,0),-45); break;
            case SlopeEast: Angle = btQuaternion(btVector3(0,1,0),-45); break;
            case SlopeWest: Angle = btQuaternion(btVector3(0,1,0),45); break;
        }
        btDefaultMotionState* StaticMotionState = new btDefaultMotionState(btTransform(Angle,btVector3(X*TS+TS/2.f,Y*TS+TS/2.f,Z*TS+TS/2.f)));
        btRigidBody::btRigidBodyConstructionInfo PlaneBody(0,StaticMotionState,Shape,btVector3(0,0,0));
        Body = new btRigidBody(PlaneBody);
        GetGame()->GetPhysics()->GetWorld()->addRigidBody(Body);
        return;
    }
    if (!IsSolid())
    {
        float TS = GetGame()->GetMap()->GetTileSize();
        if (Body != NULL)
        {
            GetGame()->GetPhysics()->GetWorld()->removeRigidBody(Body);
            delete Body->getMotionState();
            delete Body;
            delete Shape;
        }
        Shape =  new btBoxShape(btVector3(TS/2.f,TS/2.f,1));
        btDefaultMotionState* StaticMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(X*TS+TS/2.f,Y*TS+TS/2.f,Z*TS)));
        btRigidBody::btRigidBodyConstructionInfo PlaneBody(0,StaticMotionState,Shape,btVector3(0,0,0));
        Body = new btRigidBody(PlaneBody);
        GetGame()->GetPhysics()->GetWorld()->addRigidBody(Body);
        return;
    }
    float TS = GetGame()->GetMap()->GetTileSize();
    if (Body != NULL)
    {
        GetGame()->GetPhysics()->GetWorld()->removeRigidBody(Body);
        delete Body->getMotionState();
        delete Body;
        delete Shape;
    }
    Shape = new btBoxShape(btVector3(TS/2.f,TS/2.f,TS/2.f));
    btDefaultMotionState* StaticMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(X*TS+TS/2.f,Y*TS+TS/2.f,Z*TS+TS/2.f)));
    btRigidBody::btRigidBodyConstructionInfo PlaneBody(0,StaticMotionState,Shape,btVector3(0,0,0));
    Body = new btRigidBody(PlaneBody);
    GetGame()->GetPhysics()->GetWorld()->addRigidBody(Body);
}

void NTile::SetSolid(bool i_Solid)
{
    ForceSolid = true;
    Solid = i_Solid;
    GenerateBody();
}
void NTile::SetOpaque(bool i_Opaque)
{
    ForceOpaque = true;
    Opaque = i_Opaque;
}
bool NTile::IsSolid()
{
    if (ForceSolid)
    {
        return Solid;
    }
    return dSolid;
}

bool NTile::IsOpaque()
{
    if (ForceOpaque)
    {
        return Opaque;
    }
    return dOpaque;
}

bool NTile::IsOpenSpace()
{
    if (!ID)
    {
        return true;
    }
    return false;
}

bool NMap::Save(std::string Name)
{
    NWriteFile File = GetGame()->GetFileSystem()->GetWriteFile("maps/"+Name+".map");
    if (!File.Good())
    {
        GetGame()->GetLog()->Send("MAP",0,std::string("Failed to save map 'maps/") + Name + ".map!");
        return Fail;
    }
    File.Write(&Width,sizeof(unsigned int));
    File.Write(&Height,sizeof(unsigned int));
    File.Write(&Depth,sizeof(unsigned int));
    for (unsigned int x=0;x<Width;x++)
    {
        for (unsigned int y=0;y<Height;y++)
        {
            for (unsigned int z=0;z<Depth;z++)
            {
                File.Write(&(Tiles[x][y][z]->ID),sizeof(unsigned int));
                if (Tiles[x][y][z]->ID == 0)
                {
                    continue;
                }
                bool Solid = Tiles[x][y][z]->IsSolid();
                File.Write(&(Solid),sizeof(bool));
                bool Opaque = Tiles[x][y][z]->IsOpaque();
                File.Write(&(Opaque),sizeof(bool));
                TileSlope Slope = Tiles[x][y][z]->GetSlope();
                File.Write(&(Slope),sizeof(TileSlope));
            }
        }
    }
    std::vector<NNode*> Entities = GetGame()->GetScene()->GetNodesByType(NodeEntity);
    for (unsigned int i=0;i<Entities.size();i++)
    {
        NEntity* Ent = (NEntity*)Entities[i];
        std::string Temp = Ent->GetName() + '\0';
        File.Write((char*)Temp.c_str(),Temp.length());
        glm::vec3 Pos = Ent->GetPos();
        File.Write(&(Pos),sizeof(glm::vec3));
    }
    GetGame()->GetLog()->Send("MAP",2,std::string("Successfully saved map 'maps/") + Name + ".map!");
    return Success;
}

bool NMap::Load(std::string Name)
{
    NReadFile File = GetGame()->GetFileSystem()->GetReadFile("maps/"+Name+".map");
    if (!File.Good())
    {
        GetGame()->GetLog()->Send("MAP",0,std::string("Failed to save map 'maps/") + Name + ".map!");
        return Fail;
    }
    unsigned int W,H,D;
    File.Read(&W,sizeof(unsigned int));
    File.Read(&H,sizeof(unsigned int));
    File.Read(&D,sizeof(unsigned int));
    DeInit();
    Init(W,H,D);
    for (unsigned int x=0;x<Width;x++)
    {
        for (unsigned int y=0;y<Height;y++)
        {
            for (unsigned int z=0;z<Depth;z++)
            {
                unsigned int ID;
                File.Read(&ID,sizeof(unsigned int));
                Tiles[x][y][z]->SetID(ID);
                if (Tiles[x][y][z]->ID == 0)
                {
                    continue;
                }
                bool Solid;
                File.Read(&Solid,sizeof(bool));
                Tiles[x][y][z]->SetSolid(Solid);
                bool Opaque;
                File.Read(&(Opaque),sizeof(bool));
                Tiles[x][y][z]->SetOpaque(Opaque);
                TileSlope Slope;
                File.Read(&(Slope),sizeof(TileSlope));
                Tiles[x][y][z]->SetSlope(Slope);
            }
        }
    }
    std::string Entities;
    while (File.Good())
    {
        char Buffer[32];
        unsigned int Read = File.Read(Buffer,32);
        Entities.append(Buffer, Read);
    }
    int Pos = 0;
    while (Pos < Entities.length())
    {
        std::string EntName = Entities.substr(Pos,Entities.find('\0',Pos)-Pos);
        Pos += EntName.length()+1;
        glm::vec3 Vector;
        memcpy(&(Vector),Entities.substr(Pos,sizeof(glm::vec3)).c_str(),sizeof(glm::vec3));
        Pos += sizeof(glm::vec3);
        NEntity* Entity = new NEntity(EntName,Vector);
    }
    GetGame()->GetLog()->Send("MAP",2,std::string("Successfully loaded map 'maps/") + Name + ".map!");
    CallMethod("OnInitialize");
    return Success;
}

unsigned int NMap::GetWidth()
{
    return Width;
}
unsigned int NMap::GetHeight()
{
    return Height;
}
unsigned int NMap::GetTileCount()
{
    return MaxTiles;
}
void NTile::SetSlope(TileSlope i_Slope)
{
    Slope = i_Slope;
    GenerateBody();
}
TileSlope NTile::GetSlope()
{
    return Slope;
}
void NTile::CallMethod(std::string Name)
{
    lua_State* L = GetGame()->GetLua()->GetL();
    if (LuaReference != LUA_NOREF)
    {
        lua_rawgeti(L,LUA_REGISTRYINDEX,LuaReference);
        lua_getfield(L,-1,Name.c_str());
        if (!lua_isfunction(L,-1))
        {
            lua_pop(L,2);
        } else {
            lua_pushTile(L,this);
            lua_call(L,1,0);
            lua_pop(L,1);
        }
    }
}
void NMap::CallMethod(std::string Name, unsigned int AdditionalVars,  ...)
{
    lua_State* L = GetGame()->GetLua()->GetL();
    GetGameMode();
    lua_rawgeti(L,LUA_REGISTRYINDEX,LuaReference);
    lua_getfield(L,-1,Name.c_str());
    if (!lua_isfunction(L,-1))
    {
        lua_pop(L,2);
    } else {
        lua_pushMap(L,this);
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
void NMap::Resize(unsigned int X, unsigned int Y, unsigned int Z)
{
    if (X<Width)
    {
        for (unsigned int x=X;x>=Width;x--)
        {
            for (unsigned int y=0;y<Height;y++)
            {
                for (unsigned int z=0;z<Depth;z++)
                {
                    delete Tiles[x][y][z];
                }
            }
        }
        Tiles.resize(X);
    } else {
        std::vector<std::vector<NTile* > > Foo;
        Tiles.resize(X,Foo);
    }
    Width = X;
    if (Y<Height)
    {
        for (unsigned int x=0;x<Width;x++)
        {
            for (unsigned int y=Y;y>=Height;y--)
            {
                for (unsigned int z=0;z<Depth;z++)
                {
                    delete Tiles[x][y][z];
                }
            }
        }
        for (unsigned int x=0;x<Width;x++)
        {
            Tiles[x].resize(Y);
        }
    } else {
        for (unsigned int x=0;x<Width;x++)
        {
            std::vector<NTile* > Foo;
            Tiles[x].resize(Y,Foo);
        }
    }
    Height = Y;
    if (Z<Depth)
    {
        for (unsigned int x=0;x<Width;x++)
        {
            for (unsigned int y=0;y<Height;y++)
            {
                for (unsigned int z=Z;z>=Depth;z--)
                {
                    delete Tiles[x][y][z];
                }
            }
        }
        for (unsigned int x=0;x<Width;x++)
        {
            for (unsigned int y=0;y<Height;y++)
            {
                Tiles[x][y].resize(Z);
            }
        }
    } else {
        for (unsigned int x=0;x<Width;x++)
        {
            for (unsigned int y=0;y<Height;y++)
            {
                NTile* Foo;
                Tiles[x][y].resize(Z,Foo);
            }
        }
    }
    Depth = Z;
    if (ViewingLevel >= Depth)
    {
        ViewingLevel = Depth-1;
    }
}
