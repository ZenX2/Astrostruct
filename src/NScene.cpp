#include "NEngine.hpp"

void NScene::Tick()
{
	double DT = CurTime()-LastTick;
	LastTick = CurTime();
	for (unsigned int i=0;i<Nodes.size();i++)
	{
		Nodes[i]->Tick(DT);
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

NScene::NScene()
{
	LastTick = CurTime();
}

NScene::~NScene()
{
}
