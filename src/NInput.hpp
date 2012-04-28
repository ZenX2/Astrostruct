#ifndef NAELSTROF_INPUT
#define NAELSTROF_INPUT

class NInput
{
public:
	void Poll();
	int GetKey(int); //Key
	int GetMouseX();
	int GetMouseY();
private:
	int MouseX;
	int MouseY;
};

#endif
