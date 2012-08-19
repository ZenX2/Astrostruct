#include "NEngine.hpp"

NFramebuffer::NFramebuffer(unsigned char i_Flags, unsigned int i_Width, unsigned int i_Height)
{
    Width = i_Width;
    Height = i_Height;
    Flags = i_Flags;
    if (i_Flags & NStencilBuffer && i_Flags & NDepthBuffer)
    {
        GetGame()->GetLog()->Send("FRMBFFR",1,"Most hardware doesn't support using a FBO with a depth and stencil attached! Condensing them to one render buffer...");
    }
    if (GLEW_VERSION_3_0)
    {
        glGenFramebuffers(1,&Framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
        if (i_Flags & NColorBuffer)
        {
            glGenTextures(1,&Texture);
            glBindTexture(GL_TEXTURE_2D,Texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, i_Width, i_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,Texture,0);
        }

        if (i_Flags & NDepthBuffer && !(i_Flags & NStencilBuffer))
        {
            glGenRenderbuffers(1,&Depth);
            glBindRenderbuffer(GL_RENDERBUFFER,Depth);
            glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,i_Width,i_Height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,Depth);
        }

        //This is the only way I could get a proper stencil buffer, by making a GL_DEPTH_STENCIL attachment to both points.
        if (i_Flags & NStencilBuffer)
        {
            glGenRenderbuffers(1, &Stencil);
            glBindRenderbuffer(GL_RENDERBUFFER,Stencil);
            glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_STENCIL,i_Width,i_Height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,Stencil);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_STENCIL_ATTACHMENT,GL_RENDERBUFFER,Stencil);
        }
        Check();
        //Make sure we unbind when we're done.
        glBindFramebuffer(GL_FRAMEBUFFER,0);
    } else if (GLEW_EXT_framebuffer_object)
    {
        glGenFramebuffersEXT(1,&Framebuffer);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, Framebuffer);

        if (i_Flags & NColorBuffer)
        {
            glGenTextures(1,&Texture);
            glBindTexture(GL_TEXTURE_2D,Texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, i_Width, i_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D,Texture,0);
        }

        if (i_Flags & NDepthBuffer && !(i_Flags & NStencilBuffer))
        {
            glGenRenderbuffersEXT(1,&Depth);
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT,Depth);
            glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,GL_DEPTH_COMPONENT,i_Width,i_Height);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT,Depth);
        }

        //This is the only way I could get a proper stencil buffer, by making a GL_DEPTH_STENCIL attachment to both points.
        if (i_Flags & NStencilBuffer)
        {
            glGenRenderbuffersEXT(1, &Stencil);
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT,Stencil);
            glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,GL_DEPTH_STENCIL_EXT,i_Width,i_Height);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT,Stencil);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_STENCIL_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT,Stencil);
        }
        Check();
        //Make sure we unbind when we're done.
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
    } else {
        GetGame()->GetLog()->Send("FRMBFFR",0,"Failed to create framebuffer! You need OpenGL 3.0 or the GL_EXT_framebuffer_object extension!");
        exit(1);
    }
}

NFramebuffer::~NFramebuffer()
{
    if (GLEW_VERSION_3_0)
    {
        glDeleteFramebuffers(1,&Framebuffer);
        if (Flags & NColorBuffer)
        {
            glDeleteTextures(1,&Texture);
        }
        if (Flags & NDepthBuffer && !(Flags & NStencilBuffer))
        {
            glDeleteRenderbuffers(1,&Depth);
        }
        if (Flags & NStencilBuffer)
        {
            glDeleteRenderbuffers(1,&Stencil);
        }
    } else if (GLEW_EXT_framebuffer_object)
    {
        glDeleteFramebuffersEXT(1,&Framebuffer);
        if (Flags & NColorBuffer)
        {
            glDeleteTextures(1,&Texture);
        }
        if (Flags & NDepthBuffer && !(Flags & NStencilBuffer))
        {
            glDeleteRenderbuffersEXT(1,&Depth);
        }
        if (Flags & NStencilBuffer)
        {
            glDeleteRenderbuffersEXT(1,&Stencil);
        }
    }
}

int NFramebuffer::GetTexture()
{
    return Texture;
}

void NFramebuffer::Bind()
{
    if (GLEW_VERSION_3_0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER,Framebuffer);
    } else if (GLEW_EXT_framebuffer_object)
    {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,Framebuffer);
    }
}

void NFramebuffer::UnBind()
{
    if (GLEW_VERSION_3_0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER,0);
    } else if (GLEW_EXT_framebuffer_object)
    {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
    }
}

void NFramebuffer::Resize(unsigned int i_Width, unsigned int i_Height)
{
    if (Width == i_Width && Height == i_Height)
    {
        return;
    }
    Width = i_Width;
    Height = i_Height;
    if (GLEW_VERSION_3_0)
    {
        glGenFramebuffers(1,&Framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);

        if (Flags & NColorBuffer)
        {
            glBindTexture(GL_TEXTURE_2D,Texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, i_Width, i_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,Texture,0);
        }

        if (Flags & NDepthBuffer && !(Flags & NStencilBuffer))
        {
            glBindRenderbuffer(GL_RENDERBUFFER,Depth);
            glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,i_Width,i_Height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,Depth);
        }

        //This is the only way I could get a proper stencil buffer, by making a GL_DEPTH_STENCIL attachment to both points.
        if (Flags & NStencilBuffer)
        {
            glBindRenderbuffer(GL_RENDERBUFFER,Stencil);
            glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_STENCIL,i_Width,i_Height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,Stencil);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_STENCIL_ATTACHMENT,GL_RENDERBUFFER,Stencil);
        }
        //Make sure we unbind when we're done.
        glBindFramebuffer(GL_FRAMEBUFFER,0);
    } else if (GLEW_EXT_framebuffer_object)
    {
        glGenFramebuffersEXT(1,&Framebuffer);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, Framebuffer);

        if (Flags & NColorBuffer)
        {
            glBindTexture(GL_TEXTURE_2D,Texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, i_Width, i_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D,Texture,0);
        }

        if (Flags & NDepthBuffer && !(Flags & NStencilBuffer))
        {
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT,Depth);
            glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,GL_DEPTH_COMPONENT,i_Width,i_Height);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT,Depth);
        }

        //This is the only way I could get a proper stencil buffer, by making a GL_DEPTH_STENCIL attachment to both points.
        if (Flags & NStencilBuffer)
        {
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT,Stencil);
            glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,GL_DEPTH_STENCIL_EXT,i_Width,i_Height);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT,Stencil);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_STENCIL_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT,Stencil);
        }
        //Make sure we unbind when we're done.
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
    }
}

void NFramebuffer::Check()
{
    if (GLEW_VERSION_3_0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
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
        glBindFramebuffer(GL_FRAMEBUFFER,0);
    } else if (GLEW_EXT_framebuffer_object)
    {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, Framebuffer);
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
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
        GetGame()->GetLog()->Send("RENDER",0,Message.str());
    }
}
