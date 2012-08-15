#include "NEngine.hpp"

NTextSystem::NTextSystem()
{
    if (FT_Init_FreeType(&FTLib))
    {
        GetGame()->GetLog()->Send("FREETYPE",1,"FreeType failed to initialize!");
        FTLib = NULL;
    }
}

NTextSystem::~NTextSystem()
{
    for (unsigned int i=0;i<Faces.size();i++)
    {
        delete Faces[i];
    }
    FT_Done_FreeType(FTLib);
}

void NTextSystem::LoadFaces()
{
    lua_State* L = GetGame()->GetLua()->GetL();
    static const luaL_Reg FontFunctions[] =
    {
        {"LoadFontFace",LoadFace},
        {NULL,NULL}
    };
    lua_getglobal(L,"_G");
    luaL_register(L,NULL,FontFunctions);
    lua_pop(L,1);
    GetGame()->GetLua()->DoFolder("fonts");
}

void NFace::UpdateMipmaps()
{
    for (unsigned int i=0;i<Textures.size();i++)
    {
        if (Textures[i] == NULL)
        {
            continue;
        }
        Textures[i]->UpdateMipmaps();
    }
}

std::string NFace::GetName()
{
    return Name;
}

NFace* NTextSystem::GetFace(std::string Name)
{
    for (unsigned int i=0;i<Faces.size();i++)
    {
        if (!Faces[i]->GetName().compare(Name))
        {
            return Faces[i];
        }
    }
    return NULL;
}

NFace::NFace(std::string i_Name)
{
    Name = i_Name;
    Face = NULL;
    FileData = NULL;
}

NFace::~NFace()
{
    for (unsigned int i=0;i<Textures.size();i++)
    {
        delete Textures[i];
    }
    if (FileData)
    {
        delete[] FileData;
    }
}

bool NFace::Load(std::string File)
{
    FT_Library FTLib = GetGame()->GetTextSystem()->GetFreeTypeLib();
    if (!FTLib)
    {
        return 1;
    }
    NReadFile MyFile = GetGame()->GetFileSystem()->GetReadFile(File);
    if (!MyFile.Good())
    {
        GetGame()->GetLog()->Send("FREETYPE",1,"Failed to load " + File + ", it doesn't exist!");
        return 1;
    }
    FileData = new char[MyFile.Size()];
    MyFile.Read(FileData,MyFile.Size());
    if (FT_New_Memory_Face(FTLib,(const unsigned char*)FileData,MyFile.Size(),0,&Face))
    {
        delete[] FileData;
        GetGame()->GetLog()->Send("FREETYPE",1,"Failed to load " + File + " as a font, it's either corrupt or not a FreeType-compatible format!");
        return 1;
    }
    return 0;
}

NGlyph::NGlyph(FT_Face Face)
{
    FT_GlyphSlot Glyph = Face->glyph;
    AdvanceX = Glyph->advance.x >> 6;
    AdvanceY = Glyph->advance.y >> 6;
    BitmapWidth = Glyph->bitmap.width;
    BitmapHeight = Glyph->bitmap.rows;
    BitmapLeft = Glyph->bitmap_left;
    BitmapTop = Glyph->bitmap_top;
    Rendered = false;
    TextureRect = glm::vec4(0);
}

NTextureAtlas::NTextureAtlas(FT_Face Face, unsigned int i_Size)
{
    //Get Width and Height of desired textureatlas
    Changed = true;
    //FIXME: When the texture fills, resize the texture to be bigger.
    Width = 256;
    Height = 256;
    Size = i_Size;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &Texture);
    GetGame()->GetRender()->AddCachedTexture(Texture);
    glBindTexture(GL_TEXTURE_2D, Texture);
    unsigned char* Data = new unsigned char[Width*Height];
    for (int i=0;i<Width*Height;i++)
    {
        Data[i] = 0;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, Width, Height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, Data);
    Node = new NTextureNode(glm::vec4(Width,Height,0,0));
    delete[] Data;
}

NTextureAtlas::~NTextureAtlas()
{
    for (unsigned int i=0;i<Glyphs.size();i++)
    {
        delete Glyphs[i];
    }
    delete Node;
}

unsigned int NTextureAtlas::GetSize()
{
    return Size;
}

