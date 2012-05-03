/**
* @file NInput.hpp
* @brief Handles grabbing input and exposing it to other parts of the engine.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-01
*/

#ifndef NAELSTROF_INPUT
#define NAELSTROF_INPUT

/**
* @brief Handles grabbing input and exposing it to other parts of the engine.
*/
class NInput
{
public:
	/**
	* @brief Grabs all input and puts it into memory for use. Should be getting called every frame in the game loop.
	*/
	void Poll();
	/**
	* @brief Returns the state of a key on the keyboard.
	*
	* @param Key The keycode of the key. Example: 'A' would return the A key's state.
	*
	* @return The key state.
	*/
	int GetKey(int Key);
	/**
	* @brief Returns the current X position of the mouse.
	*
	* @return The X position of the mouse.
	*/
	int GetMouseX();
	/**
	* @brief Returns the Y position of the mouse.
	*
	* @return The Y position of the mouse.
	*/
	int GetMouseY();
	/**
	* @brief If the given key has changed since the last call to KeyChanged, it returns true. Otherwise false.
	*
	* @param Key They keycode of the key. Example: 'A' would return the A key's state.
	*
	* @return True if the key has changed, false if it hasn't.
	*/
	bool KeyChanged(int Key);
	/**
	* @brief Returns the mouse position as a glm::vec2.
	*
	* @return The mouse position as a glm::vec2.
	*/
	glm::vec2 GetMouse();
private:
	int MouseX;
	int MouseY;
	int Keys[128];
};

#endif
