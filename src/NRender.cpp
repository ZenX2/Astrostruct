#include "NEngine.hpp"

void NRender::LoadTextures()
{
    GetGame()->GetLua()->DoFolder("textures");
}

bool NRender::LoadShaders()
{
    int Result = 0;
    NShader* Shader = new NShader("text");
    if (!Shader->Load("shaders/text.vert","shaders/text.frag"))
    {
        Shaders.push_back(Shader);
    } else {
        Result++;
        delete Shader;
    }
    Shader = new NShader("flat");
    if (!Shader->Load("shaders/flat.vert","shaders/flat.frag"))
    {
        Shaders.push_back(Shader);
    } else {
        Result++;
        delete Shader;
    }
    Shader = new NShader("map");
    if (!Shader->Load("shaders/map.vert","shaders/map.frag"))
    {
        Shaders.push_back(Shader);
    } else {
        Result++;
        delete Shader;
    }
    Shader = new NShader("normal");
    if (!Shader->Load("shaders/normal.vert","shaders/normal.frag"))
    {
        Shaders.push_back(Shader);
    } else {
        Result++;
        delete Shader;
    }
    Shader = new NShader("post");
    if (!Shader->Load("shaders/post.vert","shaders/post.frag"))
    {
        Shaders.push_back(Shader);
    } else {
        Result++;
        delete Shader;
    }
    Shader = new NShader("flat_textureless");
    if (!Shader->Load("shaders/flat_textureless.vert","shaders/flat_textureless.frag"))
    {
        Shaders.push_back(Shader);
    } else {
        Result++;
        delete Shader;
    }
    Shader = new NShader("flat_colorless");
    if (!Shader->Load("shaders/flat_colorless.vert","shaders/flat_colorless.frag"))
    {
        Shaders.push_back(Shader);
    } else {
        Result++;
        delete Shader;
    }
    Shader = new NShader("normal_textureless");
    if (!Shader->Load("shaders/normal_textureless.vert","shaders/normal_textureless.frag"))
    {
        Shaders.push_back(Shader);
    } else {
        Result++;
        delete Shader;
    }
    return Result;
}

void NRender::GenerateFramebuffer()
{
    //Create a frame buffer that we can render too, then we can apply post effects to it.
    if (FrameBuffer != 0)
    {
        glDeleteFramebuffers(1,&FrameBuffer);
        glDeleteTextures(1,&FrameBufferTexture);
        glDeleteRenderbuffers(1,&DepthBuffer);
    }
    glGenFramebuffers(1,&FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
    glGenTextures(1,&FrameBufferTexture);

    glBindTexture(GL_TEXTURE_2D,FrameBufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, GetGame()->GetWindowWidth(), GetGame()->GetWindowHeight(), 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,FrameBufferTexture,0);

    glGenRenderbuffers(1,&DepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER,DepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,GetGame()->GetWindowWidth(),GetGame()->GetWindowHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,DepthBuffer);
    CheckFramebuffer();

    glClearColor(1,1,1,1);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    //Now create the vertex buffers to draw the quad to the screen.
    Verts.clear();
    UVs.clear();
    Verts.push_back(glm::vec2(-1,-1));
    UVs.push_back(glm::vec2(0,0));
    Verts.push_back(glm::vec2(1,-1));
    UVs.push_back(glm::vec2(1,0));
    Verts.push_back(glm::vec2(1,1));
    UVs.push_back(glm::vec2(1,1));
    Verts.push_back(glm::vec2(-1,1));
    UVs.push_back(glm::vec2(0,1));
    glBindBuffer(GL_ARRAY_BUFFER,VertexBuffers[0]);
    glBufferData(GL_ARRAY_BUFFER,Verts.size()*sizeof(glm::vec2),&Verts[0],GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,VertexBuffers[1]);
    glBufferData(GL_ARRAY_BUFFER,UVs.size()*sizeof(glm::vec2),&UVs[0],GL_STATIC_DRAW);
}

void NRender::ExtGenerateFramebuffer()
{
    //Create a frame buffer that we can render too, then we can apply post effects to it.
    if (FrameBuffer != 0)
    {
        glDeleteFramebuffersEXT(1,&FrameBuffer);
        glDeleteTextures(1,&FrameBufferTexture);
        //glDeleteRenderbuffers(1,&StencilBuffer);
        glDeleteRenderbuffers(1,&DepthBuffer);
    }
    glGenFramebuffersEXT(1, &FrameBuffer);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FrameBuffer);

    //Color
    glGenTextures(1, &FrameBufferTexture);
    glBindTexture(GL_TEXTURE_2D,FrameBufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GetGame()->GetWindowWidth(), GetGame()->GetWindowHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //According to the tutorial I'm following, frame buffers require bad filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D,FrameBufferTexture,0);

    //Depth
    glGenRenderbuffersEXT(1, &DepthBuffer);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT,DepthBuffer);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,GL_DEPTH_COMPONENT,GetGame()->GetWindowWidth(),GetGame()->GetWindowHeight());
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT,DepthBuffer);
    CheckFramebuffer();

    glClearColor(1,1,1,1);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
    //Now create the vertex buffers to draw the quad to the screen.
    Verts.clear();
    UVs.clear();
    Verts.push_back(glm::vec2(-1,-1));
    UVs.push_back(glm::vec2(0,0));
    Verts.push_back(glm::vec2(1,-1));
    UVs.push_back(glm::vec2(1,0));
    Verts.push_back(glm::vec2(1,1));
    UVs.push_back(glm::vec2(1,1));
    Verts.push_back(glm::vec2(-1,1));
    UVs.push_back(glm::vec2(0,1));
    glBindBuffer(GL_ARRAY_BUFFER,VertexBuffers[0]);
    glBufferData(GL_ARRAY_BUFFER,Verts.size()*sizeof(glm::vec2),&Verts[0],GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,VertexBuffers[1]);
    glBufferData(GL_ARRAY_BUFFER,UVs.size()*sizeof(glm::vec2),&UVs[0],GL_STATIC_DRAW);
}

