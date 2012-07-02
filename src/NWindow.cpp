#include "NEngine.hpp"

NWindow::NWindow(std::string i_Texture)
{
    UI = true;
    Persp = false;
    SizeMem = glm::vec3(0);
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
    Texture = GetGame()->GetRender()->GetTexture(i_Texture);
    if (Texture)
    {
        BorderSize = Texture->GetFloat("BorderSize");
    }
}

NWindow::~NWindow()
{
    glDeleteBuffers(2,Buffers);
    if (Texture)
    {
        delete Texture;
    }
}
void NWindow::GenerateBuffers()
{
    if (!Texture)
    {
        return;
    }
    if (!Changed && SizeMem == GetScale())
    {
        return;
    }
    Verts.clear();
    UVs.clear();
    if (BorderSize == 0)
    {
        Verts.push_back(glm::vec2(-.5,-.5));
        UVs.push_back(glm::vec2(0,1));
        Verts.push_back(glm::vec2(.5,-.5));
        UVs.push_back(glm::vec2(1,1));
        Verts.push_back(glm::vec2(.5,.5));
        UVs.push_back(glm::vec2(1,0));
        Verts.push_back(glm::vec2(-.5,.5));
        UVs.push_back(glm::vec2(0,0));
    } else {
        float SY = BorderSize/GetScale().y;
        float SX = BorderSize/GetScale().x;
        float UX = BorderSize/Texture->GetSize().x;
        float UY = BorderSize/Texture->GetSize().y;
        //Top Left Corner
        Verts.push_back(glm::vec2(-.5,.5));
        UVs.push_back(glm::vec2(0,0));
        Verts.push_back(glm::vec2(-.5+SX,.5));
        UVs.push_back(glm::vec2(UX,0));
        Verts.push_back(glm::vec2(-.5+SX,.5-SY));
        UVs.push_back(glm::vec2(UX,UY));
        Verts.push_back(glm::vec2(-.5,.5-SY));
        UVs.push_back(glm::vec2(0,UY));
        //Top
        Verts.push_back(glm::vec2(-.5+SX,.5));
        UVs.push_back(glm::vec2(UX,0));
        Verts.push_back(glm::vec2(.5-SX,.5));
        UVs.push_back(glm::vec2(1-UX,0));
        Verts.push_back(glm::vec2(.5-SX,.5-SY));
        UVs.push_back(glm::vec2(1-UX,UY));
        Verts.push_back(glm::vec2(-.5+SX,.5-SY));
        UVs.push_back(glm::vec2(UX,UY));
        //Top Right Corner
        Verts.push_back(glm::vec2(.5,.5));
        UVs.push_back(glm::vec2(1,0));
        Verts.push_back(glm::vec2(.5-SX,.5));
        UVs.push_back(glm::vec2(1-UX,0));
        Verts.push_back(glm::vec2(.5-SX,.5-SY));
        UVs.push_back(glm::vec2(1-UX,UY));
        Verts.push_back(glm::vec2(.5,.5-SY));
        UVs.push_back(glm::vec2(1,UY));
        //Left
        Verts.push_back(glm::vec2(-.5,.5-SY));
        UVs.push_back(glm::vec2(0,UY));
        Verts.push_back(glm::vec2(-.5+SX,.5-SY));
        UVs.push_back(glm::vec2(UX,UY));
        Verts.push_back(glm::vec2(-.5+SX,-.5+SY));
        UVs.push_back(glm::vec2(UX,1-UY));
        Verts.push_back(glm::vec2(-.5,-.5+SY));
        UVs.push_back(glm::vec2(0,1-UY));
        //Middle
        Verts.push_back(glm::vec2(-.5+SX,.5-SY));
        UVs.push_back(glm::vec2(UX,UY));
        Verts.push_back(glm::vec2(.5-SX,.5-SY));
        UVs.push_back(glm::vec2(1-UX,UY));
        Verts.push_back(glm::vec2(.5-SX,-.5+SY));
        UVs.push_back(glm::vec2(1-UX,1-UY));
        Verts.push_back(glm::vec2(-.5+SX,-.5+SY));
        UVs.push_back(glm::vec2(UX,1-UY));
        //Right
        Verts.push_back(glm::vec2(.5-SX,.5-SY));
        UVs.push_back(glm::vec2(1-UX,UY));
        Verts.push_back(glm::vec2(.5,.5-SY));
        UVs.push_back(glm::vec2(1,UY));
        Verts.push_back(glm::vec2(.5,-.5+SY));
        UVs.push_back(glm::vec2(1,1-UY)); Verts.push_back(glm::vec2(.5-SX,-.5+SY));
        UVs.push_back(glm::vec2(1-UX,1-UY));
        //Bottom Left Corner
        Verts.push_back(glm::vec2(-.5,-.5+SY));
        UVs.push_back(glm::vec2(0,1-UY));
        Verts.push_back(glm::vec2(-.5+SX,-.5+SY));
        UVs.push_back(glm::vec2(UX,1-UY));
        Verts.push_back(glm::vec2(-.5+SX,-.5));
        UVs.push_back(glm::vec2(UX,1));
        Verts.push_back(glm::vec2(-.5,-.5));
        UVs.push_back(glm::vec2(0,1));
        //Bottom
        Verts.push_back(glm::vec2(-.5+SX,-.5+SY));
        UVs.push_back(glm::vec2(UX,1-UY));
        Verts.push_back(glm::vec2(.5-SX,-.5+SY));
        UVs.push_back(glm::vec2(1-UX,1-UY));
        Verts.push_back(glm::vec2(.5-SX,-.5));
        UVs.push_back(glm::vec2(1-UX,1));
        Verts.push_back(glm::vec2(-.5+SX,-.5));
        UVs.push_back(glm::vec2(UX,1));
        //Bottom Right Corner
        Verts.push_back(glm::vec2(.5-SX,-.5+SY));
        UVs.push_back(glm::vec2(1-UX,1-UY));
        Verts.push_back(glm::vec2(.5,-.5+SY));
        UVs.push_back(glm::vec2(1,1-UY));
        Verts.push_back(glm::vec2(.5,-.5));
        UVs.push_back(glm::vec2(1,1));
        Verts.push_back(glm::vec2(.5-SX,-.5));
        UVs.push_back(glm::vec2(1-UX,1));
    }
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER,Verts.size()*sizeof(glm::vec2),&Verts[0],GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
    glBufferData(GL_ARRAY_BUFFER,UVs.size()*sizeof(glm::vec2),&UVs[0],GL_STATIC_DRAW);
    Changed = false;
    SizeMem = GetScale();
}

void NWindow::Draw(NCamera* View)
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
    glm::mat4 MVP;
    if (Persp)
    {
        MVP = View->GetPerspMatrix()*View->GetPerspViewMatrix()*GetModelMatrix();
    } else {
        MVP = View->GetOrthoMatrix()*View->GetViewMatrix()*GetModelMatrix();
    }
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

void NWindow::Tick(double DT)
{
    if (!UI)
    {
        if (Texture)
        {
            Texture->Tick(DT);
        }
        return;
    }
    if (Intersects(glm::vec4(GetRealPos().x,GetRealPos().y,GetScale().x,GetScale().y),GetGame()->GetInput()->GetMouse()))
    {
        GetGame()->GetInput()->SetMouseHitGUI(true);
    }
    if (Texture)
    {
        Texture->Tick(DT);
    }
}
void NWindow::Remove()
{
    delete (NWindow*)this;
}
NodeType NWindow::GetType()
{
    return NodeWindow;
}
void NWindow::SwapView()
{
    Persp = !Persp;
}
void NWindow::SetUI(bool i_UI)
{
    UI = i_UI;
}
