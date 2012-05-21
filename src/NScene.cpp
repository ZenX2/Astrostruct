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

void NScene::Draw(NCamera* View)
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
	for (unsigned int i=0;i<Nodes.size();i++)
	{
	    Nodes[i]->Remove();
	}
}

NText* NScene::AddText(std::string Font, std::string Data)
{
	if (!LoadedText)
	{
		TextSystem->LoadFaces();
		LoadedText = true;
	}
	return new NText(TextSystem->GetFace(Font),Data);
}

NTextSystem* NScene::GetTextSystem()
{
	return TextSystem;
}

NWindow* NScene::AddWindow()
{
    return new NWindow();
}
NButton* NScene::AddButton()
{
    return new NButton();
}
NSound* NScene::AddSound(std::string Name)
{
    return new NSound(Name);
}
NMap* NScene::AddMap(std::string TileSet)
{
	return new NMap(TileSet);
}