NRender::NRender()
{
    if (GetGame()->IsServer())
    {
        return;
    }
    GetGame()->GetLog()->Send("RENDER",2,"OpenGL");
    GetGame()->GetLog()->Send("RENDER",2,std::string("\tVendor: ")+(char*)glGetString(GL_VENDOR));
    GetGame()->GetLog()->Send("RENDER",2,std::string("\tVersion: ")+(char*)glGetString(GL_VERSION));
    GetGame()->GetLog()->Send("RENDER",2,std::string("\tRenderer: ")+(char*)glGetString(GL_RENDERER));
    GetGame()->GetLog()->Send("RENDER",2,std::string("\tGLSL Version: ")+(char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
    if (!GLEW_VERSION_2_0)
    {
        GetGame()->GetLog()->Send("RENDER",0,"Sorry! You need OpenGL 2.0 or higher to run this; try updating your graphics drivers!");
        exit(1);
    }
    glGenBuffers(2,VertexBuffers);
    FrameBuffer = 0;
    if (GLEW_VERSION_3_0)
    {
        GenerateFramebuffer();
    } else if (GLEW_EXT_framebuffer_object)
    {
        ExtGenerateFramebuffer();
    } else {
        GetGame()->GetLog()->Send("RENDER",0,"Sorry! You either need OpenGL 3.0 or the GL_EXT_framebuffer_object extension to play this game!");
        exit(1);
    }
    FrameTime = 0;
    LoadShaders();
    PostEffect = GetShader("post");
    if (PostEffect)
    {
        TextureLoc = PostEffect->GetUniformLocation("Texture");
        TimeLoc = PostEffect->GetUniformLocation("Time");
    }
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
    TextureFilter = StringToEnum(DesiredShader);
    if (TextureFilter == 0)
    {
        TextureFilter = GL_LINEAR;
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
    if (GLEW_VERSION_3_0)
    {
        GenerateFramebuffer();
    } else if (GLEW_EXT_framebuffer_object)
    {
        ExtGenerateFramebuffer();
    }
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
    if (!Camera)
    {
        Camera = new NCamera();
    }
    return Camera;
}

void NRender::SetCamera(NCamera* i_Camera)
{
    Camera = i_Camera;
}

void NRender::Draw()
{
    if (GetGame()->IsServer())
    {
        return;
    }
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
    if (GLEW_VERSION_3_0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER,FrameBuffer);
    } else {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,FrameBuffer);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (Camera)
    {
        GetGame()->GetScene()->Draw(Camera);
    } else {
        Camera = new NCamera();
        GetGame()->GetScene()->Draw(Camera);
    }
    if (GLEW_VERSION_3_0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER,0);
    } else {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
    }
    glEnable(GL_TEXTURE_2D);
    glUseProgram(PostEffect->GetID());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,FrameBufferTexture);
    glUniform1i(TextureLoc,0);
    glUniform1f(TimeLoc,(float)CurTime());
    glEnableVertexAttribArray(PostEffect->GetVertexAttribute());
    glBindBuffer(GL_ARRAY_BUFFER,VertexBuffers[0]);
    glVertexAttribPointer(PostEffect->GetVertexAttribute(),2,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(PostEffect->GetUVAttribute());
    glBindBuffer(GL_ARRAY_BUFFER,VertexBuffers[1]);
    glVertexAttribPointer(PostEffect->GetUVAttribute(),2,GL_FLOAT,GL_FALSE,0,NULL);

    glEnable(GL_TEXTURE_2D);
    glDrawArrays(GL_QUADS,0,Verts.size());
    glDisable(GL_TEXTURE_2D);

    glDisableVertexAttribArray(PostEffect->GetVertexAttribute());
    glDisableVertexAttribArray(PostEffect->GetUVAttribute());
    glUseProgram(0);

    glfwSwapBuffers();
    glError();
}

void NRender::glError()
{
    unsigned int ErrorCount = 0;
    GLenum Error = glGetError();
    while (Error != GL_NO_ERROR)
    {
        std::string ErrorString;
        switch (Error)
        {
            case GL_INVALID_OPERATION:              ErrorString = "GL_INVALID_OPERATION"; break;
            case GL_INVALID_ENUM:                   ErrorString = "GL_INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                  ErrorString = "GL_INVALID_VALUE"; break;
            case GL_OUT_OF_MEMORY:                  ErrorString = "GL_OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:  ErrorString = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
            default:                                ErrorString = "UNKOWN_ERROR"; break;
        }
        GetGame()->GetLog()->Send("RENDER",1,std::string("OpenGL reported an error: ") + ErrorString + ".");
        Error = glGetError();
        ErrorCount++;
        if (ErrorCount > 10)
        {
            GetGame()->GetLog()->Send("RENDER",0,"OpenGL is reporting too many errors! Submit a bug report please! We're going to exit because something has to have gone drastically wrong...");
            exit(1);
        }
    }
}

void NRender::AddCachedTexture(GLuint Texture, GLuint ForceFilter)
{
    if (ForceFilter == 0)
    {
        glBindTexture(GL_TEXTURE_2D,Texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFilter);
        CachedTextures.push_back(new NCachedTexture("NULL",Texture));
    } else {
        glBindTexture(GL_TEXTURE_2D,Texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ForceFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ForceFilter);
        CachedTextures.push_back(new NCachedTexture("NULL",Texture,true));
    }
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
        if (CachedTextures[i]->ForceFilter)
        {
            continue;
        }
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
    ForceFilter = false;
    IsGood = false;
    ID = 0;
    Width = 0;
    Height = 0;
    Name = i_Name;
    int Channels;
    NReadFile File = GetGame()->GetFileSystem()->GetReadFile(Name);
    if (!File.Good())
    {
        GetGame()->GetLog()->Send("TEXTURE",1,std::string("Couldn't load ") + Name + " as a texture, it doesn't exist!");
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
    ForceFilter = false;
    IsGood = true;
    Name = i_Name;
    ID = i_ID;
    glBindTexture(GL_TEXTURE_2D,ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGame()->GetRender()->GetTextureFilter());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGame()->GetRender()->GetTextureFilter());
    Width = 0;
    Height = 0;
}

NCachedTexture::NCachedTexture(std::string i_Name, GLuint i_ID, bool i_ForceFilter)
{
    ForceFilter = i_ForceFilter;
    IsGood = true;
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
                GetGame()->GetLog()->Send("TEXTURE",1,std::string("Something requested to use texture ") + Name + ", it is a bad texture!");
                return NULL;
            }
            NTexture* NewTexture = new NTexture(Textures[i]);
            return NewTexture;
        }
    }
    GetGame()->GetLog()->Send("TEXTURE",1,std::string("Something requested to use texture ") + Name + ", it doesn't exist!");
    return NULL;
}

