/**
* @file NScene.hpp
* @brief Handles active game objects.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-02
*/
#ifndef NAELSTROF_SCENE
#define NAELSTROF_SCENE

/**
* @brief Class that holds arbitrary objects and handles Tick-ing and drawing them.
*/
class NScene
{
public:
    NScene();
    ~NScene();
    /**
    * @brief Updates all objects in the scene.
    */
    void Tick();
    /**
    * @brief Draws all objects in the scene.
    *
    * @param View The camera view matrix.
    */
    void Draw(NCamera* View);
    /**
    * @brief Adds an object to the scene.
    *
    * @param Node The desired object to be added.
    */
    void AddNode(NNode* Node);
    /**
    * @brief Adds a text object to the scene.
    *
    * @param Font The desired font of the object.
    * @param Data The desired text of the object.
    *
    * @return A pointer to the newly created object.
    */
    NText* AddText(std::string Font, std::wstring Data);
    /**
     * @brief Adds a window to the scene.
     *
     * @return A pointer to the new window.
     */
    NWindow* AddWindow(std::string Texture);
    /**
     * @brief Adds a button to the scene.
     *
     * @return A pointer to the new button.
     */
    NButton* AddButton(std::string Texture);
    /**
     * @brief Adds a sound to the scene.
     *
     * @param Name The name of the sound we want to load.
     *
     * @return A pointer to the new sound.
     */
    NSound* AddSound(std::string Name);
    /**
     * @brief Adds a map to the scene.
     *
     * @param TileSet The name of the texture we want to use for the map.
     *
     * @return A pointer to the map created.
     */
    NMap* AddMap(std::string TileSet);
    /**
     * @brief Adds a camera to the scene.
     *
     * @return A pointer to the camera.
     */
    NCamera* AddCamera();
    /**
     * @brief Adds a player to the scene.
     *
     * @return A pointer to the player created.
     */
    NPlayer* AddPlayer(std::wstring Name);
    /**
     * @brief Adds a light to the scene.
     *
     * @param Texture The desired texture of the light.
     *
     * @return A pointer to the light created.
     */
    NLight* AddLight(std::string Texture);
    /**
     * @brief Adds a star to the scene, these manage themselves but you could still use the pointer it returns to mess with them. :u
     *
     * @return A pointer to the star created.
     */
    NStar* AddStar();
    /**
     * @brief Adds a player to the scene.
     *
     * @param Name The name of the player.
     *
     * @return A pointer to the created player.
     */
    NPlayer* AddPlayer(std::string Name);
    /**
     * @brief Adds a checkbox to the scene.
     *
     * @param Texture The desired texture name.
     *
     * @return A pointer to the created checkbox.
     */
    NCheckbox* AddCheckbox(std::string Texture);
    /**
     * @brief Adds a textbox to the scene.
     *
     * @param Texture The desired texture name for the checkbox.
     *
     * @return A pointer to the created textbox.
     */
    NTextInput* AddTextInput(std::string Texture);
    /**
     * @brief Returns a pointer to the desired render layer.
     *
     * @param The desired layer number.
     *
     * @return Layer A pointer to the vector containing the layer nodes.
     */
    std::vector<NNode*>* GetLayer(unsigned int Layer);
    /**
     * @brief Searches all the layers for the node with the specified ID.
     *
     * @param ID The ID of the node we want.
     *
     * @return A pointer to the node with the ID, or NULL if it doesn't exist.
     */
    NNode* GetNodeByID(unsigned int ID);
    /**
     * @brief Swaps the Node from it's current layer to the specified layer.
     *
     * @param Node The desired node.
     * @param Layer The desired layer.
     */
    void SwapLayer(NNode* Node, unsigned int Layer);
    /**
     * @brief Searches for a Node with the specified ID and removes it.
     *
     * @param ID The ID of the node.
     */
    void RemoveByID(unsigned int ID);
    /**
     * @brief Removes the specified Node from the scene.
     *
     * @param Node The desired node we want to remove.
     */
    void Remove(NNode* Node);
    /**
     * @brief Toggles lighting.
     */
    void ToggleFullBright();
    /**
     * @brief Turns on or off lighting.
     *
     * @param Bright True if you want to be able to see everything, false if you want proper lighting.
     */
    void SetFullBright(bool Bright);
    /**
     * @brief Returns if lighting is enabled or not.
     *
     * @return True if it's disabled, false otherwise.
     */
    bool GetFullBright();
    /**
     * @brief Searches all of the layers for nodes of the specified type and removes them.
     *
     * @param Type The desired type to be removed.
     */
    void RemoveByType(NodeType Type);
    /**
     * @brief Forces all lights to update their shadows, currently used when map changes.
     */
    void UpdateLights();
private:
    double LastTick;
    std::vector<std::vector<NNode*> > Layers;
    bool ShuttingDown;
    bool FullBright;
};

#endif
