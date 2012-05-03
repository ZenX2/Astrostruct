#include "NEngine.hpp"

NTexture::NTexture(GLuint i_TextureID)
{
	TextureID = i_TextureID;
	glBindTexture(GL_TEXTURE_2D,TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GetGame()->GetRender()->AddTexture(this);
}

NTexture::~NTexture()
{
	glDeleteTextures(1,&TextureID);
	GetGame()->GetRender()->RemoveTexture(this);
}

void NTexture::SetFilter(GLuint Filter)
{
	if (Filter != GL_NEAREST && Filter != GL_LINEAR)
	{
		return;
	}
	glBindTexture(GL_TEXTURE_2D,TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter);
}

GLuint NTexture::GetID()
{
	return TextureID;
}
