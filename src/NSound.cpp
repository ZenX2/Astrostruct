#include "NEngine.hpp"

NSoundSystem::NSoundSystem()
{
    AudioDevice = alcOpenDevice(NULL);
    if (!AudioDevice)
    {
	SetColor(Yellow);
	std::cout << "SOUND WARN: ";
	ClearColor();
	std::cout << "OpenAL couldn't find an appropriate audio device to play sound through!\n";
	return;
    }
    AudioContext = alcCreateContext(AudioDevice,NULL);
    alcMakeContextCurrent(AudioContext);
    if (!AudioContext)
    {
	SetColor(Yellow);
	std::cout << "SOUND WARN: ";
	ClearColor();
	std::cout << "OpenAL failed to create an audio context!\n";
	return;
    }
    alListener3f(AL_POSITION,0,0,0);
    alListener3f(AL_VELOCITY,0,0,0);
    alListener3f(AL_ORIENTATION,0,0,-1);
    LoadSounds();
}

NSoundSystem::~NSoundSystem()
{
    for (unsigned int i=0;i<Sounds.size();i++)
    {
	delete Sounds[i];
    }
    alcDestroyContext(AudioContext);
    alcCloseDevice(AudioDevice);
}

void NSoundSystem::LoadSounds()
{
    NSoundData* Data = new NSoundData("coin");
    Data->Load("data/sounds/coin.ogg");
    SoundData.push_back(Data);
}

NSoundData* NSoundSystem::GetSound(std::string Name)
{
    for (unsigned int i=0;i<SoundData.size();i++)
    {
	if (SoundData[i]->Name == Name)
	{
	    return SoundData[i];
	}
    }
    return 0;
} 

NSoundData::NSoundData(std::string i_Name)
{
    ID = 0;
    Name = i_Name;
}

bool NSoundData::Load(std::string FileName)
{
    int Endian = 0;
    int BitStream;
    long Bytes;
    char Array[BUFFER_SIZE];
    vorbis_info* VorbisInfo;
    OggVorbis_File OggFile;
    if (ov_fopen(FileName.c_str(), &OggFile))
    {
	SetColor(Yellow);
	std::cout << "SOUND WARN: ";
	ClearColor();
	std::cout << "Vorbis failed to open " << FileName << " as a sound file!\n";
	return Fail;
    }
    alGenBuffers(1,&ID);
    VorbisInfo = ov_info(&OggFile,-1);
    ALenum Format;
    std::vector<char> Buffer;
    switch (VorbisInfo->channels)
    {
	case 1:
	{
	    Format = AL_FORMAT_MONO16;
	    break;
	}
	default:
	{
	    Format = AL_FORMAT_STEREO16;
	    break;
	}
    }
    int Frequency = VorbisInfo->rate;
    do {
	Bytes = ov_read(&OggFile,Array,BUFFER_SIZE,Endian,2,1,&BitStream);
	Buffer.insert(Buffer.end(),Array,Array+Bytes);
    } while (Bytes>0);
    ov_clear(&OggFile);
    alBufferData(ID,Format,&Buffer[0],(ALsizei)Buffer.size(),Frequency);
    return Success;
}

NSoundData::~NSoundData()
{
    if (ID != 0)
    {
	alDeleteBuffers(1,&ID);
    }
}

NSound::NSound(std::string Name)
{
    ID = 0;
    NSoundData* Check = GetGame()->GetSoundSystem()->GetSound(Name);
    if (Check == NULL)
    {
	return;
    }
    alGenSources(1,&ID);
    alSourcef(ID,AL_PITCH,1);
    alSourcef(ID,AL_GAIN,1);
    alSource3f(ID,AL_POSITION,0,0,0);
    alSource3f(ID,AL_VELOCITY,0,0,0);
    alSourcei(ID,AL_LOOPING,AL_FALSE);
    alSourcei(ID,AL_BUFFER,Check->ID);
}

void NSound::Play()
{
    alSourcePlay(ID);
}

NSound::~NSound()
{
    if (!ID)
    {
	return;
    }
    alDeleteSources(1,&ID);
}

void NSoundSystem::AddSound(NSound* Sound)
{
    Sounds.push_back(Sound);
}

void NSoundSystem::RemoveSound(NSound* Sound)
{
    for (unsigned int i=0;i<Sounds.size();i++)
    {
	if (Sound == Sounds[i])
	{
	    delete Sound;
	    Sounds.erase(Sounds.begin()+i);
	    return;
	}
    }
}
