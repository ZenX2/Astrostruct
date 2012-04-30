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
#include <ft2build.h>
#include FT_FREETYPE_H

#define Fail false
#define Success true

#include "config.h"
#include "NColor.hpp"
#include "NInput.hpp"
#include "NNode.hpp"
#include "NShader.hpp"
#include "NText.hpp"
#include "NScene.hpp"
#include "NCamera.hpp"
#include "NRender.hpp"
#include "NGame.hpp"

static NGame* Game = new NGame();

NGame* GetGame();
void CleanUp();
double CurTime();

#endif
