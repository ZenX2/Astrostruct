#include "NEngine.hpp"

//NButton inherits NNode
NButton::NButton(std::string i_Texture) : NNode(NodeButton)
{
    Toggled = false;
    Toggleable = false;
    BorderSize = 0;
    Texture = NULL;
    Changed = true;
    DisplayText = NULL;
    IsPressed = false;
    IsChanged = false;
    PressedMemory = false;
    //Allocate some buffers
    glGenBuffers(2,Buffers);
    //Try to grab our 'flat' shader, if we succeed grab our uniform locations as well.
    Shader = GetGame()->GetRender()->GetShader("flat");
    if (Shader != NULL)
    {
        MatrixLoc = Shader->GetUniformLocation("MVP");
        TextureLoc = Shader->GetUniformLocation("Texture");
        ColorLoc = Shader->GetUniformLocation("Color");
    }
    //Try to grab our desired texture, if we succeed grab the texture's size so we can generate a proper mesh to display it.
    TextureWidth = 0;
    TextureHeight = 0;
    Texture = GetGame()->GetRender()->GetTexture(i_Texture);
    if (Texture)
    {
        TextureWidth = Texture->GetSize().x;
        TextureHeight = Texture->GetSize().y;
        //This variable is specified by the texture's lua file. If it doesn't exist it'll be 0 and will generate a normal quad.
        BorderSize = Texture->GetFloat("BorderSize");
    }
}

NButton::~NButton()
{
    //Delete our previously allocated buffers.
    glDeleteBuffers(2,Buffers);
    //If we succeeded in finding our texture, it needs to be unallocated.
    if (Texture)
    {
        delete Texture;
    }
}

NText* NButton::GetText()
{
    return DisplayText;
}
void NButton::GenerateBuffers()
{
    //Only generate new buffers if we have a texture and the button's mesh has changed in some way.
    if (!Texture || !Changed)
    {
        return;
    }
    Verts.clear();
    UVs.clear();
    //This massive block of variables generates a mesh that keeps the button texture's borders from stretching, but only if we found a proper BorderSize variable.
    //We also compensate for the scale set by NNode to make it pixel perfect (almost).
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
        float UX = BorderSize/TextureWidth;
        float UY = BorderSize/TextureHeight;
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
        UVs.push_back(glm::vec2(1,1-UY));
        Verts.push_back(glm::vec2(.5-SX,-.5+SY));
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
    //Then we stick all the data into opengl buffers in preparation to be drawn.
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER,Verts.size()*sizeof(glm::vec2),&Verts[0],GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
    glBufferData(GL_ARRAY_BUFFER,UVs.size()*sizeof(glm::vec2),&UVs[0],GL_STATIC_DRAW);
    Changed = false;
}

void NButton::Draw(NCamera* View)
{
    //Make sure we have our buffers generated.
    GenerateBuffers();
    //If we don't have a texture, are invisible, or our shader doesn't exist, don't draw!
    if (Texture == NULL || GetColor().w == 0 || Shader == NULL)
    {
        return;
    }
    glUseProgram(Shader->GetID());
    glActiveTexture(GL_TEXTURE0);
    //Make sure we have a texture before we try to call a member in it.
    if (Texture != NULL)
    {
        glBindTexture(GL_TEXTURE_2D,Texture->GetID());
    }
    glUniform1i(TextureLoc,0);
    //Generate our matrix to send to the GPU.
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
    //blahblahblah and then we finally draw using the Size of the Verts array we generated in GenerateBuffers.
    glDrawArrays(GL_QUADS,0,Verts.size());
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glUseProgram(0);
}

//Here we do a bunch of ifelseifelseifififelse to make our little quad act like a button.
void NButton::Tick(double DT)
{
    IsPressed = false;
    //Make sure if we have a texture to move it forward in time, just in case it's animated.
    if (Texture)
    {
        Texture->Tick(DT);
    }
    glm::vec2 MP = GetGame()->GetInput()->GetMouse();
    //Check if the mouse intersects with button
    if (Intersects(glm::vec4(GetRealPos().x,GetRealPos().y,GetScale().x,GetScale().y),MP))
    {
        if (GetGame()->GetInput()->GetMouseKey(0))
        {
            if (Texture)
            {
                Texture->Play("pressed");
            }
            IsPressed = true;
        } else {
            if (Texture)
            {
                Texture->Play("active");
            }
        }
        if (IsPressed != PressedMemory)
        {
            IsChanged = true;
            PressedMemory = IsPressed;
        } else {
            IsChanged = false;
        }
    } else {
        PressedMemory = false;
        IsChanged = false;
        if (Texture)
        {
            Texture->Play("idle");
        }
    }
    if (Toggleable)
    {
        if (IsPressed && IsChanged)
        {
            Toggled = !Toggled;
        }
        if (Toggled)
        {
            Texture->Play("pressed");
        }
    }
}

void NButton::SetText(std::wstring Text)
{
    if (DisplayText == NULL)
    {
        DisplayText = new NText("didactgothic",Text);
        DisplayText->SetMode(1);
        DisplayText->SetSize(GetScale().y/1.3);
        DisplayText->SetParent(this);
        DisplayText->SetPos(glm::vec3(0,4,0));
        return;
    }
    DisplayText->SetText(Text);
}

bool NButton::OnPressed()
{
    if (GetColor().w == 0)
    {
        return false;
    }
    return IsPressed;
}

bool NButton::OnRelease()
{
    if (GetColor().w == 0)
    {
        return false;
    }
    if (!IsPressed && IsChanged)
    {
    return true;
    }
    return false;
}

void NButton::Unallocate()
{
    delete (NButton*)this;
}

void NButton::SetColor(glm::vec4 i_Color)
{
    if (DisplayText != NULL)
    {
        DisplayText->SetColor(i_Color);
    }
    Color = i_Color;
}
void NButton::SetToggleable(bool i_Toggleable)
{
    Toggleable = i_Toggleable;
}
bool NButton::GetToggle()
{
    return Toggled;
}
void NButton::SetToggle(bool i_Toggled)
{
    Toggled = i_Toggled;
}
