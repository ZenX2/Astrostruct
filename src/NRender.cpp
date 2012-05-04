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
}

/**
* @brief Creates the render system and initializes it.
*/
NRender::NRender()
{
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
	if (MaxFPS != 0)
	{
		double TimeDiff = CurTime()-LastTime;
		double WantedTime = 1.f/MaxFPS;
		if (TimeDiff<WantedTime)
		{
			glfwSleep(WantedTime-TimeDiff);
		}
	}
	LastTime = CurTime();
	glClearColor(0.435294118,0.309803922,0.439215686,1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	GetGame()->GetTextSystem()->UpdateMipmaps();
	if (Camera)
	{
		GetGame()->GetScene()->Draw(Camera->GetViewMatrix());
	} else {
		GetGame()->GetScene()->Draw(glm::mat4());
	}
	
	glfwSwapBuffers();
}

void NRender::AddTexture(NTexture* Texture)
{
	Texture->SetFilter(TextureFilter);
	Textures.push_back(Texture);
}

void NRender::RemoveTexture(NTexture* Texture)
{
	for (unsigned int i=0;i<Textures.size();i++)
	{
		if (Texture == Textures[i])
		{
			Textures.erase(Textures.begin()+i);
			return;
		}
	}
}

void NRender::SetTextureFilter(GLuint Filter)
{
	if (Filter != GL_NEAREST && Filter != GL_LINEAR)
	{
		return;
	}
	TextureFilter = Filter;
	for (unsigned int i=0;i<Textures.size();i++)
	{
		Textures[i]->SetFilter(TextureFilter);
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
