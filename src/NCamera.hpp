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
	* @brief Gets the zoom level of the camera, 1 = normal zoom, 0.1 = zoom out, 10 = zooom in.
	*
	* @return The zoom level.
	*/
	float GetZoom();
	/**
	* @brief Sets the zoom level of the camera, 1 = normal zoom, 0.1 = zoom out, 10 = zooom in.
	*
	* @param Zoom The desired zoom level.
	*/
	void SetZoom(float Zoom);
	/**
	* @brief Generates and returns a view matrix, don't call this a lot!
	*
	* @return View matrix.
	*/
	glm::mat4 GetViewMatrix();
private:
	float Zoom;
};

#endif
