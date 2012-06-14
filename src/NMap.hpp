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
	NTile(unsigned int x,unsigned int y,unsigned int z);
	~NTile();
	bool IsOpaque();
	/**
	 * @brief Compares the ID to static numbers to decide if the tile is solid or not, it can be overridden by SetSolid.
	 *
	 * @return True if it's solid, false if it's not.
	 */
	bool IsSolid();
	/**
	 * @brief Returns if the tile exists by comparing the ID to 0.
	 *
	 * @return True if it doesn't exist, false if it does.
	 */
	bool IsOpenSpace();
	unsigned int ID;
	/**
	 * @brief Forcibly changes the tile's solid-ness. Useful for stepping on top of 3D blocks.
	 *
	 * @param i_Solid True for solid, false for nothingness.
	 */
	void SetSolid(bool i_Solid);
	unsigned int X,Y,Z;
private:
	bool ForceSolid;
	bool Solid;
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
	/**
	 * @brief Disables rendering of levels out of view.
	 *
	 * @param Level The level we want to render at.
	 */
	void ViewLevel(int Level);
	/**
	 * @brief Scans the map for any inconsistancies and fixes them. Right now it just ensures that non-solid wall blocks are above wall blocks.
	 */
	void FixUp();
	/**
	 * @brief Returns the current viewing level.
	 *
	 * @return The level set by ViewLevel(int Level);
	 */
	int GetLevel();
	/**
	 * @brief Returns how large tiles are, currently reads it from the tileset texture, but I should make it scaleable.
	 *
	 * @return The size of tiles in pixels.
	 */
	float GetTileSize();
	/**
	 * @brief Gets how many depth layers exist.
	 *
	 * @return The amount of Depth layers.
	 */
	unsigned int GetDepth();
	/**
	 * @brief Translates a 3d position in pixels into a tile that intersects with it. Returns NULL if the position is outside the bounds of the map.
	 *
	 * @param Pos The position.
	 *
	 * @return A pointer to a tile that intersects with the position.
	 */
	NTile* GetTile(glm::vec3 Pos);
	/**
	 * @brief Returns the center position of the tile that intersects with the point Pos.
	 *
	 * @param Pos A position in pixels.
	 *
	 * @return The position of the tile centered around the tile's origin.
	 */
	glm::vec3 TilePos(glm::vec3 Pos);
	NTile* GetTile(unsigned int X, unsigned int Y, unsigned int Z);
	bool Ready;
	std::string Type();
	int GetLevel(glm::vec3 Pos);
private:
	unsigned int ViewingLevel;
	NShader* Shader;
	NShader* OutlineShader;
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
	std::vector<std::vector<glm::vec3> > Outline;
	std::vector<std::vector<glm::vec3> > BoxVerts;
	std::vector<std::vector<glm::vec2> > BoxUVs;
	std::vector<bool> Changed;
	GLuint ColorLoc, MatrixLoc, TextureLoc, OutlineColorLoc, OutlineMatrixLoc;
};

#endif
