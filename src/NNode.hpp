#ifndef NAELSTROF_NODE
#define NAELSTROF_NODE

class NNode
{
public:
	NNode();
	~NNode();
	virtual void Tick(double);//DT
	virtual void Draw(glm::mat4);//ModelView
	virtual glm::vec2 GetPos();
	virtual void SetPos(float,float); //X, Y
	virtual void SetPos(glm::vec2); //Pos
	virtual void SetAng(float);
	virtual float GetAng();
	virtual glm::mat4 GetModelMatrix();
	virtual void UpdateMatrix();
	virtual void SetParent(NNode*);
	virtual void AddChild(NNode*);
	virtual void RemoveChild(NNode*);
	virtual NNode* GetParent();
private:
	float Angle;
	glm::vec2 Position;
	glm::mat4 Matrix;
	NNode* Parent;
	std::vector<NNode*> Children;
};

#endif
