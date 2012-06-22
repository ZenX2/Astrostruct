/**
* @file NTextInput.hpp
* @brief Handles text boxes that can eat text input.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-13
*/

#ifndef NAELSTROF_TEXTINPUT
#define NAELSTROF_TEXTINPUT

/**
 * @addtogroup GuiSystem
 * @{*/

class NTextInput : public NNode
{
public:
	NTextInput(std::string i_Texture);
	~NTextInput();
	NTexture* Texture;
	/**
	* @brief Draws the text box.
	*
	* @param View The camera view matrix, might not be used in the future.
	*/
	void Draw(NCamera* View);
	void Tick(double DT);
	/**
	 * @brief Unallocates the object.
	 */
	void Remove();
	/**
	 * @brief Returns the type of the object.
	 *
	 * @return "TextInput".
	 */
	std::string GetType();
	NText* GetText();
	void SetText(std::wstring Text);
	std::wstring GetEnteredText();
private:
	std::wstring RealText;
	bool DisplayCursor;
	unsigned int TextureWidth;
	unsigned int TextureHeight;
	float BorderSize;
	bool Changed;
	bool HasFocus;
	double Time;
	NShader* Shader;
	void GenerateBuffers();
	std::vector<glm::vec2> Verts;
	std::vector<glm::vec2> UVs;
	GLuint* Buffers;
	NText* DisplayText;
	GLuint ScreenLoc;
	GLuint TextureLoc;
	GLuint MatrixLoc;
	GLuint ColorLoc;
};

/*@}*/

#endif
