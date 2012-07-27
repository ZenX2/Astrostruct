#include "NEngine.hpp"

NCamera::NCamera() : NNode(NodeCamera)
{
    Zoom = 1.f;
    OrthoMatrix = glm::mat4(1.f);
    PerspMatrix = glm::mat4(1.f);
    ViewMatrix = glm::mat4(1.f);
    PerspViewMatrix = glm::mat4(1.f);
    Changed = true;
}

glm::mat4 NCamera::GetViewMatrix()
{
    UpdateCameraMatrix();
    return ViewMatrix;
}

glm::mat4 NCamera::GetPerspViewMatrix()
{
    UpdateCameraMatrix();
    return PerspViewMatrix;
}

glm::mat4 NCamera::GetOrthoMatrix()
{
    UpdateCameraMatrix();
    return OrthoMatrix;
}

glm::mat4 NCamera::GetPerspMatrix()
{
    UpdateCameraMatrix();
    return PerspMatrix;
}

void NCamera::UpdateCameraMatrix()
{
    if (!Changed && !GetGame()->GetWindowChanged())
    {
        return;
    }
    ViewMatrix = glm::translate(glm::mat4(1.f),-GetPos());
    ViewMatrix = glm::scale(ViewMatrix,GetScale());
    PerspViewMatrix = ViewMatrix; //Until we actually need real 3d view...
    OrthoMatrix = glm::ortho(0.f,(float)GetGame()->GetWindowWidth(),0.f,(float)GetGame()->GetWindowHeight(),0.f,1000.f);
    PerspMatrix = glm::perspective(45.f,float(GetGame()->GetWindowWidth())/float(GetGame()->GetWindowHeight()),32.f,1000.f);
    Changed = false;
}

void NCamera::SetPos(glm::vec3 i_Position)
{
    Position = i_Position;
    Changed = true;
    UpdateMatrix();
}

void NCamera::SetScale(glm::vec3 i_Scale)
{
    Scale = i_Scale;
    Changed = true;
    UpdateMatrix();
}

void NCamera::SetAng(glm::vec3 i_Angle)
{
    Angle = i_Angle;
    Changed = true;
    UpdateMatrix();
}

void NCamera::Unallocate()
{
    delete (NCamera*)this;
}
