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
	TextSystem = new NTextSystem();
	LoadedText = false;
}

NScene::~NScene()
{
	delete TextSystem;
}

NText* NScene::AddText(std::string Font, std::string Data)
{
	if (!LoadedText)
	{
		TextSystem->LoadFaces();
		LoadedText = true;
	}
	return TextSystem->AddText(Font,Data);
}

NTextSystem* NScene::GetTextSystem()
{
	return TextSystem;
}

void NScene::SwapDepth(NNode* Node, unsigned int Depth)
{
	if (Depth > Nodes.size())
	{
		return;
	}
	if (Nodes[Depth] == Node)
	{
		return;
	}
	if (Depth == GetTopDepth())
	{
		for (unsigned int i=0;i<Nodes.size();i++)
		{
			if (Nodes[i] == Node)
			{
				Nodes.erase(Nodes.begin()+i);
				Nodes.push_back(Node);
				return;
			}
		}
		return;
	}
	for (unsigned int i=0;i<Nodes.size();i++)
	{
		if (Nodes[i] == Node)
		{
			NNode* NodeMem = Nodes[Depth];
			Nodes[Depth] = Node;
			Nodes[i] = NodeMem;
			return;
		}
	}
}

unsigned int NScene::GetTopDepth()
{
	return Nodes.size()-1;
}

NWindow* NScene::AddWindow()
{
	return new NWindow();
}
