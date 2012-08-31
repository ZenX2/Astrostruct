/**
* @file NTexture.hpp
* @brief Abstracts OpenGL texture objects so we can apply texture filters to them all easily, and perhaps animations in the future.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-01
*/
#ifndef NAELSTROF_TEXTURE
#define NAELSTROF_TEXTURE

class NCachedTexture;

/**
* @brief Causes lua to recursively search data/textures for lua files. Ones that should contain code to load textures.
*/
void LoadTextures();
/**
* @brief Animation object to hold a series of image ID's and an fps.
*/
class NAnimation
{
public:
    NAnimation();
    ~NAnimation();
    /**
    * @brief Tells the render system to retreive a texture id for the file, it'll load the file into memory if it hasn't already.
    *
    * @param FileName The path to the file.
    */
    void AddFrame(std::string FileName);
    float FPS;
    /**
    * @brief Gets the texture ID based on time passed.
    *
    * @param Time The current time.
    *
    * @return The texture ID of the current animation frame.
    */
    GLuint GetID(double Time);
    /**
    * @brief Sets the name of the Animation, I'm unsure why I abstracted the name. I had some nasty bug and thought it'd help. :u
    *
    * @param i_Name The desired name of the Animation.
    */
    void SetName(std::string i_Name);
    /**
    * @brief Returns the name of the animation.
    *
    * @return Return the name of the animation.
    */
    std::string GetName();
    /**
     * @brief Returns the size of the texture at the specified time of the animation.
     *
     * @param Time Any amount of time in seconds.
     *
     * @return A vector containing the width and height of a texture.
     */
    glm::vec2 GetSize(double Time);
    /**
     * @brief Uses lua to retrieve a float.
     *
     * @param i_Name The name of the lua object.
     *
     * @return The float grabbed from the lua object, or 0 if it fails.
     */
    float GetFloat(std::string i_Name);
    /**
     * @brief Returns if all the textures successfully loaded.
     *
     * @return False on failure, true on success.
     */
    bool Good();
    /**
     * @brief Returns a lua string attached to the animation.
     *
     * @param i_Name The name of the string.
     *
     * @return The string.
     */
    std::string GetString(std::string i_Name);
    std::vector<NCachedTexture*> Frames;
    /**
     * @brief Caches a number into memory using a name. Set using lua because I couldn't get lua to chache it for me.
     *
     * @param i_Name The name of the number.
     * @param Number The value of the number as a float.
     */
    void AddNumber(std::string i_Name, float Number);
private:
    std::vector<std::string> NumberNames;
    std::vector<float> Numbers;
    std::string Name;
};
/**
* @brief Abstracts OpenGL texture ID's and allows for animations of a series of images.
*/
class NTexture
{
public:
    /**
    * @brief Initializes a copy of the given texture. Used by NRender to return a unique texture object for each GetTexture call.
    *
    * @param Texture
    */
    NTexture(NTexture* Texture);
    /**
    * @brief Creates a texture object and sets it's name.
    *
    * @param i_Name The desired texture name.
    */
    NTexture(std::string i_Name);
    /**
    * @brief Deletes the opengl texture object and itself. (So don't go trying to delete the opengl texture you created!)
    */
    ~NTexture();
    /**
    * @brief Returns an ID to the texture object.
    *
    * @return An opengl texture ID.
    */
    GLuint GetID();
    /**
    * @brief Adds an Animation object to use it's texture id's.
    *
    * @param Animation 
    */
    void AddAnimation(NAnimation* Animation);
    std::string Name;
    /**
    * @brief Plays the specified animation.
    *
    * @param i_Name The name of the animation.
    */
    void Play(std::string i_Name);
    /**
     * @brief Plays an animation based on it's ID, useful for playing random animations.
     *
     * @param i_ID The desired ID of the animation to play.
     */
    void Play(unsigned int i_ID);
    /**
    * @brief Moves the texture forward in time by DT.
    *
    * @param DT The time passed since last tick.
    */
    void Tick(double DT);
    /**
     * @brief Returns the texture dimensions of the first texture in the animation.
     *
     * @return The width and height of a texture in a glm vector.
     */
    glm::vec2 GetSize();
    /**
     * @brief Grabs float data from the first texture's lua reference, useful for GUI where knowing the bordersize of a texture is important.
     *
     * @param i_Name The name of the lua object.
     *
     * @return The desired float, or NULL if not found.
     */
    float GetFloat(std::string i_Name);
    /**
     * @brief Returns if all the textures successfully loaded.
     *
     * @return False on failure, true on success.
     */
    bool Good();
    /**
     * @brief Gets the amount of animations the texture has, useful for playing a random animation.
     *
     * @return The number of animations.
     */
    unsigned int GetAnimationCount();

    void SetResetOnPlay(bool reset);
private:
    bool GoodCheck;
    bool IsGood;
    bool ResetOnPlay;
    double CurrentTime;
    unsigned int PlayingAnimation;
    std::vector<NAnimation*> Animations;
};

#endif
