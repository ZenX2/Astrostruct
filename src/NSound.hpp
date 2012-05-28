/**
 * @file NSound.hpp
 * @brief Loads sounds into memory from lua, then exposes the ability to play the sounds to the rest of the engine.
 * @author Dalton Nell
 * @version 1.0
 * @date 2012-05-16
 */
#ifndef NAELSTROF_SOUND
#define NAELSTROF_SOUND

/** @addtogroup SoundSystem
 * @{*/

/**
 * @brief Caches loaded sounds into memory.
 */
class NSoundData
{
public:
    /**
     * @brief Initializes the cache and gives it a name.
     *
     * @param i_Name
     */
    NSoundData(std::string i_Name);
    ~NSoundData();
    std::string Name;
    /**
     * @brief Attempts to load the given file into memory.
     *
     * @param FileName The path to the file.
     *
     * @return True on success, false on failure.
     */
    bool Load(std::string FileName);
    ALuint ID;
};

/**
 * @brief A sound object used to play sounds.
 */
class NSound : public NNode
{
public:
    /**
     * @brief Initializes an ALSource and prepares to play it.
     *
     * @param Name The name of the sound we want to play.
     */
    NSound(std::string Name);
    ~NSound();
    /**
     * @brief Plays the sound from the ALSource.
     */
    void Play();
	/**
	 * @brief Unallocates the object.
	 */
    void Remove();
private:
    ALuint ID;
};

/**
 * @brief Wraps up loading and initializing sounds and OpenAL.
 */
class NSoundSystem
{
public:
    /**
     * @brief Initializes OpenAL and prepares it to play sounds.
     */
    NSoundSystem();
    /**
     * @brief Destroys the OpenAL context.
     */
    ~NSoundSystem();
    /**
     * @brief Retreives a loaded sound id from memory.
     *
     * @param Name The name of the cached sound. (Not the filepath!)
     *
     * @return A pointer to the cached sound.
     */
    NSoundData* GetSound(std::string Name);
    /**
     * @brief Caches a new sound id into memory.
     *
     * @param Data The pointer to the new sound.
     */
    void AddSoundData(NSoundData* Data);
    /**
     * @brief Uses lua to load sounds into memory.
     */
    void LoadSounds();
private:
    std::vector<NSoundData*> SoundData;
};

/*@}*/

#endif
