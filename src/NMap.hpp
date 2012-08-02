/**
 * @file NMap.hpp
 * @brief Draws a map and handles air and water inside of it.
 * @author Dalton Nell
 * @version 1.0
 * @date 2012-05-26
 */
#ifndef NAELSTROF_MAP
#define NAELSTROF_MAP

enum TileSlope
{
    SlopeNone,
    SlopeNorth,
    SlopeEast,
    SlopeSouth,
    SlopeWest
};

/**
 * @brief A single unit in the map, can be walls, floor, empty space, anything map specific.
 */
class NTile
{
public:
    /**
     * @brief Initializes a tile at the specified position, useful for when you only have a pointer to a tile and need a tile next to it. It does NOT actually set the position of the tile.
     *
     * @param x The X position.
     * @param y The Y position.
     * @param z The Z position.
     */
    NTile(unsigned int x,unsigned int y,unsigned int z);
    /**
     * @brief Generates a quick tile that can be used to test if it's solid by default or whatever.
     *
     * @param i_ID The desired ID of the tile.
     */
    NTile(unsigned int i_ID);
    ~NTile();
    /**
     * @brief Returns if a tile blocks light or not, useful for glass tiles.
     *
     * @return True if it blocks light, false if it doesn't.
     */
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
    void SetOpaque(bool i_Opaque);
    /**
     * @brief Sets the ID of the tile and tells the map to regenerate itself.
     *
     * @param i_ID The desired ID of the tile.
     */
    void SetID(int i_ID);
    unsigned int X,Y,Z;
    /**
     * @brief Returns the current slope of the tile.
     *
     * @return A TileSlope enum.
     */
    TileSlope GetSlope();
    /**
     * @brief Sets the slope of the tile.
     *
     * @param i_Slope The desired slope of the tile as a TileSlope enum.
     */
    void SetSlope(TileSlope i_Slope);
    bool GetBool(std::string Name);
    int LuaReference;
    int SelfReference;
    void CallMethod(std::string Name);
private:
    TileSlope Slope;
    bool ForceSolid;
    bool Solid;
    bool dSolid;
    bool ForceOpaque;
    bool Opaque;
    bool dOpaque;
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
    void DeInit();
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
    void Unallocate();
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
    /**
     * @brief Returns the tile at the location in the vector.
     *
     * @param X The X position.
     * @param Y The Y position.
     * @param Z The Z position.
     *
     * @return A pointer to the tile at the position, or NULL if one of the numbers is out of bounds.
     */
    NTile* GetTile(unsigned int X, unsigned int Y, unsigned int Z);
    bool Ready;
    /**
     * @brief Returns the current level that a position resides on. Currently use to decide when to render objects or not.
     *
     * @param Pos The position.
     *
     * @return The Z level the position resides on.
     */
    int GetLevel(glm::vec3 Pos);
    /**
     * @brief Sets the level to a changed state, in which the vertex buffers would be regenerated for.
     *
     * @param Level The level we want to regenerate.
     */
    void SetChanged(int Level);
    /**
     * @brief Saves the current map using the specified name to maps/Name.map.
     *
     * @param Name The desired name of the map.
     *
     * @return True on success, false otherwise.
     */
    bool Save(std::string Name);
    /**
     * @brief Loads the map using the specified name from maps/Name.map.
     *
     * @param Name The desired name of the map we want to load.
     *
     * @return True on success, false if there was a problem.
     */
    bool Load(std::string Name);
    /**
     * @brief Returns the width of the map.
     *
     * @return The width of the map.
     */
    unsigned int GetWidth();
    /**
     * @brief Returns the height of the map.
     *
     * @return The height of the map.
     */
    unsigned int GetHeight();
    /**
     * @brief Returns how many tiles should exist in a texture skin.
     *
     * @return Currently, 4.
     */
    unsigned int GetTileCount();
    int GetLuaTile(unsigned int ID);
    int SelfReference;
    void CallMethod(std::string Name, unsigned int AdditionalVars = 0, ...);
    int GetGameMode();
    void Resize(unsigned int X, unsigned int Y, unsigned int Z);
private:
    int LuaReference;
    unsigned int MaxTiles;
    std::vector<int> LuaTiles;
    float RealTileSize;
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
