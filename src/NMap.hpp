/**
 * @file NMap.hpp
 * @brief Draws a map and handles air and water inside of it.
 * @author Dalton Nell
 * @version 1.0
 * @date 2012-05-26
 */
#ifndef NAELSTROF_MAP
#define NAELSTROF_MAP

/**
 * @brief A single unit in the map, can be walls, floor, empty space, anything map specific.
 */
class NTile
{
public:
	NTile();
	~NTile();
	unsigned int ID;
};

/**
 * @brief Assembles a 3d grid of tiles and draws them. It also handles air and water simulation, but that's not quite implemented yet.
 */
class NMap : public NNode
{
public:
	/**
	 * @brief Initializes the map with the specified tileset. The tileset should be a texture with "TileSize" specified within one of it's animations, else it won't work properly!
	 *
	 * @param i_TileSet The name of the texture.
	 */
	NMap(std::string i_TileSet);
	~NMap();
	/**
	 * @brief Creates a grid of the specified size and fills it with random tiles.
	 *
	 * @param i_Width The desired hidth.
	 * @param i_Height The desired height.
	 * @param i_Depth The desired depth.
	 */
	void Init(unsigned int i_Width, unsigned int i_Height, unsigned int i_Depth);
	/**
	 * @brief Draws the map.
	 *
	 * @param View The camera to render from.
	 */
	void Draw(NCamera* View);
	/**
	 * @brief Moves the map forward in time, also moves the water and air simulations forward.
	 *
	 * @param DT Time passed since last execution.
	 */
	void Tick(double DT);
	/**
	 * @brief Unallocates the memory in use by this class.
	 */
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
