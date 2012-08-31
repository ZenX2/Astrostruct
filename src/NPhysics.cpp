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

bool Intersects(glm::vec3 BoxPos, glm::vec3 BoxSize, glm::vec3 Point)
{
    glm::vec3 AMin = BoxPos-BoxSize/2.f;
    glm::vec3 AMax = BoxPos+BoxSize/2.f;
    if (Point.x > AMax.x ||
        Point.y > AMax.y ||
        Point.z > AMax.z ||
        Point.x < AMin.x ||
        Point.y < AMin.y ||
        Point.z < AMin.z)
    {
        return false;
    }
    return true;
}
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

bool Facing(glm::vec3 Point, std::vector<glm::vec3> Face)
{
    glm::vec3 Center = glm::vec3((Face[0].x+Face[1].x+Face[2].x+Face[3].x)/4.f,(Face[0].y+Face[1].y+Face[2].y+Face[3].y)/4.f,(Face[0].z+Face[1].z+Face[2].z+Face[3].z)/4.f);
    
    glm::vec3 Aim = glm::normalize(glm::vec3(Point.x-Center.x,Point.y-Center.y,Point.z-Center.z));
    glm::vec3 Direction = glm::normalize(glm::cross(Face[0]-Face[1],Face[1]-Face[2]));

    //the angle between the normal of the face and the difference of the point and face
    double Difference = acos(glm::dot(Aim,Direction)/(glm::length(Aim)*glm::length(Direction)));

    if (Difference < PI/2)
    {
        return true;
    }
    return false;
}

void NPhysics::Step(double DT)
{
    DynamicsWorld->stepSimulation(DT,6);
}

NPhysics::NPhysics()
{
    Broadphase = new btDbvtBroadphase();
    CollisionConfig = new btDefaultCollisionConfiguration();
    Dispatcher = new btCollisionDispatcher(CollisionConfig);
    Solver = new btSequentialImpulseConstraintSolver();
    DynamicsWorld = new btDiscreteDynamicsWorld(Dispatcher,Broadphase,Solver,CollisionConfig);
    DynamicsWorld->setGravity(btVector3(0,0,-150));
}

NPhysics::~NPhysics()
{
    delete DynamicsWorld;
    delete Solver;
    delete Dispatcher;
    delete CollisionConfig;
    delete Broadphase;
}
btDiscreteDynamicsWorld* NPhysics::GetWorld()
{
    return DynamicsWorld;
}
btBroadphaseInterface* NPhysics::GetBroadphase()
{
    return Broadphase;
}
