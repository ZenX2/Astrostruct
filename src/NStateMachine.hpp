/**
 * @file NStateMachine.hpp
 * @brief Handles game states, like if the game is paused, playing, serving, map editing, etc.
 * @author Dalton Nell
 * @version 1.0
 * @date 2012-06-14
 */
#ifndef NAELSTROF_STATEMACHINE
#define NAELSTROF_STATEMACHINE

/**
 * @brief State template that other states derive from.
 */
class NState
{
public:
	NState();
	~NState();
	/**
	 * @brief Called when the engine switches to the state.
	 */
	virtual void OnEnter();
	/**
	 * @brief Moves the state forward in time.
	 *
	 * @param DT The delta time given from the state machine.
	 */
	virtual void Tick(double DT);
	/**
	 * @brief Called when the engine switches out of the state.
	 */
	virtual void OnExit();
	/**
	 * @brief Returns the name of the state.
	 *
	 * @return "NULL"
	 */
	virtual std::string GetName();
	bool Init;
};

/**
 * @brief Handles switching to/from states as well as ticking them.
 */
class NStateMachine
{
public:
	NStateMachine(std::string State);
	~NStateMachine();
	/**
	 * @brief Switches the game's current states.
	 *
	 * @param Name The desired state name to switch to.
	 */
	void SetState(std::string Name);
	/**
	 * @brief Moves the current state forward in time.
	 */
	void Tick();
private:
	double LastExecution;
	int CurrentState;
	std::vector<NState*> States;
};

/**
 * @brief Handles paused game states.
 */
class NPauseState : public NState
{
public:
	NPauseState();
	~NPauseState();
	void OnEnter();
	void OnExit();
	std::string GetName();
	void Tick(double DT);
private:
	NText* InfoText;
	NWindow* Window;
	NWindow* InfoWindow;
	NButton* PlayButton;
	NButton* QuitButton;
	NButton* MultiButton;
	NButton* MapButton;
	NSound* PlaySound;
};

/**
 * @brief Handles the normal game play state.
 */
class NGameState : public NState
{
public:
	NGameState();
	~NGameState();
	void OnEnter();
	void OnExit();
	std::string GetName();
	void Tick(double DT);
private:
	NPlayer* Player;
	NLight* Light;
};

class NServerState : public NState
{
public:
	NServerState();
	~NServerState();
	void OnEnter();
	void OnExit();
	std::string GetName();
	void Tick(double DT);
private:
};

class NOnlineState : public NState
{
public:
	NOnlineState();
	~NOnlineState();
	void OnEnter();
	void OnExit();
	std::string GetName();
	void Tick(double DT);
private:
};

class NMapState : public NState
{
public:
	NMapState();
	~NMapState();
	void OnEnter();
	void OnExit();
	std::string GetName();
	void Tick(double DT);
private:
	glm::vec3 WantedPosition;
	bool ShowWindow;
	NWindow* Window;
	NWindow* SaveWindow;
	NButton* MapIncrease[3];
	NButton* MapDecrease[3];
	NText* Dimensions[3];
	unsigned int MapDim[3];
	NButton* Increase;
	NButton* Decrease;
	NButton* SaveButton;
	NButton* Apply;
	NButton* LoadButton;
	NTextInput* Input;
	NCheckbox* CheckBox;
	NCheckbox* OCheckBox;
	NText* Text;
	NLight* Light;
	NText* CheckText;
	NText* OCheckText;
	NWindow* OtherWindow;
	NText* ChangingText;
	int CurrentTile;
};
#endif
