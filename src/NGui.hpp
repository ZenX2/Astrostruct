#ifndef NAELSTROF_GUI
#define NAELSTROF_GUI

class NWindow : public NNode
{
public:
	NWindow();
	~NWindow();
	NTexture* Texture;
	void SetTexture(std::string Name);
	void Draw(glm::mat4 View);
private:
	NShader* Shader;
	bool Changed;
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
