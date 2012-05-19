#include "NEngine.hpp"

NCamera::NCamera()
{
	OrthoMatrix = glm::mat4(1.f);
}

glm::mat4 NCamera::GetViewMatrix()
{
	glm::mat4 View = glm::translate(glm::mat4(1.f),GetPos());
	View = glm::scale(View,GetScale());
	return View;
}

glm::mat4 NCamera::GetOrthoMatrix()
{
	if (!GetGame()->GetWindowChanged() && OrthoMatrix != glm::mat4(1.f))
	{
		return OrthoMatrix;
	}
	glm::mat4 OrthoMatrix = glm::ortho(0.f,(float)GetGame()->GetWindowWidth(),0.f,(float)GetGame()->GetWindowHeight(),0.f,300.f);
	return OrthoMatrix;
}
