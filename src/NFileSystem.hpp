/**
 * @file NFileSystem.hpp
 * @brief Controls loading files, currently uses physfs to load files from archives.
 * @author Dalton Nell
 * @version 1.0
 * @date 2012-05-28
 */
#ifndef NAELSTROF_FILE_SYSTEM
#define NAELSTROF_FILE_SYSTEM

/**
 * @brief Facilitates reading files by abstracting physfs.
 */
class NReadFile
{
public:
    NReadFile();
    NReadFile(std::string Path);
    ~NReadFile();
    void Close();
    bool Good();
    unsigned int Read(void* Buffer, unsigned int Size);
    unsigned int Size();
private:
    bool Open;
    bool Exists;
    bool Eof;
    PHYSFS_File* File;
};

/**
 * @brief Facilitates writing files by abstracting physfs. Right now you can only write to one folder at a time so make sure you close these when you're done with them!
 */
class NWriteFile
{
public:
    NWriteFile();
    NWriteFile(std::string Path);
    ~NWriteFile();
    void Close();
    bool Good();
    unsigned int Write(void* Buffer, unsigned int Size);
private:
    bool Open;
    PHYSFS_File* File;
};

//To do: Append file system when needed.

/**
 * @brief Wraps up initializing the filesystem, required since physfs has a virtual filesystem.
 */
class NFileSystem
{
public:
    /**
     * @brief Initializes the virtual filesystem.
     *
     * @param CurrentPath The current path of the execution, usually argv[0].
     */
    NFileSystem(std::string CurrentPath);
    ~NFileSystem();
    /**
     * @brief Gets a recursive listing of files from the specified directory.
     *
     * @param Directory The filepath to the directory.
     *
     * @return A vector full of filepaths AND directories.
     */
    std::vector<std::string> GetFiles(std::string Directory);
    /**
     * @brief Just returns a NReadFile class of the object, it's really not necessary to use and it might be cleaner to just do NReadFile(File) instead.
     *
     * @param File The file path to the file. ex textures/human/human.png
     *
     * @return NReadFile(File)
     */
    NReadFile GetReadFile(std::string File);
    /**
     * @brief Just returns a NWriteFile class of the object, it's really not necessary to use and it might be cleaner to just do NWriteFile(File) instead.
     *
     * @param File The file path to the file. ex textures/human/human.png
     *
     * @return NWriteFile(File)
     */
    NWriteFile GetWriteFile(std::string File);
    /**
     * @brief Checks if a "file" is a directory or not.
     *
     * @param File The filepath to the file/directory.
     *
     * @return True if it is, false otherwise.
     */
    bool IsDir(std::string File);
};

#endif
