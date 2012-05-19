#ifndef NAELSTROF_MAP
#define NAELSTROF_MAP

class NTile
{
public:
	NTile();
	~NTile();
private:
	unsigned int ID;
};

class NMap : public NNode
{
public:
	NMap();
	~NMap();
	void Init(unsigned int i_Width, unsigned int i_Height, unsigned int i_Depth);
	void Draw(glm::mat4 View);
private:
	unsigned int Width,Height,Depth;
	void GenerateBuffers();
	unsigned int Floor;
	std::vector<std::vector<std::vector<NTile* > > > Tiles;
	std::vector<GLuint*> Buffers;
};

#endif
