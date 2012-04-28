#ifndef NAELSTROF_SCENE
#define NAELSTROF_SCENE

class NScene
{
public:
	void Tick();
	void Draw(glm::mat4);
	void AddNode(NNode*); //Node
private:
	std::vector<NNode*> Nodes;
};

#endif
