#ifndef NAELSTROF_PLAYER
#define NAELSTROF_PLAYER

class NPlayer : public NNode
{
public:
	NPlayer();
	~NPlayer();
	void Move(float Direction);
	void StopMove();
	void Tick(double DT);
	void SetVel(glm::vec3 i_Velocity);
	glm::vec3 GetVel();
	void Draw(NCamera* View);
	void SetControl();
private:
	float Gravity;
	bool OnGround;
	float Friction;
	bool Controlled;
	NShader* Shader;
	GLuint TextureLoc, MatrixLoc, ColorLoc;
	bool Changed;
	GLuint Buffers[2];
	void GenerateBuffers();
	float Speed;
	std::vector<glm::vec2> Verts;
	std::vector<glm::vec2> UVs;
	glm::vec3 Velocity;
	float CurrentDirection;
	bool Moving;
	NTexture* Texture;
};

#endif
