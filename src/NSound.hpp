#ifndef NAELSTROF_SOUND
#define NAELSTROF_SOUND

#define BUFFER_SIZE 4096

class NSoundData
{
public:
    NSoundData(std::string i_Name);
    ~NSoundData();
    std::string Name;
    bool Load(std::string FileName);
    ALuint ID;
};

class NSound : public NNode
{
public:
    NSound(std::string Name);
    ~NSound();
    void Play();
private:
    ALuint ID;
};

class NSoundSystem
{
public:
    NSoundSystem();
    ~NSoundSystem();
    NSoundData* GetSound(std::string Name);
    void AddSound(NSound* Sound);
    void RemoveSound(NSound* Sound);
private:
    void LoadSounds();
    ALCdevice* AudioDevice;
    ALCcontext* AudioContext;
    std::vector<NSoundData*> SoundData;
    std::vector<NSound*> Sounds;
};

#endif
