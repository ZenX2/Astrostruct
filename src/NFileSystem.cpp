#include "NEngine.hpp"

NReadFile::NReadFile()
{
    File = NULL;
}
NReadFile::NReadFile(std::string Path)
{
    Open = false;
    File = NULL;
    Exists = PHYSFS_exists(Path.c_str());
    if (Exists)
    {
        File = PHYSFS_openRead(Path.c_str());
        if (!File)
        {
            GetGame()->GetLog()->Send("FILESYS",0,Path+": "+PHYSFS_getLastError());
        }
        Eof = PHYSFS_eof(File);
        Open = true;
    }
}
NReadFile::~NReadFile()
{
    Close();
}

unsigned int NReadFile::Read(void* Buffer, unsigned int Size)
{
    if (!Good())
    {
        return 0;
    }
    return PHYSFS_read(File,Buffer,1,Size);
}

unsigned int NReadFile::Size()
{
    if (!Exists)
    {
        return 0;
    }
    return PHYSFS_fileLength(File);
}

void NReadFile::Close()
{
    PHYSFS_close(File);
    Open = false;
}

bool NReadFile::Good()
{
    return (Open && Exists && !Eof && File);
}

NWriteFile::NWriteFile()
{
    File = NULL;
}
NWriteFile::NWriteFile(std::string Path)
{
    File = NULL;
    std::string Directory = Path.substr(0,Path.find_last_of('/'));
    PHYSFS_setWriteDir(("data/"+Directory).c_str());
    std::string FileName = Path.substr(Path.find_last_of('/'));
    FileName = FileName.substr(1);
    File = PHYSFS_openWrite(FileName.c_str());
    if (!File)
    {
        GetGame()->GetLog()->Send("FILESYS",0,Path+": "+PHYSFS_getLastError());
    }
    Open = true;
}

NWriteFile::~NWriteFile()
{
    Close();
}

void NWriteFile::Close()
{
    PHYSFS_close(File);
    Open = false;
}

bool NWriteFile::Good()
{
    return (Open && File);
}

unsigned int NWriteFile::Write(void* Buffer, unsigned int Size)
{
    if (!Good())
    {
        return 0;
    }
    return PHYSFS_write(File,Buffer,1,Size);
}

NFileSystem::NFileSystem(std::string CurrentPath)
{
    if (!PHYSFS_init(CurrentPath.c_str()))
    {
        //GetGame()->GetLog()->Send("FILESYS",0,std::string("PHYSFS encountered an error while initializing: ") + PHYSFS_getLastError());
        return;
    }
    PHYSFS_permitSymbolicLinks(false);
    std::string Path = PHYSFS_getBaseDir();
    PHYSFS_addToSearchPath((Path+"data").c_str(),0);
    /*const PHYSFS_ArchiveInfo** Listing = PHYSFS_supportedArchiveTypes();
    unsigned int i=0;
    std::stringstream Archives;
    while (Listing[i] != NULL)
    {
        Archives << Listing[i]->extension << " ";
        i++;
    }*/
    //GetGame()->GetLog()->Send("FILESYS",2,std::string("Supported archives are: ") + Archives.str());
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

NReadFile NFileSystem::GetReadFile(std::string File)
{
    return NReadFile(File);
}

NWriteFile NFileSystem::GetWriteFile(std::string File)
{
    return NWriteFile(File);
}
