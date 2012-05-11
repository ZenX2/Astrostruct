#include "NEngine.hpp"

/**
* @brief Statically loads shaders into memory for other objects to use.
*
* @return Nothing.
*/
bool NRender::LoadShaders()
{
	NShader* Shader = new NShader("text");
	if (Shader->Load("data/shaders/text.vert","data/shaders/text.frag") != Fail)
	{
		Shaders.push_back(Shader);
	} else {
		delete Shader;
	}
	Shader = new NShader("flat");
	if (Shader->Load("data/shaders/flat.vert","data/shaders/flat.frag") != Fail)
	{
		Shaders.push_back(Shader);
	} else {
		delete Shader;
	}
}

/**
* @brief Creates the render system and initializes it.
*/
NRender::NRender()
{
	FrameTime = 0;
	LoadShaders();
	Camera = NULL;
	VSync = false;
	if (GetGame()->GetConfig()->GetBool("VerticalSync"))
	{
		SetVSync(true);
	}
	MaxFPS = GetGame()->GetConfig()->GetFloat("MaxFPS");
	LastTime= CurTime();
	TextureFilter = GL_LINEAR;
	std::string DesiredShader = GetGame()->GetConfig()->GetString("TextureFilter");
	if (DesiredShader == "GL_LINEAR")
	{
		TextureFilter = GL_LINEAR;
	} else if (DesiredShader == "GL_NEAREST")
	{
		TextureFilter = GL_NEAREST;
	}
}

/**
* @brief Deletes the render system and unloads all shaders from memory.
*/
NRender::~NRender()
{
	for (unsigned int i=0;i<Shaders.size();i++)
	{
		delete Shaders[i];
	}
}

/**
* @brief Returns a pointer to the specified shader object.
*
* @param Name The name of the shader.
*
* @return The pointer to the specifed shader object or null if it isn't found.
*/
NShader* NRender::GetShader(std::string Name)
{
	for (unsigned int i=0;i<Shaders.size();i++)
	{
		if (!Shaders[i]->GetName().compare(Name))
		{
			return Shaders[i];
		}
	}
	SetColor(Yellow);
	std::cout << "RENDER WARN: ";
	ClearColor();
	std::cout << "Attempted to use unknown shader " << Name << "!\n";
	return NULL;
}

NCamera* NRender::GetCamera()
{
	return Camera;
}

void NRender::SetCamera(NCamera* i_Camera)
{
	Camera = i_Camera;
}

void NRender::Draw()
{
	double TimeDiff = CurTime()-LastTime;
	FrameTime = TimeDiff;
	if (MaxFPS != 0)
	{
		double WantedTime = 1.f/MaxFPS;
		if (TimeDiff<WantedTime)
		{
			glfwSleep(WantedTime-TimeDiff);
		}
	}
	LastTime = CurTime();
	glClearColor(0.435294118,0.309803922,0.439215686,1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	if (Camera)
	{
		GetGame()->GetScene()->Draw(Camera->GetViewMatrix());
	} else {
		GetGame()->GetScene()->Draw(glm::mat4());
	}
	
	glfwSwapBuffers();
}

void NRender::AddCachedTexture(GLuint Texture)
{
	glBindTexture(GL_TEXTURE_2D,Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFilter);
	CachedTextures.push_back(new NCachedTexture("NULL",Texture));
}

void NRender::SetTextureFilter(GLuint Filter)
{
	if (Filter != GL_NEAREST && Filter != GL_LINEAR)
	{
		return;
	}
	TextureFilter = Filter;
	for (unsigned int i=0;i<CachedTextures.size();i++)
	{
		CachedTextures[i]->SetFilter(TextureFilter);
	}
}

GLuint NRender::GetTextureFilter()
{
	return TextureFilter;
}

void NRender::SetVSync(bool i_VSync)
{
	VSync = i_VSync;
	if (VSync)
	{
		glfwSwapInterval(1);
		return;
	}
	glfwSwapInterval(0);
}

bool NRender::GetVSync()
{
	return VSync;
}

double NRender::GetFrameTime()
{
	return FrameTime;
}

GLuint NRender::GetCachedTexture(std::string Name)
{
	for (unsigned int i=0;i<CachedTextures.size();i++)
	{
		if (CachedTextures[i]->Name == Name)
		{
			return CachedTextures[i]->ID;
		}
	}
	NCachedTexture* Cache = new NCachedTexture(Name,SOIL_load_OGL_texture(Name.c_str(),SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS));
	if (Cache->ID == 0)
	{
		SetColor(Yellow);
		std::cout << "TEXTURE WARN: ";
		ClearColor();
		std::cout << "Couldn't load file " << Name << " as a texture!\n";
		delete Cache;
	}
	CachedTextures.push_back(Cache);
	return Cache->ID;
}

NCachedTexture::NCachedTexture(std::string i_Name, GLuint i_ID)
{
	ID = i_ID;
	Name = i_Name;
	glBindTexture(GL_TEXTURE_2D,ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGame()->GetRender()->GetTextureFilter());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGame()->GetRender()->GetTextureFilter());
}

NCachedTexture::~NCachedTexture()
{
	glDeleteTextures(1,&ID);
}

void NCachedTexture::SetFilter(GLuint Filter)
{
	glBindTexture(GL_TEXTURE_2D,ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter);
}

void NRender::AddTexture(NTexture* Texture)
{
	Textures.push_back(Texture);
}

NTexture* NRender::GetTexture(std::string Name)
{
	for (unsigned int i=0;i<Textures.size();i++)
	{
		if (Textures[i]->Name == Name)
		{
			NTexture* NewTexture = new NTexture(Textures[i]);
			return NewTexture;
		}
	}
	return NULL;
}
