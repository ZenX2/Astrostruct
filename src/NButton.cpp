#include "NEngine.hpp"

NButton::NButton()
{
	BorderSize = 0;
	Texture = NULL;
	Changed = true;
	Buffers = new GLuint[2];
	glGenBuffers(2,Buffers);
	Shader = GetGame()->GetRender()->GetShader("flat");
	DisplayText = NULL;
	if (Shader != NULL)
	{
		MatrixLoc = Shader->GetUniformLocation("MVP");
		TextureLoc = Shader->GetUniformLocation("Texture");
		ColorLoc = Shader->GetUniformLocation("Color");
	}
	IsPressed = false;
	IsChanged = false;
	PressedMemory = false;
	TextureWidth = 0;
	TextureHeight = 0;
}

NButton::~NButton()
{
	glDeleteBuffers(2,Buffers);
	delete[] Buffers;
	if (Texture)
	{
		delete Texture;
	}
}

void NButton::SetTexture(std::string Name)
{
	Texture = GetGame()->GetRender()->GetTexture(Name);
	if (Texture)
	{
		TextureWidth = Texture->GetSize().x;
		TextureHeight = Texture->GetSize().y;
		BorderSize = Texture->GetFloat("BorderSize");
	}
}

void NButton::GenerateBuffers()
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

void NButton::Draw(NCamera* View)
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

void NButton::Tick(double DT)
{
	IsPressed = false;
	if (Texture)
	{
		Texture->Tick(DT);
	}
	glm::vec2 MP = GetGame()->GetInput()->GetMouse();
	if (Intersects(glm::vec4(GetPos().x,GetPos().y,GetScale().x,GetScale().y),MP))
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
}

void NButton::SetText(std::wstring Text)
{
	if (DisplayText == NULL)
	{
		DisplayText = GetGame()->GetScene()->AddText("didactgothic",Text);
		DisplayText->SetMode(1);
		DisplayText->SetSize(GetScale().y/1.3);
		DisplayText->SetParent(this);
		DisplayText->SetPos(0,4);
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

void NButton::Remove()
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
std::string NButton::GetType()
{
	return "Button";
}
