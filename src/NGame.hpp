#ifndef NAELSTROF_GAME
#define NAELSTROF_GAME
/**
* @file NGame.hpp
* @brief Wraps together all the other interfaces so they can interact with eachother.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-02
*/
/**
* @brief This call is used as a glfw hook for window resize events, but I guess you can use it to change the window size too. Try to use NGame's interface to change the window size though.
*
* @param W Desired width.
* @param H Desired height.
*/
void ResizeWindow(int W, int H);

/**
* @brief Wraps together all the other interfaces so they can interact with eachother.
*/
class NGame
{
public:
	NGame();
	~NGame();
	/**
	* @brief Creates a glfw window, and initializes all necessary parts of the engine. (freetype, glew, glfw, opengl, etc)
	*
	* @param Width Desired width of the window.
	* @param Height Desired height of the window.
	* @param Title Desired title of the window.
	* @param argc The amount of arguments.
	* @param argv The arguments passed to main().
	*
	* @return True on success, false on failure.
	*/
	bool Init(int Width, int Height,std::string Title, int argc, char* argv[]);
	/**
	* @brief Grabs the input interface for other parts of the engine to use.
	*
	* @return A pointer to the input interface.
	*/
	NInput* GetInput();
	/**
	* @brief Grabs the rendering interface for other parts of the engine to use.
	*
	* @return A pointer to the render interface.
	*/
	NRender* GetRender();
	/**
	* @brief Grabs the scene for other parts of the engine to use.
	*
	* @return A pointer to the scene.
	*/
	NScene* GetScene();
	/**
	* @brief Grabs the text interface for other parts of the engine to use.
	*
	* @return A pointer to the text interface.
	*/
	NTextSystem* GetTextSystem();
	/**
	* @brief Checks if the game is still running.
	*
	* @return True if it's running, false if it's not.
	*/
	bool Running();
	/**
	* @brief Gets the window width.
	*
	* @return The window width.
	*/
	int GetWindowWidth();
	/**
	* @brief Gets the window height.
	*
	* @return The window height.
	*/
	int GetWindowHeight();
	/**
	* @brief Gets the window's dimensions.
	*
	* @return A glm::vec2 containing the width and height of the window.
	*/
	glm::vec2 GetWindowSize();
	/**
	* @brief Closes the game window and makes Running() return false. 
	*/
	void Close();
	/**
	* @brief Checks if the game window needs to be resized or closed, and acts upon it.
	*/
	void Poll();
	int Width;
	int Height;
	int NewWidth;
	int NewHeight;
	/**
	* @brief Tells the game that the window size has been changed.
	*
	* @param Changed If you want the window to be changed(true) or not(false).
	*/
	void SetWindowChanged(bool Changed);
	/**
	* @brief Returns if the window size has been changed recently, this is useful for rendering objects that depend on the window size and need to be regenerated everytime the window changes.
	*
	* @return True if the window changed, false if it hasn't.
	*/ bool GetWindowChanged();
	/**
	* @brief Grabs the lua interface.
	*
	* @return A pointer to the lua interface.
	*/
	NLua* GetLua();
	/**
	* @brief Grabs the configure interface.
	*
	* @return A pointer to the lua interface.
	*/
	NConfig* GetConfig();
	/**
	* @brief Sets the window's size
	*
	* @param W Desired width of the window.
	* @param H Desired height of the window.
	*/
	void SetWindowSize(int W,int H);
	/**
	* @brief Deletes this and all of it's children.
	*/
	void CleanUp();
	/**
	 * @brief Returns a pointer to the sound system.
	 *
	 * @return A pointer to the sound system.
	 */
	NSoundSystem* GetSoundSystem();
	/**
	 * @brief Returns a pointer to the filesystem.
	 *
	 * @return A pointer to the filesystem.
	 */
	NFileSystem* GetFileSystem();
	/**
	 * @brief Gets the current map.
	 *
	 * @return A pointer to the map.
	 */
	NMap* GetMap();
private:
	bool WindowChanged;
	bool Run;
	NFileSystem* FileSystem;
	NInput* Input;
	NScene* Scene;
	NRender* Render;
	NLua* Lua;
	NConfig* Config;
	NSoundSystem* SoundSystem;
	NTextSystem* TextSystem;
	NSignalInterceptor* SignalInterceptor;
	NConsole* Console;
	NMap* Map;
	bool Valid;
};

#endif
