/**
* @file NButton.hpp
* @brief Handles pressable buttons.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-13
*/

#ifndef NAELSTROF_BUTTON
#define NAELSTROF_BUTTON

/**
* @brief Creates a pressable button from the specified texture.
*/
class NButton : public NNode
{
public:
	NButton();
	~NButton();
	NTexture* Texture;
	/**
	* @brief Sets the desired texture of this button.
	*
	* @param Name The name of the texture. (Not the file path!).
	*/
	void SetTexture(std::string Name);
	/**
	* @brief Draws the button.
	*
	* @param View The camera view matrix, might not be used in the future.
	*/
	void Draw(glm::mat4 View);
	/**
	 * @brief Checks if the mouse is hovering or clicking on the button and reacts accordingly.
	 *
	 * @param DT The delta time.
	 */
	void Tick(double DT);
	/**
	 * @brief Sets the text of the button, and creates it if it doens't exist.
	 *
	 * @param Text The desired display text.
	 */
	void SetText(std::string Text);
	bool OnPressed();
	bool OnRelease();
private:
	bool PressedMemory;
	bool IsPressed;
	bool IsChanged;
	bool Changed;
	float BorderSize;
	NText* DisplayText;
	NShader* Shader;
	void GenerateBuffers();
	std::vector<glm::vec2> Verts;
	std::vector<glm::vec2> UVs;
	GLuint* Buffers;
	GLuint ScreenLoc;
	GLuint TextureLoc;
	GLuint MatrixLoc;
	GLuint ColorLoc;
};

#endif