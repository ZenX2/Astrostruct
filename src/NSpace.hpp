/**
 * @file NSpace.hpp
 * @brief Handles objects that scroll in the background in space, like stars and planets.
 * @author Dalton Nell
 * @version 1.0
 * @date 2012-08-20
 */
#ifndef NAELSTROF_SPACE
#define NAELSTROF_SPACE

/**
 * @brief Twinkle twinkle little star.
 */
class NStar : public NNode
{
public:
    NStar();
    ~NStar();
    void Tick(double DT);
    void Draw(NCamera* View);
    void Unallocate();
    NodeType GetType();
private:
    NTexture* Texture;
    bool Changed;
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

#endif
