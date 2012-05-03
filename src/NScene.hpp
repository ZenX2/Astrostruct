/**
* @file NScene.hpp
* @brief Handles active game objects.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-02
*/
#ifndef NAELSTROF_SCENE
#define NAELSTROF_SCENE

/**
* @brief Class that holds arbitrary objects and handles Tick-ing and drawing them.
*/
class NScene
{
public:
	NScene();
	~NScene();
	/**
	* @brief Updates all objects in the scene.
	*/
	void Tick();
	/**
	* @brief Draws all objects in the scene.
	*
	* @param View The camera view matrix.
	*/
	void Draw(glm::mat4 View);
	/**
	* @brief Adds an object to the scene.
	*
	* @param Node The desired object to be added.
	*/
	void AddNode(NNode* Node);
private:
	double LastTick;
	std::vector<NNode*> Nodes;
};

#endif
