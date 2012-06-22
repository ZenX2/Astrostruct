/**
* @file NWindow.hpp
* @brief Contains all gui elements.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-10
*/

#ifndef NAELSTROF_WINDOW
#define NAELSTROF_WINDOW

/** @addtogroup GuiSystem
 * @{*/

/**
* @brief Creates a simple box that can have textures applied to it.
*/
class NWindow : public NNode
{
public:
	NWindow(std::string i_Texture);
	~NWindow();
	NTexture* Texture;
	/**
	* @brief Draws the scaled, rotated, translated texture onto screen.
	*
	* @param View the camera view matrix, in the future it might not actually be used.
	*/
	void Draw(NCamera* View);
	/**
	* @brief Currently only calls Texture->Tick.
	*
	* @param DT The time passed since last tick.
	*/
	void Tick(double DT);
	/**
	 * @brief Unallocates the object.
	 */
	void Remove();
	std::string GetType();
private:
	glm::vec3 SizeMem;
	float BorderSize;
	NShader* Shader;
	bool Changed;
	void GenerateBuffers();
	std::vector<glm::vec2> Verts;
	std::vector<glm::vec2> UVs;
	GLuint* Buffers;
	GLuint TextureLoc;
	GLuint MatrixLoc;
	GLuint ColorLoc;
};

/*@}*/

#endif
