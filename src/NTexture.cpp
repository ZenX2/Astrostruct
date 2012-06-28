#include "NEngine.hpp"
    
void NAnimation::SetName(std::string i_Name)
{
    Name = i_Name;
}

std::string NAnimation::GetName()
{
    return Name;
}

NAnimation::NAnimation()
{
    FPS = 30;
    Name = "NULL";
}

NAnimation::~NAnimation()
{
}

void NAnimation::AddFrame(std::string FileName)
{
    NCachedTexture* ID = GetGame()->GetRender()->GetCachedTexture(FileName);
    Frames.push_back(ID);
}

NTexture::NTexture(std::string i_Name)
{
    Name = i_Name;
    PlayingAnimation = 0;
    CurrentTime = 0;
    GoodCheck = false;
    IsGood = true;
}

NTexture::NTexture(NTexture* Texture)
{
    Animations = Texture->Animations;
    Name = Texture->Name;
    PlayingAnimation = 0;
    CurrentTime = 0;
    GoodCheck = false;
    IsGood = true;
}

NTexture::~NTexture()
{
}

GLuint NAnimation::GetID(double Time)
{
    unsigned int IDLoc = fmod(Time*FPS,Frames.size());
    return Frames[IDLoc]->ID;
}

bool NAnimation::Good()
{
    for (unsigned int i=0;i<Frames.size();i++)
    {
        if (!Frames[i]->Good())
        {
            return false;
        }
    }
    return true;
}

glm::vec2 NAnimation::GetSize(double Time)
{
    unsigned int IDLoc = fmod(Time*FPS,Frames.size());
    return Frames[IDLoc]->GetSize();
}

void NTexture::Play(std::string i_Name)
{
    for (unsigned int i=0;i<Animations.size();i++)
    {
        if (i_Name == Animations[i]->GetName())
        {
            PlayingAnimation = i;
            return;
        }
    }
    NTerminal::SetColor(Yellow);
    std::cout << "TEXTURE WARN: ";
    NTerminal::ClearColor();
    std::cout << "Animation " << i_Name << " not found in texture " << Name << "\n";
}

bool NTexture::Good()
{
    if (!GoodCheck)
    {
        for (unsigned int i=0;i<Animations.size();i++)
        {
            if (!Animations[i]->Good())
            {
                IsGood = false;
            }
        }
    }
    return IsGood;
}

GLuint NTexture::GetID()
{
    return Animations[PlayingAnimation]->GetID(CurrentTime);
}

void NTexture::AddAnimation(NAnimation* Animation)
{
    Animations.push_back(Animation);
}

void NTexture::Tick(double DT)
{
    CurrentTime += DT;
}

glm::vec2 NTexture::GetSize()
{
    return Animations[PlayingAnimation]->GetSize(CurrentTime);
}

float NTexture::GetFloat(std::string i_Name)
{
    return Animations[PlayingAnimation]->GetFloat(i_Name);
}

float NAnimation::GetFloat(std::string i_Name)
{
    for (unsigned int i=0;i<NumberNames.size();i++)
    {
        if (NumberNames[i] == i_Name)
        {
            return Numbers[i];
        }
    }
    NTerminal::SetColor(Yellow);
    std::cout << "LUA WARN: ";
    NTerminal::ClearColor();
    std::cout << "Tried to access " << i_Name << " as a number! (It's not a number or doesn't exist!)\n";
    return 0;
}

void NAnimation::AddNumber(std::string i_Name, float Number)
{
    NumberNames.push_back(i_Name);
    Numbers.push_back(Number);
}

unsigned int NTexture::GetAnimationCount()
{
    return Animations.size();
}

void NTexture::Play(unsigned int i_ID)
{
    if (i_ID >= Animations.size())
    {
        PlayingAnimation = Animations.size()-1;
    }
    PlayingAnimation = i_ID;
}
