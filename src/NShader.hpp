/**
* @file NShader.hpp
* @brief Abstracts OpenGL shader loading into a shader object.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-01
*/
#ifndef NAELSTROF_SHADER
#define NAELSTROF_SHADER

/**
* @brief Abstracts OpenGL shader loading, compiling, and access.
*/
class NShader
{
public:
	/**
	* @brief Only creates the shader and gives it a name, doesn't do anything else.
	*
	* @param Name What you want the shader to be named, can be anything!
	*/
	NShader(std::string Name);
	~NShader();
	/**
	* @brief Loads the specified shaders into a program.
	*
	* @param VertexShaderPath File path to the vertex shader.
	* @param FragmentShaderPath File path to the fragment shader.
	*
	* @return True on success, false on failure.
	*/
	bool Load(std::string VertexShaderPath,std::string FragmentShaderPath);
	/**
	* @brief Returns the loaded program ID.
	*
	* @return The opengl program ID.
	*/
	GLuint GetID();
	/**
	* @brief Returns the given name to the program.
	*
	* @return The name given to the program.
	*/
	std::string GetName();
private:
	GLuint ProgramID;
	std::string Name;
};

#endif
