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
	GetGame()->GetLua()->DoFolder("textures");
}

bool NRender::LoadShaders()
{
	NShader* Shader = new NShader("text");
	if (Shader->Load("shaders/text.vert","shaders/text.frag") != Fail)
	{
		Shaders.push_back(Shader);
	} else {
		delete Shader;
	}
	Shader = new NShader("flat");
	if (Shader->Load("shaders/flat.vert","shaders/flat.frag") != Fail)
	{
		Shaders.push_back(Shader);
	} else {
		delete Shader;
	}
	Shader = new NShader("map");
	if (Shader->Load("shaders/map.vert","shaders/map.frag") != Fail)
	{
		Shaders.push_back(Shader);
	} else {
		delete Shader;
	}
	Shader = new NShader("normal");
	if (Shader->Load("shaders/normal.vert","shaders/normal.frag") != Fail)
	{
		Shaders.push_back(Shader);
	} else {
		delete Shader;
	}
}

NRender::NRender()
{
	/*GLuint FBO;
	glGenFramebuffers(1,&FBO);
	glBindFramebuffer(GL_FRAMEBUFFER,FBO);
	GLuint Depth;
	glGenRenderbuffers(1,&Depth);
	glBindRenderbuffer(GL_RENDERBUFFER,Depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, GetGame()->GetWindowWidth(), GetGame()->GetWindowHeight());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,Depth);
	glBindRenderbuffer(GL_RENDERBUFFER,0);
	GLuint Texture;
	glGenTextures(1,&Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,GetGame()->GetWindowWidth(),GetGame()->GetWindowHeight(),0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER*/
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
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
	Size = GetGame()->GetWindowSize();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void NRender::SetSize(glm::vec2 i_Size)
{
	if (Size == i_Size)
	{
		return;
	}
	Size = i_Size;
	glViewport(0,0,Size.x,Size.y);
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
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (Camera)
	{
		GetGame()->GetScene()->Draw(Camera);
	} else {
		Camera = new NCamera();
		GetGame()->GetScene()->Draw(Camera);
	}
	glfwSwapBuffers();
	glError();
}

void NRender::glError()
{
	GLenum Error = glGetError();
	while (Error != GL_NO_ERROR)
	{
		std::string ErrorString;
		switch (Error)
		{
			case GL_INVALID_OPERATION: 				ErrorString = "GL_INVALID_OPERATION"; break;
			case GL_INVALID_ENUM: 					ErrorString = "GL_INVALID_ENUM"; break;
			case GL_INVALID_VALUE: 					ErrorString = "GL_INVALID_VALUE"; break;
			case GL_OUT_OF_MEMORY: 					ErrorString = "GL_OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: 	ErrorString = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
			default: 								ErrorString = "UNKOWN_ERROR"; break;
		}
		SetColor(Yellow);
		std::cout << "RENDER WARN: ";
		ClearColor();
		std::cout << "OpenGL reported an error: " << ErrorString << ".\n";
		Error = glGetError();
	}
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
	CachedTextures.push_back(Cache);
	return Cache;
}

NCachedTexture::NCachedTexture(std::string i_Name)
{
	IsGood = false;
	ID = 0;
	Width = 0;
	Height = 0;
	Name = i_Name;
	int Channels;
	NFile File = GetGame()->GetFileSystem()->GetFile(Name);
	if (!File.Good())
	{
		SetColor(Yellow);
		std::cout << "TEXTURE WARN: ";
		ClearColor();
		std::cout << "Couldn't load " << Name << " as a texture, it doesn't exist!\n";
		return;
	}
	char* ImageData = new char[File.Size()];
	File.Read(ImageData,File.Size());
	unsigned char* Image = SOIL_load_image_from_memory((const unsigned char*)ImageData,File.Size(),&Width, &Height, &Channels, SOIL_LOAD_AUTO);
	ID = SOIL_create_OGL_texture(Image, Width, Height, Channels, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	SOIL_free_image_data(Image);
	delete[] ImageData;
	glBindTexture(GL_TEXTURE_2D,ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGame()->GetRender()->GetTextureFilter());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGame()->GetRender()->GetTextureFilter());
	IsGood = true;
}

bool NCachedTexture::Good()
{
	return IsGood;
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
			if (!Textures[i]->Good())
			{
				SetColor(Yellow);
				std::cout << "RENDER WARN: ";
				ClearColor();
				std::cout << "A bad texture was attempted to be loaded: " << Name << "!\n";
				return NULL;
			}
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
