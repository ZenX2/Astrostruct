/**
 * @file NPlayer.hpp
 * @brief Handles players, they are a special case from general entities because they need really good networking.
 * @author Dalton Nell
 * @version 1.0
 * @date 2012-06-13
 */
#ifndef NAELSTROF_PLAYER
#define NAELSTROF_PLAYER

/**
 * @brief The player class, this is abstracted from the entity class due to it being highly sensitive to networking.
 */
class NPlayer : public NNode
{
public:
	NPlayer();
	~NPlayer();
	/**
	 * @brief Begins to move the player in the specified direction and plays the run animation.
	 *
	 * @param Direction The direction in radians.
	 */
	void Move(float Direction);
	/**
	 * @brief Stops the player from moving and plays the idle animation. (Move(float Direction) persists until you call this)
	 */
	void StopMove();
	/**
	 * @brief Checks the player for collisions with the map and reacts accordingly.
	 *
	 * @param DT
	 */
	void Tick(double DT);
	/**
	 * @brief Sets the player's velocity.
	 *
	 * @param i_Velocity The desired velocity.
	 */
	void SetVel(glm::vec3 i_Velocity);
	/**
	 * @brief Returns the player's velocity.
	 *
	 * @return The player's velocity.
	 */
	glm::vec3 GetVel();
	/**
	 * @brief Draws the player in it's current animation state.
	 *
	 * @param View The camera to draw from.
	 */
	void Draw(NCamera* View);
	/**
	 * @brief Allows the player to be controlled, when multiplayer is implemented, make sure to only call this on the player the server lets you!
	 */
	void SetControl();
	/**
	 * @brief Deletes the player and its draw buffers.
	 */
	void Remove();
	/**
	 * @brief Returns the type of this object.
	 *
	 * @return "Player"
	 */
	std::string Type();
private:
	glm::vec2 CollisionBox;
	float Gravity;
	bool OnGround;
	float Friction;
	bool Controlled;
	NShader* Shader;
	GLuint TextureLoc, MatrixLoc, ColorLoc;
	bool Changed;
	GLuint Buffers[2];
	void GenerateBuffers();
	float Speed;
	std::vector<glm::vec2> Verts;
	std::vector<glm::vec2> UVs;
	glm::vec3 Velocity;
	float CurrentDirection;
	bool Moving;
	NTexture* Texture;
};

#endif
