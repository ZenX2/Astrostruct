#ifndef NAELSTROF_MAP
#define NAELSTROF_MAP

class NTile
{
public:
	NTile();
	~NTile();
	unsigned int ID;
};

class NMap : public NNode
{
public:
	NMap(std::string i_TileSet);
	~NMap();
	void Init(unsigned int i_Width, unsigned int i_Height, unsigned int i_Depth);
	void Draw(NCamera* View);
	void Tick(double DT);
	void Remove();
private:
	NShader* Shader;
	float TileSize;
	unsigned int TextureWidth;
	unsigned int TextureHeight;
	NTexture* Texture;
	unsigned int Width,Height,Depth;
	void GenerateBuffers();
	std::vector<std::vector<std::vector<NTile* > > > Tiles;
	std::vector<GLuint*> Buffers;
	std::vector<std::vector<glm::vec3> > Verts;
	std::vector<std::vector<glm::vec2> > UVs;
	std::vector<bool> Changed;
	GLuint ColorLoc, MatrixLoc, TextureLoc;
};

#endif
