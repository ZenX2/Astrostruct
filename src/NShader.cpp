#include "NEngine.hpp"

NShader::NShader(std::string i_Name)
{
	ProgramID = glCreateProgram();
	Name = i_Name;
}

NShader::~NShader()
{
	for (unsigned int i=0;i<Uniforms.size();i++)
	{
		delete Uniforms[i];
	}
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

GLuint NShader::GetUniformLocation(std::string i_Name)
{
	for (unsigned int i=0;i<Uniforms.size();i++)
	{
		if (!Uniforms[i]->GetName().compare(i_Name))
		{
			return Uniforms[i]->GetUniformLocation();
		}
	}
	NUniform* NewUniform = new NUniform(GetID(), i_Name);
	Uniforms.push_back(NewUniform);
	return NewUniform->GetUniformLocation();
}

NUniform::NUniform(GLuint ProgramID, std::string i_Name)
{
	Name = i_Name;
	UniLoc = glGetUniformLocation(ProgramID,Name.c_str());
}

NUniform::~NUniform()
{
}

GLuint NUniform::GetUniformLocation()
{
	return UniLoc;
}

std::string NUniform::GetName()
{
	return Name;
}

bool NShader::Load(std::string VertexFilePath, std::string FragmentFilePath)
{
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	//Read the Vertex shader into memory from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(VertexFilePath.c_str(), std::ios::in);
	if (!VertexShaderStream.good())
	{
		SetColor(Yellow);
		std::cout << "SHADER WARN: ";
		ClearColor();
		std::cout << "Failed to load " << VertexFilePath << "!\n";
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
	std::ifstream FragmentShaderStream(FragmentFilePath.c_str(), std::ios::in);
	if (!FragmentShaderStream.good())
	{
		SetColor(Yellow);
		std::cout << "SHADER WARN: " ;
		ClearColor();
		std::cout << "Failed to load " << FragmentFilePath << "!\n";
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
	SetColor(Blue);
	std::cout << "SHADER INFO: ";
	ClearColor();
	std::cout << "Compiling shader " << VertexFilePath << "\n";
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
	if (Result == GL_FALSE)
	{
		if (VertexShaderErrorMessage.size()-2>=0)
		{
			VertexShaderErrorMessage[VertexShaderErrorMessage.size()-2] = '\0';
		}
		SetColor(Blue);
		std::cout << "SHADER INFO: ";
		ClearColor();
		std::cout << (char*)&VertexShaderErrorMessage[0] << "\n";
		SetColor(Yellow);
		std::cout << "SHADER WARN: ";
		ClearColor();
		std::cout << "Failed to compile " << VertexFilePath << "\n";
		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);
		return Fail;
	}

	//Same for the fragment shader
	SetColor(Blue);
	std::cout << "SHADER INFO: ";
	ClearColor();
	std::cout << "Compiling shader " << FragmentFilePath << "\n";
	Code = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &Code, NULL);
	glCompileShader(FragmentShaderID);
	
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	if (Result == GL_FALSE)
	{
		if (FragmentShaderErrorMessage.size()-2>=0)
		{
			FragmentShaderErrorMessage[FragmentShaderErrorMessage.size()-2] = '\0';
		}
		SetColor(Blue);
		std::cout << "SHADER INFO: ";
		ClearColor();
		std::cout << (char*)&FragmentShaderErrorMessage[0] << "\n";
		SetColor(Yellow);
		std::cout << "SHADER WARN: ";
		ClearColor();
		std::cout << "Failed to compile " << FragmentFilePath << "\n";
		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);
		return Fail;
	}

	//Then create and link the program
	SetColor(Blue);
	std::cout << "SHADER INFO: ";
	ClearColor();
	std::cout << "Linking program \"" << Name << "\"\n";
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	//Now check it for errors
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage(InfoLogLength);
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	if (Result == GL_FALSE)
	{
		if (ProgramErrorMessage.size()-2>=0)
		{
			ProgramErrorMessage[ProgramErrorMessage.size()-2] = '\0';
		}
		std::cout << &ProgramErrorMessage[0] << "\n";
		SetColor(Yellow);
		std::cout << "SHADER WARN: ";
		ClearColor();
		std::cout << "Failed to link program!\n";
		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);
		return Fail;
	}
	//Clean up
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
}
