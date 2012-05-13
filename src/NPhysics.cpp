#include "NEngine.hpp"

bool Intersects(glm::vec4 Box, glm::vec2 Point)
{
	glm::vec2 AMin = glm::vec2(Box.x,Box.y)-glm::vec2(Box.z,Box.w)/2.f;
	glm::vec2 AMax = glm::vec2(Box.x,Box.y)+glm::vec2(Box.z,Box.w)/2.f;
	if (Point.x > AMax.x ||
		Point.y > AMax.y ||
		Point.x < AMin.x ||
		Point.y < AMin.y)
	{
		return false;
	}
	return true;
}
