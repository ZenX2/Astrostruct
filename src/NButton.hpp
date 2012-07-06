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
 * @addtogroup GuiSystem
 * @{*/

/**
* @brief Creates a pressable button from the specified texture.
*/
class NButton : public NNode
{
public:
    NButton(std::string i_Texture);
    ~NButton();
    NTexture* Texture;
    /**
    * @brief Draws the button.
    *
    * @param View The camera view matrix, might not be used in the future.
    */
    void Draw(NCamera* View);
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
    void SetText(std::wstring Text);
    /**
     * @brief Returns if the button has been pressed.
     *
     * @return True if the button is currently pressed, false if it's not.
     */
    bool OnPressed();
    /**
     * @brief Returns if the button has just been released.
     *
     * @return True if the button has just barely been released, false if it hasn't.
     */
    bool OnRelease();
    /**
     * @brief Unallocates the object.
     */
    void Remove();
    /**
     * @brief Hooks into NNode to make our text follow color. Probably shouldn't do that, but if i encounter problems with it I'll change it.
     *
     * @param i_Color The desired color of the text and button.
     */
    void SetColor(glm::vec4 i_Color);
    /**
     * @brief Returns the type of the object.
     *
     * @return Button.
     */
    NodeType GetType();
    NText* GetText();
    void SetToggleable(bool i_Toggleable);
    void SetToggle(bool i_Toggled);
    bool GetToggle();
private:
    bool Toggled;
    unsigned int TextureWidth, TextureHeight;
    bool PressedMemory;
    bool IsPressed;
    bool IsChanged;
    bool Changed;
    bool Toggleable;
    float BorderSize;
    NText* DisplayText;
    NShader* Shader;
    void GenerateBuffers();
    std::vector<glm::vec2> Verts;
    std::vector<glm::vec2> UVs;
    GLuint Buffers[2];
    GLuint ScreenLoc;
    GLuint TextureLoc;
    GLuint MatrixLoc;
    GLuint ColorLoc;
};

/*@}*/

#endif
