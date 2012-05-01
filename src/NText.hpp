#ifndef NAELSTROF_TEXT
#define NAELSTROF_TEXT

class NGlyph
{
public:
	NGlyph(FT_Face,float);
	~NGlyph();
	void SetAtlas(float,float);
	float GetUV();
	bool Rendered;
	float AdvanceX;
	float AdvanceY;
	float BitmapWidth;
	float BitmapHeight;
	float BitmapLeft;
	float BitmapTop;
	float AtlasWidth;
	float AtlasHeight;
	float X;
};

class NTextureAtlas
{
public:
	NTextureAtlas(FT_Face, unsigned int);
	~NTextureAtlas();
	void Apply();
	NGlyph* GetGlyph(FT_Face, unsigned int);
	unsigned int GetSize();
	GLuint GetTexture();
	void UpdateMipmaps();
private:
	bool Changed;
	int Size;
	int Width, Height;
	GLuint Texture;
	std::vector<NGlyph*> Glyphs;
};

class NFace
{
public:
	NFace(std::string);
	~NFace();
	GLuint GetTexture(unsigned int);
	NGlyph* GetGlyph(unsigned int, unsigned int);
	bool Load(FT_Library, std::string);
	std::string GetName();
	void UpdateMipmaps();
private:
	std::string Name;
	FT_Face Face;
	std::vector<NTextureAtlas*> Textures;
};

class Text : public NNode
{
public:
	Text(std::string,std::string);
	void Draw(glm::mat4);
	float GetWidth();
	void Tick(double);
	void SetMode(int);
	~Text();
private:
	int Mode;
	float Width;
	float Size;
	void GenerateBuffers();
	NShader* Shader;
	NFace* Face;
	std::string Data;
	std::vector<glm::vec2> Verts;
	std::vector<glm::vec2> UVs;
	bool Changed;
	GLuint* Buffers;
};

class NTextSystem
{
public:
	NTextSystem();
	~NTextSystem();
	void LoadFaces();
	NFace* GetFace(std::string);
	Text* AddText(std::string,std::string);
	void UpdateMipmaps();
private:
	FT_Library FTLib;
	std::vector<NFace*> Faces;
	std::vector<Text*> Texts;
};

#endif
