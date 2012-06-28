#include "NEngine.hpp"

NMap::NMap(std::string i_TileSet)
{
    DepthMem = 0;
    Ready = false;
    RealTileSize = 64;
    Width = Height = Depth = 0;
    ViewingLevel = 1;
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
        for (unsigned int i=0;i<Depth;i++)
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
    Width = 0;
    Height = 0;
    Depth = 0;
    Changed.clear();
    Verts.clear();
    UVs.clear();
    BoxVerts.clear();
    BoxUVs.clear();
    Outline.clear();
    Tiles.clear();
    for (unsigned int i=0;i<Buffers.size();i++)
    {
        delete[] Buffers[i];
    }
    Buffers.clear();
    Ready = false;
}

void NMap::Init(unsigned int i_Width, unsigned int i_Height, unsigned int i_Depth)
{
    DepthMem = 0;
    Width = i_Width;
    Height = i_Height;
    Depth = i_Depth;
    Changed.clear();
    Verts.clear();
    UVs.clear();
    BoxVerts.clear();
    BoxUVs.clear();
    Outline.clear();
    Tiles.clear();
    Changed.resize(Depth,true);
    for (unsigned int i=0;i<Buffers.size();i++)
    {
        delete[] Buffers[i];
    }
    Buffers.clear();
    Buffers.resize(Depth,NULL);
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
void NMap::FixUp()
{
    for (unsigned int x=0;x<Width;x++)
    {
        for (unsigned int y=0;y<Height;y++)
        {
            for (unsigned int z=0;z<Depth;z++)
            {
                NTile* Tile = Tiles[x][y][z];
                if (Tile->IsLight() && !Tile->Light)
                {
                    Tile->Light = GetGame()->GetScene()->AddLight("point");
                    Tile->Light->SetScale(glm::vec3(712,712,1));
                    Tile->Light->SetPos(glm::vec3(x*RealTileSize+RealTileSize/2.f,y*RealTileSize+RealTileSize/2.f,z*RealTileSize));
                }
            }
        }
    }
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
    if (!Texture || !Ready)
    {
        return;
    }
    if (DepthMem != Depth)
    {
        for (unsigned int i=0;i<Depth;i++)
        {
            Buffers[i] = new GLuint[6];
            glGenBuffers(5,Buffers[i]);
        }
        DepthMem = Depth;
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
                float TS = TileSize;
                float X = x*TS;
                float Y = y*TS;
                float Z = i*TS;
                float UTW = TS/(float)TextureWidth;
                float UTH = TS/(float)TextureHeight;
                float UTWS = UTW*((ID-1)%((unsigned int)(TextureWidth/TS)));
                float UTHS = UTH*((ID-1)/((unsigned int)(TextureHeight/TS)));
                Verts[i].push_back(glm::vec3(X,Y,Z));
                UVs[i].push_back(glm::vec2(UTWS,UTHS+UTH));
                Verts[i].push_back(glm::vec3(X+TS,Y,Z));
                UVs[i].push_back(glm::vec2(UTWS+UTW,UTHS+UTH));
                Verts[i].push_back(glm::vec3(X+TS,Y+TS,Z));
                UVs[i].push_back(glm::vec2(UTWS+UTW,UTHS));
                Verts[i].push_back(glm::vec3(X,Y+TS,Z));
                UVs[i].push_back(glm::vec2(UTWS,UTHS));

                if (!Tiles[x][y][i]->IsSolid()) //If we are solid object, outline it and make sure it has a 3d version!
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
                    Outline[i].push_back(glm::vec3(X,Y+TS,Z));
                }
                if (right >= Width || (!Tiles[right][y][i]->IsSolid()))
                {
                    //outline right
                    Outline[i].push_back(glm::vec3(X+TS,Y,Z));
                    Outline[i].push_back(glm::vec3(X+TS,Y+TS,Z));
                }
                if (bottom < 0 || (!Tiles[x][bottom][i]->IsSolid()))
                {
                    //outline bottom
                    Outline[i].push_back(glm::vec3(X,Y,Z));
                    Outline[i].push_back(glm::vec3(X+TS,Y,Z));
                }
                if (top >= Height || (!Tiles[x][top][i]->IsSolid()))
                {
                    //outline top
                    Outline[i].push_back(glm::vec3(X,Y+TS,Z));
                    Outline[i].push_back(glm::vec3(X+TS,Y+TS,Z));
                }
                BoxVerts[i].push_back(glm::vec3(X,Y,Z));
                BoxUVs[i].push_back(glm::vec2(UTWS,UTHS+UTH));
                BoxVerts[i].push_back(glm::vec3(X+TS,Y,Z));
                BoxUVs[i].push_back(glm::vec2(UTWS+UTW,UTHS+UTH));
                BoxVerts[i].push_back(glm::vec3(X+TS,Y,Z+TS));
                BoxUVs[i].push_back(glm::vec2(UTWS+UTW,UTHS));
                BoxVerts[i].push_back(glm::vec3(X,Y,Z+TS));
                BoxUVs[i].push_back(glm::vec2(UTWS,UTHS));

                BoxVerts[i].push_back(glm::vec3(X,Y+TS,Z));
                BoxUVs[i].push_back(glm::vec2(UTWS,UTHS+UTH));
                BoxVerts[i].push_back(glm::vec3(X+TS,Y+TS,Z));
                BoxUVs[i].push_back(glm::vec2(UTWS+UTW,UTHS+UTH));
                BoxVerts[i].push_back(glm::vec3(X+TS,Y+TS,Z+TS));
                BoxUVs[i].push_back(glm::vec2(UTWS+UTW,UTHS));
                BoxVerts[i].push_back(glm::vec3(X,Y+TS,Z+TS));
                BoxUVs[i].push_back(glm::vec2(UTWS,UTHS));

                BoxVerts[i].push_back(glm::vec3(X,Y,Z));
                BoxUVs[i].push_back(glm::vec2(UTWS,UTHS+UTH));
                BoxVerts[i].push_back(glm::vec3(X,Y+TS,Z));
                BoxUVs[i].push_back(glm::vec2(UTWS+UTW,UTHS+UTH));
                BoxVerts[i].push_back(glm::vec3(X,Y+TS,Z+TS));
                BoxUVs[i].push_back(glm::vec2(UTWS+UTW,UTHS));
                BoxVerts[i].push_back(glm::vec3(X,Y,Z+TS));
                BoxUVs[i].push_back(glm::vec2(UTWS,UTHS));

                BoxVerts[i].push_back(glm::vec3(X+TS,Y,Z));
                BoxUVs[i].push_back(glm::vec2(UTWS,UTHS+UTH));
                BoxVerts[i].push_back(glm::vec3(X+TS,Y+TS,Z));
                BoxUVs[i].push_back(glm::vec2(UTWS+UTW,UTHS+UTH));
                BoxVerts[i].push_back(glm::vec3(X+TS,Y+TS,Z+TS));
                BoxUVs[i].push_back(glm::vec2(UTWS+UTW,UTHS));
                BoxVerts[i].push_back(glm::vec3(X+TS,Y,Z+TS));
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
        int Level = round((GetGame()->GetRender()->GetCamera()->GetPos().z-500)/TileSize);
        ViewLevel(Level);
    }
}
void NMap::Remove()
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
    ID = 0;
    Solid = false;
    ForceSolid = false;
    Opaque = false;
    ForceOpaque = false;
    X = x;
    Y = y;
    Z = z;
    Light = NULL;
}
NTile::NTile(unsigned int i_ID)
{
    ID = i_ID; 
    Solid = false;
    ForceSolid = false;
    Opaque = false;
    ForceOpaque = false;
    X = 0;
    Y = 0;
    Z = 0;
    Light = NULL;
}
void NTile::SetID(int i_ID)
{
    ID = i_ID;
    GetGame()->GetScene()->UpdateLights();
    GetGame()->GetMap()->SetChanged(Z);
}
NTile::~NTile()
{
}
void NTile::SetSolid(bool i_Solid)
{
    if (IsSolid() == i_Solid)
    {
        return;
    }
    ForceSolid = true;
    Solid = i_Solid;
}
void NTile::SetOpaque(bool i_Opaque)
{
    if (IsOpaque() == i_Opaque)
    {
        return;
    }
    ForceOpaque = true;
    Opaque = i_Opaque;
}
bool NTile::IsSolid()
{
    if (ForceSolid)
    {
        if (Solid)
        {
            return true;
        }
        return false;
    }
    switch (ID)
    {
        case 2: return true;
        case 3: return true;
    }
    return false;
}

