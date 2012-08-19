#include "NEngine.hpp"

NShader::NShader(std::string i_Name)
{
    ProgramID = glCreateProgram();
    VertexAttribute = 0;
    UVAttribute = 0;
    Name = i_Name;
}

GLuint NShader::GetUVAttribute()
{
    if (UVAttribute == 0)
    {
        UVAttribute = glGetAttribLocation(ProgramID,"UVPosition");
    }
    return UVAttribute;
}

GLuint NShader::GetVertexAttribute()
{
    if (VertexAttribute == 0)
    {
        VertexAttribute = glGetAttribLocation(ProgramID,"VertexPosition");
    }
    return VertexAttribute;
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
    char* VertexShaderCode;
    NReadFile VertexShaderStream = GetGame()->GetFileSystem()->GetReadFile(VertexFilePath);
    if (!VertexShaderStream.Good())
    {
        GetGame()->GetLog()->Send("SHADER",1,"Failed to load " + VertexFilePath + "!");
        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);
        return 1;
    }
    VertexShaderCode = new char[VertexShaderStream.Size()];
    VertexShaderStream.Read(VertexShaderCode,VertexShaderStream.Size());
    //Now do the same with the fragment shader
    char* FragmentShaderCode;
    NReadFile FragmentShaderStream = GetGame()->GetFileSystem()->GetReadFile(FragmentFilePath);
    if (!FragmentShaderStream.Good())
    {
        GetGame()->GetLog()->Send("SHADER",1,"Failed to load " + FragmentFilePath + "!");
        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);
        return 1;
    }
    FragmentShaderCode = new char[FragmentShaderStream.Size()];
    FragmentShaderStream.Read(FragmentShaderCode,FragmentShaderStream.Size());

    //Now compile the vertex shader
    GetGame()->GetLog()->Send("SHADER",2,"Compiling shader " + VertexFilePath + "...");
    std::string Conversion;
    Conversion.append(VertexShaderCode,VertexShaderStream.Size());
    const char* Code = Conversion.c_str();
    glShaderSource(VertexShaderID, 1, &Code, NULL);
    glCompileShader(VertexShaderID);
    delete[] VertexShaderCode;
    
    GLint Result;
    int InfoLogLength;
    //And then check it to make sure it's all valid and whatever
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    if (Result == GL_FALSE)
    {
        if (int(VertexShaderErrorMessage.size())-2>=0)
        {
            VertexShaderErrorMessage[VertexShaderErrorMessage.size()-2] = '\0';
        }
        GetGame()->GetLog()->Send("SHADER",1,(char*)&VertexShaderErrorMessage[0]);
        GetGame()->GetLog()->Send("SHADER",1,"Failed to compile " + VertexFilePath + "!");
        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);
        return 1;
    }

    //Same for the fragment shader
    GetGame()->GetLog()->Send("SHADER",2,"Compiling shader " + FragmentFilePath + "...");
    Conversion.clear();
    Conversion.append(FragmentShaderCode,FragmentShaderStream.Size());
    Code = Conversion.c_str();
    glShaderSource(FragmentShaderID, 1, &Code, NULL);
    glCompileShader(FragmentShaderID);
    delete[] FragmentShaderCode;
    
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    if (Result == GL_FALSE)
    {
        if (int(FragmentShaderErrorMessage.size())-2>=0)
        {
            FragmentShaderErrorMessage[FragmentShaderErrorMessage.size()-2] = '\0';
        }
        GetGame()->GetLog()->Send("SHADER",1,(char*)&FragmentShaderErrorMessage[0]);
        GetGame()->GetLog()->Send("SHADER",1,"Failed to compile " + FragmentFilePath + "!");
        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);
        return 1;
    }

    //Then create and link the program
    GetGame()->GetLog()->Send("SHADER",2,"Linking program \"" + Name + "\"...");
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
        if (int(ProgramErrorMessage.size())-2>=0)
        {
            ProgramErrorMessage[ProgramErrorMessage.size()-2] = '\0';
        }
        GetGame()->GetLog()->Send("SHADER",1,&ProgramErrorMessage[0]);
        GetGame()->GetLog()->Send("SHADER",1,"Failed to link program" + Name + "!");
        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);
        return 1;
    }
    //Clean up
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    return 0;
}
