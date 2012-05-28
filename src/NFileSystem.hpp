#ifndef NAELSTROF_FILE_SYSTEM
#define NAELSTROF_FILE_SYSTEM

class NFile
{
public:
	NFile(std::string i_File);
	~NFile();
	unsigned int Size();
	unsigned int Read(void* Buffer, unsigned int Size);
	void Seek(unsigned int Pos);
	bool Good();
private:
	PHYSFS_file* File;
	bool Exists;
	bool Eof;
};

class NFileSystem
{
public:
	NFileSystem(char* CurrentPath);
	~NFileSystem();
	std::vector<std::string> GetFiles(std::string Directory);
	NFile GetFile(std::string File);
};

#endif
