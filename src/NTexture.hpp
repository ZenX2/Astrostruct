/**
* @file NTexture.hpp
* @brief Abstracts OpenGL texture objects so we can apply texture filters to them all easily, and perhaps animations in the future.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-01
*/
#ifndef NAELSTROF_TEXTURE
#define NAELSTROF_TEXTURE

int CreateAnimation(lua_State* L);
int Animation__index(lua_State* L);
int Animation__newindex(lua_State* L);
int LoadTexture(lua_State* L);
void LoadTextures();
class NAnimation
{
public:
	NAnimation();
	~NAnimation();
	void AddFrame(std::string FileName);
	float FPS;
	int Reference;
	GLuint GetID(double Time);
	void SetName(std::string i_Name);
	std::string GetName();
private:
	std::string Name;
	std::vector<GLuint> Frames;
};
NAnimation* lua_checkAnimation(lua_State* L, int narg);
/**
* @brief Abstracts OpenGL texture ID's.
*/
class NTexture
{
public:
	NTexture(NTexture*);
	NTexture(std::string i_Name);
	/**
	* @brief Deletes the opengl texture object and itself. (So don't go trying to delete the opengl texture you created!)
	*/
	~NTexture();
	/**
	* @brief Returns an ID to the texture object.
	*
	* @return An opengl texture ID.
	*/
	GLuint GetID();
	void AddAnimation(NAnimation* Animation);
	std::string Name;
	void Play(std::string i_Name);
private:
	unsigned int PlayingAnimation;
	double TimeOffset;
	std::vector<NAnimation*> Animations;
};

#endif
