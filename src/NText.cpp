#include "NEngine.hpp"

int LoadFace(lua_State* L)
{
	const char* Name = luaL_checkstring(L,1);
	const char* Data = luaL_checkstring(L,2);
	std::tstringstream tName, tData;
	tName << Name;
	tData << Data;
	NFace* FontFace = new NFace(tName.str());
	if (FontFace->Load(GetGame()->GetTextSystem()->GetFreeTypeLib(),tData.str()))
	{
		GetGame()->GetTextSystem()->AddFace(FontFace);
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

std::tstring NFace::GetName()
{
	return Name;
}

NFace* NTextSystem::GetFace(std::tstring Name)
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

NFace::NFace(std::tstring i_Name)
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

bool NFace::Load(FT_Library FTLib, std::tstring File)
{
	if (!FTLib)
	{
		return Fail;
	}
	
	//Load the font face
	if (FT_New_Face(FTLib,ToMBS(File).c_str(),0,&Face))
	{
		SetColor(Yellow);
		std::tcout << _T("FREETYPE WARN: ");
		ClearColor();
		std::tcout << _T("Failed to load ") << File << _T("!\n");
		return Fail;
	}
	
	return Success;
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
	Width = 256;
	Height = 256;
	Size = i_Size;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &Texture);
	GetGame()->GetRender()->AddCachedTexture(Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);
	unsigned char* Data = new unsigned char[Width*Height];
	for (unsigned int i=0;i<Width*Height;i++)
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
}

unsigned int NTextureAtlas::GetSize()
{
	return Size;
}

NGlyph* NTextureAtlas::GetGlyph(FT_Face Face, tchar ID)
{
	FT_Set_Pixel_Sizes(Face,0,Size);
	if (ID > Glyphs.size())
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

NGlyph* NFace::GetGlyph(tchar ID, unsigned int Size)
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

NText::NText(std::tstring i_Face, std::tstring i_Data) : NNode()
{
	Face = GetGame()->GetTextSystem()->GetFace(i_Face);
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
		MatrixLoc = Shader->GetUniformLocation("MVP");
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

		glm::vec4 UV = Glyph->TextureRect;
		UVs.push_back(glm::vec2(UV.x,UV.y+UV.w));
		UVs.push_back(glm::vec2(UV.x+UV.z,UV.y+UV.w));
		UVs.push_back(glm::vec2(UV.x+UV.z,UV.y));
		UVs.push_back(glm::vec2(UV.x,UV.y));
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

void NText::Draw(NCamera* View)
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
	glm::mat4 MVP = View->GetOrthoMatrix()*View->GetViewMatrix()*GetModelMatrix();
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
	glDeleteBuffers(2,Buffers);
	delete[] Buffers;
}

NGlyph::~NGlyph()
{
}

void NText::SetText(std::tstring i_Data)
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

void NText::Remove()
{
    delete (NText*)this;
}
/*
class NTextureNode
{
public:
	NTextureNode* Insert(glm::vec2 Rect);
private:
	glm::vec4 Rect;
	bool HasChildren;
	NTextureNode Children[2];
	NTextureNode* Parent;
};*/

NTextureNode::NTextureNode(glm::vec4 i_Rect)
{
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
