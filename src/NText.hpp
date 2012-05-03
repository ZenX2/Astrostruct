/**
* @file NText.hpp
* @brief Exposes interfaces to draw text to screen to the rest of the engine.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-01
*/
#ifndef NAELSTROF_TEXT
#define NAELSTROF_TEXT

/** @defgroup TextSystem
 *
 * These classes collaborate together to create a somewhat optimal text rendering system.
 *
 */

/** @addtogroup TextSystem
 * @{*/

/**
* @brief Caches data about a glyph.
*/
class NGlyph
{
public:
	/**
	* @brief Initializes a glyph using a font face, and a position on the texture atlas.
	*
	* @param Face The freetype font face.
	* @param i_X Position on the texture atlas. 
	*/
	NGlyph(FT_Face Face,float i_X);
	~NGlyph();
	/**
	* @brief Stores information about the atlas the contains this glyph.
	*
	* @param Width The width of the atlas. 
	* @param Height The weight of the atlas. 
	*/
	void SetAtlas(float Width,float Height);
	/**
	* @brief Gets the X uv based on the position of the glyph on the atlas, and the width of the atlas.
	*
	* @return The X position of the UV.
	*/
	float GetUV();
	bool Rendered;
	float AdvanceX;
	float AdvanceY;
	float BitmapWidth;
	float BitmapHeight;
	float BitmapLeft;
	float BitmapTop;
	float AtlasWidth;
	float AtlasHeight;
	float X;
};

/**
* @brief Caches freetype glyphs into a massive texture, called a texture atlas.
*/
class NTextureAtlas
{
public:
	/**
	* @brief Creates the texture atlas based on the size of the glyphs in the freetype font face.
	*
	* @param Face The freetype font face that we want to render.
	* @param i_Size The size of the 
	*/
	NTextureAtlas(FT_Face Face, unsigned int i_Size);
	~NTextureAtlas();
	/**
	* @brief Renders a glyph to the texture if it doesn't exist, and returns cached data about the glyph necessary for rendering.
	*
	* @param Face The freetype font face to render from.
	* @param ID The character code to render.
	*
	* @return Cached glyph data containing data necessary to render it.
	*/
	NGlyph* GetGlyph(FT_Face Face, unsigned int ID);
	/**
	* @brief Returns an OpenGL texture ID to the texture atlas.
	*
	* @return An OpenGL texture ID to the texture atlas.
	*/
	GLuint GetTexture();
	/**
	* @brief This regenerates the textures mipmaps, but only if the texture has changed. This is necessary for when textures are rendered crooked or far away.
	*/
	void UpdateMipmaps();
	/**
	* @brief Returns the size of the text rendered to this texture atlas.
	*
	* @return The size of the text rendered to this texture atlas.
	*/
	unsigned int GetSize();
private:
	bool Changed;
	int Size;
	int Width, Height;
	NTexture* Texture;
	std::vector<NGlyph*> Glyphs;
};

/**
* @brief Contains a freetype font and exposes interfaces to render glyphs from the freetype font.
*/
class NFace
{
public:
	/**
	* @brief Creates the object and gives it a name.
	*
	* @param i_Name The name you want the face to have, it can be anything!
	*/
	NFace(std::string i_Name);
	~NFace();
	/**
	* @brief Returns an OpenGL texture that coorisponds to the given size, if it doesn't exist, create it!
	*
	* @param Size The size of the desired text.
	*
	* @return An OpenGL texture ID.
	*/
	GLuint GetTexture(unsigned int Size);
	/**
	* @brief Renders the specified glyph into it's coorisponding texture atlas if it hasn't been rendered already, then returns an object full of cached data of the glyph.
	*
	* @param ID The character code of the desired glyph.
	* @param Size The desired size of the rendered glyph.
	*
	* @return 
	*/
	NGlyph* GetGlyph(unsigned int ID, unsigned int Size);
	/**
	* @brief Loads a freetype font face into memory.
	*
	* @param FTLib The freetype library.
	* @param File The filepath to the freetype-readable font.
	*
	* @return False on failure, true on success.
	*/
	bool Load(FT_Library FTLib, std::string File);
	/**
	* @brief Returns the name of the freetype font face given at the constructor.
	*
	* @return The name given in the constructor.
	*/
	std::string GetName();
	/**
	* @brief Updates the mipmaps of all textures that are rendering from this face.
	*/
	void UpdateMipmaps();
private:
	std::string Name;
	FT_Face Face;
	std::vector<NTextureAtlas*> Textures;
};

/**
* @brief The actual text object used to render strings of text.
*/
class NText : public NNode
{
public:
	/**
	* @brief Grabs the specified font in preparation to render the text given.
	*
	* @param Font The name of the font that we want to render.
	* @param i_Data The string of text we want to render.
	*/
	NText(std::string Font,std::string i_Data);
	/**
	* @brief Generates a vertex and UV array if the string it needs to render has changed or doesn't exist, then draws it to screen.
	*
	* @param View Camera view matrix
	*/
	void Draw(glm::mat4 View);
	/**
	* @brief Grabs the width of the rendered text, returns 0 if it hasn't been rendered yet!
	*
	* @return The width of the rendered text.
	*/
	float GetWidth();
	/**
	* @brief Updates the text node to do stuff, currently sets it to the position of the mouse and spins it.
	*
	* @param DT The delta time, aka the amount of time passed since last execution.
	*/
	void Tick(double DT);
	/**
	* @brief Sets the render mode of the text.
	*
	* @param i_Mode The render mode, currently accepts (0)Left, (1)Centered, and (2)Right.
	*/
	void SetMode(int i_Mode);
	/**
	* @brief Sets the desired text to be rendered at the next Draw call.
	*
	* @param i_Data The desired string of text to be rendered.
	*/
	void SetText(std::string i_Data);
	~NText();
private:
	int Mode;
	float Width;
	float Size;
	/**
	* @brief Generates the vertex and UV buffer to be rendered, but only generates them if needed.
	*/
	void GenerateBuffers();
	NShader* Shader;
	NFace* Face;
	std::string Data;
	std::vector<glm::vec2> Verts;
	std::vector<glm::vec2> UVs;
	bool Changed;
	GLuint* Buffers;
};

/**
* @brief Object that exposes ability to render text to other parts of the engine.
*/
class NTextSystem
{
public:
	NTextSystem();
	~NTextSystem();
	/**
	* @brief Statically loads font faces into memory.
	*/
	void LoadFaces();
	/**
	* @brief Grabs a font face from memory.
	*
	* @param Name The name of the font face.
	*
	* @return The font face.
	*/
	NFace* GetFace(std::string Name);
	/**
	* @brief Allocates room for a text object and inserts it into the scene.
	*
	* @param Font The desired font of the text object.
	* @param Data The desired string to be rendered from the text object.
	*
	* @return A pointer to the new text object.
	*/
	NText* AddText(std::string Font,std::string Data);
	/**
	* @brief Updates the mipmaps of all children, but only if we need to! (You can call it as much as you like.)
	*/
	void UpdateMipmaps();
private:
	FT_Library FTLib;
	std::vector<NFace*> Faces;
	std::vector<NText*> Texts;
};
/*@}*/

#endif
