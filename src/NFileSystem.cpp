#include "NEngine.hpp"

NFile::NFile(std::string i_File, bool i_Writing)
{
    Writeable = true;
    Eof = false;
    Writing = i_Writing;
    if (!PHYSFS_exists(i_File.c_str()) && !Writing)
    {
        Exists = false;
        return;
    }
    Exists = true;
    FileName = i_File;
    if (!Writing)
    {
        File = PHYSFS_openRead(i_File.c_str());
    } else {
        std::string Directory = i_File.substr(0,i_File.find_last_of('/'));
        PHYSFS_setWriteDir(("data/"+Directory).c_str());

        GetGame()->GetLog()->Send("FILESYS",2,std::string("Setting write directory to ") + Directory + ".");

        std::string FileN = i_File.substr(i_File.find_last_of('/'));
        FileN = FileN.substr(1);
        File = PHYSFS_openWrite(FileN.c_str());
    }
}

unsigned int NFile::Size()
{
    if (!Exists)
    {
        return 0;
    }
    return PHYSFS_fileLength(File);
}

int NFile::Read(void* Buffer, unsigned int Size)
{
    if (!Good())
    {
        return 0;
    }
    int Readed = PHYSFS_read(File,Buffer,1,Size);
    if (Readed < Size || Readed == -1)
    {
        GetGame()->GetLog()->Send("FILESYS",0,std::string("Had trouble reading file ") + FileName + ". PHYSFS reported error: " + PHYSFS_getLastError() + "!");
    }
    if (PHYSFS_eof(File))
    {
        Eof = true;
    }
    return Readed;
}

int NFile::Write(void* Buffer, unsigned int Size)
{
    if (!Good())
    {
        return 0;
    }
    int Written = PHYSFS_write(File,Buffer,1,Size);
    if (Written < Size || Written == -1)
    {
        GetGame()->GetLog()->Send("FILESYS",0,std::string("Had trouble writing to file ") + FileName + ". PHYSFS reported error: " + PHYSFS_getLastError() + "!");
        Writeable = false;
    }
    return Written;
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
    if (!Writing)
    {
        return (!Eof && Exists && Writeable);
    }
    return (Writeable);
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
        GetGame()->GetLog()->Send("FILESYS",0,std::string("PHYSFS encountered an error while initializing: ") + PHYSFS_getLastError());
        return;
    }
    PHYSFS_permitSymbolicLinks(false);
    std::string Path = PHYSFS_getBaseDir();
    PHYSFS_addToSearchPath((Path+"data").c_str(),0);
    const PHYSFS_ArchiveInfo** Listing = PHYSFS_supportedArchiveTypes();
    unsigned int i=0;
    std::stringstream Archives;
    while (Listing[i] != NULL)
    {
        Archives << Listing[i]->extension << " ";
        i++;
    }
    GetGame()->GetLog()->Send("FILESYS",2,std::string("Supported archives are: ") + Archives.str());
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

NFile NFileSystem::GetFile(std::string File, bool Writable)
{
    return NFile(File.c_str(),Writable);
}
