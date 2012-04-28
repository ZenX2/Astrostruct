#ifndef NAELSTROF_SHADER
#define NAELSTROF_SHADER

class NShader
{
public:
	NShader(std::string); //Name
	~NShader();
	bool Load(std::string,std::string); //VertexShaderPath, FragmentShaderPath
	GLuint GetID();
	std::string GetName();
private:
	GLuint ProgramID;
	std::string Name;
};

#endif
