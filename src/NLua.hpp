/**
* @file NLua.hpp
* @brief Initializes Lua and exposes it to the rest of the engine.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-02
*/
#ifndef NAELSTROF_LUA
#define NAELSTROF_LUA

/**
* @brief Used in lua to include other files.
*
* @param L This is handled by lua. You shouldn't be calling this directly anyway.
*
* @return 0
*/
int Include(lua_State* L);
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
	bool DoFolder(std::string Folder);
private:
	lua_State* L;
};

#endif
