/**
* @file NTexture.hpp
* @brief Abstracts OpenGL texture objects so we can apply texture filters to them all easily, and perhaps animations in the future.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-01
*/
#ifndef NAELSTROF_TEXTURE
#define NAELSTROF_TEXTURE

/**
* @brief Abstracts OpenGL texture ID's.
*/
class NTexture
{
public:
	/**
	* @brief Initializes the texture object from a given (already created) opengl texture ID.
	*
	* @param i_TextureID An already allocated, created, etc, opengl texture ID.
	*/
	NTexture(GLuint i_TextureID);
	/**
	* @brief Deletes the opengl texture object and itself. (So don't go trying to delete the opengl texture you created!)
	*/
	~NTexture();
	/**
	* @brief Returns an ID to the texture object.
	*
	* @return An opengl texture ID.
	*/
	GLuint GetID();
	/**
	* @brief Sets the texture's filters.
	*
	* @param Filter Either GL_LINEAR or GL_NEAREST.
	*/
	void SetFilter(GLuint);
private:
	GLuint TextureID;
};

#endif
