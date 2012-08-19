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
* GNU General Public License for more details.  * 
* You should have received a copy of the GNU General Public License * along with this program; if not, write to the Free Software
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
 * @subsection howdoi I'm looking at your code and I don't even...
 * This is a rough drawing of how the engine works.
 * <img src="http://farmpolice.com/content/images/0Vi0rsUkohfWQZL3.png" alt="Some oogly boogly chart" />
 * Game is a global that any part of the engine can access at any time, this means if Lua is a child of Game: you'll have access to lua in every part of the engine through a simple call of GetGame()->GetLua()!<br>
 * I think it's a really elegant design for a game engine, but I have yet for someone more experienced to prove me wrong.
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
//windows.h required for some console things.
#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
#endif
#include "config.h"
#include <sys/time.h>
#include <cstdlib>
#include <string>
#include <utf8.h>
#include "NUnicode.hpp"
#include <GL/glew.h>
#include <GL/glfw.h>
#ifdef _WIN32
#include <SOIL.h>
#else
#include <SOIL/SOIL.h>
#endif
#include <iostream>
#include <fstream>
#include <cctype>
#include <sstream>
#include <vector>
#include <signal.h>
#include <enet/enet.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/projection.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>
#include <AL/alure.h>
#include <physfs.h>
#define LUA_COMPAT_MODULE
#include <lua.hpp>
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/btBulletCollisionCommon.h>

#define PI 3.14159265

#include "NSignal.hpp"
#include "NFileSystem.hpp"
#include "NLog.hpp"
#include "NPhysics.hpp"
/** @defgroup TerminalColor TerminalColor
 *
 * Colors incoming terminal text into several predefined colors. Currently only supports linux (and maybe mac).
 *
 */
#include "NColor.hpp"
#include "NConfig.hpp"
#include "NInput.hpp"
#include "NNode.hpp"
#include "NShader.hpp"
/** @defgroup TextSystem TextSystem
 *
 * These classes collaborate together to create a somewhat optimal text rendering system.
 *
 */
#include "NText.hpp"
/** @defgroup SoundSystem SoundSystem
 *
 * These classes collaborate together to create sounds.
 *
 */
#include "NSound.hpp"
#include "NTexture.hpp"
/** @defgroup GuiSystem GuiSystem
 *
 * These classes collaborate together to create a skinnable gui system.
 *
 */
#include "NSpace.hpp"
#include "NFramebuffer.hpp"
#include "NLight.hpp"
#include "NMap.hpp"
#include "NEntity.hpp"
#include "NWindow.hpp"
#include "NCheckbox.hpp"
#include "NTextInput.hpp"
#include "NButton.hpp"
#include "NPlayer.hpp"
#include "NCamera.hpp"
#include "NScene.hpp"
#include "NRender.hpp"
#include "NConsole.hpp"
#include "NLua.hpp"
#include "NPacket.hpp"
#include "NStateMachine.hpp"
#include "NNetwork.hpp"
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
double CurTime();
/**
* @brief Returns a random float between the two limits.
*
* @param Min The minimum desired float.
* @param Max The maximum desired float.
*
* @return A random float between the two limits.
*/
float Rand(float Min, float Max);

#endif
