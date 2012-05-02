#include "NEngine.hpp"

bool NRender::LoadShaders()
{
	NShader* Shader = new NShader("flat");
	if (Shader->Load("data/shaders/flat.vert","data/shaders/flat.frag") != Fail)
	{
		Shaders.push_back(Shader);
	} else {
		delete Shader;
	}
}

NRender::NRender()
{
	LoadShaders();
	Camera = NULL;
	MaxFPS = 60;
	LastTime= CurTime();
	TextureFilter = GL_LINEAR;
}

NRender::~NRender()
{
	for (unsigned int i=0;i<Shaders.size();i++)
	{
		delete Shaders[i];
	}
}

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
	double WantedTime = 1.f/MaxFPS;
	if (TimeDiff<WantedTime)
	{
		glfwSleep(WantedTime-TimeDiff);
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
