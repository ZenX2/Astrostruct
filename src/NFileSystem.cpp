#include "NEngine.hpp"

NFile::NFile(std::string i_File)
{
	Eof = false;
	if (!PHYSFS_exists(i_File.c_str()))
	{
		Exists = false;
		return;
	}
	Exists = true;
	File = PHYSFS_openRead(i_File.c_str());
}

unsigned int NFile::Size()
{
	if (!Exists)
	{
		return 0;
	}
	return PHYSFS_fileLength(File);
}

unsigned int NFile::Read(void* Buffer, unsigned int Size)
{
	if (!Good())
	{
		return 0;
	}
	unsigned int Readed = PHYSFS_read(File,Buffer,1,Size);
	if (PHYSFS_eof(File))
	{
		Eof = true;
	}
	return Readed;
}

void NFile::Seek(unsigned int Pos)
{
	if (!Exists)
	{
		return;
	}
	PHYSFS_seek(File,Pos);
}

bool NFile::Good()
{
	return (!Eof && Exists);
}

NFile::~NFile()
{
	if (Exists)
	{
		PHYSFS_close(File);
	}
}

NFileSystem::NFileSystem(std::string CurrentPath)
{
	if (!PHYSFS_init(CurrentPath.c_str()))
	{
		SetColor(Yellow);
		std::cout << "FILESYSTEM WARN: ";
		ClearColor();
		std::cout << "PHYSFS encountered an error: " << PHYSFS_getLastError() << "\n";
		return;
	}
	PHYSFS_permitSymbolicLinks(false);
	PHYSFS_addToSearchPath("data",0);
	SetColor(Blue);
	std::cout << "FILESYSTEM INFO: ";
	ClearColor();
	std::cout << "Supported archives are: ";
	const PHYSFS_ArchiveInfo** Listing = PHYSFS_supportedArchiveTypes();
	unsigned int i=0;
	while (Listing[i] != NULL)
	{
		std::cout << Listing[i]->extension << " ";
		i++;
	}
	std::cout << "\n";
}

NFileSystem::~NFileSystem()
{
	PHYSFS_deinit();
}

std::vector<std::string> NFileSystem::GetFiles(std::string Directory)
{
	char** Listing = PHYSFS_enumerateFiles(Directory.c_str());
	unsigned int i = 0;
	std::vector<std::string> Files;
	while (Listing[i] != NULL)
	{
		Files.push_back((Directory)+'/'+std::string(Listing[i]));
		if (PHYSFS_isDirectory((Directory+'/'+std::string(Listing[i])).c_str()))
		{
			std::vector<std::string> NewFiles = GetFiles(Directory+'/'+std::string(Listing[i]));
			Files.insert(Files.end(),NewFiles.begin(),NewFiles.end());
			i++;
			continue;
		}
		i++;
	}
	PHYSFS_freeList(Listing);
	return Files;
}

NFile NFileSystem::GetFile(std::string File)
{
	return NFile(File.c_str());
}
