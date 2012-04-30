#ifndef NAELSTROF_SCENE
#define NAELSTROF_SCENE

class NScene
{
public:
	NScene();
	~NScene();
	void Tick();
	void Draw(glm::mat4);
	void AddNode(NNode*); //Node
private:
	double LastTick;
	std::vector<NNode*> Nodes;
};

#endif
