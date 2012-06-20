/**
* @file NText.hpp
* @brief Exposes interfaces to draw text to screen to the rest of the engine.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-01
*/
#ifndef NAELSTROF_TEXT
#define NAELSTROF_TEXT

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
	*/
	NGlyph(FT_Face Face);
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
	glm::vec4 TextureRect;
};

/**
 * @brief Used to pack boxes into the texture atlas
 */
class NTextureNode
{
public:
	/**
	 * @brief Recursively looks for a place to store the specified box onto the texture.
	 *
	 * @param i_Rect The width and height of the box we want to store on it.
	 *
	 * @return The node that the rectangle has been placed into.
	 */
	NTextureNode* Insert(glm::vec2 i_Rect);
	/**
	 * @brief Creates a node with the specified rectangle.
	 *
	 * @param i_Rect The width, height, x, and y of the rectangle.
	 */
	NTextureNode(glm::vec4 i_Rect);
	~NTextureNode();
	glm::vec4 Rect;
private:
	bool HasImage;
	bool HasChildren;
	NTextureNode* Children[2];
	NTextureNode* Parent;
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
	NGlyph* GetGlyph(FT_Face Face, wchar_t ID);
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
	NTextureNode* Node;
	bool Changed;
	int Size;
	int Width, Height;
	GLuint Texture;
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
	NGlyph* GetGlyph(wchar_t ID, unsigned int Size);
	/**
	* @brief Loads a freetype font face into memory.
	*
	* @param File The filepath to the freetype-readable font.
	*
	* @return False on failure, true on success.
	*/
	bool Load(std::string File);
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
	char* FileData;
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
	* @param i_Face The font that we want to render.
	* @param i_Data The string of text we want to render.
	*/
	NText(std::string i_Face, std::wstring i_Data);
	/**
	* @brief Generates a vertex and UV array if the string it needs to render has changed or doesn't exist, then draws it to screen.
	*
	* @param View Camera view matrix
	*/
	void Draw(NCamera* View);
	/**
	* @brief Grabs the width of the text.
	*
	* @return The width of the text.
	*/
	float GetWidth();
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
	void SetText(std::wstring i_Data);
	/**
	* @brief Sets the desired text to be rendered at the next Draw call.
	*
	* @param i_Data The desired string of text to be rendered.
	*/
	void SetText(std::string i_Data);
	/**
	* @brief Returns the size of the rendered glyphs, the actual size depends on the font!
	*
	* @return The approximate size of the rendered glyphs.
	*/
	float GetSize();
	/**
	* @brief Sets the size to the closest approximate rendersize available from the float.
	*
	* @param i_Size The desired text render size.
	*/
	void SetSize(float i_Size);
	/**
	 * @brief Unallocates the object.
	 */
	void Remove();
	/**
	 * @brief Creates a box where the text will wrap within. (It's not very good at the moment and still need vertical text support)
	 *
	 * @param i_W The desired width of the box.
	 * @param i_H The desired height of the box.
	 */
	void SetBorder(float i_W, float i_H);
	/**
	 * @brief Returns the type of the object.
	 *
	 * @return "Text".
	 */
	std::string GetType();
	~NText();
	/**
	 * @brief Toggles the view from orthographic to perspective, useful for rendering text in world space.
	 */
	void SwapView();
private:
	glm::vec2 Velocity;
	float W,H;
	int Mode;
	float Width;
	float Size;
	/**
	* @brief Generates the vertex and UV buffer to be rendered, but only generates them if needed.
	*/
	void GenerateBuffers();
	NShader* Shader;
	NFace* Face;
	std::wstring Data;
	std::vector<glm::vec2> Verts;
	std::vector<glm::vec2> UVs;
	bool Changed;
	GLuint* Buffers;
	GLuint ColorLoc,TextureLoc,MatrixLoc,ScreenLoc;
	bool Persp;
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
	* @brief Returns the freetype library for lua.
	*
	* @return The freetype library.
	*/
	FT_Library GetFreeTypeLib();
	/**
	* @brief Adds a face to cache.
	*
	* @param Face The face we want to cache.
	*/
	void AddFace(NFace* Face);
private:
	FT_Library FTLib;
	std::vector<NFace*> Faces;
};

/*@}*/

#endif
