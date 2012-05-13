#include "NEngine.hpp"

NNode::NNode()
{
	Position = glm::vec2(0,0);
	Scale = glm::vec2(1,1);
	Color = glm::vec4(1);
	Angle = 0;
	Parent = NULL;
	Matrix = glm::mat4();
	GetGame()->GetScene()->AddNode(this);
}

NNode::~NNode()
{
}

void NNode::SetPos(float X, float Y)
{
	Position = glm::vec2(X,Y);
	UpdateMatrix();
	for (unsigned int i=0;i<Children.size();i++)
	{
		Children[i]->UpdateMatrix();
	}
}

NNode* NNode::GetParent()
{
	return Parent;
}

void NNode::SetPos(glm::vec2 i_Position)
{
	Position = i_Position;
	UpdateMatrix();
	for (unsigned int i=0;i<Children.size();i++)
	{
		Children[i]->UpdateMatrix();
	}
}

void NNode::SetAng(float i_Angle)
{
	Angle = i_Angle;
	UpdateMatrix();
	for (unsigned int i=0;i<Children.size();i++)
	{
		Children[i]->UpdateMatrix();
	}
}

void NNode::SetParent(NNode* Node)
{
	if (Parent != NULL)
	{
		Parent->RemoveChild(this);
	}
	Parent = Node;
	Parent->AddChild(this);
}

void NNode::AddChild(NNode* Node)
{
	Children.push_back(Node);
}

void NNode::RemoveChild(NNode* Node)
{
	for (unsigned int i=0;i<Children.size();i++)
	{
		if (Children[i] == Node)
		{
			Children[i]->SetParent(NULL);
			Children[i]->UpdateMatrix();
			Children.erase(Children.begin()+i);
		}
	}
}

float NNode::GetAng()
{
	return Angle;
}

glm::vec2 NNode::GetPos()
{
	return Position;
}

glm::mat4 NNode::GetModelMatrix()
{
	return Matrix;
}

void NNode::UpdateMatrix()
{
	glm::mat4 Pos = glm::translate(glm::mat4(),glm::vec3(GetPos(),0));
	glm::mat4 Ang = glm::rotate(glm::mat4(),Angle,glm::vec3(0,0,1));
	glm::mat4 Sca = glm::scale(glm::mat4(),glm::vec3(Scale,1));
	if (Parent == NULL)
	{
		Matrix = Pos*Ang*Sca;
	} else {
		Matrix = Parent->GetModelMatrix()*(Pos*Ang*Sca);
	}
}

glm::vec2 NNode::GetScale()
{
	return Scale;
}

void NNode::SetScale(float W, float H)
{
	SetScale(glm::vec2(W,H));
}

void NNode::SetScale(glm::vec2 i_Scale)
{
	Scale = i_Scale;
}

void NNode::SetScale(float i_Scale)
{
	SetScale(glm::vec2(i_Scale));
}

void NNode::Tick(double DT) {}
void NNode::Draw(glm::mat4 View) {}

void NNode::SetColor(float R, float G, float B, float A)
{
	Color = glm::vec4(R,G,B,A);
}
void NNode::SetColor(float R, float G, float B)
{
	float A = Color.w;
	Color = glm::vec4(R,G,B,A);
}
void NNode::SetColor(glm::vec3 i_Color)
{
	float A = Color.w;
	Color = glm::vec4(i_Color,A);
}
void NNode::SetColor(glm::vec4 i_Color)
{
	Color = i_Color;
}
glm::vec4 NNode::GetColor()
{
	return Color;
}
void NNode::SwapDepth(unsigned int Depth)
{
	GetGame()->GetScene()->SwapDepth(this, Depth);
}
