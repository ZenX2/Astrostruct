#include "NEngine.hpp"

NStar::NStar() : NNode(NodeStar)
{
    Texture = NULL;
    Changed = true;
    glGenBuffers(2,Buffers);
    Shader = GetGame()->GetRender()->GetShader("flat");
    if (Shader != NULL)
    {
        MatrixLoc = Shader->GetUniformLocation("MVP");
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
    if (Texture == NULL || GetColor().w == 0)
    {
        return;
    }
    if (Shader == NULL)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
        glVertexPointer(2,GL_FLOAT,0,NULL);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
        glTexCoordPointer(2,GL_FLOAT,0,NULL);

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        if (Texture != NULL)
        {
            glBindTexture(GL_TEXTURE_2D,Texture->GetID());
        }

        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(&View->GetOrthoMatrix()[0][0]);
        glMatrixMode(GL_MODELVIEW);
        glm::mat4 MVP = View->GetViewMatrix()*GetModelMatrix();
        glLoadMatrixf(&MVP[0][0]);

        glColor4fv(&(GetColor()[0]));
        glDrawArrays(GL_QUADS,0,Verts.size());
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        return;
    }
    glUseProgram(Shader->GetID());
    glActiveTexture(GL_TEXTURE0);
    if (Texture != NULL)
    {
        glBindTexture(GL_TEXTURE_2D,Texture->GetID());
    }
    glUniform1i(TextureLoc,0);
    glm::mat4 MVP = View->GetOrthoMatrix()*View->GetViewMatrix()*GetModelMatrix();
    glUniformMatrix4fv(MatrixLoc,1,GL_FALSE,&MVP[0][0]);
    glUniform4fv(ColorLoc,1,&(GetColor()[0]));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,NULL);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glDrawArrays(GL_QUADS,0,Verts.size());
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
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