GLuint NRender::StringToEnum(std::string Name)
{
    if (Name == "GL_LINEAR")
    {
        return GL_LINEAR;
    } else if (Name == "GL_NEAREST")
    {
        return GL_NEAREST;
        //Unfortuneately these generate an invalid enum error in opengl, it appears gl_linear uses the mipmaps regardless.
    //} else if (Name == "GL_NEAREST_MIPMAP_NEAREST")
    //{
        //TextureFilter = GL_NEAREST_MIPMAP_NEAREST;
    //} else if (Name == "GL_LINEAR_MIPMAP_NEAREST")
    //{
        //TextureFilter = GL_LINEAR_MIPMAP_NEAREST;
    //} else if (Name == "GL_LINEAR_MIPMAP_LINEAR")
    //{
        //TextureFilter = GL_LINEAR_MIPMAP_LINEAR;
    }
    return 0;
}
void NRender::glPushFramebuffer()
{
    GLint CurrentFrameBuffer;
    if (GLEW_VERSION_3_0)
    {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING,&CurrentFrameBuffer);
    } else {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT,&CurrentFrameBuffer);
    }
    FrameBufferMem.push_back(CurrentFrameBuffer);
}
void NRender::glPopFramebuffer()
{
    if (FrameBufferMem.size()<1)
    {
        return;
    }
    if (GLEW_VERSION_3_0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER,FrameBufferMem.back());
    } else {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,FrameBufferMem.back());
    }
    FrameBufferMem.pop_back();
}

