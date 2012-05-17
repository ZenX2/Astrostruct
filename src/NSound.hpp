/**
 * @file NSound.hpp
 * @brief Loads sounds into memory from lua, then exposes the ability to play the sounds to the rest of the engine.
 * @author Dalton Nell
 * @version 1.0
 * @date 2012-05-16
 */
#ifndef NAELSTROF_SOUND
#define NAELSTROF_SOUND

#define BUFFER_SIZE 4096

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
     * @brief Adds a Sound object to memory, this is necessary in order to free all the ALSources.
     *
     * @param Sound A pointer to the new sound.
     */
    void AddSound(NSound* Sound);
    /**
     * @brief Removes a Sound object from memory. Doesn't do anything other than keeps it from attempting to free the ALSource on exit.
     *
     * @param Sound The pointer to the sound.
     */
    void RemoveSound(NSound* Sound);
    /**
     * @brief Uses lua to load sounds into memory.
     */
    void LoadSounds();
private:
    ALCdevice* AudioDevice;
    ALCcontext* AudioContext;
    std::vector<NSoundData*> SoundData;
    std::vector<NSound*> Sounds;
};

#endif
