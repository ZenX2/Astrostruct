#ifndef NAELSTROF_ENGINE
#define NAELSTROF_ENGINE

#include <GL/glew.h>
#include <GL/glfw.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define Fail false
#define Success true

#include "NInput.hpp"
#include "NNode.hpp"
#include "NScene.hpp"
#include "NCamera.hpp"
#include "NShader.hpp"
#include "NRender.hpp"
#include "NGame.hpp"

static NGame* Game = new NGame();

NGame* GetGame();
void CleanUp();
double CurTime();

#endif
