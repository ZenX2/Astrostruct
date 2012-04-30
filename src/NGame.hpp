#ifndef NAELSTROF_GAME
#define NAELSTROF_GAME

void ResizeWindow(int, int);

class NGame
{
public:
	NGame();
	~NGame();
	bool Init(int,int,std::string); //Width Height Title
	NInput* GetInput();
	NRender* GetRender();
	NScene* GetScene();
	NTextSystem* GetTextSystem();
	bool Running();
	int GetWindowWidth();
	int GetWindowHeight();
	glm::vec2 GetWindowSize();
	void Close();
	void Poll();
	int Width;
	int Height;
	int NewWidth;
	int NewHeight;
	void SetWindowChanged(bool);
	bool GetWindowChanged();
private:
	bool WindowChanged;
	bool Run;
	NInput* Input;
	NScene* Scene;
	NRender* Render;
	NTextSystem* TextSystem;
	bool Valid;
};

#endif
