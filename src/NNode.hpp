#ifndef NAELSTROF_NODE
#define NAELSTROF_NODE

class NNode
{
public:
	NNode();
	~NNode();
	virtual void Tick();
	virtual void Draw(glm::mat4);
	virtual glm::vec2 GetPos();
	virtual void SetPos(float,float); //X, Y
	virtual void SetPos(glm::vec2); //Pos
	virtual glm::mat4 GetModelMatrix();
private:
	glm::vec2 Position;
};

#endif
