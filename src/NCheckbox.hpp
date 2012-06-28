/**
* @file NCheckbox.hpp
* @brief Handles checkable checkboxes.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-13
*/

#ifndef NAELSTROF_CHECKBOX
#define NAELSTROF_CHECKBOX

/**
 * @addtogroup GuiSystem
 * @{*/

/**
* @brief Creates a checkable checkbox from the specified texture.
*/
class NCheckbox : public NNode
{
public:
    NCheckbox(std::string i_Texture);
    ~NCheckbox();
    NTexture* Texture;
    /**
    * @brief Draws the button.
    *
    * @param View The camera view matrix, might not be used in the future.
    */
    void Draw(NCamera* View);
    void Tick(double DT);
    /**
     * @brief Returns if the box has been checked or not.
     *
     * @return True if it's checked, false if it isn't.
     */
    bool IsChecked();
    /**
     * @brief Sets if the checkbox is checked or not and sets it's animations accordingly.
     *
     * @param Check True if we want it checked, false otherwise.
     */
    void SetCheck(bool Check);
    /**
     * @brief Unallocates the object.
     */
    void Remove();
    /**
     * @brief Returns the type of the object.
     *
     * @return Checkbox.
     */
    NodeType GetType();
private:
    bool Changed;
    bool Pressed;
    bool PressedMemory;
    bool Checked;
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

/*@}*/

#endif
