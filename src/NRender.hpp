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
	* @brief Adds a texture into memory, used to globally set texture filters.
	*
	* @param Texture The desired texture we want the engine to handle.
	*/
	void AddTexture(NTexture* Texture);
	/**
	* @brief Removes a texture from memory, but doesn't delete it!
	*
	* @param Texture The desired texture we want to remove.
	*/
	void RemoveTexture(NTexture* Texture);
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
private:
	bool VSync;
	GLuint TextureFilter;
	float MaxFPS;
	double LastTime;
	unsigned int FPS;
	std::vector<NShader*> Shaders;
	std::vector<NTexture*> Textures;
	NCamera* Camera;
};

#endif