void NRender::CheckFramebuffer()
{
    if (GLEW_VERSION_3_0)
    {
        GLuint Check = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (Check == GL_FRAMEBUFFER_COMPLETE)
        {
            return;
        }
        std::stringstream Message;
        switch (Check)
        {
            case GL_FRAMEBUFFER_UNDEFINED:
            {
                Message << "GL_FRAMEBUFFER_UNDEFINED";
                break;
            }
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            {
                Message << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
                break;
            }
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            {
                Message << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
                break;
            }
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            {
                Message << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
                break;
            }
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            {
                Message << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
                break;
            }
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            {
                Message << "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
                break;
            }
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            {
                Message << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
                break;
            }
            case GL_FRAMEBUFFER_UNSUPPORTED:
            {
                Message << "GL_FRAMEBUFFER_UNSUPPORTED";
                break;
            }
            default:
            {
                Message << "Unknown framebuffer error!";
                break;
            }
        }
        GetGame()->GetLog()->Send("RENDER",0,Message.str());
    } else if (GLEW_EXT_framebuffer_object)
    {
        GLuint Check = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        if (Check == GL_FRAMEBUFFER_COMPLETE_EXT)
        {
            return;
        }
        std::stringstream Message;
        switch (Check)
        {
            /* Apparently doesn't exist as an extension.
               case GL_FRAMEBUFFER_UNDEFINED_EXT:
            {
                Message << "GL_FRAMEBUFFER_UNDEFINED_EXT";
                break;
            }*/
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            {
                Message << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT";
                break;
            }
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            {
                Message << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT";
                break;
            }
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            {
                Message << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT";
                break;
            }
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            {
                Message << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT";
                break;
            }
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT:
            {
                Message << "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT";
                break;
            }
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT:
            {
                Message << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT";
                break;
            }
            case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            {
                Message << "GL_FRAMEBUFFER_UNSUPPORTED_EXT";
                break;
            }
            default:
            {
                Message << "Unknown framebuffer error!";
                break;
            }
        }
        GetGame()->GetLog()->Send("RENDER",0,Message.str());
    }
}
