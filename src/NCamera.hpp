/**
* @file NCamera.hpp
* @brief Acts as a scene object and generates a view matrix for other parts of the engine.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-02
*/
#ifndef NAELSTROF_CAMERA
#define NAELSTROF_CAMERA

/**
* @brief Acts as a scene object and generates a view matrix for other parts of the engine.
*/
class NCamera : public NNode
{
public:
	NCamera();
	/**
	* @brief Generates and returns a view matrix, don't call this a lot!
	*
	* @return View matrix.
	*/
	glm::mat4 GetPerspViewMatrix();
	glm::mat4 GetViewMatrix();
	glm::mat4 GetOrthoMatrix();
	glm::mat4 GetPerspMatrix();
private:
	glm::mat4 ViewMatrix;
	glm::mat4 OrthoMatrix;
	glm::mat4 PerspMatrix;
	float Zoom;
};

#endif
