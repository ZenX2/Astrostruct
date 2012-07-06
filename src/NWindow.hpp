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
    /**
     * @brief Creates a window with the specified texture.
     *
     * @param i_Texture The desired texture name.
     */
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
    /**
     * @brief Returns the type of the node.
     *
     * @return NodeWindow.
     */
    NodeType GetType();
    /**
     * @brief Swaps the view matrix from orthographic to perspective and vise-versa.
     */
    void SwapView();
    /**
     * @brief Sets if this window should block mouse clicks. When the mouse is "blocked" by a UI element, it will only affect other UI elements. Rather than the world.
     *
     * @param i_UI True to block the mouse, false otherwise.
     */
    void SetUI(bool i_UI);
private:
    glm::vec3 SizeMem;
    float BorderSize;
    NShader* Shader;
    bool Changed;
    void GenerateBuffers();
    std::vector<glm::vec2> Verts;
    std::vector<glm::vec2> UVs;
    GLuint Buffers[2];
    GLuint TextureLoc;
    GLuint MatrixLoc;
    GLuint ColorLoc;
    bool Persp;
    bool UI;
};

/*@}*/

#endif
