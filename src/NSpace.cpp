#include "NEngine.hpp"

NStar::NStar() : NNode(NodeStar)
{
    Texture = NULL;
    Changed = true;
    glGenBuffers(2,Buffers);
    Shader = GetGame()->GetRender()->GetShader("flat");
    if (Shader != NULL)
    {
        MatrixLoc = Shader->GetUniformLocation("Model");
        TextureLoc = Shader->GetUniformLocation("Texture");
        ColorLoc = Shader->GetUniformLocation("Color");
    }
    Texture = GetGame()->GetRender()->GetTexture("star");
    if (Texture)
    {
        unsigned int AnimationCount = Texture->GetAnimationCount();
        Texture->Play(rand()%AnimationCount);
    }
    float Size = Rand(5,32);
    SetScale(glm::vec3(Size,Size,1));
    //float Rot = Rand(0,360);
    //SetAng(glm::vec3(0,0,Rot));
    SetParent(GetGame()->GetRender()->GetCamera());
    SetPos(glm::vec3(GetGame()->GetWindowWidth()+GetScale().x,Rand(0,GetGame()->GetWindowHeight()),0));
}

NStar::~NStar()
{
    if (Texture)
    {
        delete Texture;
    }
}

void NStar::GenerateBuffers()
{
    if (!Texture || !Changed)
    {
        return;
    }
    Verts.clear();
    UVs.clear();
    Verts.push_back(glm::vec2(-.5,-.5));
    UVs.push_back(glm::vec2(0,1));
    Verts.push_back(glm::vec2(.5,-.5));
    UVs.push_back(glm::vec2(1,1));
    Verts.push_back(glm::vec2(.5,.5));
    UVs.push_back(glm::vec2(1,0));
    Verts.push_back(glm::vec2(-.5,.5));
    UVs.push_back(glm::vec2(0,0));
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER,Verts.size()*sizeof(glm::vec2),&Verts[0],GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
    glBufferData(GL_ARRAY_BUFFER,UVs.size()*sizeof(glm::vec2),&UVs[0],GL_STATIC_DRAW);
    Changed = false;
}

void NStar::Draw(NCamera* View)
{
    GenerateBuffers();
    if (Texture == NULL || GetColor().w == 0 || Shader == NULL)
    {
        return;
    }
    glUseProgram(Shader->GetID());
    glActiveTexture(GL_TEXTURE0);
    if (Texture != NULL)
    {
        glBindTexture(GL_TEXTURE_2D,Texture->GetID());
    }
    glUniform1i(TextureLoc,0);
    glUniformMatrix4fv(MatrixLoc,1,GL_FALSE,glm::value_ptr(GetModelMatrix()));
    glUniform4fv(ColorLoc,1,&(GetColor()[0]));
    glEnableVertexAttribArray(Shader->GetVertexAttribute());
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
    glVertexAttribPointer(Shader->GetVertexAttribute(),2,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(Shader->GetUVAttribute());
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
    glVertexAttribPointer(Shader->GetUVAttribute(),2,GL_FLOAT,GL_FALSE,0,NULL);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glDrawArrays(GL_QUADS,0,Verts.size());
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glDisableVertexAttribArray(Shader->GetVertexAttribute());
    glDisableVertexAttribArray(Shader->GetUVAttribute());
    glUseProgram(0);
}

void NStar::Tick(double DT)
{
    if (Texture)
    {
        Texture->Tick(DT);
    }
    SetPos(GetPos()-glm::vec3(GetScale().x*DT,0,0));
    if (GetPos().x+GetScale().x/2.f < 0)
    {
        GetGame()->GetScene()->Remove(this);
    }
}

NodeType NStar::GetType()
{
    return NodeStar;
}

void NStar::Unallocate()
{
    delete (NStar*)this;
}
