#ifndef NAELSTROF_FRAMEBUFFER
#define NAELSTROF_FRAMEBUFFER

enum NFramebufferFlags
{
    NColorBuffer = 0x01,
    NDepthBuffer = 0x02,
    NStencilBuffer = 0x04
};

class NFramebuffer
{
public:
    NFramebuffer(unsigned char i_Flags, unsigned int Width, unsigned int Height);
    ~NFramebuffer();
    void Resize(unsigned int Width, unsigned int Height);
    void Bind();
    void UnBind();
    int GetTexture();
private:
    unsigned int Width,Height;
    void Check();
    unsigned char Flags;
    GLuint Framebuffer;
    GLuint Texture;
    GLuint Depth;
    GLuint Stencil;
};

#endif
