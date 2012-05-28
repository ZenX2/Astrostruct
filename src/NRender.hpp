/**
* @file NRender.hpp
* @brief Handles most rendering calls, and exposes them to the rest of the engine.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-02
*/
#ifndef NAELSTROF_RENDER
#define NAELSTROF_RENDER

/**
* @brief Stores texture id's and associates them with a file path, useful for caching textures.
*/
class NCachedTexture
{
public:
	/**
	* @brief Sets the OpenGL texture filter of the stored texture id.
	*
	* @param GLuint
	*/
	void SetFilter(GLuint Filter);
	/**
	 * @brief Creates an OpenGL id from the specified texture filepath and associates it with a name for later retreival.
	 *
	 * @param i_Name The filepath to the file.
	 */
	NCachedTexture(std::string i_Name);
	/**
	* @brief Saves the OpenGL id and associates it with a name for later retreival.
	*
	* @param i_Name The name of the texture id (normally the filepath).
	* @param i_ID The texture id we want to cache.
	*/
	NCachedTexture(std::string i_Name, GLuint ID);
	~NCachedTexture();
	GLuint ID;
	std::string Name;
	/**
	 * @brief Returns the dimensions of the texture in a glm vec2.
	 *
	 * @return The dimensions of the texture.
	 */
	glm::vec2 GetSize();
	/**
	 * @brief Returns the width of the texture.
	 *
	 * @return The width of the texture.
	 */
	float GetWidth();
	/**
	 * @brief Returns the height of the texture.
	 *
	 * @return The height of the texture.
	 */
	float GetHeight();
	bool Good();
private:
	float IsGood;
	int Width, Height;
};
/**
* @brief Handles most rendering calls, and exposes them to the rest of the engine.
*/
class NRender
{
public:
	NRender();
	~NRender();
	/**
	* @brief Statically loads shaders into memory.
	*
	* @return False on failure, true on success.
	*/
	bool LoadShaders();
	/**
	* @brief Grabs the desired shader from memory.
	*
	* @param Name The name of the desired shader.
	*
	* @return A pointer to the shader.
	*/
	NShader* GetShader(std::string Name);
	/**
	* @brief Clears the color buffer, draws all objects in the current scene, then swaps the buffer.
	*/
	void Draw();
	/**
	* @brief Sets the camera to use to generate a view matrix from.
	*
	* @param Camera The desired camera to use.
	*/
	void SetCamera(NCamera* Camera);
	/**
	* @brief Returns the current camera in use, or NULL if none.
	*
	* @return A pointer to the current camera in use, or NULL if none.
	*/
	NCamera* GetCamera();
	/**
	* @brief Sets all active texture's texture filters. Useful for graphical settings.
	*
	* @param Param GL_LINEAR or GL_NEAREST
	*/
	void SetTextureFilter(GLuint Param);
	/**
	* @brief Returns what texture filter we're currently using.
	*
	* @return Either GL_LINEAR or GL_NEAREST.
	*/
	GLuint GetTextureFilter();
	/**
	* @brief Enables or disables VSync.
	*
	* @param i_VSync true for enable, false for disable.
	*/
	void SetVSync(bool i_VSync);
	/**
	* @brief Returns if VSync is enabled or not.
	*
	* @return true if enabled, false if it's disabled.
	*/
	bool GetVSync();
	/**
	* @brief Returns the amount of time in seconds it took to render the last frame.
	*
	* @return The amount of time in seconds it took to render the last frame.  */
	double GetFrameTime();
	/**
	* @brief Attempts to grab a texture already in memory and return an ID for it, if it doesn't exist it uses SOIL to load one into memory and returns that ID.
	*
	* @param Name The file path to the image/texture.
	*
	* @return A texture ID for the already loaded image.
	*/
	NCachedTexture* GetCachedTexture(std::string Name);
	/**
	* @brief Returns a Texture object from memory.
	*
	* @param Name The name of the texture object.
	*
	* @return A newly allocated unique texture object. It allocates a new texture to keep unique time locations in each texture for animations.
	*/
	NTexture* GetTexture(std::string Name);
	/**
	* @brief Adds a texture object into memory.
	*
	* @param Texture The desired texture.
	*/
	void AddTexture(NTexture* Texture);
	/**
	* @brief Adds a texture ID into memory, this is so global texture filters can be applied to dynamically created textures.
	*
	* @param ID The OpenGL texture ID.
	*/
	void AddCachedTexture(GLuint ID);
	/**
	 * @brief Uses lua to load all textures into memory.
	 */
	void LoadTextures();
	/**
	 * @brief Adds an animation to cache so it can be unallocated later.
	 *
	 * @param Animation A pointer to the animation.
	 */
	void AddCachedAnimation(NAnimation* Animation);
	/**
	 * @brief Sets the OpenGL context's viewport.
	 *
	 * @param i_Size The size of the current render window.
	 */
	void SetSize(glm::vec2 i_Size);
	/**
	 * @brief Sets the OpenGL context's viewport.
	 *
	 * @param Width The width of the current render window.
	 * @param Height The width of the current render window.
	 */
	void SetSize(float Width, float Height);
private:
	void glError();
	glm::vec2 Size;
	double FrameTime;
	bool VSync;
	GLuint TextureFilter;
	float MaxFPS;
	double LastTime;
	unsigned int FPS;
	std::vector<NShader*> Shaders;
	std::vector<NCachedTexture*> CachedTextures;
	std::vector<NTexture*> Textures;
	std::vector<NAnimation*> Animations;
	NCamera* Camera;
};

#endif
