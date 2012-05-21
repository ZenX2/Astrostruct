#include "NEngine.hpp"

NCamera::NCamera()
{
	OrthoMatrix = glm::mat4(1.f);
	PerspMatrix = glm::mat4(1.f);
}

glm::mat4 NCamera::GetViewMatrix()
{
	glm::mat4 View = glm::translate(glm::mat4(1.f),-GetPos());
	View = glm::scale(View,GetScale());
	return View;
}
glm::mat4 NCamera::GetPerspViewMatrix()
{
	glm::mat4 View = glm::lookAt(glm::vec3(480,480,-480),glm::vec3(0,0,480),glm::vec3(0,0,1));
	//View = glm::scale(View,GetScale());
	//return View;
	return View;
}

glm::mat4 NCamera::GetOrthoMatrix()
{
	glm::mat4 OrthoMatrix = glm::ortho(0.f,(float)GetGame()->GetWindowWidth(),0.f,(float)GetGame()->GetWindowHeight(),0.f,3000.f);
	return OrthoMatrix;
}

glm::mat4 NCamera::GetPerspMatrix()
{
	glm::mat4 PerspMatrix = glm::perspective(45.f,(float)GetGame()->GetWindowWidth()/(float)GetGame()->GetWindowHeight(),0.f,3000.f);
	return PerspMatrix;
}
