#ifndef NAELSTROF_BUTTON
#define NAELSTROF_BUTTON

class NButton : public NNode
{
public:
	NButton();
	~NButton();
	NTexture* Texture;
	void SetTexture(std::string Name);
	void Draw(glm::mat4 View);
	void Tick(double DT);
	void SetText(std::string Text);
private:
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
