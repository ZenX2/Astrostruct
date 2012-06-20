#include "NEngine.hpp"

NSoundSystem::NSoundSystem()
{
	ALCsizei Count;
	const char** DeviceNames = alureGetDeviceNames(true,&Count);
	for (unsigned int i=0;i<Count;i++)
	{
		SetColor(Blue);
		std::cout << "ALURE INFO: ";
		ClearColor();
		std::cout << " Found device: " << DeviceNames[i] << "\n";
	}
	bool Found = false;
	for (unsigned int i=0;i<Count;i++)
	{
		if (alureInitDevice(DeviceNames[0],NULL) != AL_FALSE)
		{
			SetColor(Blue);
			std::cout << "ALURE INFO: ";
			ClearColor();
			std::cout << " Using device: " << DeviceNames[0] << "\n";
			Found = true;
			break;
		}
	}
	if (!Found)
	{
		SetColor(Yellow);
		std::cout << "ALURE WARN: ";
		ClearColor();
		std::cout << "Failed to use any of the devices!!\n";
		alureFreeDeviceNames(DeviceNames);
		return;
	}
	alureFreeDeviceNames(DeviceNames);
    alListener3f(AL_POSITION,0,0,0);
    alListener3f(AL_VELOCITY,0,0,0);
    alListener3f(AL_ORIENTATION,0,0,-1);
}

void NSoundSystem::AddSoundData(NSoundData* Data)
{
    SoundData.push_back(Data);
}

NSoundSystem::~NSoundSystem()
{
    for (unsigned int i=0;i<SoundData.size();i++)
    {
		delete SoundData[i];
    }
	alureShutdownDevice();
}

void NSoundSystem::LoadSounds()
{
    lua_State* L = GetGame()->GetLua()->GetL();
    static const luaL_Reg SoundFunctions[] =
    {
	    {"LoadSound",LoadSound},
	    {NULL,NULL}
    };
    lua_getglobal(L,"_G");
    luaL_register(L,NULL,SoundFunctions);
    lua_pop(L,1);
    GetGame()->GetLua()->DoFolder("sounds");
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
	ALenum Error = alGetError(); //Ensure all errors are cleared before making alure load the sound file, else it won't load.
	while (Error != AL_NO_ERROR)
	{
		SetColor(Yellow);
		std::cout << "OPENAL WARN: ";
		ClearColor();
		switch(Error)
		{
			case AL_INVALID_NAME:
			{
				std::cout << "Invalid name parameter\n";
				break;
			}
			case AL_INVALID_ENUM:
			{
				std::cout << "Invalid parameter\n";
				break;
			}
			case AL_INVALID_VALUE:
			{
				std::cout << "Invalid enum parameter value\n";
				break;
			}
			case AL_INVALID_OPERATION:
			{
				std::cout << "Illegal call\n";
				break;
			}
			case AL_OUT_OF_MEMORY:
			{
				std::cout << "Unable to allocate memory\n";
				break;
			}
			default:
			{
				std::cout << "Unkown error\n";
				break;
			}
		}
		Error = alGetError();
	}
	NFile File = GetGame()->GetFileSystem()->GetFile(FileName);
	if (!File.Good())
	{
		SetColor(Yellow);
		std::cout << "SOUND WARN: ";
		ClearColor();
		std::cout << "Failed to open " << FileName << " as a sound file, it doesn't exist!\n";
		return Fail;
	}
	unsigned char* Data = new unsigned char[File.Size()];
	File.Read(Data,File.Size());
	ID = alureCreateBufferFromMemory(Data,File.Size());
	delete[] Data;
	if (ID == AL_NONE)
	{
		SetColor(Yellow);
		std::cout << "SOUND WARN: ";
		ClearColor();
		std::cout << "Failed to open " << FileName << " as a sound file for reason: " << alureGetErrorString() << "!\n";
		return Fail;
	}
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

void NSound::Remove()
{
    delete (NSound*)this;
}

std::string NSound::GetType()
{
	return "Sound";
}
