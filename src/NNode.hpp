/**
* @file NNode.hpp
* @brief A class that all other scene objects derive from. 
* @author Dalton Nell
* @version 1.0
* @date 2012-05-01
*/

#ifndef NAELSTROF_NODE
#define NAELSTROF_NODE

class NCamera;

/**
* @brief Base scene class that all other classes derive from.
*/
class NNode
{
public:
	NNode();
	~NNode();
	/**
	* @brief Updates the object, depending on what it does.
	*
	* @param DT The delta time, aka the time passed since last execution.
	*/
	virtual void Tick(double DT);
	/**
	 * @brief Draws the desired node.
	 *
	 * @param View The camera it's being drawn from.
	 */
	virtual void Draw(NCamera* View);
	/**
	* @brief Gets the postition of the object.
	*
	* @return The position in a glm::vec3.
	*/
	virtual glm::vec3 GetPos();
	/**
	* @brief Sets the position of the object.
	*
	* @param X The desired X position.
	* @param Y The desired Y position.
	* @param Z The desired Z position.
	*/
	virtual void SetPos(float X,float Y,float Z);
	/**
	 * @brief Sets the position of the object in 2D space, retaining depth.
	 *
	 * @param X The desired X position.
	 * @param Y The desired Y position.
	 */
	virtual void SetPos(float X,float Y);
	/**
	* @brief Sets the position of the object in 2D space, retaining depth.
	*
	* @param i_Position The desired position.
	*/
	virtual void SetPos(glm::vec2 i_Position);
	/**
	 * @brief Sets the position of the object.
	 *
	 * @param i_Position The desired position.
	 */
	virtual void SetPos(glm::vec3 i_Position);
	/**
	* @brief Sets the angle of the object.
	*
	* @param i_Angle The desired angle.
	*/
	virtual void SetAng(glm::vec3 i_Angle);
	/**
	 * @brief Sets the angle of the object in 2D space. It will lose all other angle data though!
	 *
	 * @param i_Angle The desired angle.
	 */
	virtual void SetAng(float i_Angle);
	/**
	* @brief Gets the angle of the object.
	*
	* @return The angle of the object.
	*/
	virtual glm::vec3 GetAng();
	/**
	* @brief Returns a pre-calculated matrix created from the position and angle of the object.
	*
	* @return A pre-calculated model matrix.
	*/
	virtual glm::mat4 GetModelMatrix();
	/**
	* @brief Calculates the model matrix from the position and angle of this object.
	*/
	virtual void UpdateMatrix();
	/**
	* @brief Attaches this node to another.
	*
	* @param i_Parent The parent node we want to attach to.
	*/
	virtual void SetParent(NNode* i_Parent);
	/**
	* @brief Attaches another node to this one.
	*
	* @param Child The node we want to attach to.
	*/
	virtual void AddChild(NNode* Child);
	/**
	* @brief Detaches the specified node from this one.
	*
	* @param Child The node we want to detach from.
	*/
	virtual void RemoveChild(NNode* Child);
	/**
	* @brief Returns the parent we're attached to, or null if we don't one.
	*
	* @return A pointer to the parent.
	*/
	virtual NNode* GetParent();
	/**
	* @brief Sets the scale of the object.
	*
	* @param W The desired width scale factor.
	* @param H The desired height scale factor.
	*/
	virtual void SetScale(float W, float H);
	/**
	* @brief Returns the current scale of the object.
	*
	* @return The current scale of the object.
	*/
	virtual glm::vec3 GetScale();
	/**
	* @brief Sets the scale of the object in 2D space. Retaining z scaling.
	*
	* @param i_Scale The desired scale factors.
	*/
	virtual void SetScale(glm::vec2 i_Scale);
	/**
	 * @brief Sets the scale of the object.
	 *
	 * @param i_Scale The desired scale factors.
	 */
	virtual void SetScale(glm::vec3 i_Scale);
	/**
	* @brief Sets the scale of the object.
	*
	* @param i_Scale The desired scale factor.
	*/
	virtual void SetScale(float i_Scale);
	/**
	* @brief Sets the color of the object.
	*
	* @param R Desired red component of the color (0-1).
	* @param G Desired green component of the color (0-1).
	* @param B Desired blue component of the color (0-1).
	* @param A Desired alpha component of the color (0-1).
	*/
	virtual void SetColor(float R, float G, float B, float A);
	/**
	* @brief Sets the color of the object.
	*
	* @param R Desired red component of the color (0-1).
	* @param G Desired green component of the color (0-1).
	* @param B Desired blue component of the color (0-1).
	*/
	virtual void SetColor(float R, float G, float B);
	/**
	* @brief Sets the color of the object.
	*
	* @param i_Color The desired color.
	*/
	virtual void SetColor(glm::vec4 i_Color);
	/**
	* @brief Sets the color of the object.
	*
	* @param i_Color The desired color.
	*/
	virtual void SetColor(glm::vec3 i_Color);
	/**
	* @brief Gets the current color of the object.
	*
	* @return The current color of the object.
	*/
	virtual glm::vec4 GetColor();
	/**
	 * @brief Gets a matrix that doesn't include scaling; I'm using this because GUI doesn't function properly when it inherits scaling.
	 *
	 * @return A matrix without scale applied to it.
	 */
	virtual glm::mat4 GetNonScaleMatrix();
	/**
	 * @brief Unallocates the object, this is used by NScene to unallocate all objects, regardless of what kind of object it really is. (Lots of things inherit NNode!)
	 */
	virtual void Remove();
	virtual NNode* GetRoot();
	glm::vec3 Angle;
	glm::vec4 Color;
	glm::vec3 Position;
	glm::vec3 Scale;
	glm::mat4 Matrix;
	glm::mat4 SMatrix;
	NNode* Parent;
	std::vector<NNode*> Children;
};

#endif
