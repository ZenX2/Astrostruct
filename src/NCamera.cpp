#include "NEngine.hpp"

NCamera::NCamera()
	: NNode()
{
	Zoom = 0;
}

void NCamera::SetZoom(float i_Zoom)
{
	Zoom = i_Zoom;
}

float NCamera::GetZoom()
{
	return Zoom;
}

glm::mat4 NCamera::GetViewMatrix()
{
	glm::mat4 View = glm::translate(glm::mat4(1.f),glm::vec3(GetPos(),0));
	View = glm::scale(View,glm::vec3(Zoom));
	return View;
}
