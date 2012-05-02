#ifndef NAELSTROF_TEXTURE
#define NAELSTROF_TEXTURE

class NTexture
{
public:
	NTexture(GLuint);
	~NTexture();
	GLuint GetID();
	void SetFilter(GLuint);
private:
	GLuint TextureID;
};

#endif
