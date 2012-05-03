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
	virtual void AddChild(NNode*);
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
private:
	float Angle;
	glm::vec2 Position;
	glm::mat4 Matrix;
	NNode* Parent;
	std::vector<NNode*> Children;
};

#endif
