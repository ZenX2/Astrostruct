#include "NEngine.hpp"

NGame* GetGame()
{
	return Game;
}

double CurTime()
{
	return glfwGetTime();
}

void CleanUp()
{
	delete Game;
}
