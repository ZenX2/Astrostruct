#include "NEngine.hpp"

int main(int argc, char** argv)
{
	SetColor(Blue);
	std::cout << "ENGINE INFO: ";
	ClearColor();
	std::cout << "Weclome to " << PACKAGE_STRING << "! Report bugs to " << PACKAGE_BUGREPORT << "!\n";
	if (!GetGame()->Init(512,512,"Astrostruct"))
	{
		SetColor(Red);
		std::cout << "ENGINE ERROR: ";
		ClearColor();
		std::cout << "Failed to initialize the game!\n";
		CleanUp();
		return 1;
	}
	//Initialize game stuff here
	GetGame()->GetRender()->SetVSync(true);
	NText* InfoText = GetGame()->GetTextSystem()->AddText("opensans", "(Press T) Texture filter: GL_LINEAR");
	InfoText->SetMode(1);
	//
	while(GetGame()->Running())
	{
		//Update game stuff here
		if (GetGame()->GetInput()->KeyChanged('T') && GetGame()->GetInput()->GetKey('T'))
		{
			if (GetGame()->GetRender()->GetTextureFilter() == GL_NEAREST)
			{
				GetGame()->GetRender()->SetTextureFilter(GL_LINEAR);
				InfoText->SetText("Texture filter: GL_LINEAR");
			} else {
				GetGame()->GetRender()->SetTextureFilter(GL_NEAREST);
				InfoText->SetText("Texture filter: GL_NEAREST");
			}
		}
		//
		InfoText->SetScale(glm::vec2(1.f)+GetGame()->GetInput()->GetMouse()/GetGame()->GetWindowSize());
		InfoText->SetColor(glm::vec4(0,GetGame()->GetInput()->GetMouse()/GetGame()->GetWindowSize(),fmod(CurTime()/2.f,1)));
		GetGame()->GetInput()->Poll();
		GetGame()->GetScene()->Tick();
		GetGame()->GetRender()->Draw();
		GetGame()->Poll();
	}
	CleanUp();
	return 0;
}
