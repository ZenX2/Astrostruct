#ifndef NAELSTROF_PHYSICS
#define NAELSTROF_PHYSICS

bool Intersects(glm::vec4 Box, glm::vec2 Point);
bool Intersects(glm::vec4 ABox, glm::vec4 BBox);
glm::vec2 MinimumTranslation(glm::vec4 ABox, glm::vec4 BBox);

#endif
