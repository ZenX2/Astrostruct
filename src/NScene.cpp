#include "NEngine.hpp"

void NScene::Tick()
{
	for (unsigned int i=0;i<Nodes.size();i++)
	{
		Nodes[i]->Tick();
	}
}

void NScene::Draw(glm::mat4 View)
{
	for (unsigned int i=0;i<Nodes.size();i++)
	{
		Nodes[i]->Draw(View);
	}
}

void NScene::AddNode(NNode* Node)
{
	Nodes.push_back(Node);
}
