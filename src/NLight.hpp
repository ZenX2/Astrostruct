/**
 * @file NLight.hpp
 * @brief Handles drawing lights as well as shadows. Uses an FBO with stenciling for proper blending.
 * @author Dalton Nell
 * @version 1.0
 * @date 2012-06-14
 */
#ifndef NAELSTROF_LIGHT
#define NAELSTROF_LIGHT

/**
 * @brief Light object that only draws specifically to NLightSystem's FBO, although that's entirely controlled by NScene to keep from doing useless framebuffer pushes and pops.
 */
class NLight : public NNode
{
public:
	/**
	 * @brief Creates a light based off the given texture, you can create cone lights as well if you use a cone texture!
	 *
	 * @param i_Texture The desired light texture.
	 */
	NLight(std::string i_Texture);
	~NLight();
	/**
	 * @brief Draws shadow to the stencil to create a mask, then splashes down the textured quad for a light.
	 *
	 * @param View The camera view.
	 */
	void Draw(NCamera* View);
	/**
	 * @brief Doesn't do anything at the moment, probably will create flickering and other things using this.
	 *
	 * @param DT The delta time.
	 */
	void Tick(double DT);
	/**
	 * @brief Deletes the object.
	 */
	void Remove();
	/**
	 * @brief Plugs this into NNode to ensure we regenerate our shadows when we change scale.
	 *
	 * @param i_Scale Desired scale of the object.
	 */
	void SetScale(glm::vec3 i_Scale);
	/**
	 * @brief Plugs this into NNode to ensure we regenerate our shadows when we change position.
	 *
	 * @param i_Position Desired position of object.
	 */
	void SetPos(glm::vec3 i_Position);
	/**
	 * @brief Returns the type of the object.
	 *
	 * @return "Light".
	 */
	std::string GetType();
private:
	void DrawLight(NCamera* View);
	void DrawShadow(NCamera* View);
	NTexture* Texture;
	NShader* Shader;
	NShader* ShadowShader;
	bool Changed;
	bool SChanged;
	void GenerateLightBuffers();
	void GenerateShadowBuffers();
	std::vector<glm::vec2> Verts;
	std::vector<glm::vec2> UVs;
	std::vector<glm::vec3> Shadows;
	GLuint Buffers[3];
	GLuint TextureLoc;
	GLuint STextureLoc;
	GLuint MatrixLoc;
	GLuint SMatrixLoc;
	GLuint ColorLoc;
	GLuint SColorLoc;
};

/**
 * @brief Handles creating and drawing our FBO to screen for our lights.
 */
class NLightSystem
{
public:
	/**
	 * @brief Generates an FBO fit for stenciling.
	 */
	NLightSystem();
	~NLightSystem();
	/**
	 * @brief Grabs our generated FBO you can do stuff with it.
	 *
	 * @return An OpenGL handle to the FBO.
	 */
	GLuint GetFramebuffer();
	/**
	 * @brief Draws a textured quad to screen with the FBO.
	 */
	void Draw();
	/**
	 * @brief Regenerates our FBO when the screen changes size for us.
	 */
	void CheckFBO();
private:
	NShader* Shader;
	GLuint TextureLoc;
	GLuint VertexBuffers[2];
	std::vector<glm::vec2> Verts;
	std::vector<glm::vec2> UVs;
	GLuint FrameBuffer;
	GLuint FrameBufferTexture;
	GLuint StencilBuffer;
	std::vector<NLight*> Lights;
};

#endif
