/**
 * @file NPhysics.hpp
 * @brief Handles physics calculations, but doesn't do any fancy invisible physics environments. It just lets you check collisions and how to respond to them with 2D boxes and points.
 * @author Dalton Nell
 * @version 1.0
 * @date 2012-06-13
 */
#ifndef NAELSTROF_PHYSICS
#define NAELSTROF_PHYSICS

bool Intersects(glm::vec3 BoxPos, glm::vec3 BoxSize, glm::vec3 Point);
/**
 * @brief Checks if a point intersects with a box.
 *
 * @param Box The box in a vec4 like so: vec4(xpos,ypos,width,height)
 * @param Point The point.
 *
 * @return True if they intersect, false if they don't.
 */
bool Intersects(glm::vec4 Box, glm::vec2 Point);
/**
 * @brief Checks if a box intersects with another box.
 *
 * @param ABox The box in a vec4 like so: vec4(xpos,ypos,width,height)
 * @param BBox The other box.
 *
 * @return True if they intersect, false if they don't.
 */
bool Intersects(glm::vec4 ABox, glm::vec4 BBox);
/**
 * @brief Gets the minimum amount of movement you need applied to both boxes to make them not intersect.
 *
 * @param ABox The box in a vec4 like so: vec4(xpos,ypos,width,height)
 * @param BBox The other box.
 *
 * @return A vec2 which can be halved and applied positively to BBox to make BBox jump out of the way, or negatively to ABox to make it jump out of the way.
 */
glm::vec2 MinimumTranslation(glm::vec4 ABox, glm::vec4 BBox);

/**
 * @brief Checks if the given face is facing the given point. Useful for lighting.
 *
 * @param Point The point we are checking from. Usually the view point.
 * @param Face Two points in a vec4 defining a 2d face.
 *
 * @return True if it's facing the point, false if it isn't.
 */
bool Facing(glm::vec2 Point, glm::vec4 Face);
bool Facing(glm::vec3 Point, std::vector<glm::vec3> Face);

class NPhysics
{
public:
    NPhysics();
    ~NPhysics();
    void Step(double DT);
    btDiscreteDynamicsWorld* GetWorld();
    btBroadphaseInterface* GetBroadphase();
private:
    btBroadphaseInterface* Broadphase;
    btDefaultCollisionConfiguration* CollisionConfig;
    btCollisionDispatcher* Dispatcher;
    btSequentialImpulseConstraintSolver* Solver;
    btDiscreteDynamicsWorld* DynamicsWorld;
    std::vector<btCollisionShape*> Shapes;
};

#endif
