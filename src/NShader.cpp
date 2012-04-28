#include "NEngine.hpp"

NShader::NShader(std::string i_Name)
{
	ProgramID = glCreateProgram();
	Name = i_Name;
}

NShader::~NShader()
{
	glDeleteProgram(ProgramID);
}

GLuint NShader::GetID()
{
	return ProgramID;
}

std::string NShader::GetName()
{
	return Name;
}

bool NShader::Load(std::string VertexFilePath, std::string FragmentFilePath)
{
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	//Read the Vertex shader into memory from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(VertexFilePath, std::ios::in);
	if (!VertexShaderStream.good())
	{
		std::cout << "SHADER WARN: Failed to load " << VertexFilePath << "!\n";
		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);
		return Fail;
	}
	std::string Line;
	while (getline(VertexShaderStream,Line))
	{
		VertexShaderCode += Line + "\n";
	}
	VertexShaderStream.close();

	//Now do the same with the fragment shader
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(FragmentFilePath, std::ios::in);
	if (!FragmentShaderStream.good())
	{
		std::cout << "SHADER WARN: Failed to load " << FragmentFilePath << "!\n";
		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);
		return Fail;
	}
	while (getline(FragmentShaderStream,Line))
	{
		FragmentShaderCode += Line + "\n";
	}
	FragmentShaderStream.close();

	//Now compile the vertex shader
	std::cout << "Compiling shader: " << VertexFilePath << "\n";
	const char* Code = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &Code, NULL);
	glCompileShader(VertexShaderID);
	
	GLint Result;
	int InfoLogLength;
	//And then check it to make sure it's all valid and whatever
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	std::cout << (char*)&VertexShaderErrorMessage[0] << "\n";
	if (Result == GL_FALSE)
	{
		std::cout << "SHADER WARN: Failed to compile " << VertexFilePath << "\n";
		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);
		return Fail;
	}

	//Same for the fragment shader
	std::cout << "Compiling shader: " << FragmentFilePath << "\n";
	Code = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &Code, NULL);
	glCompileShader(FragmentShaderID);
	
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	std::cout << (char*)&FragmentShaderErrorMessage[0] << "\n";
	if (Result == GL_FALSE)
	{
		std::cout << "SHADER WARN: Failed to compile " << FragmentFilePath << "\n";
		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);
		return Fail;
	}

	//Then create and link the program
	std::cout << "Linking program...\n";
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	//Now check it for errors
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage(InfoLogLength);
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	std::cout << &ProgramErrorMessage[0] << "\n";
	if (Result == GL_FALSE)
	{
		std::cout << "SHADER WARN: Failed to link program!\n";
		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);
		return Fail;
	}

	//Everything checked out ok! delete the shader objects and return a program id
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
}
