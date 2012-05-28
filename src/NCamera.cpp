#include "NEngine.hpp"

NCamera::NCamera()
{
	OrthoMatrix = glm::mat4(1.f);
	PerspMatrix = glm::mat4(1.f);
	ViewMatrix = glm::mat4(1.f);
	PerspViewMatrix = glm::mat4(1.f);
	Changed = true;
}

glm::mat4 NCamera::GetViewMatrix()
{
	UpdateMatrix();
	return ViewMatrix;
}

glm::mat4 NCamera::GetPerspViewMatrix()
{
	UpdateMatrix();
	return PerspViewMatrix;
}

glm::mat4 NCamera::GetOrthoMatrix()
{
	UpdateMatrix();
	return OrthoMatrix;
}

glm::mat4 NCamera::GetPerspMatrix()
{
	UpdateMatrix();
	return PerspMatrix;
}

void NCamera::UpdateMatrix()
{
	if (!Changed && !GetGame()->GetWindowChanged())
	{
		return;
	}
	ViewMatrix = glm::translate(glm::mat4(1.f),-GetPos());
	ViewMatrix = glm::scale(ViewMatrix,GetScale());
	PerspViewMatrix = ViewMatrix; //Until we actually need real 3d view...
	OrthoMatrix = glm::ortho(0.f,(float)GetGame()->GetWindowWidth(),0.f,(float)GetGame()->GetWindowHeight(),0.f,3000.f);
	PerspMatrix = glm::perspective(45.f,float(GetGame()->GetWindowWidth())/float(GetGame()->GetWindowHeight()),32.f,3000.f);
	Changed = false;
}

void NCamera::SetPos(glm::vec3 i_Position)
{
	Position = i_Position;
	Changed = true;
}

void NCamera::SetScale(glm::vec3 i_Scale)
{
	Scale = i_Scale;
	Changed = true;
}

void NCamera::SetAng(glm::vec3 i_Angle)
{
	Angle = i_Angle;
	Changed = true;
}
