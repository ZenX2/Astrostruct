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
* @brief Caches uniform locations in shaders to avoid having drivers recompile shaders every time a uniform is set on some systems.
*/
class NUniform
{
public:
	/**
	* @brief Grabs a uniform location and stores it.
	*
	* @param ProgramID The program to grab the uniform location from.
	* @param Name The name of the uniform.
	*/
	NUniform(GLuint ProgramID,std::string Name);
	~NUniform();
	/**
	* @brief Grab and return the cached uniform location.
	*
	* @return The uniform location.
	*/
	GLuint GetUniformLocation();
	/**
	* @brief Get the name of the uniform that we tried to grab.
	*
	* @return The name of the uniform.
	*/
	std::string GetName();
private:
	std::string Name;
	GLuint UniLoc;
};

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
	/**
	* @brief Grabs a uniform location within this shader, or creates it if it doesn't exist.
	*
	* @param i_Name The name of the desired uniform.
	*
	* @return An OpenGL uniform location ID.
	*/
	GLuint GetUniformLocation(std::string i_Name);
private:
	GLuint ProgramID;
	std::string Name;
	std::vector<NUniform*> Uniforms;
};

#endif