NGlyph* NTextureAtlas::GetGlyph(FT_Face Face, wchar_t ID)
{
    FT_Set_Pixel_Sizes(Face,0,Size);
    if (ID >= (int)Glyphs.size())
    {
        Glyphs.resize(ID+1,NULL);
        FT_Load_Char(Face,ID,FT_LOAD_RENDER);
        NGlyph* NewGlyph = new NGlyph(Face);
        Glyphs[ID] = NewGlyph;
    }
    if (Glyphs[ID] == NULL)
    {
        FT_Load_Char(Face,ID,FT_LOAD_RENDER);
        NGlyph* NewGlyph = new NGlyph(Face);
        Glyphs[ID] = NewGlyph;
    }
    if (Glyphs[ID]->Rendered)
    {
        return Glyphs[ID];
    }
    FT_Load_Char(Face,ID,FT_LOAD_RENDER);
    FT_GlyphSlot Glyph = Face->glyph;
    NTextureNode* Temp = Node->Insert(glm::vec2(Glyph->bitmap.width+2,Glyph->bitmap.rows+2));
    if (Temp == NULL)
    {
        return Glyphs[ID];
    }
    glBindTexture(GL_TEXTURE_2D, Texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, Temp->Rect.z+1, Temp->Rect.w+1, Glyph->bitmap.width, Glyph->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE, Glyph->bitmap.buffer);
    Glyphs[ID]->Rendered = true;
    Glyphs[ID]->TextureRect = glm::vec4(float(Temp->Rect.z+1)/float(Width), float(Temp->Rect.w+1)/float(Height), float(Glyph->bitmap.width)/float(Width), float(Glyph->bitmap.rows)/float(Height));
    Changed = true;
    return Glyphs[ID];
}

