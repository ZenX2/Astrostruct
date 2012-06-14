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
	* @brief Returns a view matrix.
	*
	* @return A View matrix.
	*/
	glm::mat4 GetPerspViewMatrix();
	/**
	 * @brief Returns a view matrix.
	 *
	 * @return A View matrix.
	 */
	glm::mat4 GetViewMatrix();
	/**
	 * @brief Returns an orthagraphic matrix.
	 *
	 * @return An orthagraphic matrix.
	 */
	glm::mat4 GetOrthoMatrix();
	/**
	 * @brief Returns a perspective matrix.
	 *
	 * @return A perspective matrix.
	 */
	glm::mat4 GetPerspMatrix();
	/**
	 * @brief Hooks into NNode to ensure our view matricies are only generated when needed.
	 *
	 * @param i_Position The desired position of the camera.
	 */
	void SetPos(glm::vec3 i_Position);
	/**
	 * @brief Hooks into NNode to ensure our view matricies are only generated when needed.
	 *
	 * @param i_Angle The desired angle of the camera.
	 */
	void SetAng(glm::vec3 i_Angle);
	/**
	 * @brief Hooks into NNode to ensure our view matricies are only generated when needed.
	 *
	 * @param i_Scale The desired scale of the camera.
	 */
	void SetScale(glm::vec3 i_Scale);
	/**
	 * @brief Removes the object from memory.
	 */
	void Remove();
	/**
	 * @brief Returns the type of the object.
	 *
	 * @return "Camera".
	 */
	std::string Type();
private:
	bool Changed;
	void UpdateCameraMatrix();
	glm::mat4 ViewMatrix;
	glm::mat4 PerspViewMatrix;
	glm::mat4 OrthoMatrix;
	glm::mat4 PerspMatrix;
	float Zoom;
};

#endif
