#ifndef NAELSTROF_ENTITY
#define NAELSTROF_ENTITY

class NLuaEntity
{
public:
    NLuaEntity(std::string i_Name, int i_LuaReference);
    std::string Name;
    int LuaReference;
};

class NEntityManager
{
public:
    NEntityManager();
    ~NEntityManager();
    void Reload();
    int GetLuaEntity(std::string Name);
    std::vector<std::string> GetEntityNames();
private:
    std::vector<NLuaEntity*> Entities;
};

class NEntity : public NNode
{
public:
    NEntity(std::string i_Name);
    NEntity(std::string i_Name, glm::vec3 i_Position);
    ~NEntity();
    void Tick(double DT);
    void SetVel(glm::vec3 i_Velocity);
    glm::vec3 GetVel();
    void Draw(NCamera* View);
    void Unallocate();
    int SelfReference;
    void CallMethod(std::string Name, unsigned int AdditionalVars = 0, ...);
    std::string GetString(std::string Name);
    float GetFloat(std::string Name);
    std::string GetName();
private:
    std::string Name;
    int LuaSelf;
    float Friction;
    NShader* Shader;
    GLuint TextureLoc, MatrixLoc, ColorLoc;
    bool Changed;
    GLuint Buffers[2];
    void GenerateBuffers();
    std::vector<glm::vec2> Verts;
    std::vector<glm::vec2> UVs;
    glm::vec3 Velocity;
    NTexture* Texture;
};

#endif