bool NTile::IsOpaque()
{
    if (ForceOpaque)
    {
        if (Opaque)
        {
            return true;
        }
        return false;
    }
    switch (ID)
    {
        case 2: return true;
    }
    return false;
}

bool NTile::IsLight()
{
    switch(ID)
    {
        case 4: return true;
    }
    return false;
}

bool NTile::IsOpenSpace()
{
    if (!ID)
    {
        return true;
    }
    return false;
}

NodeType NMap::GetType()
{
    return NodeMap;
}

bool NMap::Save(std::string Name)
{
    NFile File = GetGame()->GetFileSystem()->GetFile("maps/"+Name+".map", true);
    if (!File.Good())
    {
        NTerminal::SetColor(Red);
        std::cout << "MAP ERROR: ";
        NTerminal::ClearColor();
        std::cout << "Failed to save map maps/" << Name << ".map!\n";
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
                bool Solid = Tiles[x][y][z]->IsSolid();
                File.Write(&(Solid),sizeof(bool));
                bool Opaque = Tiles[x][y][z]->IsOpaque();
                File.Write(&(Opaque),sizeof(bool));
            }
        }
    }
    NTerminal::SetColor(Blue);
    std::cout << "MAP INFO: ";
    NTerminal::ClearColor();
    std::cout << "Successfully saved map maps/" << Name << ".map.\n";
    return Success;
}

bool NMap::Load(std::string Name)
{
    NFile File = GetGame()->GetFileSystem()->GetFile("maps/"+Name+".map",false);
    if (!File.Good())
    {
        NTerminal::SetColor(Red);
        std::cout << "MAP ERROR: ";
        NTerminal::ClearColor();
        std::cout << "Failed to load map maps/" << Name << ".map!\n";
        return Fail;
    }
    File.Read(&Width,sizeof(unsigned int));
    File.Read(&Height,sizeof(unsigned int));
    File.Read(&Depth,sizeof(unsigned int));
    Init(Width,Height,Depth);
    for (unsigned int x=0;x<Width;x++)
    {
        for (unsigned int y=0;y<Height;y++)
        {
            for (unsigned int z=0;z<Depth;z++)
            {
                File.Read(&(Tiles[x][y][z]->ID),sizeof(unsigned int));
                bool Solid;
                File.Read(&Solid,sizeof(bool));
                Tiles[x][y][z]->SetSolid(Solid);
                bool Opaque;
                File.Read(&(Opaque),sizeof(bool));
            }
        }
    }
    NTerminal::SetColor(Blue);
    std::cout << "MAP INFO: ";
    NTerminal::ClearColor();
    std::cout << "Successfully loaded map maps/" << Name << ".map.\n";
    GetGame()->GetScene()->RemoveByType(NodeLight);
    FixUp();
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
    return 4;
}
