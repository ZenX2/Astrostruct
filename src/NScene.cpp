#include "NEngine.hpp"

void NScene::Tick()
{
	double DT = CurTime()-LastTick;
	LastTick = CurTime();
	for (unsigned int i=0;i<GUI.size();i++)
	{
		GUI[i]->Tick(DT);
	}
	for (unsigned int i=0;i<World.size();i++)
	{
		World[i]->Tick(DT);
	}
	for (unsigned int i=0;i<Lights.size();i++)
	{
		Lights[i]->Tick(DT);
	}
}

void NScene::Draw(NCamera* View)
{
	glEnable(GL_DEPTH_TEST);
	for (unsigned int i=0;i<World.size();i++)
	{
		World[i]->Draw(View);
	}
	glDisable(GL_DEPTH_TEST);
	GetGame()->GetRender()->glPushFramebuffer();
	glBindFramebuffer(GL_FRAMEBUFFER, GetGame()->GetLightSystem()->GetFramebuffer());
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);
	for (unsigned int i=0;i<Lights.size();i++)
	{
		Lights[i]->Draw(View);
	}
	GetGame()->GetRender()->glPopFramebuffer();
	GetGame()->GetLightSystem()->Draw();
	for (unsigned int i=0;i<GUI.size();i++)
	{
		GUI[i]->Draw(View);
	}
}

void NScene::AddNode(NNode* Node)
{
	std::string Type = Node->Type();
	if (Type == "Window" || Type == "Button" || Type == "Text")
	{
		GUI.push_back(Node);
		return;
	}
	if (Type == "Map" || Type == "Player")
	{
		World.push_back(Node);
		return;
	}
	if (Type == "Light")
	{
		Lights.push_back(Node);
		return;
	}
	Garbage.push_back(Node);
}

NScene::NScene()
{
	LastTick = CurTime();
}

NScene::~NScene()
{
	for (unsigned int i=0;i<GUI.size();i++)
	{
	    GUI[i]->Remove();
	}
	for (unsigned int i=0;i<World.size();i++)
	{
	    World[i]->Remove();
	}
	for (unsigned int i=0;i<Lights.size();i++)
	{
	    Lights[i]->Remove();
	}
	for (unsigned int i=0;i<Garbage.size();i++)
	{
	    Garbage[i]->Remove();
	}
}

NText* NScene::AddText(std::string Font, std::wstring Data)
{
	NText* Text = new NText(Font,Data);
	AddNode(Text);
	return Text;
}

NWindow* NScene::AddWindow()
{
	NWindow* Window = new NWindow();
	AddNode(Window);
	return Window;
}
NButton* NScene::AddButton()
{
	NButton* Button = new NButton();
	AddNode(Button);
	return Button;
}
NSound* NScene::AddSound(std::string Name)
{
	NSound* Sound = new NSound(Name);
	AddNode(Sound);
	return Sound;
}
NMap* NScene::AddMap(std::string TileSet)
{
	NMap* Map = new NMap(TileSet);
	AddNode(Map);
	return Map;
}
NCamera* NScene::AddCamera()
{
	NCamera* Camera = new NCamera();
	AddNode(Camera);
	return Camera;
}
NPlayer* NScene::AddPlayer()
{
	NPlayer* Player = new NPlayer();
	AddNode(Player);
	return Player;
}
NLight* NScene::AddLight(std::string Texture)
{
	NLight* Light = new NLight(Texture);
	AddNode(Light);
	return Light;
}
