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
	if (!FullBright)
	{
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
	}
	for (unsigned int i=0;i<GUI.size();i++)
	{
		if ((bool)GUI[i]->GetFlags())
		{
			World.push_back(GUI[i]);
			GUI.erase(GUI.begin()+i);
		}
		GUI[i]->Draw(View);
	}
}

void NScene::ToggleFullBright()
{
	FullBright = !FullBright;
}

void NScene::AddNode(NNode* Node)
{
	std::string Type = Node->GetType();
	if (Type == "Window" || Type == "Button" || Type == "Text" || Type == "Checkbox")
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
	ShuttingDown = false;
	FullBright = false;
}

NScene::~NScene()
{
	ShuttingDown = true;
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
NPlayer* NScene::AddPlayer(std::wstring Name)
{
	NPlayer* Player = new NPlayer(Name);
	AddNode(Player);
	return Player;
}
NPlayer* NScene::AddPlayer(std::string Name)
{
	NPlayer* Player = new NPlayer(ToMBS(Name));
	AddNode(Player);
	return Player;
}
NLight* NScene::AddLight(std::string Texture)
{
	NLight* Light = new NLight(Texture);
	AddNode(Light);
	return Light;
}
NCheckbox* NScene::AddCheckbox(std::string Texture)
{
	NCheckbox* Checkbox = new NCheckbox(Texture);
	AddNode(Checkbox);
	return Checkbox;
}
std::vector<NNode*>* NScene::GetWorld()
{
	return &World;
}
NNode* NScene::GetNodeByID(unsigned int ID)
{
	for (unsigned int i=0;i<World.size();i++)
	{
		if (World[i]->GetID() == ID)
		{
			return World[i];
		}
	}
	return NULL;
}
void NScene::RemoveByID(unsigned int ID)
{
	for (unsigned int i=0;i<World.size();i++)
	{
		if (World[i]->GetID() == ID)
		{
			World[i]->Remove();
			World.erase(World.begin()+i);
			break;
		}
	}
}
void NScene::Remove(NNode* Node)
{
	if (ShuttingDown)
	{
		return;
	}
	std::string Type = Node->GetType();
	if (Type == "Window" || Type == "Button" || Type == "Text")
	{
		for (unsigned int i=0;i<GUI.size();i++)
		{
			if (GUI[i] == Node)
			{
				GUI[i]->Remove();
				GUI.erase(GUI.begin()+i);
				return;
			}
		}
		return;
	}
	if (Type == "Map" || Type == "Player")
	{
		for (unsigned int i=0;i<World.size();i++)
		{
			if (World[i] == Node)
			{
				World[i]->Remove();
				World.erase(World.begin()+i);
				return;
			}
		}
		return;
	}
	if (Type == "Light")
	{
		for (unsigned int i=0;i<Lights.size();i++)
		{
			if (Lights[i] == Node)
			{
				Lights[i]->Remove();
				Lights.erase(Lights.begin()+i);
				return;
			}
		}
		return;
	}
}
