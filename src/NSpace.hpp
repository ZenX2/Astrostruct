#ifndef NAELSTROF_SPACE
#define NAELSTROF_SPACE

class NStar : public NNode
{
public:
    NStar();
    ~NStar();
    void Tick(double DT);
    void Draw(NCamera* View);
    void Remove();
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
