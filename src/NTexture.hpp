/**
* @file NTexture.hpp
* @brief Abstracts OpenGL texture objects so we can apply texture filters to them all easily, and perhaps animations in the future.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-01
*/
#ifndef NAELSTROF_TEXTURE
#define NAELSTROF_TEXTURE

/**
* @brief Lua function to load textures into memory.
*
* @param L The lua state.
*
* @return 1
*/
int CreateAnimation(lua_State* L);
/**
* @brief AnimationBase __index function for lua.
*
* @param L The lua state.
*
* @return 1
*/
int Animation__index(lua_State* L);
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
* @brief Causes lua to recursively search data/textures for lua files. Ones that should contain code to load textures.
*/
void LoadTextures();
/**
* @brief Animation object to hold a series of image ID's and an fps.
*/
class NAnimation
{
public:
	NAnimation();
	~NAnimation();
	/**
	* @brief Tells the render system to retreive a texture id for the file, it'll load the file into memory if it hasn't already.
	*
	* @param FileName The path to the file.
	*/
	void AddFrame(std::string FileName);
	float FPS;
	/**
	* @brief This is a lua reference to this AnimationBase object's class. It's used to store variables that don't exist in the Animation class itself.
	*/
	int Reference;
	/**
	* @brief Gets the texture ID based on time passed.
	*
	* @param Time The current time.
	*
	* @return The texture ID of the current animation frame.
	*/
	GLuint GetID(double Time);
	/**
	* @brief Sets the name of the Animation, I'm unsure why I abstracted the name. I had some nasty bug and thought it'd help. :u
	*
	* @param i_Name The desired name of the Animation.
	*/
	void SetName(std::string i_Name);
	/**
	* @brief Returns the name of the animation.
	*
	* @return Return the name of the animation.
	*/
	std::string GetName();
private:
	std::string Name;
	std::vector<GLuint> Frames;
};
/**
* @brief Takes the argument from lua and transforms it into a pointer to the NAnimation class.
*
* @param L The lua state.
* @param narg The number of the arg given to the lua function.
*
* @return A pointer to the NAnimation class.
*/
NAnimation* lua_checkAnimation(lua_State* L, int narg);
/**
* @brief Abstracts OpenGL texture ID's and allows for animations of a series of images.
*/
class NTexture
{
public:
	/**
	* @brief Initializes a copy of the given texture. Used by NRender to return a unique texture object for each GetTexture call.
	*
	* @param Texture
	*/
	NTexture(NTexture* Texture);
	/**
	* @brief Creates a texture object and sets it's name.
	*
	* @param i_Name The desired texture name.
	*/
	NTexture(std::string i_Name);
	/**
	* @brief Deletes the opengl texture object and itself. (So don't go trying to delete the opengl texture you created!)
	*/
	~NTexture();
	/**
	* @brief Returns an ID to the texture object.
	*
	* @return An opengl texture ID.
	*/
	GLuint GetID();
	/**
	* @brief Adds an Animation object to use it's texture id's.
	*
	* @param Animation 
	*/
	void AddAnimation(NAnimation* Animation);
	std::string Name;
	/**
	* @brief Plays the specified animation.
	*
	* @param i_Name The name of the animation.
	*/
	void Play(std::string i_Name);
	/**
	* @brief Moves the texture forward in time by DT.
	*
	* @param DT The time passed since last tick.
	*/
	void Tick(double DT);
private:
	double CurrentTime;
	unsigned int PlayingAnimation;
	std::vector<NAnimation*> Animations;
};

#endif
