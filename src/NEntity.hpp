#ifndef NAELSTROF_ENTITY
#define NAELSTROF_ENTITY

/**
 * @brief Stores a loaded lua entity in memory.
 */
class NLuaEntity
{
public:
    /**
     * @brief Stores a lua entity in memory.
     *
     * @param i_Name The name of the entity.
     * @param i_LuaReference The reference to the loaded lua stuff.
     */
    NLuaEntity(std::string i_Name, int i_LuaReference);
    std::string Name;
    int LuaReference;
};

/**
 * @brief Handles loading and grabbing lua entities from memory.
 */
class NEntityManager
{
public:
    /**
     * @brief Loads all entities in the data/gamemodes/<current gamemode>/entities
     */
    NEntityManager();
    ~NEntityManager();
    /**
     * @brief Reloads all entities, useful for when we change gamemodes, or detect changes.
     */
    void Reload();
    /**
     * @brief Grabs a lua entity from memory by name.
     *
     * @param Name The name of the entity we want.
     *
     * @return A lua reference to the entity.
     */
    int GetLuaEntity(std::string Name);
    /**
     * @brief Grabs a list of the entity names loaded.
     *
     * @return A vector full of entity names.
     */
    std::vector<std::string> GetEntityNames();
private:
    std::vector<NLuaEntity*> Entities;
};

/**
 * @brief A game entity that is powered by lua.
 */
class NEntity : public NNode
{
public:
    /**
     * @brief Creates and adds an entity to the scene powered by the lua entity name specified.
     *
     * @param i_Name The name of the lua entity to power this node.
     */
    NEntity(std::string i_Name);
    /**
     * @brief Does the same as the default constructor, except sets it's position before calling the lua's "OnInitialize" function.
     *
     * @param i_Name The name of hte lua entity to power this node.
     * @param i_Position The desired position of the node.
     */
    NEntity(std::string i_Name, glm::vec3 i_Position);
    ~NEntity();
    /**
     * @brief Moves the texture forward in time and calls the lua's "OnTick" function.
     *
     * @param DT The time passed since last execution.
     */
    void Tick(double DT);
    /**
     * @brief Draws the entity if it has a texture and size.
     *
     * @param View The camera to draw it from.
     */
    void Draw(NCamera* View);
    /**
     * @brief Unallocates the node.
     */
    void Unallocate();
    int SelfReference;
    /**
     * @brief Calls the lua function with the given arguments, FIXME: make it parse varying arguments.
     *
     * @param Name The name of the function.
     * @param AdditionalVars The number of additional arguments.
     * @param ... The additional arguments.
     */
    void CallMethod(std::string Name, unsigned int AdditionalVars = 0, ...);
    /**
     * @brief Gets a string from the loaded lua reference or returns NULL if it doesn't exist.
     *
     * @param Name The name of the string.
     *
     * @return The string or "NULL".
     */
    std::string GetString(std::string Name);
    /**
     * @brief Gets a float from the loaded lua reference or 0 if it doesn't exit.
     *
     * @param Name The name of hte float.
     *
     * @return The float or 0.
     */
    float GetFloat(std::string Name);
    /**
     * @brief Gets the name of the lua entity.
     *
     * @return The name of the lua entity.
     */
    std::string GetName();
private:
    std::string Name;
    int LuaSelf;
    float Friction;
    NShader* Shader;
    GLuint TextureLoc, MatrixLoc, ColorLoc;
    bool Changed;
    GLuint Buffers[2];
    void GenerateBuffers();
    std::vector<glm::vec2> Verts;
    std::vector<glm::vec2> UVs;
    NTexture* Texture;
};

#endif
