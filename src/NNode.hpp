/**
* @file NNode.hpp
* @brief A class that all other scene objects derive from. 
* @author Dalton Nell
* @version 1.0
* @date 2012-05-01
*/

#ifndef NAELSTROF_NODE
#define NAELSTROF_NODE

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
	* @brief Draws the object.
	*
	* @param ModelView Model matrix * View matrix, used in vertex transformations.
	*/
	virtual void Draw(glm::mat4 ModelView);
	/**
	* @brief Gets the postition of the object.
	*
	* @return The position in a glm::vec2.
	*/
	virtual glm::vec2 GetPos();
	/**
	* @brief Sets the position of the object.
	*
	* @param X The desired X position.
	* @param Y The desired Y position.
	*/
	virtual void SetPos(float X,float Y);
	/**
	* @brief Sets the position of the object.
	*
	* @param i_Position The desired position.
	*/
	virtual void SetPos(glm::vec2 i_Position);
	/**
	* @brief Sets the angle of the object.
	*
	* @param i_Angle The desired angle.
	*/
	virtual void SetAng(float i_Angle);
	/**
	* @brief Gets the angle of the object.
	*
	* @return The angle of the object.
	*/
	virtual float GetAng();
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
	virtual glm::vec2 GetScale();
	/**
	* @brief Sets the scale of the object.
	*
	* @param i_Scale The desired scale factors.
	*/
	virtual void SetScale(glm::vec2 i_Scale);
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
	* @brief Swaps this depth with the desired depth.
	*
	* @param Depth The desired depth we want to be rendered at.
	*/
	virtual void SwapDepth(unsigned int Depth);
	/**
	 * @brief Gets a matrix that doesn't include scaling; I'm using this because GUI doesn't function properly when it inherits scaling.
	 *
	 * @return A matrix without scale applied to it.
	 */
	virtual glm::mat4 GetNonScaleMatrix();
	virtual void Remove();
private:
	float Angle;
	glm::vec4 Color;
	glm::vec2 Position;
	glm::vec2 Scale;
	glm::mat4 Matrix;
	NNode* Parent;
	std::vector<NNode*> Children;
};

#endif
