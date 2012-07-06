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
    /**
     * @brief Generates a text box with the given texture.
     *
     * @param i_Texture The name of the desired texture.
     */
    NTextInput(std::string i_Texture);
    ~NTextInput();
    NTexture* Texture;
    /**
    * @brief Draws the text box.
    *
    * @param View The camera view matrix, might not be used in the future.
    */
    void Draw(NCamera* View);
    /**
     * @brief Moves the text box forward in time, making a cursor blink and eat new input.
     *
     * @param DT Time passed since last execution.
     */
    void Tick(double DT);
    /**
     * @brief Unallocates the object.
     */
    void Remove();
    /**
     * @brief Returns the type of the object.
     *
     * @return TextInput.
     */
    NodeType GetType();
    /**
     * @brief Returns the text object we're using, useful for changing it's color or size.
     *
     * @return A pointer to the text object.
     */
    NText* GetText();
    /**
     * @brief Sets the text currently entered into the box.
     *
     * @param Text The desired text entered.
     */
    void SetText(std::wstring Text);
    /**
     * @brief Returns the text currently entered in the box.
     *
     * @return The currently entered text.
     */
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
