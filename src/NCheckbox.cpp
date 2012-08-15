#include "NEngine.hpp"

NCheckbox::NCheckbox(std::string i_Texture) : NNode(NodeCheckbox)
{
    //Bunch of random vars.
    Pressed = false;
    PressedMemory = false;
    Changed = true;
    Checked = false;
    //Allocate our opengl buffers.
    glGenBuffers(2,Buffers);
    //Attempt to grab our flat shader.
    Shader = GetGame()->GetRender()->GetShader("flat");
    if (Shader != NULL)
    {
        //If we succeed, grab our uniforms.
        MatrixLoc = Shader->GetUniformLocation("MVP");
        TextureLoc = Shader->GetUniformLocation("Texture");
        ColorLoc = Shader->GetUniformLocation("Color");
    }
    //Attempt to grab the desired texture.
    Texture = GetGame()->GetRender()->GetTexture(i_Texture);
}

NCheckbox::~NCheckbox()
{
    //Thou whom allocates, deallocates!
    glDeleteBuffers(2,Buffers);
    if (Texture)
    {
        delete Texture;
    }
}

void NCheckbox::SetCheck(bool Check)
{
    Checked = Check;
}

void NCheckbox::GenerateBuffers()
{
    //If we changed and have a texture, generate a simple quad.
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

void NCheckbox::Draw(NCamera* View)
{
    GenerateBuffers();
    //If we have everything to draw, draw!
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

void NCheckbox::Tick(double DT)
{
    if (!Texture)
    {
        return;
    }
    Texture->Tick(DT);
    //Checks if the mouse intersects and changes state if the mouse is down.
    if (Intersects(glm::vec4(GetRealPos().x,GetRealPos().y,GetScale().x,GetScale().y),GetGame()->GetInput()->GetMouse()))
    {
        if (GetGame()->GetInput()->GetMouseKey(0))
        {
            Pressed = true;
        } else if (PressedMemory != Pressed && Pressed == true)
        {
            Checked = !Checked;
            PressedMemory = Pressed;
        }
        if (Checked)
        {
            Texture->Play("checked_active");
        } else {
            Texture->Play("unchecked_active");
        }
    } else {
        PressedMemory = Pressed = false;
        if (Checked)
        {
            Texture->Play("checked_idle");
        } else {
            Texture->Play("idle");
        }
    }
}

bool NCheckbox::IsChecked()
{
    return Checked;
}

void NCheckbox::Unallocate()
{
    delete (NCheckbox*)this;
}
