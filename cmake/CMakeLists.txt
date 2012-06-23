cmake_minimum_required(VERSION 2.8)
project(astrostruct)

if (NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE Debug CACHE STRING
		"The type of build. Options: None Debug Release RelWithDebInfo MinSizeRel."
		FORCE)
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CSS_FLAGS
		"${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic-errors -std=c++0x")
		# -Wall:   Enable all warnings.
		# -Wextra: Enable some more warnings.
		# -Werror: Have errors on warnings.
		# -pedantic-errors: Error on non-standard ISO C++ code.
		# -std=c++0x: Compile C++0x code.

	add_definitions(-D_CMAKE_LINUX_)
elseif(MSVC)
	set(CMAKE_CXX_FLAGS
		"${CMAKE_CXX_FLAGS} /W4 /WX /wd4355 /wd4512")
		# /W4: Maximum warnings.
	    # /WX: Have errors on warnings.
	    # /wd4355: Disable warning 4355:
	    #          Using the 'this' pointer in initializer lists.
	    # /wd4512: Disable warning 4512:
	    #          Assignment operators NOT being able to be generated.

	add_definitions(-D_CMAKE_WINDOWS_)
endif()

# CPPCHECK

find_program(CPPCHECK_EXECUTABLE cppcheck
			 DOC "A tool for static C/C++ code analysis.")
if (CPPCHECK_EXECUTABLE)
	add_custom_target("check"
					  COMMAND "${CPPCHECK_EXECUTABLE}" "--enable=all" "*"
					  WORKING_DIRECTORY ../src VERBATIM)
endif()

# DOXYGEN

find_package(Doxygen)
if (DOXYGEN_FOUND)
	add_custom_target("docs"
					  COMMAND "${DOXYGEN_EXECUTABLE}" "config.doxygen"
					  WORKING_DIRECTORY ../bin/doc VERBATIM)
endif()

# SOURCE FILES

set(source
	../src/main.cpp
	../src/NButton.cpp
	../src/NCamera.cpp
	../src/NCheckbox.cpp
	../src/NColor.cpp
	../src/NConfig.cpp
	../src/NConsole.cpp
	../src/NEngine.cpp
	../src/NFileSystem.cpp
	../src/NGame.cpp
	../src/NInput.cpp
	../src/NLight.cpp
	../src/NLua.cpp
	../src/NMap.cpp
	../src/NNetwork.cpp
	../src/NNode.cpp
	../src/NPacket.cpp
	../src/NPhysics.cpp
	../src/NPlayer.cpp
	../src/NRender.cpp
	../src/NScene.cpp
	../src/NShader.cpp
	../src/NSignal.cpp
	../src/NSound.cpp
	../src/NStateMachine.cpp
	../src/NText.cpp
	../src/NTextInput.cpp
	../src/NTexture.cpp
	../src/NUnicode.cpp
	../src/NWindow.cpp)

# GAME
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules")

# OpenGL
find_package(OpenGL)
if(NOT OPENGL_FOUND)
	message(FATAL_ERROR "Couldn't find OpenGL!")
endif()

# OpenAL
find_package(OpenAL)
if(NOT OPENAL_FOUND)
	message(FATAL_ERROR "Couldn't find OpenAL!")
endif()

# Lua51
find_package(Lua51)
if(NOT LUA51_FOUND)
	message(FATAL_ERROR "Couldn't find Lua51!")
endif()

# ENet
find_package(ENet)
if (NOT ENET_FOUND)
	message(FATAL_ERROR "Couldn't find ENet!")
endif()

# GLEW
find_package(GLEW)
if (NOT GLEW_FOUND)
	message(FATAL_ERROR "Couldn't find GLEW!")
endif()

# GLFW
find_package(GLFW)
if (NOT GLFW_FOUND)
	message(FATAL_ERROR "Couldn't find GLFW!")
endif()

# Freetype
find_package(Freetype)
if (NOT FREETYPE_FOUND)
	message(FATAL_ERROR "Couldn't find Freetype!")
endif()

# SOIL
find_package(SOIL)
if (NOT SOIL_FOUND)
	message(FATAL_ERROR "Couldn't find SOIL!")
endif()

# GLM
find_package(GLM)
if (NOT GLM_FOUND)
	message(FATAL_ERROR "Couldn't find GLM!")
endif()

# UTF8
find_package(UTF8)
if (NOT UTF8_FOUND)
	message(FATAL_ERROR "Couldn't find UTF8!")
endif()

# Alure
find_package(Alure)
if (NOT ALURE_FOUND)
	message(FATAL_ERROR "Couldn't find Alure!")
endif()

# PhysFS
find_package(Physfs)
if (NOT PHYSFS_FOUND)
	message(FATAL_ERROR "Couldn't find PHYSFS!")
endif()

include_directories(${OPENGL_INCLUDE_DIR}
					${LUA_INCLUDE_DIR}
					${ENET_INCLUDE_DIR}
					${GLEW_INCLUDE_PATH}
					${GLFW_INCLUDE_DIRS}
					${FREETYPE_INCLUDE_DIRS}
					${SOIL_INCLUDE_DIR}
					${GLM_INCLUDE_DIR}
					${UTF8_INCLUDE_DIR}
					${ALURE_INCLUDE_DIR}
					${OPENAL_INCLUDE_DIR}
					${PHYSFS_INCLUDE_DIR})

set(GAME_TARGET "../bin/astrostruct")
add_executable(${GAME_TARGET} ${source})
target_link_libraries(${GAME_TARGET}
					  ${OPENGL_LIBRARIES}
					  ${LUA_LIBRARIES}
					  ${ENET_LIBRARIES}
					  ${GLEW_LIBRARY}
					  ${GLFW_LIBRARIES}
					  ${FREETYPE_LIBRARIES}
					  ${SOIL_LIBRARIES}
					  ${ALURE_LIBRARIES}
					  ${OPENAL_LIBRARY}
					  ${PHYSFS_LIBRARY})
