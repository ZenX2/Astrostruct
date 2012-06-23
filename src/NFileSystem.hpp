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
 * @brief Abstracts loading and reading of files, currently doesn't have support to write things.
 */
class NFile
{
public:
    /**
     * @brief Attempts to open a file handle to the specified file, use Good() to figure out if it succeeded or not.
     *
     * @param i_File The path to the file.
     * @param Writing if we want to write to the file or not.
     */
    NFile(std::string i_File, bool Writing);
    ~NFile();
    /**
     * @brief Returns the size in bytes of the file.
     *
     * @return The size in bytes of the file.
     */
    unsigned int Size();
    /**
     * @brief Reads the specified amount of bytes into the specified buffer.
     *
     * @param Buffer Pointer to the buffer, should have a size greater than or equal to Size.
     * @param Size The amount of bytes you want to read.
     *
     * @return The amount of bytes successfully read into the buffer.
     */
    int Read(void* Buffer, unsigned int Size);
    int Write(void* Buffer, unsigned int Size);
    /**
     * @brief Sets the current read position to the origin+Pos of the file.
     *
     * @param Pos The desired read position from the origin.
     */
    void Seek(unsigned int Pos);
    /**
     * @brief Checks for errors.
     *
     * @return False on error, true if it's safe.
     */
    bool Good();
private:
    PHYSFS_file* File;
    std::string FileName;
    bool Exists;
    bool Eof;
    bool Writeable;
    bool Writing;
};

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
     * @brief Returns a file handle to read the specified file.
     *
     * @param File Filepath to file in the virtual filesystem.
     *
     * @return A file handle to the file.
     */
    NFile GetFile(std::string File, bool Writable);
};

#endif
