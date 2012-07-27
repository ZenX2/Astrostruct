/**
* @file NLua.hpp
* @brief Initializes Lua and exposes it to the rest of the engine.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-02
*/
#ifndef NAELSTROF_LUA
#define NAELSTROF_LUA

int MapLoad(lua_State* L);
int MapSave(lua_State* L);
/**
* @brief Used in lua to include other files.
*
* @param L This is handled by lua. You shouldn't be calling this directly anyway.
*
* @return 0
*/
int Include(lua_State* L);
/**
 * @brief Prints help on how to use the lua console to console.
 *
 * @param L The lua state.
 *
 * @return 0
 */
int ConsoleHelp(lua_State* L);
/**
 * @brief Exits the game via lua. It produces an INFO message in order to make malicious scripts more obvious.
 *
 * @param L The lua state.
 *
 * @return 0
 */
int Quit(lua_State* L);
int Print(lua_State* L);
/**
* @brief Class that initializes and exposes lua to the rest of the engine.
*/
class NLua
{
public:
    NLua();
    ~NLua();
    /**
    * @brief Gets the lua state.
    *
    * @return The lua state.
    */
    lua_State* GetL();
    /**
    * @brief Reads a lua file into lua, if there's errors it will output them.
    *
    * @param File The path to the lua file.
    *
    * @return False on failure, true on success.
    */
    bool DoFile(std::string File);
    /**
     * @brief Recursviely searches a folder for files preceded by '.lua' and runs them through lua.
     *
     * @param Folder The desired folder to search through.
     *
     * @return True on success, false if an error occured in one of the lua files.
     */
    bool DoFolder(std::string Folder);
private:
    lua_State* L;
};
/**
* @brief Lua function to load textures into memory.
*
* @param L The lua state.
*
* @return 1
*/
int CreateAnimation(lua_State* L);
/**
* @brief AnimationBase __newindex function for lua.
*
* @param L The lua state.
*
* @return 0
*/
int Animation__newindex(lua_State* L);
/**
* @brief Loads a texture into memory, it's used by lua and takes infinite arguments. The first argument is the desired name of the texture and the rest are AnimationBase objects.
*
* @param L The lua state.
*
* @return 0
*/
int LoadTexture(lua_State* L);
/**
* @brief Takes the argument from lua and transforms it into a pointer to the NAnimation class.
*
* @param L The lua state.
* @param narg The number of the arg given to the lua function.
*
* @return A pointer to the NAnimation class.
*/
NAnimation* lua_checkAnimation(lua_State* L, int narg);
NAnimation* lua_toAnimation(lua_State* L, int index);
void lua_pushAnimation(lua_State* L, NAnimation* Animation);

/**
* @brief Lua function that loads font faces into memory.
*
* @param L The lua state.
*
* @return 0
*/
int LoadFace(lua_State* L);
/**
 * @brief Lua function that loads sounds into memory.
 *
 * @param L The lua state.
 *
 * @return 0
 */
int LoadSound(lua_State* L);

int CreateVector(lua_State* L);
int Vector__index(lua_State* L);
int Vector__newindex(lua_State* L);
int Vector__sub(lua_State* L);
int Vector__add(lua_State* L);
glm::vec3* lua_checkVector(lua_State* L, int narg);
glm::vec3* lua_toVector(lua_State* L, int index);
void lua_pushVector(lua_State* L, glm::vec3 Vector);

int CreateColor(lua_State* L);
int Color__index(lua_State* L);
int Color__newindex(lua_State* L);
int Color__sub(lua_State* L);
int Color__add(lua_State* L);
glm::vec4* lua_checkColor(lua_State* L, int narg);
glm::vec4* lua_toColor(lua_State* L, int index);
void lua_pushColor(lua_State* L, glm::vec4 Color);

int CreateLight(lua_State* L);
int Light__index(lua_State* L);
int Light__newindex(lua_State* L);
NLight* lua_checkLight(lua_State* L, int narg);
NLight* lua_toLight(lua_State* L, int index);
int LightSetPos(lua_State* L);
int LightSetRadius(lua_State* L);
int LightSetColor(lua_State* L);
void lua_pushLight(lua_State* L, NLight* Light);
int LightRemove(lua_State* L);

int Tile__index(lua_State* L);
int Tile__newindex(lua_State* L);
NTile* lua_toTile(lua_State* L, int index);
NTile* lua_checkTile(lua_State* L, int narg);
int TileGetPos(lua_State* L);
void lua_pushTile(lua_State* L, NTile* Tile);
#endif
