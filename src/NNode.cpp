#include "NEngine.hpp"

NNode::NNode()
{
	Position = glm::vec2(0,0);
	GetGame()->GetScene()->AddNode(this);
}

void NNode::SetPos(float X, float Y)
{
	Position = glm::vec2(X,Y);
}

void NNode::SetPos(glm::vec2 i_Position)
{
	Position = i_Position;
}

glm::vec2 NNode::GetPos()
{
	return Position;
}

glm::mat4 NNode::GetModelMatrix()
{
	glm::mat4 Model = glm::translate(glm::mat4(1.f),glm::vec3(GetPos(),0));
	return Model;
}

void NNode::Tick() {}
void NNode::Draw(glm::mat4 View) {}
