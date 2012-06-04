#include "NEngine.hpp"

NNode::NNode()
{
	Position = glm::vec3(0);
	Scale = glm::vec3(1);
	Color = glm::vec4(1);
	Angle = glm::vec3(0);
	Parent = NULL;
	Matrix = glm::mat4();
	SMatrix = glm::mat4();
	GetGame()->GetScene()->AddNode(this);
}

NNode::~NNode()
{
}

void NNode::SetPos(float X, float Y, float Z)
{
	SetPos(glm::vec3(X,Y,Z));
}
void NNode::SetPos(float X, float Y)
{
	SetPos(glm::vec3(X,Y,GetPos().z));
}

NNode* NNode::GetParent()
{
	return Parent;
}

void NNode::SetPos(glm::vec3 i_Position)
{
	Position = i_Position;
	UpdateMatrix();
}

void NNode::SetPos(glm::vec2 i_Position)
{
	SetPos(glm::vec3(i_Position,Position.z));
}

void NNode::SetAng(glm::vec3 i_Angle)
{
	Angle = i_Angle;
	UpdateMatrix();
}

void NNode::SetAng(float i_Angle)
{
	SetAng(glm::vec3(0,0,i_Angle));
}

void NNode::SetParent(NNode* Node)
{
	if (Parent != NULL)
	{
		Parent->RemoveChild(this);
	}
	Parent = Node;
	Parent->AddChild(this);
	UpdateMatrix();
}

void NNode::AddChild(NNode* Node)
{
	Children.push_back(Node);
	UpdateMatrix();
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

glm::vec3 NNode::GetAng()
{
	return Angle;
}

glm::vec3 NNode::GetPos()
{
	if (Parent)
	{
		return Parent->GetPos()+Position;
	}
	return Position;
}

glm::mat4 NNode::GetModelMatrix()
{
	return Matrix;
}

void NNode::UpdateMatrix()
{
	glm::mat4 Pos = glm::translate(glm::mat4(),Position);
	glm::mat4 Ang = glm::rotate(glm::mat4(),Angle.z,glm::vec3(0,0,1));
	Ang = glm::rotate(Ang,Angle.y,glm::vec3(0,1,0));
	Ang = glm::rotate(Ang,Angle.x,glm::vec3(1,0,0));
	glm::mat4 Sca = glm::scale(glm::mat4(),Scale);
	if (Parent == NULL)
	{
		SMatrix = Pos*Ang;
		Matrix = Pos*Ang*Sca;
	} else {
		SMatrix = Parent->GetNonScaleMatrix()*(Pos*Ang);
		Matrix = Parent->GetNonScaleMatrix()*(Pos*Ang*Sca);
	}
	for (unsigned int i=0;i<Children.size();i++)
	{
		Children[i]->UpdateMatrix();
	}
}

NNode* NNode::GetRoot()
{
	if (Parent == NULL)
	{
		return this;
	}
	return Parent->GetRoot();
}

glm::mat4 NNode::GetNonScaleMatrix()
{
	return SMatrix;
}

glm::vec3 NNode::GetScale()
{
	return Scale;
}

void NNode::SetScale(float W, float H)
{
	SetScale(glm::vec3(W,H,GetScale().z));
}

void NNode::SetScale(glm::vec3 i_Scale)
{
	Scale = i_Scale;
}

void NNode::SetScale(glm::vec2 i_Scale)
{
	Scale = glm::vec3(i_Scale,GetScale().z);
}

void NNode::SetScale(float i_Scale)
{
	SetScale(glm::vec3(i_Scale));
}

void NNode::Tick(double DT) {}
void NNode::Draw(NCamera* View) {}

void NNode::SetColor(glm::vec4 i_Color)
{
	Color = i_Color;
}
void NNode::SetColor(float R, float G, float B, float A)
{
	SetColor(glm::vec4(R,G,B,A));
}
void NNode::SetColor(float R, float G, float B)
{
	float A = Color.w;
	SetColor(glm::vec4(R,G,B,A));
}
void NNode::SetColor(glm::vec3 i_Color)
{
	float A = Color.w;
	SetColor(glm::vec4(i_Color,A));
}
glm::vec4 NNode::GetColor()
{
	return Color;
}
void NNode::Remove()
{
}
