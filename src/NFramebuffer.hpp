/**
 * @file NFramebuffer.hpp
 * @brief Handles creation and binding of framebuffers.
 * @author Dalton Nell
 * @version 1.0
 * @date 2012-08-20
 */
#ifndef NAELSTROF_FRAMEBUFFER
#define NAELSTROF_FRAMEBUFFER

enum NFramebufferFlags
{
    NColorBuffer = 0x01,
    NDepthBuffer = 0x02,
    NStencilBuffer = 0x04
};

/**
 * @brief Creates a frame buffer based on the OGL version and available extensions.
 */
class NFramebuffer
{
public:
    /**
     * @brief Creates a frame buffer based on the flags and size.
     *
     * @param i_Flags The desired features you want in the fbo, NColorBuffer for color, NDepthBuffer for depth, and NStencilBuffer for stencils. eg NColorBuffer | NStencilBuffer
     * @param Width The desired width of the buffer.
     * @param Height The desired height of the buffer.
     */
    NFramebuffer(unsigned char i_Flags, unsigned int Width, unsigned int Height);
    ~NFramebuffer();
    /**
     * @brief Destroys and recreates the fbo with the desired width and height. Must destroy because otherwise the memory leaks.
     *
     * @param Width The desired width.
     * @param Height The desired height.
     */
    void Resize(unsigned int Width, unsigned int Height);
    /**
     * @brief Binds the buffer with the available binding utilties (glBindFramebuffer or glBindFramebufferEXT).
     */
    void Bind();
    /**
     * @brief Binds the current OGL fbo to 0 using available binding utilities (glBindFramebuffer or glBindFramebufferEXT).
     */
    void UnBind();
    /**
     * @brief Returns a texture id that is bound to the FBO.
     *
     * @return The texture id bound to the fbo.
     */
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
