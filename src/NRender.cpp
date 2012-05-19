#include "NEngine.hpp"

void NRender::LoadTextures()
{
	lua_State* L = GetGame()->GetLua()->GetL();
	static const luaL_Reg BaseFunctions[] = {
		{"Animation",CreateAnimation},
		{"LoadTexture",LoadTexture},
		{NULL,NULL}
	};
	lua_getglobal(L,"_G");
	luaL_register(L,NULL,BaseFunctions);
	lua_pop(L,1);
	luaL_getmetatable(L,"AnimationBase");
	if (lua_isnoneornil(L,-1))
	{
		lua_pop(L,1);
		luaL_newmetatable(L,"AnimationBase");
	}
	static const luaL_Reg AnimationMethods[] = {
		{"__index",Animation__index},
		{"__newindex",Animation__newindex},
		{NULL,NULL}
	};
	luaL_register(L,NULL,AnimationMethods);
	lua_pushstring(L,"Animation");
	lua_setfield(L,-2,"__type");
	lua_pop(L,1);
	GetGame()->GetLua()->DoFolder("data/textures");
}

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
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glViewport(0,0,GetGame()->GetWindowWidth(),GetGame()->GetWindowHeight());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, GetGame()->GetWindowWidth(), 0, GetGame()->GetWindowHeight(), 0, 300);
	glMatrixMode(GL_MODELVIEW);
	Size = GetGame()->GetWindowSize();
}

void NRender::SetSize(glm::vec2 i_Size)
{
	if (Size == i_Size)
	{
		return;
	}
	Size = i_Size;
	glViewport(0,0,Size.x,Size.y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, Size.x, 0, Size.y, 0, 300);
	glMatrixMode(GL_MODELVIEW);
}

void NRender::SetSize(float Width, float Height)
{
	SetSize(glm::vec2(Width,Height));
}

NRender::~NRender()
{
	for (unsigned int i=0;i<Shaders.size();i++)
	{
		delete Shaders[i];
	}
	for (unsigned int i=0;i<CachedTextures.size();i++)
	{
	    delete CachedTextures[i];
	}
	for (unsigned int i=0;i<Textures.size();i++)
	{
	    delete Textures[i];
	}
	for (unsigned int i=0;i<Animations.size();i++)
	{
	    delete Animations[i];
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
		GetGame()->GetScene()->Draw(Camera);
	} else {
		Camera = new NCamera();
		GetGame()->GetScene()->Draw(Camera);
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

void NRender::AddCachedAnimation(NAnimation* Animation)
{
    Animations.push_back(Animation);
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

NCachedTexture* NRender::GetCachedTexture(std::string Name)
{
	for (unsigned int i=0;i<CachedTextures.size();i++)
	{
		if (CachedTextures[i]->Name == Name)
		{
			return CachedTextures[i];
		}
	}
	NCachedTexture* Cache = new NCachedTexture(Name);
	if (Cache->ID == 0)
	{
		SetColor(Yellow);
		std::cout << "TEXTURE WARN: ";
		ClearColor();
		std::cout << "Couldn't load file " << Name << " as a texture!\n";
		delete Cache;
	}
	CachedTextures.push_back(Cache);
	return Cache;
}

NCachedTexture::NCachedTexture(std::string i_Name)
{
	Name = i_Name;
	int Channels;
	unsigned char* Image = SOIL_load_image(Name.c_str(), &Width, &Height, &Channels, SOIL_LOAD_AUTO);
	ID = SOIL_create_OGL_texture(Image, Width, Height, Channels, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	SOIL_free_image_data(Image);
	glBindTexture(GL_TEXTURE_2D,ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGame()->GetRender()->GetTextureFilter());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGame()->GetRender()->GetTextureFilter());
}

NCachedTexture::NCachedTexture(std::string i_Name, GLuint i_ID)
{
	Name = i_Name;
	ID = i_ID;
	glBindTexture(GL_TEXTURE_2D,ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGame()->GetRender()->GetTextureFilter());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGame()->GetRender()->GetTextureFilter());
	Width = 0;
	Height = 0;
}

float NCachedTexture::GetWidth()
{
	return Width;
}

float NCachedTexture::GetHeight()
{
	return Height;
}

glm::vec2 NCachedTexture::GetSize()
{
	return glm::vec2(GetWidth(),GetHeight());
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
	SetColor(Yellow);
	std::cout << "RENDER WARN: ";
	ClearColor();
	std::cout << "An unkown texture was attempted to be loaded: " << Name << "\n";
	return NULL;
}