void NTextureAtlas::UpdateMipmaps()
{
    if (!Changed)
    {
        return;
    }
    glBindTexture(GL_TEXTURE_2D, Texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    Changed = false;
}

GLuint NTextureAtlas::GetTexture()
{
    return Texture;
}

NGlyph* NFace::GetGlyph(wchar_t ID, unsigned int Size)
{
    if (Textures.size()<=Size)
    {
        Textures.resize(Size+1,NULL);
    }
    if (Textures[Size] == NULL)
    {
        Textures[Size] = new NTextureAtlas(Face,Size);
    }
    return Textures[Size]->GetGlyph(Face,ID);
}

GLuint NFace::GetTexture(unsigned int Size)
{
    if (Textures.size()<=Size)
    {
        Textures.resize(Size+1,NULL);
    }
    if (Textures[Size] == NULL)
    {
        Textures[Size] = new NTextureAtlas(Face,Size);
    }
    return Textures[Size]->GetTexture();
}

NText::NText(std::string i_Face, std::wstring i_Data) : NNode(NodeText)
{
    Multiline = true;
    Persp = false;
    Face = GetGame()->GetTextSystem()->GetFace(i_Face);
    Data = i_Data;
    Changed = true;
    glGenBuffers(3,Buffers);
    Size = 32;
    Width = 0;
    Mode = 0;
    Velocity = glm::vec2(Rand(-10,10),Rand(-10,10));
    W = 0;
    H = 0;
    Shader = GetGame()->GetRender()->GetShader("text");
    if (Shader != NULL)
    {
        TextureLoc = Shader->GetUniformLocation("Texture");
        MatrixLoc = Shader->GetUniformLocation("MVP");
        ColorLoc = Shader->GetUniformLocation("Color");
    }
    MaskShader = GetGame()->GetRender()->GetShader("flat_textureless");
    if (MaskShader != NULL)
    {
        MMatrixLoc = Shader->GetUniformLocation("MVP");
        MColorLoc = Shader->GetUniformLocation("Color");
    }
}

void NText::SwapView()
{
    Persp = !Persp;
}

void NText::SetBorder(float i_W, float i_H)
{
    W = i_W;
    H = i_H;
    Changed = true;
}

float NText::GetWidth()
{
    if (Width == 0 && Data.size() > 0)
    {
        for (unsigned int i=0;i<Data.size();i++)
        {
            NGlyph* Glyph = Face->GetGlyph(Data[i],Size);
            if (Glyph == NULL)
            {
                continue;
            }
            Width += Glyph->AdvanceX;
        }
    }
    return Width;
}

void NText::GenerateBuffers()
{
    if (!Changed && !GetGame()->GetWindowChanged())
    {
        return;
    }
    if (!Face)
    {
        return;
    }
    Verts.clear();
    UVs.clear();
    std::vector<glm::vec2> Word;
    std::vector<glm::vec2> UVWord;
    float PenX = 0;
    float PenY = 0;
    if (!Multiline)
    {
        float Offset = 0;
        for (unsigned int i=0;i<Data.size();i++)
        {
            NGlyph* Glyph = Face->GetGlyph(Data[i],Size);
            if (Glyph == NULL)
            {
                continue;
            }
            Offset += Glyph->AdvanceX;
        }
        Offset -= W;
        if (Offset > 0)
        {
            PenX -= Offset;
        }
    }
    Width = 0;
    if (Mode == 1 || Mode == 2)
    {
        for (unsigned int i=0;i<Data.size();i++)
        {
            NGlyph* Glyph = Face->GetGlyph(Data[i],Size);
            if (Glyph == NULL)
            {
                continue;
            }
            Width += Glyph->AdvanceX;
        }
    }
    for (unsigned int i=0;i<Data.size();i++)
    {
        NGlyph* Glyph = Face->GetGlyph(Data[i],Size);
        if (Glyph == NULL)
        {
            continue;
        }
        if (Data[i] == ' ' && W != 0)
        {
            float TempX = PenX;
            for (unsigned int o=i+1;o<Data.size();o++)
            {
                NGlyph* GlyphCheck = Face->GetGlyph(Data[o],Size);
                TempX += GlyphCheck->AdvanceX;
                if (Data[o] == ' ')
                {
                    break;
                }
            }
            if (TempX > W)
            {
                if (Multiline)
                {
                    PenX = 0;
                    PenY -= Size;
                }
                continue;
            }
        }
        float X = Glyph->BitmapWidth;
        float Y = Glyph->BitmapHeight;
        float YOff = -Glyph->BitmapHeight+Glyph->BitmapTop-Size/2.f;
        float XOff = Glyph->BitmapLeft;
        switch (Mode)
        {
            case 0: { break; }
            case 1:
            {
                XOff += -Width/2.f;
                break;
            }
            case 2:
            {
                XOff += -Width;
                break;
            }
            default:
            {
                break;
            }
        }
        if (Multiline)
        {
            if (W != 0 && PenX+Glyph->AdvanceX > W )
            {
                    PenX = 0;
                    PenY -= Size;
            }
        }
        Verts.push_back(glm::vec2(PenX+XOff,PenY+YOff));
        Verts.push_back(glm::vec2(PenX+XOff+X,PenY+YOff));
        Verts.push_back(glm::vec2(PenX+XOff+X,Y+PenY+YOff));
        Verts.push_back(glm::vec2(PenX+XOff,Y+PenY+YOff));

        glm::vec4 UV = Glyph->TextureRect;
        UVs.push_back(glm::vec2(UV.x,UV.y+UV.w));
        UVs.push_back(glm::vec2(UV.x+UV.z,UV.y+UV.w));
        UVs.push_back(glm::vec2(UV.x+UV.z,UV.y));
        UVs.push_back(glm::vec2(UV.x,UV.y));
        PenX += Glyph->AdvanceX;
    }
    Mask.push_back(glm::vec2(0,Size/2.f));
    Mask.push_back(glm::vec2(W,Size/2.f));
    Mask.push_back(glm::vec2(W,-H+Size));
    Mask.push_back(glm::vec2(0,-H+Size));
    Changed = false;
    Width = PenX;
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER,Verts.size()*sizeof(glm::vec2),&Verts[0],GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
    glBufferData(GL_ARRAY_BUFFER,UVs.size()*sizeof(glm::vec2),&UVs[0],GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[2]);
    glBufferData(GL_ARRAY_BUFFER,Mask.size()*sizeof(glm::vec2),&Mask[0],GL_STATIC_DRAW);
    Face->UpdateMipmaps();
}

void NText::SetMode(int i_Mode)
{
    Mode = i_Mode;
    Changed = true;
}
void NText::Draw(NCamera* View)
{
    if (!Face)
    {
        return;
    }
    if (W<=0)
    {
        DrawText(View);
        return;
    }
    glClear(GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS,0x1,0x1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    DrawMask(View);
    glStencilFunc(GL_EQUAL,0x1,0x1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    DrawText(View);
    glDisable(GL_STENCIL_TEST);
}

void NText::DrawMask(NCamera* View)
{
    if (GetColor().w == 0)
    {
        return;
    }
    GenerateBuffers();
    if (MaskShader == NULL)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[2]);
        glVertexPointer(2,GL_FLOAT,0,NULL);

        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(&View->GetPerspMatrix()[0][0]);
        glMatrixMode(GL_MODELVIEW);
        glm::mat4 MVP = View->GetViewMatrix();
        glLoadMatrixf(&MVP[0][0]);

        glColor4f(1,1,1,1);
        glDrawArrays(GL_QUADS,0,Mask.size());

        glDisableClientState(GL_VERTEX_ARRAY);
        return;
    }
    glUseProgram(MaskShader->GetID());
    glm::mat4 MVP = View->GetOrthoMatrix()*View->GetViewMatrix()*GetModelMatrix();
    glUniformMatrix4fv(MMatrixLoc,1,GL_FALSE,&MVP[0][0]);
    glUniform4f(MColorLoc,0,0,0,0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[2]);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,NULL);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_QUADS,0,Mask.size());
    glDisable(GL_BLEND);

    glDisableVertexAttribArray(0);
    glUseProgram(0);
}

