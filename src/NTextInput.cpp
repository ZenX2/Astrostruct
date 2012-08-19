#include "NEngine.hpp"

NTextInput::NTextInput(std::string i_Texture) : NNode(NodeTextInput)
{
    HasFocus = false;
    DisplayCursor = false;
    Changed = true;
    Time = 0;
    Buffers = new GLuint[2];
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
        TextureWidth = Texture->GetSize().x;
        TextureHeight = Texture->GetSize().y;
        BorderSize = Texture->GetFloat("BorderSize");
    }
    DisplayText = NULL;
}

NTextInput::~NTextInput()
{
    glDeleteBuffers(2,Buffers);
    delete[] Buffers;
    if (Texture)
    {
        delete Texture;
    }
    if (HasFocus)
    {
        GetGame()->GetInput()->SetFocus(false);
    }
}

NText* NTextInput::GetText()
{
    if (!DisplayText)
    {
        DisplayText = new NText("gui",_t(""));
        DisplayText->SetParent(this);
        DisplayText->SetSize(13);
        DisplayText->SetPos(glm::vec3(-GetScale().x/2.f,0,0));
        DisplayText->SetBorder(GetScale().x,GetScale().y);
    }
    return DisplayText;
}

std::wstring NTextInput::GetEnteredText()
{
    return RealText;
}

void NTextInput::SetText(std::wstring Text)
{
    if (!DisplayText)
    {
        DisplayText = new NText("gui",_t(""));
        DisplayText->SetParent(this);
        DisplayText->SetSize(13);
        DisplayText->SetPos(glm::vec3(-GetScale().x/2.f,0,0));
        DisplayText->SetBorder(GetScale().x,GetScale().y);
    }
    RealText = Text;
    DisplayText->SetText(RealText);
}

void NTextInput::GenerateBuffers()
{
    if (!Texture || !Changed)
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
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER,Verts.size()*sizeof(glm::vec2),&Verts[0],GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,Buffers[1]);
    glBufferData(GL_ARRAY_BUFFER,UVs.size()*sizeof(glm::vec2),&UVs[0],GL_STATIC_DRAW);
    Changed = false;
}

void NTextInput::Draw(NCamera* View)
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
    glm::mat4 MVP = View->GetOrthoMatrix()*View->GetViewMatrix()*GetModelMatrix();
    glUniformMatrix4fv(MatrixLoc,1,GL_FALSE,&MVP[0][0]);
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

void NTextInput::Tick(double DT)
{
    if (!DisplayText)
    {
        DisplayText = new NText("gui",_t(""));
        DisplayText->SetParent(this);
        DisplayText->SetSize(13);
    }
    if (GetGame()->GetInput()->GetMouseKey(0))
    {
        if (Intersects(glm::vec4(GetRealPos().x,GetRealPos().y,GetScale().x,GetScale().y),GetGame()->GetInput()->GetMouse()))
        {
            GetGame()->GetInput()->SetFocus(true);
            DisplayCursor = true;
            Time = 0;
            HasFocus = true;
        } else {
            GetGame()->GetInput()->SetFocus(false);
            HasFocus = false;
        }
    }
    if (HasFocus)
    {
        DisplayText->SetPos(glm::vec3(-GetScale().x/2.f,0,0));
        DisplayText->SetBorder(GetScale().x,GetScale().y);
        Time += DT;
        if (Time>1)
        {
            DisplayCursor = !DisplayCursor;
            Time = 0;
        }
        std::vector<int> StringInput = GetGame()->GetInput()->GetStringInput();
        for (unsigned int i=0;i<StringInput.size();i++)
        {
            RealText.push_back(StringInput[i]);
            if (StringInput[i] == GLFW_KEY_BACKSPACE)
            {
                RealText.erase(RealText.end()-1);
                if (RealText.size()>0)
                {
                    RealText.erase(RealText.end()-1);
                }
            }
        }
    } else {
        DisplayCursor = false;
    }
    if (DisplayCursor)
    {
        DisplayText->SetText(RealText+_t("_"));
    } else {
        DisplayText->SetText(RealText);
    }
    if (!GetGame()->GetInput()->GetFocus())
    {
        DisplayCursor = false;
        HasFocus = false;
    }
}
void NTextInput::Unallocate()
{
    delete (NTextInput*)this;
}
