#include "NEngine.hpp"

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
	NFace* FontFace = new NFace("opensans");
	if (FontFace->Load(FTLib,"data/fonts/OpenSans.ttf"))
	{
		Faces.push_back(FontFace);
	} else {
		delete FontFace;
	}
}

void NTextSystem::UpdateMipmaps()
{
	for (unsigned int i=0;i<Faces.size();i++)
	{
		Faces[i]->UpdateMipmaps();
	}
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

Text* NTextSystem::AddText(std::string Font, std::string Data)
{
	Text* NewText = new Text(Font,Data);
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
	glBindTexture(GL_TEXTURE_2D, Texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, Width, Height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

NTextureAtlas::~NTextureAtlas()
{
	for (unsigned int i=0;i<Glyphs.size();i++)
	{
		delete Glyphs[i];
	}
	glDeleteTextures(1, &Texture);
}

unsigned int NTextureAtlas::GetSize()
{
	return Size;
}

NGlyph* NTextureAtlas::GetGlyph(FT_Face Face, unsigned int ID)
{
	if (Glyphs[ID] == NULL)
	{
		return NULL;
	}
	FT_GlyphSlot Glyph = Face->glyph;
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
	if (Textures.size()<Size)
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

Text::Text(std::string Font, std::string i_Data) : NNode()
{
	Face = GetGame()->GetTextSystem()->GetFace(Font);
	Shader = GetGame()->GetRender()->GetShader("flat");
	Data = i_Data;
	Changed = true;
	Buffers = new GLuint[2];
	glGenBuffers(2,Buffers);
	Size = 32;
	Width = 0;
	Mode = 0;
}

float Text::GetWidth()
{
	return Width;
}

void Text::GenerateBuffers()
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
		if (!Glyph->BitmapWidth || !Glyph->BitmapHeight)
		{
			PenX += Glyph->AdvanceX;
			continue;
		}
		float X = Glyph->BitmapWidth+Glyph->BitmapLeft;
		float Y = Glyph->BitmapHeight;
		float YOff = -Glyph->BitmapHeight+Glyph->BitmapTop-Size/2.f;
		float XOff = 0;
		switch (Mode)
		{
			case 0: { break; }
			case 1:
			{
				XOff = -Width/2.f;
				break;
			}
			case 2:
			{
				XOff = -Width;
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
		Verts.push_back(glm::vec2(XOff+PenX,YOff));
		Verts.push_back(glm::vec2(XOff+PenX,Y+YOff));
		Verts.push_back(glm::vec2(XOff+PenX+X,Y+YOff));

		float UV = Glyph->GetUV();
		float UV2 = UV+Glyph->BitmapWidth/Glyph->AtlasWidth;
		float UV3 = Glyph->BitmapHeight/Glyph->AtlasHeight;
		UVs.push_back(glm::vec2(UV,UV3));
		UVs.push_back(glm::vec2(UV2,UV3));
		UVs.push_back(glm::vec2(UV2,0));
		UVs.push_back(glm::vec2(UV,UV3));
		UVs.push_back(glm::vec2(UV,0));
		UVs.push_back(glm::vec2(UV2,0));
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
}

void Text::SetMode(int i_Mode)
{
	Mode = i_Mode;
	Changed = true;
}

void Text::Draw(glm::mat4 View)
{
	GenerateBuffers();
	glUseProgram(Shader->GetID());
	GLuint Sampler = glGetUniformLocation(Shader->GetID(), "Texture");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,Face->GetTexture(Size));
	glUniform1i(Sampler,0);
	glm::mat4 ModelView = GetModelMatrix()*View;
	glUniformMatrix4fv(glGetUniformLocation(Shader->GetID(), "ModelView"),1,GL_FALSE,&ModelView[0][0]);
	glUniform2f(glGetUniformLocation(Shader->GetID(), "Screen"),GetGame()->GetWindowWidth()/2.f,GetGame()->GetWindowHeight()/2.f);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,NULL);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glDrawArrays(GL_TRIANGLES,0,Verts.size());
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glUseProgram(0);
}

Text::~Text()
{
	glDeleteBuffers(2,Buffers);
	delete[] Buffers;
}

NGlyph::~NGlyph()
{
}

void Text::Tick(double DT)
{
	if (!GetParent())
	{
		SetPos(GetGame()->GetInput()->GetMouse());
		SetAng(GetAng()+DT*30);
	} else {
		SetAng(GetAng()-DT*60);
	}
}
