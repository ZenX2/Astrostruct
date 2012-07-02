#include "NEngine.hpp"

void NScene::Tick()
{
    double DT = CurTime()-LastTick;
    LastTick = CurTime();
    for (unsigned int i=0;i<Layers.size();i++)
    {
        for (unsigned int o=0;o<Layers[i].size();o++)
        {
            Layers[i][o]->Tick(DT);
        }
    }
}

void NScene::Draw(NCamera* View)
{
    for (unsigned int i=0;i<Layers[0].size();i++)
    {
        Layers[0][i]->Draw(View);
    }
    glEnable(GL_DEPTH_TEST);
    for (unsigned int i=0;i<Layers[1].size();i++)
    {
        Layers[1][i]->Draw(View);
    }
    glDisable(GL_DEPTH_TEST);
    if (!FullBright)
    {
        GetGame()->GetRender()->glPushFramebuffer();
        glBindFramebuffer(GL_FRAMEBUFFER, GetGame()->GetLightSystem()->GetFramebuffer());
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);
        for (unsigned int i=0;i<Layers[2].size();i++)
        {
            Layers[2][i]->Draw(View);
        }
        GetGame()->GetRender()->glPopFramebuffer();
        GetGame()->GetLightSystem()->Draw();
    } else {
        GetGame()->GetLightSystem()->CheckFBO();
    }
    for (unsigned int i=0;i<Layers[3].size();i++)
    {
        Layers[3][i]->Draw(View);
    }
}

void NScene::ToggleFullBright()
{
    FullBright = !FullBright;
}

void NScene::AddNode(NNode* Node)
{
    NodeType Type = Node->GetType();
    switch(Type)
    {
        //Space
        case NodeStar:      Layers[0].push_back(Node); return;

        //World
        case NodeMap:       Layers[1].push_back(Node); return;
        case NodePlayer:    Layers[1].push_back(Node); return;

        //Lights
        case NodeLight:     Layers[2].push_back(Node); return;

        //GUI
        case NodeWindow:    Layers[3].push_back(Node); return;
        case NodeButton:    Layers[3].push_back(Node); return;
        case NodeText:      Layers[3].push_back(Node); return;
        case NodeCheckbox:  Layers[3].push_back(Node); return;
        case NodeTextInput: Layers[3].push_back(Node); return;

        case NodeCamera:    Layers[4].push_back(Node); return;
        case NodeSound:     Layers[4].push_back(Node); return;
    }
    delete Node;
}

NScene::NScene()
{
    LastTick = CurTime();
    ShuttingDown = false;
    FullBright = false;
    std::vector<NNode*> Temp;
    Layers.resize(6, Temp);
}

NScene::~NScene()
{
    ShuttingDown = true;
    for (unsigned int i=0;i<Layers.size();i++)
    {
        for (unsigned int o=0;o<Layers[i].size();o++)
        {
            Layers[i][o]->Remove();
        }
    }
}

NText* NScene::AddText(std::string Font, std::wstring Data)
{
    NText* Text = new NText(Font,Data);
    AddNode(Text);
    return Text;
}

NWindow* NScene::AddWindow(std::string Texture)
{
    NWindow* Window = new NWindow(Texture);
    AddNode(Window);
    return Window;
}
NButton* NScene::AddButton(std::string Texture)
{
    NButton* Button = new NButton(Texture);
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
NTextInput* NScene::AddTextInput(std::string Texture)
{
    NTextInput* TextInput = new NTextInput(Texture);
    AddNode(TextInput);
    return TextInput;
}
NStar* NScene::AddStar()
{
    NStar* Star = new NStar();
    AddNode(Star);
    return Star;
}
std::vector<NNode*>* NScene::GetLayer(unsigned int Layer)
{
    return &Layers[Layer];
}
NNode* NScene::GetNodeByID(unsigned int ID)
{
    for (unsigned int i=0;i<Layers.size();i++)
    {
        for (unsigned int o=0;o<Layers[i].size();o++)
        {
            if (Layers[i][o]->GetID() == ID)
            {
                return Layers[i][o];
            }
        }
    }
    return NULL;
}
void NScene::RemoveByID(unsigned int ID)
{
    for (unsigned int i=0;i<Layers.size();i++)
    {
        for (unsigned int o=0;o<Layers[i].size();o++)
        {
            if (Layers[i][o]->GetID() == ID)
            {
                Layers[i][o]->Remove();
                Layers[i].erase(Layers[i].begin()+o);
                break;
            }
        }
    }
}
void NScene::Remove(NNode* Node)
{
    if (ShuttingDown)
    {
        return;
    }
    for (unsigned int i=0;i<Layers.size();i++)
    {
        for (unsigned int o=0;o<Layers[i].size();o++)
        {
            if (Layers[i][o] == Node)
            {
                Node->Remove();
                Layers[i].erase(Layers[i].begin()+o);
                return;
            }
        }
    }
}
bool NScene::GetFullBright()
{
    return FullBright;
}
void NScene::RemoveByType(NodeType Type)
{
    if (ShuttingDown)
    {
        return;
    }
    for (unsigned int i=0;i<Layers.size();i++)
    {
        for (unsigned int o=0;o<Layers[i].size();o++)
        {
            if (Layers[i][o]->GetType() == Type)
            {
                Layers[i][o]->Remove();
                Layers[i].erase(Layers[i].begin()+o);
                o--;
            }
        }
    }
}
void NScene::UpdateLights()
{
    for (unsigned int i=0;i<Layers.size();i++)
    {
        for (unsigned int o=0;o<Layers[i].size();o++)
        {
            if (Layers[i][o]->GetType() == NodeLight)
            {
                NLight* Light = (NLight*)Layers[i][o];
                Light->SChanged = true;
            }
        }
    }
}
void NScene::SwapLayer(NNode* Node, unsigned int Layer)
{
    for (unsigned int i=0;i<Layers.size();i++)
    {
        if (i == Layer)
        {
            continue;
        }
        for (unsigned int o=0;o<Layers[i].size();o++)
        {
            if (Layers[i][o] == Node)
            {
                Layers[Layer].push_back(Node);
                Layers[i].erase(Layers[i].begin()+o);
                return;
            }
        }
    }
}
    
void NScene::SetFullBright(bool Bright)
{
    FullBright = Bright;
}
