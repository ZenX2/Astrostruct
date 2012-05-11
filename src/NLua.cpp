#include "NEngine.hpp"

NLua::NLua()
{
	L = lua_open();
	luaL_openlibs(L);
	static const luaL_Reg BaseFunctions[] =
	{
		{"include",Include},
		{NULL,NULL}
	};
	lua_getglobal(L,"_G");
	luaL_register(L, NULL, BaseFunctions);
}

NLua::~NLua()
{
	lua_close(L);
}

int Include(lua_State* L)
{
	lua_Debug ar1;
	lua_getstack(L,1,&ar1);
	lua_getinfo(L,"f",&ar1);
	lua_Debug ar2;
	lua_getinfo(L,">S",&ar2);
	std::string Source = ar2.source;
	int Found = Source.find_last_of("/\\");
	std::string Folder = Source.substr(1,Found);
	std::string File = luaL_checkstring(L,1);
}

bool NLua::DoFile(std::string File)
{
	if (luaL_dofile(L,File.c_str()))
	{
		SetColor(Yellow);
		std::cout << "LUA WARN: ";
		ClearColor();
		std::cout << lua_tostring(L, -1) << "\n";
		return Fail;
	}
	return Success;
}

std::vector<std::string> GetDir(std::string DirectoryName, std::string ParentDirectory)
{
	std::vector<std::string> Folders;
	DIR *dp;
	dirent *dirp;
	dp = opendir(DirectoryName.c_str());
	if (dp == NULL)
	{
		return Folders;
	}
	while ((dirp = readdir(dp)) != NULL) // While we have more files to find
	{
		struct stat stat_base;
		stat(std::string(DirectoryName+'/'+dirp->d_name).c_str(),&stat_base); // Stat the file we're working on
		if ((stat_base.st_mode & S_IFMT) == S_IFDIR) // If the file we're reading is a directory add it to the list
		{
			if (strcmp(dirp->d_name,".") && strcmp(dirp->d_name,".."))
			{
				Folders.push_back(ParentDirectory+'/'+std::string(dirp->d_name));
			}
		}
	}
	for (unsigned int i=0;i<Folders.size();i++) // Now recursively search for directories and add them to the list as well
	{
		std::vector<std::string> RFolders = GetDir(Folders[i], ParentDirectory);
		Folders.insert(Folders.end(),RFolders.begin(),RFolders.end());
	}
	closedir(dp);
	return Folders;
}

std::vector<std::string> GetFiles(std::string DirectoryName)
{
	std::vector<std::string> Files;
	DIR *dp;
	dirent *dirp;
	dp = opendir(DirectoryName.c_str());
	if (dp == NULL)
	{
		return Files;
	}
	while ((dirp = readdir(dp)) != NULL) // While we have more files to find
	{
		struct stat stat_base;
		stat(std::string(DirectoryName+'/'+dirp->d_name).c_str(),&stat_base); // Stat the file we're working on
		if ((stat_base.st_mode & S_IFMT) == S_IFREG) // If the file we're reading is a directory add it to the list
		{
			Files.push_back(DirectoryName+'/'+std::string(dirp->d_name));
		}
	}
	closedir(dp);
	return Files;
}

bool NLua::DoFolder(std::string Folder)
{
	SetColor(Blue);
	std::cout << "LUA INFO: ";
	ClearColor();
	std::cout << "Scanning " << Folder << " for lua files...\n";
	std::string ParentFolder = Folder;
	bool Result = Success;
	std::vector<std::string> Folders = GetDir(Folder,ParentFolder);
	std::vector<std::string> Files;
	for (unsigned int i=0;i<Folders.size();i++)
	{
		std::vector<std::string> NewFiles = GetFiles(Folders[i]);
		Files.insert(Files.end(),NewFiles.begin(),NewFiles.end());
	}
	std::vector<std::string> NewFiles = GetFiles(Folder);
	Files.insert(Files.end(),NewFiles.begin(),NewFiles.end());
	for (unsigned int i=0;i<Files.size();i++)
	{
		if (Files[i].find(".lua")!=std::string::npos && Files[i][Files[i].length()-1] == 'a')
		{
			SetColor(Blue);
			std::cout << "LUA INFO: ";
			ClearColor();
			std::cout << "Found " << Files[i] << "\n";
			Result = (DoFile(Files[i]) && Result);
		}
	}
	return Result;
}

lua_State* NLua::GetL()
{
	return L;
}
