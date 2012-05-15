#ifndef NAELSTROF_SOUND
#define NAELSTROF_SOUND

#define BUFFER_SIZE 4096

class NSoundData
{
public:
    NSoundData();
    ~NSoundData();
    std::string Name;
private:
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
private:
    void LoadSounds();
    ALCdevice* AudioDevice;
    ALCcontext* AudioContext;
    std::vector<NSoundData*> SoundData;
};

#endif
