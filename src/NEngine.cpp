#include "NEngine.hpp"

NGame* GetGame()
{
	return Game;
}

double CurTime()
{
	return glfwGetTime();
}

float Rand(float Min, float Max)
{
	float Difference = Max-Min;
	float X = (float(rand())/float(RAND_MAX))*Difference;
	return Min+X;
}
