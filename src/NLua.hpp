/**
* @file NLua.hpp
* @brief Initializes Lua and exposes it to the rest of the engine.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-02
*/
#ifndef NAELSTROF_LUA
#define NAELSTROF_LUA

#define luaL_ndostring(L, s, n) \
            (luaL_nloadstring(L, s, n) || lua_pcall(L, 0, LUA_MULTRET, 0))

int LuaGetEntitiesByName(lua_State* L);
int MapLoad(lua_State* L);
int MapSave(lua_State* L);
int OnPanic(lua_State* L);
int LoadShader(lua_State* L);
int lua_protcall(lua_State* L, int nargs, int nresults);
int lua_nloadstring(lua_State* L, const char* s, const char* name);
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
enum LanguageElement
{
    UIMakeMap = 1,
    UIPlayOnline,
    UIPlay,
    UIQuit,
    UIIntro,
    UIMapEditIntro,
    UITile,
    UISolid,
    UIOpaque,
    UISave,
    UILoad,
    UIMapDim,
    UIWidth,
    UIHeight,
    UIDepth,
    UIApply
};
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
    std::string GetCurrentDoFile();
    void SetCurrentDoFile(std::string File);
    std::wstring Translate(LanguageElement Foo);
private:
    lua_State* L;
    std::string CurrentDoFile;
    std::string Language;
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
int LightGetPos(lua_State* L);
int LightSetRadius(lua_State* L);
int LightSetColor(lua_State* L);
void lua_pushLight(lua_State* L, NLight* Light);
int LightRemove(lua_State* L);

int CreateStaticCube(lua_State* L);
int StaticCube__index(lua_State* L);
int StaticCube__newindex(lua_State* L);
NStaticCube* lua_checkStaticCube(lua_State* L, int narg);
NStaticCube* lua_toStaticCube(lua_State* L, int index);
int StaticCubeSetSolid(lua_State* L);
int StaticCubeIsSolid(lua_State* L);
void lua_pushStaticCube(lua_State* L, NStaticCube* StaticCube);
int StaticCubeRemove(lua_State* L);

int Tile__index(lua_State* L);
int Tile__newindex(lua_State* L);
NTile* lua_toTile(lua_State* L, int index);
NTile* lua_checkTile(lua_State* L, int narg);
int TileGetPos(lua_State* L);
void lua_pushTile(lua_State* L, NTile* Tile);

int Map__index(lua_State* L);
int Map__newindex(lua_State* L);
NMap* lua_toMap(lua_State* L, int index);
NMap* lua_checkMap(lua_State* L, int narg);
void lua_pushMap(lua_State* L, NMap* Map);

int Player__index(lua_State* L);
int PlayerSetPos(lua_State* L);
int Player__newindex(lua_State* L);
NPlayer* lua_toPlayer(lua_State* L, int index);
NPlayer* lua_checkPlayer(lua_State* L, int narg);
void lua_pushPlayer(lua_State* L, NPlayer* Player);

int Entity__index(lua_State* L);
int Entity__newindex(lua_State* L);
NEntity* lua_toEntity(lua_State* L, int index);
NEntity* lua_checkEntity(lua_State* L, int narg);
void lua_pushEntity(lua_State* L, NEntity* Entity);
int EntityGetPos(lua_State* L);
int EntitySetPos(lua_State* L);
int EntityPlayAnim(lua_State* L);
int EntityResetAnimOnPlay(lua_State* L);

#endif
