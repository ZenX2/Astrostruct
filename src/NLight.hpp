#ifndef NAELSTROF_LIGHT
#define NAELSTROF_LIGHT

class NLight : public NNode
{
public:
	NLight(std::string i_Texture);
	~NLight();
	void Draw(NCamera* View);
	void Tick(double DT);
	void Remove();
	void DrawLight(NCamera* View);
	void DrawShadow(NCamera* View);
	std::string Type();
private:
	NTexture* Texture;
	NShader* Shader;
	NShader* ShadowShader;
	bool Changed;
	bool SChanged;
	void GenerateLightBuffers();
	void GenerateShadowBuffers();
	std::vector<glm::vec2> Verts;
	std::vector<glm::vec2> UVs;
	std::vector<glm::vec3> Shadows;
	GLuint Buffers[3];
	GLuint TextureLoc;
	GLuint STextureLoc;
	GLuint MatrixLoc;
	GLuint SMatrixLoc;
	GLuint ColorLoc;
	GLuint SColorLoc;
};

class NLightSystem
{
public:
	NLightSystem();
	~NLightSystem();
	GLuint GetFramebuffer();
	void Draw();
private:
	void CheckFBO();
	NShader* Shader;
	GLuint TextureLoc;
	GLuint VertexBuffers[2];
	std::vector<glm::vec2> Verts;
	std::vector<glm::vec2> UVs;
	GLuint FrameBuffer;
	GLuint FrameBufferTexture;
	GLuint StencilBuffer;
	std::vector<NLight*> Lights;
};

#endif
