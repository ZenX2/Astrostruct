#ifndef NAELSTROF_RENDER
#define NAELSTROF_RENDER

class NRender
{
public:
	NRender();
	~NRender();
	bool LoadShaders();
	NShader* GetShader(std::string); //Name
	void Draw();
	void SetCamera(NCamera*); //Camera
	NCamera* GetCamera();
private:
	std::vector<NShader*> Shaders;
	NCamera* Camera;
};

#endif
