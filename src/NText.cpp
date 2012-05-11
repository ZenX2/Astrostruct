#include "NEngine.hpp"

int LoadFace(lua_State* L)
{
	const char* Name = luaL_checkstring(L,1);
	const char* Data = luaL_checkstring(L,2);
	NFace* FontFace = new NFace(Name);
	if (FontFace->Load(GetGame()->GetScene()->GetTextSystem()->GetFreeTypeLib(),Data))
	{
		GetGame()->GetScene()->GetTextSystem()->AddFace(FontFace);
	} else {
		delete FontFace;
	}
	return 0;
}

NTextSystem::NTextSystem()
{
	if (FT_Init_FreeType(&FTLib))
	{
		SetColor(Yellow);
		std::cout << "FREETYPE WARN: ";
		ClearColor();
		std::cout << "FreeType failed to initialize!\n";
		FTLib = NULL;
	}
}

NTextSystem::~NTextSystem()
{
	for (unsigned int i=0;i<Faces.size();i++)
	{
		delete Faces[i];
	}
	for (unsigned int i=0;i<Texts.size();i++)
	{
		delete Texts[i];
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
	GetGame()->GetLua()->DoFolder("data/fonts");
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

NText* NTextSystem::AddText(std::string Font, std::string Data)
{
	NText* NewText = new NText(GetFace(Font),Data);
	Texts.push_back(NewText);
	return NewText;
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
}

NFace::~NFace()
{
	for (unsigned int i=0;i<Textures.size();i++)
	{
		delete Textures[i];
	}
}

bool NFace::Load(FT_Library FTLib, std::string File)
{
	if (!FTLib)
	{
		return Fail;
	}
	//Load the font face
	if (FT_New_Face(FTLib,File.c_str(),0,&Face))
	{
		SetColor(Yellow);
		std::cout << "FREETYPE WARN: ";
		ClearColor();
		std::cout << "Failed to load " << File << "!\n";
		return Fail;
	}
	return Success;
}

NGlyph::NGlyph(FT_Face Face, float i_X)
{
	FT_GlyphSlot Glyph = Face->glyph;
	AdvanceX = Glyph->advance.x >> 6;
	AdvanceY = Glyph->advance.y >> 6;
	BitmapWidth = Glyph->bitmap.width;
	BitmapHeight = Glyph->bitmap.rows;
	BitmapLeft = Glyph->bitmap_left;
	BitmapTop = Glyph->bitmap_top;
	X = i_X;
	AtlasWidth = 0;
	AtlasHeight = 0;
	Rendered = false;
}

float NGlyph::GetUV()
{
	return X/AtlasWidth;
}

void NGlyph::SetAtlas(float Width, float Height)
{
	AtlasWidth = Width;
	AtlasHeight = Height;
}

NTextureAtlas::NTextureAtlas(FT_Face Face, unsigned int i_Size)
{
	//Get Width and Height of desired textureatlas
	Changed = true;
	Width = 0;
	Height = 0;
	Size = i_Size;
	FT_Set_Pixel_Sizes(Face,0,Size);
	FT_GlyphSlot Glyph = Face->glyph;
	Glyphs.resize(128,NULL);
	for (unsigned int i=32;i<128;i++)
	{
		if (FT_Load_Char(Face,i,FT_LOAD_RENDER))
		{
			//Ignore errors
			continue;
		}
		NGlyph* NewGlyph = new NGlyph(Face,Width);
		Glyphs[i] = NewGlyph;
		Width += Glyph->bitmap.width;
		Height = std::max(Height,Glyph->bitmap.rows);
	}
	//Generate glyph info
	for (unsigned int i=0;i<Glyphs.size();i++)
	{
		if (Glyphs[i] == NULL)
		{
			continue;
		}
		Glyphs[i]->SetAtlas(Width,Height);
	}
	//Now tell opengl to actually create the texture
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &Texture);
	GetGame()->GetRender()->AddCachedTexture(Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, Width, Height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
}

NTextureAtlas::~NTextureAtlas()
{
	for (unsigned int i=0;i<Glyphs.size();i++)
	{
		delete Glyphs[i];
	}
}

unsigned int NTextureAtlas::GetSize()
{
	return Size;
}

NGlyph* NTextureAtlas::GetGlyph(FT_Face Face, unsigned int ID)
{
	if (ID > Glyphs.size())
	{
		return NULL;
	}
	if (Glyphs[ID] == NULL)
	{
		return NULL;
	}
	FT_Set_Pixel_Sizes(Face,0,Size);
	FT_GlyphSlot Glyph = Face->glyph;
	if (Glyph == NULL)
	{
		SetColor(Yellow);
		std::cout << "FREETYPE WARN: ";
		ClearColor();
		std::cout << "Failed to load glyph for " << (char)ID << "!\n";
	}
	if (!Glyphs[ID]->Rendered)
	{
		if (FT_Load_Char(Face,ID,FT_LOAD_RENDER))
		{
			return NULL;
		}
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, Glyphs[ID]->X, 0, Glyph->bitmap.width, Glyph->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE, Glyph->bitmap.buffer);
		Glyphs[ID]->Rendered = true;
		Changed = true;
	}
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

NGlyph* NFace::GetGlyph(unsigned int ID, unsigned int Size)
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
	return Textures[Size]->GetTexture();
}

NText::NText(NFace* i_Face, std::string i_Data) : NNode()
{
	Face = i_Face;
	Shader = GetGame()->GetRender()->GetShader("text");
	Data = i_Data;
	Changed = true;
	Buffers = new GLuint[2];
	glGenBuffers(2,Buffers);
	Size = 32;
	Width = 0;
	Mode = 0;
	Velocity = glm::vec2(Rand(-10,10),Rand(-10,10));
	if (Shader != NULL)
	{
		TextureLoc = Shader->GetUniformLocation("Texture");
		MatrixLoc = Shader->GetUniformLocation("ModelView");
		ScreenLoc = Shader->GetUniformLocation("Screen");
		ColorLoc = Shader->GetUniformLocation("Color");
	}
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
	Verts.clear();
	UVs.clear();
	float PenX = 0;
	for (unsigned int i=0;i<Data.size();i++)
	{
		NGlyph* Glyph = Face->GetGlyph(Data[i],Size);
		if (Glyph == NULL)
		{
			continue;
		}
		if (!Glyph->BitmapWidth || !Glyph->BitmapHeight)
		{
			PenX += Glyph->AdvanceX;
			continue;
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
		Verts.push_back(glm::vec2(XOff+PenX,YOff));
		Verts.push_back(glm::vec2(XOff+PenX+X,YOff));
		Verts.push_back(glm::vec2(XOff+PenX+X,Y+YOff));
		Verts.push_back(glm::vec2(XOff+PenX,Y+YOff));

		float UV = Glyph->GetUV();
		float UV2 = UV+Glyph->BitmapWidth/Glyph->AtlasWidth;
		float UV3 = Glyph->BitmapHeight/Glyph->AtlasHeight;
		UVs.push_back(glm::vec2(UV,UV3));
		UVs.push_back(glm::vec2(UV2,UV3));
		UVs.push_back(glm::vec2(UV2,0));
		UVs.push_back(glm::vec2(UV,0));
		PenX += Glyph->AdvanceX;
	}
	Changed = false;
	if (Width == 0 && Mode != 0)
	{
		Changed = true;
	}
	Width = PenX;
	glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
	glBufferData(GL_ARRAY_BUFFER,Verts.size()*sizeof(glm::vec2),&Verts[0],GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
	glBufferData(GL_ARRAY_BUFFER,UVs.size()*sizeof(glm::vec2),&UVs[0],GL_STATIC_DRAW);
	Face->UpdateMipmaps();
}

void NText::SetMode(int i_Mode)
{
	Mode = i_Mode;
	Changed = true;
}

void NText::Draw(glm::mat4 View)
{
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
		glPushMatrix();
		glColor4fv(&(GetColor()[0]));
		glTranslatef(GetPos().x,GetPos().y,0);
		glRotatef(GetAng(),0,0,1);
		glScalef(GetScale().x,GetScale().y,0);
		glDrawArrays(GL_QUADS,0,Verts.size());
		glPopMatrix();
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
	glm::mat4 ModelView = GetModelMatrix()*View;
	glUniformMatrix4fv(MatrixLoc,1,GL_FALSE,&ModelView[0][0]);
	glUniform2f(ScreenLoc,GetGame()->GetWindowWidth()/2.f,GetGame()->GetWindowHeight()/2.f);
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
	glDeleteBuffers(2,Buffers);
	delete[] Buffers;
}

NGlyph::~NGlyph()
{
}

void NText::Tick(double DT)
{
	/*SetPos(GetPos()+Velocity);
	Velocity.y-=DT*4.f;
	if (GetPos().y<0)
	{
		Velocity.y *= -0.9;
		SetPos(GetPos().x,0);
	}
	if (GetPos().y>GetGame()->GetWindowHeight())
	{
		Velocity.y *= -0.9;
		SetPos(GetPos().x,GetGame()->GetWindowHeight());
	}
	if (GetPos().x<0)
	{
		Velocity.x *= -0.9;
		SetPos(0,GetPos().y);
	}
	if (GetPos().x>GetGame()->GetWindowWidth())
	{
		Velocity.x *= -0.9;
		SetPos(GetGame()->GetWindowWidth(),GetPos().y);
	}
	SetAng(GetAng()+30*DT);
	SetScale(glm::vec2(1.f)+GetPos()/GetGame()->GetWindowSize()*2.f);
	SetColor(glm::vec4(0,GetPos()/GetGame()->GetWindowSize(),1));*/
}

void NText::SetText(std::string i_Data)
{
	if (!Data.compare(i_Data))
	{
		return;
	}
	Data = i_Data;
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