void NText::DrawText(NCamera* View)
{
    if (Face == NULL || GetColor().w == 0)
    {
        return;
    }
    GenerateBuffers();
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
        glBindTexture(GL_TEXTURE_2D,Face->GetTexture(Size));

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
    glBindTexture(GL_TEXTURE_2D,Face->GetTexture(Size));
    glUniform1i(TextureLoc,0);
    glm::mat4 MVP;
    if (!Persp)
    {
        MVP = View->GetOrthoMatrix()*View->GetViewMatrix()*GetModelMatrix();
    } else {
        MVP = View->GetPerspMatrix()*View->GetPerspViewMatrix()*GetModelMatrix();
    }
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

NText::~NText()
{
    glDeleteBuffers(3,Buffers);
}

NGlyph::~NGlyph()
{
}

void NText::SetText(std::wstring i_Data)
{
    if (!Data.compare(i_Data))
    {
        return;
    }
    Data = i_Data;
    Changed = true;
}

void NText::SetText(std::string i_Data)
{
    std::wstringstream NewData;
    NewData << i_Data.c_str();
    if (!Data.compare(NewData.str()))
    {
        return;
    }
    Data = NewData.str();
    Changed = true;
}

float NText::GetSize()
{
    return Size;
}

void NText::SetSize(float i_Size)
{
    if (Size == i_Size)
    {
        return;
    }
    Size = i_Size;
    Changed = true;
}

FT_Library NTextSystem::GetFreeTypeLib()
{
    return FTLib;
}

void NTextSystem::AddFace(NFace* Face)
{
    Faces.push_back(Face);
}

void NText::Unallocate()
{
    delete (NText*)this;
}

NTextureNode::NTextureNode(glm::vec4 i_Rect)
{
    Parent = NULL;
    Children[0] = NULL;
    Children[1] = NULL;
    Rect = i_Rect;
    HasChildren = false;
    HasImage = false;
}

NTextureNode* NTextureNode::Insert(glm::vec2 i_Rect)
{
    //If we're trying to store a non-existant box, return nothing.
    if (i_Rect.x == 0 || i_Rect.y == 0)
    {
        return NULL;
    }
    //If we're a branch then attempt to insert the rectangle in it's children
    if (HasChildren)
    {
        NTextureNode* Node = Children[0]->Insert(i_Rect);
        if (Node != NULL)
        {
            return Node;
        }
        return Children[1]->Insert(i_Rect);
    }
    //If we're a leaf..
    //If we already have an image here, return null.
    if (HasImage)
    {
        return NULL;
    }
    //If the image doesn't fit, return null.
    if (Rect.x < i_Rect.x || Rect.y < i_Rect.y)
    {
        return NULL;
    }
    //If the image fits perfectly, accept
    if (Rect.x == i_Rect.x && Rect.y == i_Rect.y)
    {
        HasImage = true;
        return this;
    }
    //Otherwise we need to split into more nodes
    //Decide which way to split (horizontal or vertical)
    float W = Rect.x - i_Rect.x;
    float H = Rect.y - i_Rect.y;
    if (W>H)
    {
        //Vertical split
        Children[0] = new NTextureNode(glm::vec4(i_Rect.x,Rect.y,Rect.z,Rect.w));
        Children[1] = new NTextureNode(glm::vec4(W,Rect.y,Rect.z+i_Rect.x,Rect.w));
    } else {
        //Horizontal split
        Children[0] = new NTextureNode(glm::vec4(Rect.x,i_Rect.y,Rect.z,Rect.w));
        Children[1] = new NTextureNode(glm::vec4(Rect.x,H,Rect.z,Rect.w+i_Rect.y));
    }
    HasChildren = true;
    //Then attempt to insert it into the first child we created.
    return Children[0]->Insert(i_Rect);
}

NTextureNode::~NTextureNode()
{
    if (HasChildren)
    {
        delete Children[0];
        delete Children[1];
    }
}
void NText::SetMultiline(bool i_Multiline)
{
    Multiline = i_Multiline;
}
bool NText::GetMultiline()
{
    return Multiline;
}
