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
	return true; }

bool Intersects(glm::vec4 ABox, glm::vec4 BBox)
{
	glm::vec2 AMin = glm::vec2(ABox.x,ABox.y)-(glm::vec2(ABox.z,ABox.w)/2.f);
	glm::vec2 AMax = glm::vec2(ABox.x,ABox.y)+(glm::vec2(ABox.z,ABox.w)/2.f);
	glm::vec2 BMin = glm::vec2(BBox.x,BBox.y)-(glm::vec2(BBox.z,BBox.w)/2.f);
	glm::vec2 BMax = glm::vec2(BBox.x,BBox.y)+(glm::vec2(BBox.z,BBox.w)/2.f);
	if (AMax.x < BMin.x ||
		AMax.y < BMin.y ||
		AMin.x > BMax.x ||
		AMin.y > BMax.y)
	{
		return false;
	}
	return true;
}

glm::vec2 MinimumTranslation(glm::vec4 ABox, glm::vec4 BBox)
{
	glm::vec2 AMin = glm::vec2(ABox.x,ABox.y)-(glm::vec2(ABox.z,ABox.w)/2.f);
	glm::vec2 AMax = glm::vec2(ABox.x,ABox.y)+(glm::vec2(ABox.z,ABox.w)/2.f);
	glm::vec2 BMin = glm::vec2(BBox.x,BBox.y)-(glm::vec2(BBox.z,BBox.w)/2.f);
	glm::vec2 BMax = glm::vec2(BBox.x,BBox.y)+(glm::vec2(BBox.z,BBox.w)/2.f);

	glm::vec2 mtd;

	float left = (BMin.x - AMax.x);
	float right = (BMax.x - AMin.x);
	float top = (BMin.y - AMax.y);
	float bottom = (BMax.y - AMin.y);
	if (abs(left) < right)
	{
		mtd.x = left;
	} else {
		mtd.x = right;
	}

	if (abs(top) < bottom)
	{
		mtd.y = top;
	} else {
		mtd.y = bottom;
	}

	if (abs(mtd.x) < abs(mtd.y))
	{
		mtd.y = 0;
	} else {
		mtd.x = 0;
	}
	return -mtd;
}

bool Facing(glm::vec2 Point, glm::vec4 Face)
{
	glm::vec2 Center = glm::vec2((Face.x+Face.z)/2.f,(Face.y+Face.w)/2.f);
	float Radians = atan2(Center.x-Point.x,Center.y-Point.y);
	glm::vec2 Direction = glm::vec2(sin(Radians),cos(Radians));

	Radians = atan2(Face.x-Face.z,Face.y-Face.w)+PI/2.f;
	glm::vec2 BDirection = glm::vec2(sin(Radians),cos(Radians));

	float Difference = glm::dot(Direction,BDirection);
	if (Difference > 0)
	{
		return true;
	}
	return false;
}
