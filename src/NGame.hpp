#ifndef NAELSTROF_GAME
#define NAELSTROF_GAME

class NGame
{
public:
	NGame();
	~NGame();
	bool Init(int,int,std::string); //Width Height Title
	NInput* GetInput();
	NRender* GetRender();
	NScene* GetScene();
	bool Running();
	int GetWindowWidth();
	int GetWindowHeight();
	void Close();
	void Poll();
private:
	bool Run;
	NInput* Input;
	NScene* Scene;
	NRender* Render;
	int Width;
	int Height;
	bool Valid;
};

#endif
