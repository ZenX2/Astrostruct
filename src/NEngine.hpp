/* Copyright (C) 
* 2012 - Dalton Nell
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
* 
*/

/**
 * @mainpage Welcome to astrostruct!
 * @section intro Introduction
 * @subsection what What is this?
 * astrostruct is an open-source, multi-platform, multi-player, highly-modifiable, space-like, rogue-like game!
 * It's obviously in very early development; right now all I have is an engine code-base and text rendering, but I have high hopes for it.
 * @subsection install How do I get it?
 * Currently the whole thing is stored on github <a href="https://github.com/naelstrof/Astrostruct">here.</a> But you won't find any binaries there; you'll have to compile it yourself by using autotools.
 * @subsection help Can I help with development?
 * Of course! Add me on steam (Naelstrof) or contact me through email (naelstrof@gmail.com) so we can talk.
 */

/**
* @file NEngine.hpp
* @brief Includes necessary headers and wraps together the rest of the engine.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-01
*/

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
#define LUA_COMPAT_MODULE
extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#define Fail false
#define Success true

#include "config.h"
#include "NColor.hpp"
#include "NLua.hpp"
#include "NInput.hpp"
#include "NNode.hpp"
#include "NTexture.hpp"
#include "NShader.hpp"
#include "NText.hpp"
#include "NScene.hpp"
#include "NCamera.hpp"
#include "NRender.hpp"
#include "NGame.hpp"

static NGame* Game = new NGame();

/**
* @brief Gets the global game pointer to access the rest of the engine.
*
* @return A global pointer to the game
*/
NGame* GetGame();
/**
* @brief Gets the current game time.
*
* @return The current time in a double. 
*/
void CleanUp();
/**
* @brief Deletes the global game object and all of it's children.
*/
double CurTime();

#endif
